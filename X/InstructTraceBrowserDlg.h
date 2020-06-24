#pragma once
#include "afxwin.h"


// CInstructTraceBrowserDlg 对话框

class CInstructTraceBrowserDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CInstructTraceBrowserDlg)

public:
	CInstructTraceBrowserDlg(CWnd* pParent = NULL, CString title = _T(""), CString filePath = _T(""));   // 标准构造函数
	virtual ~CInstructTraceBrowserDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_InstructTrace_Browser_Dlg
	};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedInstructtracebrowserButton();
	CString m_title;
	CString m_filePath;
	virtual BOOL OnInitDialog();
};
