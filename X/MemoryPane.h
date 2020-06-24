#pragma once
#include "CPUDump.h"

// CMemoryPane

class CMemoryPane : public CDockablePane
{
	DECLARE_DYNAMIC(CMemoryPane)

public:
	CMemoryPane();
	virtual ~CMemoryPane();
	virtual CCPUDump*		getCCPUDump() { return &m_wndMemoryView; }

private:
	CCPUDump m_wndMemoryView;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


