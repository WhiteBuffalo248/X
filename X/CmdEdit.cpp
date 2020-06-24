// CmdEdit.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "X.h"
#include "CmdEdit.h"
#include "Bridge.h"
#include "CmdBar.h"
#include "windowsx.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// CCmdEdit

IMPLEMENT_DYNAMIC(CCmdEdit, CEdit)

CCmdEdit::CCmdEdit()
{

}

CCmdEdit::~CCmdEdit()
{
}

HHOOK hhookMs = NULL;
HWND editHwnd = NULL;
HWND listHwnd = NULL;

BOOL CCmdEdit::UninstallMouseHook()
{

	BOOL fOk = FALSE;
	if (hhookMs) {
		fOk = UnhookWindowsHookEx(hhookMs);
		hhookMs = NULL;
	}

	return(fOk);
}

LRESULT CALLBACK LowLevelMouseProc(INT nCode, WPARAM wParam, LPARAM lParam)
{
	MSLLHOOKSTRUCT *pkbhs = (MSLLHOOKSTRUCT *)lParam;
	TCHAR strMsg[100] = { 0 };

	switch (nCode)
	{
	case HC_ACTION:
	{
		//����ƶ�
		if (wParam == WM_MOUSEMOVE)
		{
			wsprintf(strMsg, _T("WM_MOUSEMOVE: x= %d, y= %d\n"), pkbhs->pt.x, pkbhs->pt.y);
			
			OutputDebugString(strMsg);
		}

		//������
		if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN)
		{
			wsprintf(strMsg, _T("WM_BUTTONDOWN: x= %d, y= %d\n"), pkbhs->pt.x, pkbhs->pt.y);
			OutputDebugString(strMsg);

			CRect editWindowsRect;
			::GetWindowRect(editHwnd, &editWindowsRect);

			CRect listWindowsRect;
			::GetWindowRect(listHwnd, &listWindowsRect);
			POINT currPoint = { pkbhs->pt.x, pkbhs->pt.y };
			
			if (!editWindowsRect.PtInRect(currPoint) && !listWindowsRect.PtInRect(currPoint))
			{
				::ShowWindow(listHwnd, SW_HIDE);
				CCmdEdit::UninstallMouseHook();
			}
		}

		// 			//�����¼�
		// 			if (wParam == WM_MOUSEWHEEL)
		// 			{
		// 				sprintf(strMsg, "WM_MOUSEWHEEL: %d\n", HIWORD(pkbhs->mouseData));
		// 				OutputDebugString(strMsg);
		// 			}
	}
	default:
		break;
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

BOOL CCmdEdit::InstallMouseHook()
{

	if (hhookMs)
		UninstallMouseHook();

	hhookMs = SetWindowsHookEx(WH_MOUSE_LL,
		(HOOKPROC)LowLevelMouseProc, AfxGetApp()->m_hInstance, NULL);

	return(hhookMs != NULL);

}



void CCmdEdit::showDropDown(BOOL bShowIt)
{
	if (bShowIt)
	{
		InstallMouseHook();
		m_listBox.ShowWindow(SW_SHOWNA);
	}
	else
	{
		CCmdEdit::UninstallMouseHook();
		m_listBox.ShowWindow(SW_HIDE);
	}
}

void CCmdEdit::registerScriptType(SCRIPTTYPEINFO* info)
{
	// Must be valid pointer
	if (!info)
	{
		Bridge::getBridge()->setResult(0);
		return;
	}

	// Insert
	info->id = mScriptInfo.size();
	mScriptInfo.push_back(*info);

	// Update
	CCmdBar* pCmdBar = (CCmdBar*)GetParent();	
	//int index = pCmdBar->getCComboBox()->AddString(CA2T(info->name, CP_UTF8));
	
	int index = ComboBox_AddString(h_cmdScriptTypeComboBoxHwnd, CA2T(info->name, CP_UTF8));
	if (index != LB_ERR)
		if (ComboBox_GetCurSel(h_cmdScriptTypeComboBoxHwnd) != 0)
			ComboBox_SetCurSel(h_cmdScriptTypeComboBoxHwnd, 0);	

	if (info->id == 0)
		mCurrentScriptIndex = 0;

	Bridge::getBridge()->setResult(1);
}

void CCmdEdit::unregisterScriptType(int id)
{
	// The default script type can't be unregistered
	if (id <= 0)
		return;

	CCmdBar* pCmdBar = (CCmdBar*)GetParent();
	// Loop through the vector and invalidate entry (validate id)	
	for (int i = 0; i < mScriptInfo.size(); i++)
	{
		if (mScriptInfo[i].id == id)
		{
			mScriptInfo.erase(mScriptInfo.begin() + i);
			pCmdBar->getCComboBox()->DeleteString(i);
			break;
		}
	}

	// Update selected index
	if (mCurrentScriptIndex > 0)
		mCurrentScriptIndex--;

	pCmdBar->getCComboBox()->SetCurSel(mCurrentScriptIndex);
}

void CCmdEdit::cmdExecute(CString cmd)
{
	if (cmd.IsEmpty())
		return;
	GUISCRIPTEXECUTE exec = mScriptInfo[mCurrentScriptIndex].execute;


	if (exec)
	{
		// Send this string directly to the user
		exec(CT2A(cmd, CP_UTF8));
	}
}

void CCmdEdit::autoCompleteAddCmd(const CString cmd)
{
	CString cmdtext = cmd;
	CString resToken;
	int curPos = 0;
	resToken = cmdtext.Tokenize(_T(","), curPos);
	while (resToken != _T(""))
	{
		CString finalCmd = resToken.Trim();
		m_cmdArray.Add(finalCmd);
		//m_listBox.AddString(finalCmd);		
		resToken = cmdtext.Tokenize(_T(","), curPos);
	}
}

void CCmdEdit::addArrayToList(CStringArray& array)
{
	for (int i = 0; i < array.GetCount(); i++)
		m_listBox.AddString(array[i]);
}

void CCmdEdit::removeList()
{
	/*
	for (int i = 0; i < m_listBox.GetCount(); i++)
		m_listBox.DeleteString(i);
	m_listBox.UpdateData();
	*/
	m_listBox.ResetContent();
	
}

void CCmdEdit::setPopupPos()
{
	CRect windowRect;
	GetWindowRect(&windowRect);
	m_listBox.SetWindowPos(NULL, windowRect.left, windowRect.bottom, windowRect.Width(), 200, SWP_NOACTIVATE | SWP_NOREDRAW);
}


BEGIN_MESSAGE_MAP(CCmdEdit, CEdit)
	ON_CONTROL_REFLECT(EN_CHANGE, &CCmdEdit::OnEnChange)
	ON_WM_CREATE()
	ON_CONTROL_REFLECT(EN_SETFOCUS, &CCmdEdit::OnEnSetfocus)
	ON_CONTROL_REFLECT(EN_KILLFOCUS, &CCmdEdit::OnEnKillfocus)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_KEYDOWN()
	ON_WM_HOTKEY()
	ON_WM_CHAR()
	ON_WM_SYSCHAR()
	ON_WM_MOUSEACTIVATE()
	ON_WM_ACTIVATEAPP()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CCmdEdit ��Ϣ�������




void CCmdEdit::OnEnChange()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CEdit::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString culText;
	GetWindowText(culText);
	if (culText.IsEmpty())
	{
		removeList();
		addArrayToList(m_cmdArray);
	}
	else
	{
		CStringArray filterArray;
		for (int i = 0; i < m_cmdArray.GetCount(); i++)
		{
			CString sourceText = m_cmdArray[i];
			CString findText = culText;
			sourceText.MakeLower();
			findText.MakeLower();
			if (sourceText.Find(findText) == 0)
			{
				filterArray.Add(m_cmdArray[i]);
			}				
		}
		removeList();
		addArrayToList(filterArray);
	}
	
	if (m_listBox.GetCount())
	{
		if (!m_listBox.IsWindowVisible())
			showDropDown(TRUE);
	}
	else
		showDropDown(FALSE);
}


int CCmdEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

	//RegisterHotKey(m_hWnd, 1, NULL, VK_RETURN);

	// TODO:  �ڴ������ר�õĴ�������

	LOGFONT defaultfont;
	memset(&defaultfont, 0, sizeof(defaultfont));
	defaultfont.lfWeight = FW_MEDIUM;
	_tcscpy_s(defaultfont.lfFaceName, LF_FACESIZE, _T("Rockwell")); //Rockwell
	defaultfont.lfHeight = MulDiv(10, 96, 72);;

	CFont font;
	VERIFY(font.CreateFontIndirect(&defaultfont));

	SetFont(&font);

	CString strCmdPopupClass = AfxRegisterWndClass(
		CS_VREDRAW | CS_HREDRAW,
		::LoadCursor(NULL, IDC_ARROW),
		(HBRUSH) ::GetStockObject(WHITE_BRUSH),
		::LoadIcon(NULL, IDI_APPLICATION));
	m_listBox.CreateEx(WS_EX_TRANSPARENT, _T("LISTBOX"), NULL, WS_HSCROLL | WS_VSCROLL | WS_POPUPWINDOW | LBS_NOTIFY | LBS_SORT, CRect(300, 300, 200, 200), AfxGetMainWnd(), 0, m_hWnd);
	
	h_cmdScriptTypeComboBoxHwnd = (HWND)lpCreateStruct->lpCreateParams;
	editHwnd = m_hWnd;
	listHwnd = m_listBox.GetSafeHwnd();
	int count = GetLimitText();
	return 0;
}


void CCmdEdit::OnEnSetfocus()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString text;
	GetWindowText(text);
	if (!m_listBox.IsWindowVisible() && text.IsEmpty())
	{
		removeList();
		addArrayToList(m_cmdArray);
		showDropDown();
	}
		
}


void CCmdEdit::OnEnKillfocus()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CWnd* foucsWnd = GetFocus();
	if (foucsWnd->GetSafeHwnd() != m_listBox.GetSafeHwnd())
		showDropDown(FALSE);
	
}


HBRUSH CCmdEdit::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	// TODO:  �ڴ˸��� DC ���κ�����

	// TODO:  �����Ӧ���ø����Ĵ�������򷵻ط� null ����
	/*
	LOGFONT defaultfont;
	memset(&defaultfont, 0, sizeof(defaultfont));
	defaultfont.lfWeight = FW_MEDIUM;
	_tcscpy_s(defaultfont.lfFaceName, LF_FACESIZE, _T("Lucida Console")); //Rockwell
	defaultfont.lfHeight = MulDiv(10, 96, 72);;

	CFont font;
	VERIFY(font.CreateFontIndirect(&defaultfont));
	pDC->SelectObject(&font);
	*/	

	return NULL;
}


void CCmdEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CCmdEdit::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	

	CEdit::OnHotKey(nHotKeyId, nKey1, nKey2);
}


void CCmdEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CEdit::OnChar(nChar, nRepCnt, nFlags);
}


void CCmdEdit::OnSysChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CEdit::OnSysChar(nChar, nRepCnt, nFlags);
}


BOOL CCmdEdit::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		CString cmd;
		GetWindowText(cmd);
		if (!cmd.IsEmpty())
			cmdExecute(cmd);
		return TRUE;
	}

	return CEdit::PreTranslateMessage(pMsg);
}


int CCmdEdit::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	return CEdit::OnMouseActivate(pDesktopWnd, nHitTest, message);
}


void CCmdEdit::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
	CEdit::OnActivateApp(bActive, dwThreadID);

	// TODO: �ڴ˴������Ϣ����������
}


void CCmdEdit::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
		
	CEdit::OnLButtonDown(nFlags, point);
}
