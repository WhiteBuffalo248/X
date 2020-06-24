#pragma once

// CToolbarLabel ÃüÁîÄ¿±ê

class CToolbarLabel : public CMFCToolBarButton
{
		COLORREF	m_TextColor;
		COLORREF	m_BackgroundColor;
	DECLARE_SERIAL(CToolbarLabel)

public:
	CToolbarLabel(UINT nID = 0, LPCTSTR lpszText = NULL, COLORREF textColor = 0x000000, COLORREF textBackgroundColor = 0x000000);

	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages,
		BOOL bHorz = TRUE, BOOL bCustomizeMode = FALSE,
		BOOL bHighlight = FALSE,
		BOOL bDrawBorder = TRUE,
		BOOL bGrayDisabledButtons = TRUE);
};


// CMyToolBar

class CMyToolBar : public CMFCToolBar
{
	DECLARE_DYNAMIC(CMyToolBar)

public:
	CMyToolBar();
	virtual ~CMyToolBar();

	struct MyLabelAttribute
	{
		CString		m_text;
		COLORREF	m_textColor;
		COLORREF	m_backgroundColor;
	};

	MyLabelAttribute& getLabelAttribute() { return m_labelAttribute; }

private:


	MyLabelAttribute m_labelAttribute;

protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL DrawButton(CDC* pDC, CMFCToolBarButton* pButton, CMFCToolBarImages* pImages, BOOL bHighlighted, BOOL bDrawDisabledImages);
};


