// InstructTraceBrowserDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "X.h"
#include "InstructTraceBrowserDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// CInstructTraceBrowserDlg 对话框
IMPLEMENT_DYNAMIC(CInstructTraceBrowserDlg, CDialogEx)

CInstructTraceBrowserDlg::CInstructTraceBrowserDlg(CWnd* pParent, CString title, CString filePath)
	: CDialogEx(IDD_InstructTrace_Browser_Dlg, pParent), m_title(title), m_filePath(filePath)
{

}

CInstructTraceBrowserDlg::~CInstructTraceBrowserDlg()
{
}

void CInstructTraceBrowserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_Browser_Edit, m_filePath);
}


BEGIN_MESSAGE_MAP(CInstructTraceBrowserDlg, CDialogEx)
	ON_BN_CLICKED(IDC_InstructTraceBrowser_Button, &CInstructTraceBrowserDlg::OnBnClickedInstructtracebrowserButton)
END_MESSAGE_MAP()


// CInstructTraceBrowserDlg 消息处理程序


void CInstructTraceBrowserDlg::OnBnClickedInstructtracebrowserButton()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog fileDlg(TRUE);
	fileDlg.m_ofn.lpstrTitle = _T("Select stored file");
#ifdef _WIN64
	fileDlg.m_ofn.lpstrFilter = _T("trace Files(*.trace64)\0*.trace64\0All Files(*.*)\0*.*\0\0");
#else
	fileDlg.m_ofn.lpstrFilter = _T("trace Files(*.trace32)\0*.trace32\0All Files(*.*)\0*.*\0\0");
#endif //_WIN64

	if (fileDlg.DoModal() == IDOK)
	{
		m_filePath = fileDlg.m_ofn.lpstrFile;
		UpdateData(FALSE);
	}
}


BOOL CInstructTraceBrowserDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetWindowText(m_title);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
