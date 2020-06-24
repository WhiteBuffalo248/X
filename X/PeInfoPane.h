#pragma once
#include "MyListCtrl.h"
class CPeInfoPane :
	public CDockablePane
{
public:
	CMyListCtrl m_List;
	CPeInfoPane(void);
	~CPeInfoPane(void);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
//	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

