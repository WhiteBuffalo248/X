// MyProgressCtrl.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "X.h"
#include "MyProgressCtrl.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// CMyProgressCtrl

IMPLEMENT_DYNAMIC(CMyProgressCtrl, CProgressCtrl)

CMyProgressCtrl::CMyProgressCtrl()
{
	memset(&defaultfont, 0, sizeof(defaultfont));
	defaultfont.lfWeight = FW_NORMAL;
	_tcscpy_s(defaultfont.lfFaceName, LF_FACESIZE, _T("Lucida Console"));
	defaultfont.lfHeight = MulDiv(10, 96, 72);;
}

CMyProgressCtrl::~CMyProgressCtrl()
{
}

void CMyProgressCtrl::setLabalText(CString labalText)
{
	m_labalText = labalText;
}


BEGIN_MESSAGE_MAP(CMyProgressCtrl, CProgressCtrl)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CMyProgressCtrl ��Ϣ�������




void CMyProgressCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: �ڴ˴������Ϣ����������
					   // ��Ϊ��ͼ��Ϣ���� CProgressCtrl::OnPaint()

	CRect viewRect;
	GetClientRect(&viewRect);

	CDC MenDC;
	CBitmap MemMap;
	MenDC.CreateCompatibleDC(&dc);
	MemMap.CreateCompatibleBitmap(&dc, viewRect.Width(), viewRect.Height());
	MenDC.SelectObject(&MemMap);	
	
	DefWindowProc(WM_PAINT, (WPARAM)MenDC.m_hDC, (LPARAM)0);

	MenDC.SetBkMode(TRANSPARENT);
	CFont font;
	VERIFY(font.CreateFontIndirect(&defaultfont));
	MenDC.SelectObject(&font);

	MenDC.DrawText(m_labalText, viewRect, DT_VCENTER | DT_CENTER | DT_SINGLELINE);

	dc.BitBlt(0, 0, viewRect.Width(), viewRect.Height(), &MenDC, 0, 0, SRCCOPY);
}
