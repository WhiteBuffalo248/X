#pragma once
#include "afxwin.h"


// LineEditDlg �Ի���

class CLineEditDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLineEditDlg)

public:
	CLineEditDlg(CWnd* pParent = NULL, CString title = _T(""), CString editText = _T(""));   // ��׼���캯��
	virtual ~CLineEditDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LineEdit_Dialog };
#endif

public:
	CString m_editText;

public:
	void ForceSize(unsigned int size);
	void setFpuMode();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

private:
	CString title;
	unsigned int fixed_size;
	bool fpuMode;

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	CStatic CHintStatic;
	afx_msg void OnNMThemeChangedHintStatic(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangetextedit();
};
