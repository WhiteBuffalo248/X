#pragma once
#include "afxwin.h"


// CWordEditDlg �Ի���

class CWordEditDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWordEditDlg)

public:
	CWordEditDlg(CString title = _T(""), duint defVal = 0, int byteCount = 0, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CWordEditDlg();

	ULONG getValue();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WordEdit_Dialog };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	virtual void PreInitDialog();
	
private:
	CMutex data;
	CString title;
	duint defVal;
	int byteCount;
public:
	CEdit hexLineEdit;
	CEdit byteLineEdit;
	CEdit signedLineEdit;
	CEdit unsignedLineEdit;
	CEdit asciiLineEdit;

	CString m_hexValue;
	CString m_byteValue;
	LONGLONG m_signedDecValue;
	ULONGLONG m_unsignedDecValue;

	afx_msg void OnEnChangeHexEdit();
	afx_msg void OnEnChangeDecEdit();
	afx_msg void OnEnChangeUdecEdit();
	afx_msg void OnEnUpdateHexEdit();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
