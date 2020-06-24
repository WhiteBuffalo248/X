// InstructTracePane.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "X.h"
#include "InstructTracePane.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// CInstructTracePane

IMPLEMENT_DYNAMIC(CInstructTracePane, CDockablePane)

CInstructTracePane::CInstructTracePane()
{

}

CInstructTracePane::~CInstructTracePane()
{
}


BEGIN_MESSAGE_MAP(CInstructTracePane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CInstructTracePane ��Ϣ�������




int CInstructTracePane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������
	m_wndInstructTraceView.Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, lpCreateStruct->cx, lpCreateStruct->cy), this, NULL);
	return 0;
}


void CInstructTracePane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������
	m_wndInstructTraceView.SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
}

