// CloseDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "X.h"
#include "CloseDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// CCloseDlg �Ի���

IMPLEMENT_DYNAMIC(CCloseDlg, CDialogEx)

CCloseDlg::CCloseDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_Close_Dlg, pParent)
{

}

CCloseDlg::~CCloseDlg()
{
}

void CCloseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCloseDlg, CDialogEx)
END_MESSAGE_MAP()


// CCloseDlg ��Ϣ�������
