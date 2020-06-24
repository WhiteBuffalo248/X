// MyTabCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "X.h"
#include "MyTabCtrl.h"
#include "ReferencesPane.h"
#include "GlobalID.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// CMyTabCtrl

IMPLEMENT_DYNAMIC(CMyTabCtrl, CMFCTabCtrl)

CMyTabCtrl::CMyTabCtrl()
{
	
}

CMyTabCtrl::~CMyTabCtrl()
{
	
}

void CMyTabCtrl::onDeleteTab()
{
	int nIndex = GetActiveTab();
	
	if (nIndex >= 0)
	{
		RemoveTab(nIndex);
		CReferencesPane* pParent = (CReferencesPane*)GetParent();
		pParent->closeTab(nIndex);
	}		
}

void CMyTabCtrl::onTabAllClose()
{	
	RemoveAllTabs();
	CReferencesPane* pParent = (CReferencesPane*)GetParent();
	pParent->closeAllTab();
}


BEGIN_MESSAGE_MAP(CMyTabCtrl, CMFCTabCtrl)
	ON_WM_MOVE()
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_MyTabCtrl_AllClose, onTabAllClose)
END_MESSAGE_MAP()



// CMyTabCtrl 消息处理程序




void CMyTabCtrl::OnMove(int x, int y)
{
	CMFCTabCtrl::OnMove(x, y);

	// TODO: 在此处添加消息处理程序代码
}


int CMyTabCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMFCTabCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	CArray<COLORREF, COLORREF> arColors;
	arColors.Add(RGB(121, 210, 231));
	arColors.Add(RGB(135, 206, 250));
	arColors.Add(RGB(176, 196, 222));
	EnableAutoColor(TRUE);
	SetAutoColors(arColors);

	EnableTabSwap(TRUE);
	SetActiveTabBoldFont(TRUE);
	SetTabBorderSize(0);
	HideNoTabs(TRUE);
	EnableActiveTabCloseButton(FALSE);

	return 0;
}


BOOL CMyTabCtrl::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	if ((HWND)lParam == m_btnClose.GetSafeHwnd())
	{
		if (GetParent() != nullptr)
		{
			onDeleteTab();
		}

		return TRUE;
	}

	return CMFCTabCtrl::OnCommand(wParam, lParam);
}


void CMyTabCtrl::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: 在此处添加消息处理程序代码
	CPoint pt(point);
	ScreenToClient(&pt);
	int iTab = GetTabFromPoint(pt);
	if (iTab < 0)
	{
		return;
	}

	SetActiveTab(iTab);

	CMenu menu;
	menu.CreatePopupMenu();		// 创建弹出菜单
	menu.AppendMenu(MF_STRING, ID_MyTabCtrl_AllClose, _T("全部关闭"));

	ClientToScreen(&pt);
	menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);
	menu.DestroyMenu();

}
