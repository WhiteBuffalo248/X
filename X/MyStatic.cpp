// MyStatic.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "X.h"
#include "MyStatic.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// CMyStatic

IMPLEMENT_DYNAMIC(CMyStatic, CStatic)

CMyStatic::CMyStatic()
{
	memset(&defaultfont, 0, sizeof(defaultfont));
	defaultfont.lfWeight = FW_MEDIUM;

	_tcscpy_s(defaultfont.lfFaceName, LF_FACESIZE, _T("΢���ź�")); //Lucida Console
	defaultfont.lfHeight = MulDiv(13, 96, 72);;
}

CMyStatic::~CMyStatic()
{
}

void CMyStatic::setStaticText(CString text)
{
	m_text = text;
	Invalidate();
}


BEGIN_MESSAGE_MAP(CMyStatic, CStatic)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_CREATE()
END_MESSAGE_MAP()



// CMyStatic ��Ϣ�������




void CMyStatic::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: �ڴ˴������Ϣ����������
					   // ��Ϊ��ͼ��Ϣ���� CStatic::OnPaint()
	CRect viewRect;
	GetClientRect(&viewRect);
	dc.FillSolidRect(&viewRect, RGB(250, 250, 250));
	dc.SetBkMode(TRANSPARENT);
	CFont font;
	VERIFY(font.CreateFontIndirect(&defaultfont));
	dc.SelectObject(&font);
	CRect textRect = viewRect;
	textRect.DeflateRect(1, 1);
	dc.DrawText(m_text, viewRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	
}


HBRUSH CMyStatic::CtlColor(CDC* pDC, UINT nCtlColor)
{
	// TODO:  �ڴ˸��� DC ���κ�����

	

	// TODO:  �����Ӧ���ø����Ĵ�������򷵻ط� null ����
	
	return NULL;
}


int CMyStatic::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������
	
	m_text = lpCreateStruct->lpszName;

	return 0;
}
