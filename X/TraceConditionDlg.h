#pragma once
#include "afxwin.h"
#include "TraceEdit.h"

// CTraceConditionDlg �Ի���

class CTraceConditionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTraceConditionDlg)

public:
	CTraceConditionDlg(CString tittle, CString cmd, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTraceConditionDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum {
		IDD = IDD_Trace_Condition_Dlg
};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();

private:
	CString tittle;
	CString traceCmd;
	CString mLogFile;

	CTraceEdit m_breakConditionEdit;
	CTraceEdit m_logTextEdit;
	CTraceEdit m_logConditionEdit;
	CTraceEdit m_cmdTextEdit;
	CTraceEdit m_cmdConditionEdit;
	CTraceEdit m_traceMaxNumEdit;
	CTraceEdit m_swithConditionEdit;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
