// MyHeaderCtrl.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "X.h"
#include "MyHeaderCtrl.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// CMyHeaderCtrl

IMPLEMENT_DYNAMIC(CMyHeaderCtrl, CHeaderCtrl)

CMyHeaderCtrl::CMyHeaderCtrl()
{

}

CMyHeaderCtrl::~CMyHeaderCtrl()
{
}


BEGIN_MESSAGE_MAP(CMyHeaderCtrl, CHeaderCtrl)
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()



// CMyHeaderCtrl ��Ϣ�������




BOOL CMyHeaderCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	DWORD dwPos = GetMessagePos();
	CPoint point(LOWORD(dwPos), HIWORD(dwPos));

	ScreenToClient(&point);
	int ColumnCount = GetItemCount();
	
	CRect ColumnRect;
	GetItemRect(0, &ColumnRect);
	CRect ColumnRect1;
	GetItemRect(1, &ColumnRect1);
	if ((abs(point.x - ColumnRect.right) < 2) && (ColumnRect.top <= point.y) && (point.y <= ColumnRect.bottom))
	{
		HCURSOR HdefaultCursor = GetCursor();
		HINSTANCE Hx = AfxGetInstanceHandle();
		HCURSOR HdefaultCursor1 = LoadCursor(NULL, IDC_ARROW);
		DWORD LastError = GetLastError();
			
	//	SetCursor(LoadCursor(NULL, IDC_APPSTARTING));
	}
	return CHeaderCtrl::OnSetCursor(pWnd, nHitTest, message);
}
