#pragma once
#include "HexDump.h"
class CCPUStack :
	public CHexDump
{
public:
	CCPUStack();
	~CCPUStack();

public:
	void updateFonts() override;
	void updateColors() override;
	void selectionUpdated();
	void onLButtonOnHeader(int wColIndex){}
	void stackDumpAt(duint addr, duint csp);
	void getColumnRichText(int col, dsint rva, RichTextPainter::List & richText) override;
	CString paintContent(CDC* painter, Graphics& graphics, dsint rowBase, int rowOffset, int col, int x, int y, int w, int h);










private:
	Color mUserStackFrameColor;
	Color mSystemStackFrameColor;
	Color mStackReturnToColor;
	Color mStackSEHChainColor;
	duint mCsp;
	struct CPUCallStack
	{
		duint addr;
		int party;
	};
	std::vector<CPUCallStack> mCallstack;
public:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
};

