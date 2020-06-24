// CmdBar.cpp : 实现文件
//

#include "stdafx.h"
#include "X.h"
#include "CmdBar.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// CCmdBar

IMPLEMENT_DYNAMIC(CCmdBar, CWnd)

CCmdBar::CCmdBar()
{
	
}

CCmdBar::~CCmdBar()
{
}




BEGIN_MESSAGE_MAP(CCmdBar, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CCmdBar 消息处理程序




int CCmdBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	m_commondTittle.Create(_T("命令："), WS_CHILD | WS_VISIBLE | SS_CENTER, CRect(0, 10, 50, lpCreateStruct->cy), this);
	
	m_CmdScriptTypeComboBox.Create(WS_CHILD | WS_VISIBLE | CBS_OWNERDRAWFIXED| CBS_HASSTRINGS | CBS_DROPDOWNLIST, CRect(10, 10, 110, 50), this, 0);

	m_cmdEdit.CreateEx(NULL, _T("EDIT"), NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL , CRect(10, 10, 100, 100), this, 1, m_CmdScriptTypeComboBox.m_hWnd);

	CString strCmdPopupClass = AfxRegisterWndClass(
		CS_VREDRAW | CS_HREDRAW,
		::LoadCursor(NULL, IDC_ARROW),
		(HBRUSH) ::GetStockObject(WHITE_BRUSH),
		::LoadIcon(NULL, IDI_APPLICATION));

	return 0;
}


void CCmdBar::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	m_commondTittle.SetWindowPos(NULL, 1, 1, 50, cy - 2, NULL);
	m_cmdEdit.SetWindowPos(NULL, 60, 1, 300, cy - 2, NULL);
	m_CmdScriptTypeComboBox.SetWindowPos(NULL, 370, 1, 100, cy - 2, NULL);
	

}


void CCmdBar::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CWnd::OnPaint()
	CRect viewRect;
	GetClientRect(&viewRect);
	dc.FillSolidRect(&viewRect, RGB(200, 200, 200));
	
	dc.Draw3dRect(viewRect, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
	
}


BOOL CCmdBar::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


BOOL CCmdBar::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CWnd::OnChildNotify(message, wParam, lParam, pLResult);
}


BOOL CCmdBar::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CWnd::OnWndMsg(message, wParam, lParam, pResult);
}


BOOL CCmdBar::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CWnd::OnNotify(wParam, lParam, pResult);
}
