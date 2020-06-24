#pragma once
#include "OutputBox.h"

// CLogPane

class CLogPane : public CDockablePane
{
	DECLARE_DYNAMIC(CLogPane)

public:
	CLogPane();
	virtual ~CLogPane();
	virtual COutputBox*		getCOutputBox() { return &m_wndOutputView; }
private:

	COutputBox m_wndOutputView;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


