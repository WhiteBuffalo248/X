// WordEditDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "X.h"
#include "WordEditDlg.h"
#include "afxdialogex.h"
#include "StringUtil.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// CWordEditDlg 对话框

IMPLEMENT_DYNAMIC(CWordEditDlg, CDialogEx)

CWordEditDlg::CWordEditDlg(CString title, duint defVal, int byteCount, CWnd* pParent /*=NULL*/) 
	: title(title), defVal(defVal), byteCount(byteCount), CDialogEx(IDD_WordEdit_Dialog, pParent)	
	, m_hexValue(_T(""))
	, m_byteValue(_T(""))
	, m_signedDecValue(0)
	, m_unsignedDecValue(0)
{

}

ULONG CWordEditDlg::getValue()
{
	std::string s;
	s = CT2A(m_hexValue);
	return strtoul(s.data(), NULL, 16);
}

CWordEditDlg::~CWordEditDlg()
{
}

void CWordEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	
	DDX_Control(pDX, IDC_HEX_EDIT, hexLineEdit);
	DDX_Control(pDX, IDC_BYTE_EDIT, byteLineEdit);
	DDX_Control(pDX, IDC_DEC_EDIT, signedLineEdit);
	DDX_Control(pDX, IDC_UDEC_EDIT, unsignedLineEdit);
	DDX_Control(pDX, IDC_ASCII_EDIT, signedLineEdit);
	
	DDX_Text(pDX, IDC_HEX_EDIT, m_hexValue);
	DDV_MaxChars(pDX, m_hexValue, byteCount * 2);
	DDX_Text(pDX, IDC_BYTE_EDIT, m_byteValue);
	DDV_MaxChars(pDX, m_byteValue, byteCount * 2);
	DDX_Text(pDX, IDC_DEC_EDIT, m_signedDecValue);
	DDX_Text(pDX, IDC_UDEC_EDIT, m_unsignedDecValue);
	
	
	//DDV_MinMaxUInt(pDX, m_unsignedDecValue, 1, 1111111111);
	
}


BEGIN_MESSAGE_MAP(CWordEditDlg, CDialogEx)
	ON_EN_CHANGE(IDC_HEX_EDIT, &CWordEditDlg::OnEnChangeHexEdit)
	ON_EN_CHANGE(IDC_DEC_EDIT, &CWordEditDlg::OnEnChangeDecEdit)
	ON_EN_CHANGE(IDC_UDEC_EDIT, &CWordEditDlg::OnEnChangeUdecEdit)
	ON_EN_UPDATE(IDC_HEX_EDIT, &CWordEditDlg::OnEnUpdateHexEdit)
	ON_BN_CLICKED(IDOK, &CWordEditDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CWordEditDlg 消息处理程序


void CWordEditDlg::PreInitDialog()
{
	// TODO: 在此添加专用代码和/或调用基类
	
	SetWindowText(title);
	
	CString expressionFormat = _T("%0");
	expressionFormat.AppendFormat(_T("%dX"), byteCount * 2);
	
	m_hexValue.Format(expressionFormat, defVal);
	m_signedDecValue = defVal;
	m_unsignedDecValue = defVal;
	
	CDialogEx::PreInitDialog();
}


void CWordEditDlg::OnEnChangeHexEdit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	/*
	CString hexValue;
	GetDlgItemText(IDC_HEX_EDIT, hexValue);
	std::string s;
	s = CT2A(m_hexValue);
	int oldHexValue = strtol(s.data(), NULL, 16);
	s = CT2A(hexValue);
	int newHexValue = strtol(s.data(), NULL, 16);

	if (oldHexValue != newHexValue)
	{
	UpdateData();
	m_signedDecValue = _ttol(m_hexValue);
	std::string s;
	s = CT2A(m_hexValue);
	m_signedDecValue = strtol(s.data(), NULL, 16);

	m_unsignedDecValue = m_signedDecValue;
	int i = signedLineEdit.GetSel();
	SetDlgItemInt(IDC_DEC_EDIT, m_signedDecValue);
	signedLineEdit.SetSel(i);
	int ui = unsignedLineEdit.GetSel();
	SetDlgItemInt(IDC_UDEC_EDIT, m_unsignedDecValue);
	signedLineEdit.SetSel(ui);
	}
	*/
	CString hexValue;
	GetDlgItemText(IDC_HEX_EDIT, hexValue);
	std::string s;
	s = CT2A(hexValue);
	duint value;
	bool validExpression = DbgFunctions()->ValFromString(s.data(), &value);
	if (validExpression)
	{
		UpdateData();
		m_signedDecValue = value;
		m_unsignedDecValue = value;
		GetDlgItem(IDOK)->EnableWindow(TRUE);
		UpdateData(FALSE);
	}
	else
	{
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}	
}


void CWordEditDlg::OnEnChangeDecEdit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString preSignedDecValue;
	GetDlgItemText(IDC_DEC_EDIT, preSignedDecValue);
	UpdateData();
	CString signedDecValue;
	signedDecValue.Format(_T("%d"), m_signedDecValue);
	if (preSignedDecValue == signedDecValue)
	{
		CString expressionFormat = _T("%0");
		expressionFormat.AppendFormat(_T("%dX"), byteCount * 2);
		m_hexValue.Format(expressionFormat, m_signedDecValue);
		m_unsignedDecValue = m_signedDecValue;
		GetDlgItem(IDOK)->EnableWindow(TRUE);
		UpdateData(FALSE);
	}
	else
	{
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}
	
}


void CWordEditDlg::OnEnChangeUdecEdit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	/*
	UpdateData();
	std::string s;
	s = CT2A(m_unsignedDecValue);
	LONGLONG value;
	if (sscanf_s(s.data(), "%lld", &value) == 1)
	{
		CString expressionFormat = _T("%0");
		expressionFormat.AppendFormat(_T("%dX"), byteCount * 2);
		m_hexValue.Format(expressionFormat, value);
		m_unsignedDecValue.Format(_T("%llu"), value);
		UpdateData(FALSE);
		GetDlgItem(IDOK)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}
	*/
	CString expressionValue;
	GetDlgItemText(IDC_UDEC_EDIT, expressionValue);
	UpdateData();
	CString unSignedDecValue;
	unSignedDecValue.Format(_T("%d"), m_unsignedDecValue);
	if (expressionValue == unSignedDecValue)
	{
		CString expressionFormat = _T("%0");
		expressionFormat.AppendFormat(_T("%dX"), byteCount * 2);
		m_hexValue.Format(expressionFormat, m_unsignedDecValue);
		m_signedDecValue = m_unsignedDecValue;
		GetDlgItem(IDOK)->EnableWindow(TRUE);
		UpdateData(FALSE);
	}
	else
	{
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}
}


void CWordEditDlg::OnEnUpdateHexEdit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数，以将 EM_SETEVENTMASK 消息发送到该控件，
	// 同时将 ENM_UPDATE 标志“或”运算到 lParam 掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


BOOL CWordEditDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CWordEditDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}
