// CmdListBox.cpp : 实现文件
//

#include "stdafx.h"
#include "X.h"
#include "CmdListBox.h"
#include "CmdEdit.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// CCmdListBox

IMPLEMENT_DYNAMIC(CCmdListBox, CListBox)

CCmdListBox::CCmdListBox()
{
}

CCmdListBox::~CCmdListBox()
{
}


BEGIN_MESSAGE_MAP(CCmdListBox, CListBox)
	ON_WM_CREATE()
	ON_CONTROL_REFLECT(LBN_SETFOCUS, &CCmdListBox::OnLbnSetfocus)
	ON_WM_KILLFOCUS()
	ON_WM_CTLCOLOR_REFLECT()
	ON_CONTROL_REFLECT(LBN_DBLCLK, &CCmdListBox::OnLbnDblclk)
	ON_CONTROL_REFLECT(LBN_KILLFOCUS, &CCmdListBox::OnLbnKillfocus)
END_MESSAGE_MAP()



// CCmdListBox 消息处理程序




int CCmdListBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	editHwnd = (HWND)lpCreateStruct->lpCreateParams;
	//SetFont(&afxGlobalData.fontTooltip);

	return 0;
}


void CCmdListBox::OnLbnSetfocus()
{
	// TODO: 在此添加控件通知处理程序代码
	
}


void CCmdListBox::OnKillFocus(CWnd* pNewWnd)
{
	CListBox::OnKillFocus(pNewWnd);

	// TODO: 在此处添加消息处理程序代码
	
}


HBRUSH CCmdListBox::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	// TODO:  在此更改 DC 的任何特性

	// TODO:  如果不应调用父级的处理程序，则返回非 null 画笔
	LOGFONT defaultfont;
	memset(&defaultfont, 0, sizeof(defaultfont));
	defaultfont.lfWeight = FW_NORMAL;
	_tcscpy_s(defaultfont.lfFaceName, LF_FACESIZE, _T("Rockwell"));
	defaultfont.lfHeight = MulDiv(10, 96, 72);;

	CFont font;
	VERIFY(font.CreateFontIndirect(&defaultfont));
	pDC->SelectObject(&font);

	return NULL;
}


void CCmdListBox::OnLbnDblclk()
{
	// TODO: 在此添加控件通知处理程序代码
	int index = GetCurSel();
	
	if (index != LB_ERR)
	{
		CString itemText;
		GetText(index, itemText);
		::SetWindowText(editHwnd, itemText);
		ShowWindow(SW_HIDE);
		CCmdEdit::UninstallMouseHook();
	}
		
}


void CCmdListBox::OnLbnKillfocus()
{
	// TODO: 在此添加控件通知处理程序代码
	
	ShowWindow(SW_HIDE);
	CCmdEdit::UninstallMouseHook();
}

