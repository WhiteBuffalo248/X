#pragma once


// CMyStatuBar
extern const int nSystemStatus;
extern const int nStatusOutput;

class CMyStatuBar : public CMFCStatusBar
{
	DECLARE_DYNAMIC(CMyStatuBar)

public:
	CMyStatuBar();
	virtual ~CMyStatuBar();

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
};


