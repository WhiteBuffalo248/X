// ProgressBar.cpp : 实现文件
//

#include "stdafx.h"
#include "X.h"
#include "ProgressBar.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// CProgressBar

IMPLEMENT_DYNAMIC(CProgressBar, CWnd)

CProgressBar::CProgressBar()
{

}

CProgressBar::~CProgressBar()
{
}


BEGIN_MESSAGE_MAP(CProgressBar, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CProgressBar 消息处理程序




int CProgressBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	
	m_regionProgress.Create(WS_CHILD | WS_VISIBLE | PBS_SMOOTH, CRect(10, 1, 50, lpCreateStruct->cy), this, 1);
	m_totalProgress.Create(WS_CHILD | WS_VISIBLE | PBS_SMOOTH, CRect(10 + 60, 1, 50, lpCreateStruct->cy), this, 2);
	m_itemCount.Create(_T("my static"), WS_CHILD | WS_VISIBLE | SS_CENTER, CRect(100, 5, 50, lpCreateStruct->cy), this);

	int nFirstStep = m_regionProgress.SetStep(1);

	return 0;
}


void CProgressBar::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	int iprogresWidth = (cx - staticWidth - 2 * 10) / 2;
	m_regionProgress.SetWindowPos(NULL, 0, 0, iprogresWidth, cy, SWP_NOACTIVATE | SWP_NOZORDER);
	m_totalProgress.SetWindowPos(NULL, iprogresWidth + 10, 0, iprogresWidth, cy, SWP_NOACTIVATE | SWP_NOZORDER);
	m_itemCount.SetWindowPos(NULL, (iprogresWidth + 10) * 2, 0, staticWidth, cy, SWP_NOACTIVATE | SWP_NOZORDER);
}


void CProgressBar::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CWnd::OnPaint()
	CRect viewRect;
	GetClientRect(&viewRect);

	dc.FillSolidRect(&viewRect, RGB(255, 250, 240));
	
	/*
	CDC MenDC;
	CBitmap MemMap;
	MenDC.CreateCompatibleDC(&dc);
	MemMap.CreateCompatibleBitmap(&dc, viewRect.Width(), viewRect.Height());
	MenDC.SelectObject(&MemMap);
	MenDC.FillSolidRect(&viewRect, RGB(255, 250, 240));
	dc.BitBlt(0, 0, viewRect.Width(), viewRect.Height(), &MenDC, 0, 0, SRCCOPY);
	*/
	
}
