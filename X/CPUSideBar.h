#pragma once
#include "QBeaEngine.h"
#include "CodeFolding.h"

// CCPUSideBar
class CDisassembly;

class CCPUSideBar : public CWnd
{
	DECLARE_DYNAMIC(CCPUSideBar)

public:
	CCPUSideBar();
	virtual ~CCPUSideBar();

	void reload();
	void updateColors();
	void updateFonts();
	void setViewableRows(int rows);
	void setSelection(dsint selVA);
	void changeTopmostAddress(dsint i);
	int getDisassemblyViewHeaderHeight();

protected:
	bool isJump(int i) const;
	

protected:
	DECLARE_MESSAGE_MAP()

private:
	struct JumpLine
	{
		int line;
		int destLine;
		unsigned int jumpOffset;
		bool isSelected;
		bool isConditional;
		bool isJumpGoingToExecute;
	};

	struct LabelArrow
	{
		int line;
		int startX;
		int endX;
	};

	CFont* mDefaultFont;

	REGDUMP regDump;

	int fontWidth, fontHeight;
	dsint selectedVA = 0;
	dsint topVA= -1;
	int viewableRows = 0;
	CList<Instruction_t>* mInstrBuffer;
	CDisassembly* mDisas;
	CodeFoldingHelper mCodeFoldingManager;

	Color mCipLabelColor;
	Color mCipLabelBackgroundColor;
	Color mConditionalJumpLineFalseColor;
	Color mUnconditionalJumpLineFalseColor;
	Color mConditionalJumpLineTrueColor;
	Color mUnconditionalJumpLineTrueColor;

	LabelArrow drawLabel(CDC* painter, int Line, const CString & Text);
	void drawLabelArrows(CDC* painter, const std::vector<LabelArrow> & labelArrows);
	void drawJump(CDC* painter, Gdiplus::Graphics& graphics, int startLine, int endLine, int jumpoffset, bool conditional, bool isexecute, bool isactive);
	void AllocateJumpOffsets(std::vector<JumpLine> & jumpLines, std::vector<LabelArrow> & labelArrows);
	void drawStraightArrow(CDC* painter, int x1, int y1, int x2, int y2);
	int isFoldingGraphicsPresent(int line);

public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


