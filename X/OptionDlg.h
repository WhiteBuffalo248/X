#pragma once
#include "afxcmn.h"


// COptionDlg �Ի���

class COptionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(COptionDlg)

public:
	COptionDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~COptionDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum {
		IDD = IDD_Options_Dlg
};
#endif

protected:
	CDialogEx Page1;
	CDialogEx Page2;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CTreeCtrl m_TreeCtrl;
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult);
};
