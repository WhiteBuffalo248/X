#pragma once
#include "CPUStack.h"

// CCPUStackPane

class CCPUStackPane : public CDockablePane
{
	DECLARE_DYNAMIC(CCPUStackPane)

public:
	CCPUStackPane();
	virtual ~CCPUStackPane();
	virtual CCPUStack*		getCCPUStack() { return &m_wndCPUStackView; }

private:
	CCPUStack m_wndCPUStackView;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


