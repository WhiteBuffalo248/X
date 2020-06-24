// LineEditDlg.cpp : ʵ���ļ�
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
// LineEditDlg �Ի���

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


// LineEditDlg ��Ϣ�������


BOOL CLineEditDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	SetWindowText(title);
	if (fixed_size)
		CHintStatic.ShowWindow(SW_SHOW);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


void CLineEditDlg::OnNMThemeChangedHintStatic(NMHDR *pNMHDR, LRESULT *pResult)
{
	// �ù���Ҫ��ʹ�� Windows XP ����߰汾��
	// ���� _WIN32_WINNT ���� >= 0x0501��
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
	
}


void CLineEditDlg::OnEnChangetextedit()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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
