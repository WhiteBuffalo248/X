#pragma once
#include "HandleTable.h"

// CHandlesPane

class CHandlesPane : public CDockablePane
{
	DECLARE_DYNAMIC(CHandlesPane)

public:
	CHandlesPane();
	virtual ~CHandlesPane();

	virtual	CHandleTable*	getCHandlesView() { return &m_wndHandlesView; }
	virtual	CHandleTable*	getCWinProgView() { return &m_wndWinProgView; }
	virtual	CHandleTable*	getCTcpConnectionsView() { return &m_wndTcpConnectionsView; }

	void enumHandles();
	void enumWindows();
	void enumTcpConnections();
	void dataRefresh();
	void MyAdjustLayout();
	void drawAdjustLine(CPoint point);
	void RecordInitDC();
	
protected:
	CHandleTable m_wndHandlesView;
	CHandleTable m_wndWinProgView;
	CHandleTable m_wndTcpConnectionsView;

public:
	int leastHeight;

	int iHight1;
	int iHight2;
	CDC m_intiDC;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
};


