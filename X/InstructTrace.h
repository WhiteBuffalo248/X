#pragma once
#include "AbstractTableView.h"
#include "QBeaEngine.h"
#include "VaHistory.h"

class CTraceFileReader;

class CInstructTrace :
	public CAbstractTableView
{
public:
	CInstructTrace();
	~CInstructTrace();

public:
	void parseFinished();
	void update();
	void updateColors();

	void expandSelectionUpTo(duint to);
	void setSingleSelection(duint index);
	duint getInitialSelection();
	duint getSelectionStart();
	duint getSelectionEnd();

	void onToggleRunTrace();
	void onOpenFile();
	void onCloseFile();
	void onCloseAndDelete();
	void onCopySelection(bool);
	void onFollowDisassembly();

	void open(const CString & fileName);
	
	
	CString paintContent(CDC* painter, Graphics& graphics, dsint rowBase, int rowOffset, int col, int x, int y, int w, int h);



private:
	CString getIndexText(duint index);
	CString getAddrText(dsint cur_addr, char label[MAX_LABEL_SIZE], bool getLabel);

	struct SelectionData
	{
		duint firstSelectedIndex;
		duint fromIndex;
		duint toIndex;
	};

	VaHistory mHistory;
	SelectionData mSelection;
	CTraceFileReader* mTraceFile;
	CapstoneTokenizer::SingleToken mHighlightToken;
	QBeaEngine* mDisasm;

	duint mRvaDisplayBase;

	bool mHighlightingMode;
	bool mRvaDisplayEnabled;
	bool mPermanentHighlightingMode;

	CString mFileName;

	Color mBytesColor;
	Color mBytesBackgroundColor;

	Color mInstructionHighlightColor;
	Color mSelectionColor;

	Color mCipBackgroundColor;
	Color mCipColor;

	Color mBreakpointBackgroundColor;
	Color mBreakpointColor;

	Color mHardwareBreakpointBackgroundColor;
	Color mHardwareBreakpointColor;

	Color mBookmarkBackgroundColor;
	Color mBookmarkColor;

	Color mLabelColor;
	Color mLabelBackgroundColor;

	Color mSelectedAddressBackgroundColor;
	Color mTracedAddressBackgroundColor;
	Color mSelectedAddressColor;
	Color mAddressBackgroundColor;
	Color mAddressColor;

	Color mAutoCommentColor;
	Color mAutoCommentBackgroundColor;
	Color mCommentColor;
	Color mCommentBackgroundColor;



public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnHandle(UINT id);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

