#pragma once


// CCloseDlg �Ի���

class CCloseDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCloseDlg)

public:
	CCloseDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCloseDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_Close_Dlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
