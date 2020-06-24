#pragma once
#include "MyHeaderCtrl.h"
using namespace Gdiplus;

// CMyListCtrl

class CMyListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CMyListCtrl)

public:
	CMyListCtrl();
	virtual ~CMyListCtrl();

private:
public:
	unsigned int LIST_ITEM_HEIGHT;
	CMyHeaderCtrl m_header;
	DWORD m_Width;
	COLORREF m_BkColor;
	COLORREF m_SelectColor;
	COLORREF m_CutLineColor;
	bool	m_bSelected;
	int		m_BreakPointItem[21]; // 1个断点数+20个断点行
	bool	m_bDrawCirclePoint;
	RECT	m_rcCirclePoint;


	

protected:
	DECLARE_MESSAGE_MAP()

protected:
	void Init();
	
public:
	void draw_row_bg(CDC *pDC, RECT rc, bool bSelected, bool bFocus,int nRow);
	void draw_row_bg(CDC *pDC, RECT rc, bool bSelected, bool bFocus, bool bOdd);
	void DrawSubItem(CDC *pDC, int nItem, int nSubItem, CRect &rSubItem, bool bSelected, bool bFocus);
	void DrawRemainSpace(LPNMLVCUSTOMDRAW lpnmcd);
	void DrawBPCircle(int);
public:
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	virtual void PreSubclassWindow();
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	afx_msg void OnSize(UINT nType, int cx, int cy);	
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndScroll(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHdnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPaint();
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};


