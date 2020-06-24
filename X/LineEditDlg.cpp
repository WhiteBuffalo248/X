// LineEditDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "X.h"
#include "LineEditDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// LineEditDlg 对话框

IMPLEMENT_DYNAMIC(CLineEditDlg, CDialogEx)

CLineEditDlg::CLineEditDlg(CWnd* pParent /*=NULL*/, CString title, CString editText)
	: CDialogEx(IDD_LineEdit_Dialog, pParent)
	, title(title)
	, m_editText(editText)
{
	fixed_size = 0;
}

CLineEditDlg::~CLineEditDlg()
{
}

void CLineEditDlg::ForceSize(unsigned int size)
{
	fixed_size = size;
}

void CLineEditDlg::setFpuMode()
{
	fpuMode = true;
}

void CLineEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_textEdit, m_editText);
	DDX_Control(pDX, IDC_HINT_STATIC, CHintStatic);
}


BEGIN_MESSAGE_MAP(CLineEditDlg, CDialogEx)
	ON_NOTIFY(NM_THEMECHANGED, IDC_HINT_STATIC, &CLineEditDlg::OnNMThemeChangedHintStatic)
	ON_EN_CHANGE(IDC_textEdit, &CLineEditDlg::OnEnChangetextedit)
END_MESSAGE_MAP()


// LineEditDlg 消息处理程序


BOOL CLineEditDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetWindowText(title);
	if (fixed_size)
		CHintStatic.ShowWindow(SW_SHOW);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CLineEditDlg::OnNMThemeChangedHintStatic(NMHDR *pNMHDR, LRESULT *pResult)
{
	// 该功能要求使用 Windows XP 或更高版本。
	// 符号 _WIN32_WINNT 必须 >= 0x0501。
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	
}


void CLineEditDlg::OnEnChangetextedit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString editText;
	GetDlgItemText(IDC_textEdit, editText);
	if (this->fixed_size != 0)
	{
		CString editTextLower = editText;
		editTextLower.MakeLower();

		if (editText.GetLength() != this->fixed_size && (!fpuMode || !(editText.Find('.') != -1) || editTextLower == "inf" || editTextLower == "nan" || editTextLower == "+inf" || editTextLower == "-inf"))
		{
			//TODO: QNaN & SNaN
			GetDlgItem(IDOK)->EnableWindow(FALSE);
			CString symbolct = _T("count: ");
			int ct = editText.GetLength() - (int) this->fixed_size;
			if (ct > 0)
				symbolct += "+";
			//ui->label->setText(tr("<font color='red'>CT: %1%2</font>").arg(symbolct).arg(ct));
			symbolct.AppendFormat(_T("%d"), ct);
			CHintStatic.SetWindowText(symbolct);
		}
		else
		{
			GetDlgItem(IDOK)->EnableWindow(TRUE);
			CHintStatic.SetWindowText(_T(""));
		}
	}
}
