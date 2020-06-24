// CmdListBox.cpp : ʵ���ļ�
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



// CCmdListBox ��Ϣ�������




int CCmdListBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������
	editHwnd = (HWND)lpCreateStruct->lpCreateParams;
	//SetFont(&afxGlobalData.fontTooltip);

	return 0;
}


void CCmdListBox::OnLbnSetfocus()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
}


void CCmdListBox::OnKillFocus(CWnd* pNewWnd)
{
	CListBox::OnKillFocus(pNewWnd);

	// TODO: �ڴ˴������Ϣ����������
	
}


HBRUSH CCmdListBox::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	// TODO:  �ڴ˸��� DC ���κ�����

	// TODO:  �����Ӧ���ø����Ĵ�������򷵻ط� null ����
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
	ShowWindow(SW_HIDE);
	CCmdEdit::UninstallMouseHook();
}

