// RegistersPane.cpp : 实现文件
//

#include "stdafx.h"
#include "X.h"
#include "RegistersPane.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// CRegistersPane

IMPLEMENT_DYNAMIC(CRegistersPane, CDockablePane)

CRegistersPane::CRegistersPane()
{

}

CRegistersPane::~CRegistersPane()
{
}


BEGIN_MESSAGE_MAP(CRegistersPane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CRegistersPane 消息处理程序




int CRegistersPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	m_wndRegistersView.Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, lpCreateStruct->cx, lpCreateStruct->cy), this, NULL);
	
	return 0;
}


void CRegistersPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	m_wndRegistersView.SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
}
