#pragma once


// CCmdScriptTypeComboBox

class CCmdScriptTypeComboBox : public CComboBox
{
	DECLARE_DYNAMIC(CCmdScriptTypeComboBox)

public:
	CCmdScriptTypeComboBox();
	virtual ~CCmdScriptTypeComboBox();

	static WNDPROC lpfnListDefaultWndProc;
	static WNDPROC lpfnEditDefaultWndProc;
	static HWND comboxHwnd;

private:
	COLORREF	ITEM_SELECT_BK_COLOR;
	COLORREF	ITEM_SELECT_TEXT_COLOR;

	

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnCbnDropdown();
	virtual void MeasureItem(LPMEASUREITEMSTRUCT /*lpMeasureItemStruct*/);
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
};


