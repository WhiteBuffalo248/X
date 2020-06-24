#pragma once
#include "afxwin.h"
#include <map>


// CMessagesBPDlg �Ի���

class CMessagesBPDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMessagesBPDlg)

public:
	struct MsgBreakpointData
	{
		CString procVA;
		CString wndHandle;
	};

	CMessagesBPDlg(MsgBreakpointData pbpData, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMessagesBPDlg();

	MsgBreakpointData bpData;
	CComboBox m_messageComBox;
	BOOL m_bBPType;
	
	BOOL m_bUseTM;

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_Message_BreakPoint_Dlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

private:
	CStringArray filterMessages;
	std::map<duint, CString> messages;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	
	virtual BOOL OnInitDialog();
	
};
