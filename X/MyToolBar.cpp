// MyToolBar.cpp : 实现文件
//

#include "stdafx.h"
#include "X.h"
#include "MyToolBar.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CToolbarLabel, CMFCToolBarButton, 1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToolbarLabel::CToolbarLabel(UINT nID, LPCTSTR lpszText, COLORREF textColor, COLORREF textBackgroundColor)
{
	if (lpszText != NULL)
	{
		m_strText = lpszText;
		m_TextColor = textColor;
		m_BackgroundColor = textBackgroundColor;
	}
	m_bText = TRUE;
	m_nID = nID;
	m_iImage = -1;
}

void CToolbarLabel::OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages,
	BOOL bHorz, BOOL /*bCustomizeMode*/, BOOL /*bHighlight*/,
	BOOL /*bDrawBorder*/, BOOL /*bGrayDisabledButtons*/)
{

	UINT nStyle = m_nStyle;
	m_nStyle &= ~TBBS_DISABLED;

	CMFCToolBarButton::OnDraw(pDC, rect, pImages, bHorz, FALSE,
		FALSE, FALSE, FALSE);

	m_nStyle = nStyle;
	/*
	pDC->FillSolidRect(&rect, m_BackgroundColor);
	pDC->SetTextColor(m_TextColor);
	pDC->DrawText(m_strText, CRect(rect), DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	*/
}

// CMyToolBar

IMPLEMENT_DYNAMIC(CMyToolBar, CMFCToolBar)

CMyToolBar::CMyToolBar()
{
	m_labelAttribute.m_textColor = 0x000000;
	m_labelAttribute.m_backgroundColor = 0xFFFFFF;
}

CMyToolBar::~CMyToolBar()
{
}


BEGIN_MESSAGE_MAP(CMyToolBar, CMFCToolBar)
END_MESSAGE_MAP()



// CMyToolBar 消息处理程序
BOOL CMyToolBar::DrawButton(CDC* pDC, CMFCToolBarButton* pButton, CMFCToolBarImages* pImages, BOOL bHighlighted, BOOL bDrawDisabledImages)
{	
	if (pButton->m_nID == ID_DEBUG_STATUS)
	{
		CString text = m_labelAttribute.m_text;
		if (text.IsEmpty())
			text = pButton->m_strText;
		CRect btnRect = pButton->Rect();
		
		int hy = btnRect.Height() - pImages->GetImageSize().cy;
		btnRect.DeflateRect(0, hy / 2);
		pDC->FillSolidRect(&btnRect, m_labelAttribute.m_backgroundColor);
		pDC->SetTextColor(m_labelAttribute.m_textColor);
		pDC->DrawText(text, &btnRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		pDC->Draw3dRect(btnRect, 0xF0F0F0, 0xC0C0C0);
		
				
		return TRUE;
	}	
	return CMFCToolBar::DrawButton(pDC, pButton, pImages, bHighlighted, bDrawDisabledImages);
}


