#include "StdAfx.h"
#include "PeInfoPane.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CPeInfoPane::CPeInfoPane(void)
{
}


CPeInfoPane::~CPeInfoPane(void)
{
}
BEGIN_MESSAGE_MAP(CPeInfoPane, CDockablePane)
	ON_WM_SIZE()
	ON_WM_CREATE()
//	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


void CPeInfoPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	/*
	if (m_List.GetSafeHwnd() == NULL)
	{
		//	Invalidate(TRUE);
		return;
	}
	CRect rect;
	GetClientRect(rect);
	m_List.SetWindowPos(NULL, rect.left, rect.top, rect.right, rect.bottom, SWP_NOACTIVATE | SWP_NOZORDER);
	*/
}


int CPeInfoPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
//	m_List.Create(WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_CLIPSIBLINGS | WS_VSCROLL | WS_BORDER/*WS_THICKFRAME*/,
//		CRect(10, 10, 200, 400), this, 0x1);
	//m_List.SetBkColor(RGB(168,168,168));
//	m_List.SetBkColor(CLR_NONE);
//	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_JUSTIFYCOLUMNS);

	return 0;
}

/*
void CPeInfoPane::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	LVHITTESTINFO ht;
	m_List.ScreenToClient(&ht.pt);
	m_List.HitTest(&ht);
	int   nItem = m_List.SubItemHitTest(&ht);
	if (nItem != -1)return ;


	CDockablePane::OnLButtonUp(nFlags, point);
}
*/