#pragma once


// CCmdListBox

class CCmdListBox : public CListBox
{
	DECLARE_DYNAMIC(CCmdListBox)

public:
	CCmdListBox();
	virtual ~CCmdListBox();
	

private:
	HWND editHwnd;
	
	

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLbnSetfocus();
	
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
	afx_msg void OnLbnDblclk();
	afx_msg void OnLbnKillfocus();
};


