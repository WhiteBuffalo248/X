#pragma once
#include "InstructTrace.h"

// CInstructTracePane

class CInstructTracePane : public CDockablePane
{
	DECLARE_DYNAMIC(CInstructTracePane)

public:
	CInstructTracePane();
	virtual ~CInstructTracePane();
	CInstructTrace* getCInstructTrace() { return &m_wndInstructTraceView; }

private:
	CInstructTrace m_wndInstructTraceView;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


