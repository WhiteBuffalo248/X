#pragma once
#include "afxwin.h"


// CInstructTraceBrowserDlg �Ի���

class CInstructTraceBrowserDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CInstructTraceBrowserDlg)

public:
	CInstructTraceBrowserDlg(CWnd* pParent = NULL, CString title = _T(""), CString filePath = _T(""));   // ��׼���캯��
	virtual ~CInstructTraceBrowserDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_InstructTrace_Browser_Dlg
	};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedInstructtracebrowserButton();
	CString m_title;
	CString m_filePath;
	virtual BOOL OnInitDialog();
};
