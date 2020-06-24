#pragma once


// CMyProgressCtrl

class CMyProgressCtrl : public CProgressCtrl
{
	DECLARE_DYNAMIC(CMyProgressCtrl)

public:
	CMyProgressCtrl();
	virtual ~CMyProgressCtrl();
	void setLabalText(CString labalText);

private:
	LOGFONT defaultfont;
	CString m_labalText;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};


