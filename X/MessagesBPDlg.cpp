// MessagesBPDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "X.h"
#include "MessagesBPDlg.h"
#include "afxdialogex.h"
#include "Imports.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// CMessagesBPDlg 对话框

IMPLEMENT_DYNAMIC(CMessagesBPDlg, CDialogEx)

CMessagesBPDlg::CMessagesBPDlg(MsgBreakpointData pbpData, CWnd* pParent /*=NULL*/)
	:CDialogEx(IDD_Message_BreakPoint_Dlg, pParent), bpData(pbpData), m_bBPType(0), m_bUseTM(0)
{

}

CMessagesBPDlg::~CMessagesBPDlg()
{
}

void CMessagesBPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_Message_COMBOX, m_messageComBox);
	DDX_Radio(pDX, IDC_BP_AnyWindow_RADIO, m_bBPType);
	DDX_Check(pDX, IDC_UseTM_CHECK, m_bUseTM);
}


BEGIN_MESSAGE_MAP(CMessagesBPDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CMessagesBPDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CMessagesBPDlg 消息处理程序


void CMessagesBPDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	duint procVA;
	duint wndHandle;
	CString bpCondCmd;

	if (!DbgFunctions()->ValFromString(CT2A(bpData.wndHandle, CP_UTF8), &wndHandle) ||
		!DbgFunctions()->ValFromString(CT2A(bpData.procVA, CP_UTF8), &procVA))
		return;

	if (!DbgMemIsValidReadPtr(procVA) || !IsWindow((HWND)wndHandle))
		return;

	CString title;
	m_messageComBox.GetLBText(m_messageComBox.GetCurSel(), title);
	duint key;
	for (auto iterator : messages)
		if (iterator.second == title)
		{
			key = iterator.first;
			break;
		}
			

	if (!(((CButton *)GetDlgItem(IDC_UseTM_CHECK))->GetCheck()))
	{
		BPXTYPE wBpType = DbgGetBpxTypeAt(procVA);
		CString cmd;
		cmd.Format(_T("bp 0x%s"), bpData.procVA);
		if (wBpType == bp_none)
			DbgCmdExec(CT2A(cmd, CP_UTF8));		

		bpCondCmd.Format(_T("bpcnd 0x%s, \"arg.get(1) == 0x%x"), bpData.procVA, key);
		CString additionText;
		if (((CButton *)GetDlgItem(IDC_BP_CurrentWindow_RADIO))->GetCheck())
			additionText.Format(_T(" && arg.get(0) == 0x%s\""), bpData.wndHandle);
		else
			additionText = _T("\"");
		bpCondCmd += additionText;
	}
	else
	{
		BPXTYPE wBpType = DbgGetBpxTypeAt(DbgValFromString("TranslateMessage"));
		if (wBpType == bp_none)
			DbgCmdExec("bp TranslateMessage");

		CString additionText;

#ifdef _WIN64
		bpCondCmd.Format(_T("bpcnd TranslateMessage, \"4:[arg.get(0)+8] == 0x%x"), key);
		if (((CButton *)GetDlgItem(IDC_BP_CurrentWindow_RADIO))->GetCheck())
			additionText.Format(_T(" && 4:[arg.get(0)] == 0x%s\""), bpData.wndHandle);
		else
			additionText = _T("\"");
		bpCondCmd += additionText;
#else //x86
		bpCondCmd.Format(_T("bpcnd TranslateMessage, \"[arg.get(0)+4] == 0x%x"), key);
		if (((CButton *)GetDlgItem(IDC_BP_CurrentWindow_RADIO))->GetCheck())
			additionText.Format(_T(" && [arg.get(0)] == 0x%s\""), bpData.wndHandle);
		else
			additionText = _T("\"");
		bpCondCmd += additionText;
#endif //_WIN64
	}

	DbgCmdExec(CT2A(bpCondCmd, CP_UTF8));

	CDialogEx::OnOK();
}


BOOL CMessagesBPDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	BridgeList<CONSTANTINFO> constants;
	DbgFunctions()->EnumConstants(&constants);

	filterMessages.Add(_T("ACM_"));
	filterMessages.Add(_T("BFFM_"));
	filterMessages.Add(_T("BM_"));
	filterMessages.Add(_T("CB_"));
	filterMessages.Add(_T("CBEM_"));
	filterMessages.Add(_T("CCM_"));
	filterMessages.Add(_T("CDM_"));
	filterMessages.Add(_T("CTL3D_"));
	filterMessages.Add(_T("DL_"));
	filterMessages.Add(_T("DM_"));
	filterMessages.Add(_T("DTM_"));
	filterMessages.Add(_T("EM_"));
	filterMessages.Add(_T("HDM_"));
	filterMessages.Add(_T("HKM_"));
	filterMessages.Add(_T("IE_"));
	filterMessages.Add(_T("IPM_"));
	filterMessages.Add(_T("LB_"));
	filterMessages.Add(_T("LVM_"));
	filterMessages.Add(_T("MCIWNDM_"));
	filterMessages.Add(_T("MCM_"));
	filterMessages.Add(_T("MN_"));
	filterMessages.Add(_T("MSG_"));
	filterMessages.Add(_T("NIN_"));
	filterMessages.Add(_T("OCM_"));
	filterMessages.Add(_T("PBM_"));
	filterMessages.Add(_T("PGM_"));
	filterMessages.Add(_T("PSM_"));
	filterMessages.Add(_T("RB_"));
	filterMessages.Add(_T("SB_"));
	filterMessages.Add(_T("SBM_"));
	filterMessages.Add(_T("SM_"));
	filterMessages.Add(_T("STM_"));
	filterMessages.Add(_T("TAPI_"));
	filterMessages.Add(_T("TB_"));
	filterMessages.Add(_T("TBM_"));
	filterMessages.Add(_T("TCM_"));
	filterMessages.Add(_T("TTM_"));
	filterMessages.Add(_T("TV_"));
	filterMessages.Add(_T("TVM_"));
	filterMessages.Add(_T("UDM_"));
	filterMessages.Add(_T("UM_"));
	filterMessages.Add(_T("WIZ_"));
	filterMessages.Add(_T("WLX_"));
	filterMessages.Add(_T("WM_"));
	
	for (int i = 0; i < constants.Count(); i++)
	{
		for (int j = 0; j < filterMessages.GetSize(); j++)
		{
			CString constantTitle = CA2T(constants[i].name, CP_UTF8);
			if (constantTitle.Find(filterMessages[j]) == 0)
			{
				messages.insert(std::pair<duint, CString>(constants[i].value, constantTitle));
				m_messageComBox.AddString(constantTitle);
				break;
			}				
		}
	}
	int index = 0;
	index = m_messageComBox.SelectString(0, _T("WM_COMMAND"));
	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
