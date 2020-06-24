#pragma once
#include "BreakPointsManage.h"

// CBreakPointsManagePane

class CBreakPointsManagePane : public CDockablePane
{
	DECLARE_DYNAMIC(CBreakPointsManagePane)

public:
	CBreakPointsManagePane();
	virtual ~CBreakPointsManagePane();
	virtual CBreakPointsManage* getCBreakPointsManage() { return &m_wndBreakPointsManageView; }

private:
	CBreakPointsManage m_wndBreakPointsManageView;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


