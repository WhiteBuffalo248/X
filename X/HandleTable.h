#pragma once
#include "TableVessel.h"


class CHandleTable :
	public CTableVessel
{
public:
	CHandleTable();
	~CHandleTable();

public:
	enum GuiState
	{
		NoState,
		ReadyToResizeH,
		ReadyToResizeV,
		ResizeColumnState,
		ResizeWindowState,
		HeaderButtonPressed,
		HeaderButtonReordering
	};

	int getGuiState() const;
	void setTitle(bool has, int height, CString text);

protected:
	void onMessagesBP();
	void onFollowInDisasm();
	void onToggleBP();
	void onShowSearch();

private:
	GuiState m_GuiState = CHandleTable::NoState;
	bool bLButtonDown = false;
	int iResizinglastPosY = 0;
	int iLButtonDownPos = 0;
	bool bHasTitle = false;
	int iTitleHeight = 0;
	CString cTitleText;

public:
	DECLARE_MESSAGE_MAP()
	
	afx_msg void OnHandle(UINT id);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnPaint();
};

