#pragma once
#include "MyProgressCtrl.h"
#include "MyStatic.h"

// CProgressBar

class CProgressBar : public CWnd
{
	DECLARE_DYNAMIC(CProgressBar)

public:
	CProgressBar();
	virtual ~CProgressBar();
	CMyProgressCtrl* getRegionProgressCtrl() { return &m_regionProgress; }
	CMyProgressCtrl* getTotalProgressCtrl() { return &m_totalProgress; }
	CMyStatic* getItemCountStatic() { return &m_itemCount; }

private:
	int staticWidth = 50;
	CMyProgressCtrl m_regionProgress;
	CMyProgressCtrl m_totalProgress;
	CMyStatic m_itemCount;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
};


