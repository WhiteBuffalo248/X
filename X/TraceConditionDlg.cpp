// TraceConditionDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "X.h"
#include "TraceConditionDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// CTraceConditionDlg 对话框

IMPLEMENT_DYNAMIC(CTraceConditionDlg, CDialogEx)

CTraceConditionDlg::CTraceConditionDlg(CString tittle, CString cmd,	CWnd* pParent /*=NULL*/)
	: tittle(tittle), traceCmd(cmd), CDialogEx(IDD_Trace_Condition_Dlg, pParent)
{

}

CTraceConditionDlg::~CTraceConditionDlg()
{
}

void CTraceConditionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_BREAK_CONDITION, m_breakConditionEdit);
	DDX_Control(pDX, IDC_EDIT_LOG_TEXT, m_logTextEdit);
	DDX_Control(pDX, IDC_EDIT_LOG_CONDITION, m_logConditionEdit);
	DDX_Control(pDX, IDC_EDIT_CMD_TEXT, m_cmdTextEdit);
	DDX_Control(pDX, IDC_EDIT_CMD_CONDITION, m_cmdConditionEdit);
	DDX_Control(pDX, IDC_EDIT_MAXNUM, m_traceMaxNumEdit);
	DDX_Control(pDX, IDC_EDIT_SWITH_CONDITION, m_swithConditionEdit);
}


BEGIN_MESSAGE_MAP(CTraceConditionDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CTraceConditionDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CTraceConditionDlg 消息处理程序


BOOL CTraceConditionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetWindowText(tittle);
	m_breakConditionEdit.SetCueBanner(_T("Example: eax == 0 && ebx == 0"), TRUE);
	m_logTextEdit.SetCueBanner(_T("Example: 0x{p:cip} {i:cip}"), TRUE);
	m_logConditionEdit.SetCueBanner(_T("Example: eax == 0 && ebx == 0"), TRUE);
	m_cmdTextEdit.SetCueBanner(_T("Example: eax=4;StepOut"), TRUE);
	m_cmdConditionEdit.SetCueBanner(_T("Example: eax == 0 && ebx == 0"), TRUE);
	m_swithConditionEdit.SetCueBanner(_T("Example: mod.party(dis.branchdest(cip)) == 1"), TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

static CString escapeText(CString str)
{
	str.Replace(_T("\\"), _T("\\\\"));
	str.Replace(_T("\""), _T("\\\""));
	return str;
}

void CTraceConditionDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CString editText;
	GetDlgItemText(IDC_EDIT_LOG_TEXT, editText);
	if (!mLogFile.IsEmpty() && editText.IsEmpty())
		if (MessageBox(_T("It appears you have set the log file, but not the log text. <b>This will result in an empty log</b>. Do you really want to continue?"),
			_T("Trace log file"), MB_ICONWARNING | MB_YESNO) == IDYES)
			return;
	auto logText = m_logTextEdit.addHistoryClear();
	auto logCondition = m_logConditionEdit.addHistoryClear();
	CString cmd;
	cmd.Format(_T("TraceSetLog \"%s\", \"%s\""), escapeText(logText).GetBuffer(), escapeText(logCondition).GetBuffer());
	if (!DbgCmdExecDirect(CT2A(cmd, CP_UTF8)))
	{
		MessageBox(_T("Failed to set log text/condition!"), _T("Error"), MB_ICONWARNING | MB_OK);
		return;
	}
	auto commandText = m_cmdTextEdit.addHistoryClear();
	auto commandCondition = m_cmdConditionEdit.addHistoryClear();
	cmd.Format(_T("TraceSetCommand \"%s\", \"%s\""), escapeText(commandText).GetBuffer(), escapeText(commandCondition).GetBuffer());
	if (!DbgCmdExecDirect(CT2A(cmd, CP_UTF8)))
	{
		MessageBox(_T("Failed to set command text/condition!"), _T("Error"), MB_ICONWARNING | MB_OK);
		return;
	}
	auto switchCondition = m_swithConditionEdit.addHistoryClear();
	cmd.Format(_T("TraceSetSwitchCondition \"%s\""), escapeText(switchCondition).GetBuffer());
	if (!DbgCmdExecDirect(CT2A(cmd, CP_UTF8)))
	{
		MessageBox(_T("Failed to set switch condition!"), _T("Error"), MB_ICONWARNING | MB_OK);
		return;
	}
	cmd.Format(_T("TraceSetLogFile \"%s\""), escapeText(mLogFile).GetBuffer());
	if (!DbgCmdExecDirect(CT2A(cmd, CP_UTF8)))
	{
		MessageBox(_T("Failed to set log file!"), _T("Error"), MB_ICONWARNING | MB_OK);
		return;
	}
	auto breakCondition = m_breakConditionEdit.addHistoryClear();
	CString numText;
	m_traceMaxNumEdit.GetWindowText(numText);
	auto maxTraceCount = _ttol(numText);
	cmd.Format(_T("%s \"%s\", .%d"), traceCmd.GetBuffer(), escapeText(breakCondition).GetBuffer(), maxTraceCount);
	if (!DbgCmdExecDirect(CT2A(cmd, CP_UTF8)))
	{
		MessageBox(_T("Failed to start trace!"), _T("Error"), MB_ICONWARNING | MB_OK);
		return;
	}
	CDialogEx::OnOK();
}
