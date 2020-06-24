#pragma once
#include "RegistersView.h"

// CRegistersPane

class CRegistersPane : public CDockablePane
{
	DECLARE_DYNAMIC(CRegistersPane)

public:
	CRegistersPane();
	virtual ~CRegistersPane();
	virtual CRegistersView* getCRegisters() { return &m_wndRegistersView; }

private:
	CRegistersView m_wndRegistersView;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


