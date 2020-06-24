#pragma once
#include "CPUInfoBox.h"

// CCPUInfoPane

class CCPUInfoPane : public CDockablePane
{
	DECLARE_DYNAMIC(CCPUInfoPane)

public:
	CCPUInfoPane();
	virtual ~CCPUInfoPane();

protected:
	CCPUInfoBox m_wndCPUInfoView;

protected:
	DECLARE_MESSAGE_MAP()
};


