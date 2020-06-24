#pragma once
#include "AbstractTableView.h"
class CAbstractStdTable :
	public CAbstractTableView
{
public:
	CAbstractStdTable();
	~CAbstractStdTable();


	
public:
	CString paintContent(CDC* painter, Graphics& graphics, dsint rowBase, int rowOffset, int col, int x, int y, int w, int h) override;
	void updateColors();

	void OnButtonDown(UINT nFlags, CPoint point, bool& wAccept);
	void enableMultiSelection(bool enabled);

	// Selection Management
	bool isSelected(int base, int offset) const;
	void setSingleSelection(int index);
	int getInitialSelection() const;
	vector<int> getSelection() const;
	void expandSelectionUpTo(int to);

	// Data Management
	virtual CString getCellContent(int r, int c) = 0;
	virtual bool isValidIndex(int r, int c) = 0;
	//context menu helpers
	void setupCopyMenu(CMenu* copyMenu);

	//draw helpers
	void setHighlightText(CString highlightText)
	{
		mHighlightText = highlightText;
	}

	void setAddressColumn(int col, bool cipBase = false)
	{
		mAddressColumn = col;
		bCipBase = cipBase;
	}

protected:
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
	Color mSelectedAddressColor;
	Color mAddressBackgroundColor;
	Color mAddressColor;
	Color mTracedBackgroundColor;
	Color mTracedSelectedAddressBackgroundColor;

	CString mHighlightText;
	
	bool mIsMultiSelectionAllowed = false;
	bool mCopyMenuOnly = false;
	bool mCopyMenuDebugOnly = true;
	bool mIsColumnSortingAllowed = true;
	bool bAddressLabel = true;
	bool bCipBase = false;

	int mAddressColumn = -1;

	enum
	{
		NoState,
		MultiRowsSelectionState
	} mGuiState = NoState;

	struct SelectionData
	{
		int firstSelectedIndex = 0;
		int fromIndex = 0;
		int toIndex = 0;
	};
	SelectionData mSelection;

	std::vector<CString> mCopyTitles;

private:
	bool bSHIFT = false;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnGlobalCopy(UINT nCopyID);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

