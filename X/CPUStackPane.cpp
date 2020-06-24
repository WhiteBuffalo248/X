// CPUStackPane.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "X.h"
#include "CPUStackPane.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// CCPUStackPane

IMPLEMENT_DYNAMIC(CCPUStackPane, CDockablePane)

CCPUStackPane::CCPUStackPane()
{

}

CCPUStackPane::~CCPUStackPane()
{
}


BEGIN_MESSAGE_MAP(CCPUStackPane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CCPUStackPane ��Ϣ�������




int CCPUStackPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������
	m_wndCPUStackView.Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, lpCreateStruct->cx, lpCreateStruct->cy), this, NULL);
	return 0;
}


void CCPUStackPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������
	m_wndCPUStackView.SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
}
