#pragma once


// CMyHeaderCtrl

class CMyHeaderCtrl : public CHeaderCtrl
{
	DECLARE_DYNAMIC(CMyHeaderCtrl)

public:
	CMyHeaderCtrl();
	virtual ~CMyHeaderCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};


