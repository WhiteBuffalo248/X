// MyStatuBar.cpp : 实现文件
//

#include "stdafx.h"
#include "X.h"
#include "MyStatuBar.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
//-----------------
// Statusbar panes:
//-----------------

const int nStatusOutput = 0;
// CMyStatuBar

static UINT indicators[] =
{
	IDS_INDICATOR_OUTPUTINFO,
	ID_SEPARATOR,           // 状态行指示器
	
	
	IDS_X_BRIEF,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

IMPLEMENT_DYNAMIC(CMyStatuBar, CMFCStatusBar)

CMyStatuBar::CMyStatuBar()
{
#ifndef _WIN32_WCE
	EnableActiveAccessibility();
#endif

}

CMyStatuBar::~CMyStatuBar()
{
}


BEGIN_MESSAGE_MAP(CMyStatuBar, CMFCStatusBar)
	ON_WM_CREATE()	
	ON_WM_DRAWITEM()
END_MESSAGE_MAP()

// CMyStatuBar 消息处理程序

int CMyStatuBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMFCStatusBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));
	SetPaneStyle(nStatusOutput, SBPS_NOBORDERS | SBPS_STRETCH);
	
	SetPaneWidth(1, 200);
	return 0;
}

LRESULT CMyStatuBar::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	UINT state = message - WM_USER;
	int index = (int)wParam;
	CString text;

	//COLORREF 0x00FFFF与RGB(255, 255, 0)对应
	COLORREF textColor;
	COLORREF textBackgroundColor;
	switch (state)
	{
	case initialized:
		text = _T("initialized");
		textColor = 0x00DD00;
		textBackgroundColor = 0xC0C0C0;
		break;
	case paused:
		text = _T("paused");
		textColor = 0x0000FF;
		textBackgroundColor = 0x00FFFF;
		break;
	case running:
		text = _T("running");
		textColor = 0x000000;
		textBackgroundColor = 0x00FF00;
		break;
	case stopped:
		text = _T("stopped");
		textColor = 0x0000FF;
		textBackgroundColor = 0xC0C0C0;
		break;
	case SB_SETTEXT:
		text = (LPCTSTR)lParam;
		textColor = 0x000000;
		textBackgroundColor = 0xCCFFFF;// 0xC8FAFA;
		break;
	default:
		return CMFCStatusBar::WindowProc(message, wParam, lParam);
		break;
	}
	
	SetPaneBackgroundColor(index, textBackgroundColor);
	SetPaneTextColor(index, textColor);
	SetPaneText(index, text);
	return 0;
}



void CMyStatuBar::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CMFCStatusBar::OnDrawItem(nIDCtl, lpDrawItemStruct);
}
