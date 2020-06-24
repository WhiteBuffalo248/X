// CmdScriptTypeComboBox.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "X.h"
#include "CmdScriptTypeComboBox.h"
#include "windowsx.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// CCmdScriptTypeComboBox

IMPLEMENT_DYNAMIC(CCmdScriptTypeComboBox, CComboBox)

CCmdScriptTypeComboBox::CCmdScriptTypeComboBox()
{
	ITEM_SELECT_BK_COLOR = 0xeecc00;
	ITEM_SELECT_TEXT_COLOR = 0xeeeeee;
}

CCmdScriptTypeComboBox::~CCmdScriptTypeComboBox()
{
}

WNDPROC CCmdScriptTypeComboBox::lpfnListDefaultWndProc = nullptr;
WNDPROC CCmdScriptTypeComboBox::lpfnEditDefaultWndProc = nullptr;
HWND CCmdScriptTypeComboBox::comboxHwnd = NULL;

LRESULT CALLBACK HookListboxWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{

	}
	return 0;

	case WM_LBUTTONUP:
	{
		int index = ListBox_GetCurSel(hWnd);
		ComboBox_SetCurSel(CCmdScriptTypeComboBox::comboxHwnd, index);
		/*
		CString itemText;
		ListBox_GetText(hWnd, index, itemText);
		Edit_SetText(CCmdScriptTypeComboBox::comboxHwnd, itemText);
		itemText.ReleaseBuffer();
		*/		
		ComboBox_ShowDropdown(CCmdScriptTypeComboBox::comboxHwnd, FALSE);
		
	}
	return 0;

	case LBN_DBLCLK:
		return 0;
	default:
		//�����б��֮ǰ��Ĭ����Ϣ������
		return ::CallWindowProc(CCmdScriptTypeComboBox::lpfnListDefaultWndProc, hWnd, message, wParam, lParam);
	}
}

LRESULT CALLBACK HookEditBoxWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//TRACE("HookEditBoxWndProc, Msg: %d\n", message);
	switch (message)
	{
	case WM_SETFOCUS:
		return ::CallWindowProc(CCmdScriptTypeComboBox::lpfnEditDefaultWndProc, hWnd, message, wParam, lParam);
	
	default:
		return ::CallWindowProc(CCmdScriptTypeComboBox::lpfnEditDefaultWndProc, hWnd, message, wParam, lParam);
	}	
}


BEGIN_MESSAGE_MAP(CCmdScriptTypeComboBox, CComboBox)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_CONTROL_REFLECT(CBN_DROPDOWN, &CCmdScriptTypeComboBox::OnCbnDropdown)
END_MESSAGE_MAP()



// CCmdScriptTypeComboBox ��Ϣ�������




void CCmdScriptTypeComboBox::OnSize(UINT nType, int cx, int cy)
{
	CComboBox::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������
}


int CCmdScriptTypeComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;	

	// TODO:  �ڴ������ר�õĴ�������
	COMBOBOXINFO cbi;
	cbi.cbSize = sizeof(COMBOBOXINFO);
	::GetComboBoxInfo(this->m_hWnd, &cbi);  //��ȡ��Ͽ���Ϣ

	comboxHwnd = cbi.hwndCombo;

	LONG_PTR editStyle = GetWindowLongPtr(cbi.hwndItem, GWL_STYLE);

	int newEditStyle = editStyle | ES_SAVESEL;

	SetWindowLongPtr(cbi.hwndItem, GWL_STYLE, newEditStyle);

	editStyle = GetWindowLongPtr(cbi.hwndItem, GWL_STYLE);

	LONG_PTR style = GetWindowLongPtr(cbi.hwndCombo, GWL_STYLE);

	if ((style & CBS_SIMPLE) == CBS_SIMPLE)
	{
	}
	else if ((style & CBS_DROPDOWNLIST) == CBS_DROPDOWNLIST)
	{
	}
	else if ((style & CBS_DROPDOWN) == CBS_DROPDOWN)
	{
		lpfnListDefaultWndProc = (WNDPROC)::SetWindowLongPtr(cbi.hwndList, GWLP_WNDPROC, reinterpret_cast<LONG_PTR> (HookListboxWndProc));
		CEdit* pEdit = (CEdit*)GetWindow(GW_CHILD);
		if (pEdit)
		{
			lpfnEditDefaultWndProc = (WNDPROC)::SetWindowLongPtr(pEdit->GetSafeHwnd(), GWLP_WNDPROC, reinterpret_cast<LONG_PTR> (HookEditBoxWndProc));
		}			
	}

	SetItemHeight(-1, 14);

	LOGFONT defaultfont;
	memset(&defaultfont, 0, sizeof(defaultfont));
	defaultfont.lfWeight = FW_MEDIUM;

	_tcscpy_s(defaultfont.lfFaceName, LF_FACESIZE, _T("΢���ź�")); //
	defaultfont.lfHeight = MulDiv(70, 96, 72);;

	CFont font;
	ASSERT(font.CreatePointFontIndirect(&defaultfont));
	SetFont(&font);

	return 0;
}

void CCmdScriptTypeComboBox::OnCbnDropdown()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CCmdScriptTypeComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{

	// TODO:  ������Ĵ�����ȷ��ָ����Ĵ�С
	if (lpMeasureItemStruct->itemID == -1)	
		lpMeasureItemStruct->itemHeight = 50;
	else
	{
		lpMeasureItemStruct->itemHeight = 20;
	}
}

void CCmdScriptTypeComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{

	// TODO:  ������Ĵ����Ի���ָ����
	
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);//��Ͽ�DC
	CRect itemRect(lpDrawItemStruct->rcItem);//������
	int nState = lpDrawItemStruct->itemState;//��״̬
	int nIndex = lpDrawItemStruct->itemID;//������
	int nAction = lpDrawItemStruct->itemAction;

	LOGFONT defaultfont;
	memset(&defaultfont, 0, sizeof(defaultfont));
	defaultfont.lfWeight = FW_MEDIUM;

	_tcscpy_s(defaultfont.lfFaceName, LF_FACESIZE, _T("΢���ź�")); //
	defaultfont.lfHeight = MulDiv(70, 96, 72);;

	CFont font;
	ASSERT(font.CreatePointFontIndirect(&defaultfont));
	dc.SelectObject(&font);

	TRACE("Index: %d(DelBtn), state: %04X, Action: %04X\n", nIndex, nState, nAction);

	if (nIndex != -1)
	{

		if (nState & ODS_SELECTED)//���ѡ�и���
		{
			dc.FillSolidRect(&itemRect, ITEM_SELECT_BK_COLOR);
			dc.SetTextColor(ITEM_SELECT_TEXT_COLOR);
		}
		else
		{
			dc.SetTextColor(RGB(200, 0, 0));
			dc.FillSolidRect(itemRect, RGB(229, 241, 251));
		}
		dc.SetBkMode(TRANSPARENT);
		if (nState == (ES_WANTRETURN | EN_CHANGE)) // 0x1300
		{
			
		}
		CString str;
		GetLBText(nIndex, str);
		itemRect.DeflateRect(2, 0, 0, 0);

		dc.DrawText(str, itemRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);//��ʾ�ı�
	}

	dc.Detach();
	
}
