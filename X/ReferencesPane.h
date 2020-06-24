#pragma once
#include "ReferenceTable.h"
#include "MyTabCtrl.h"


// CReferencesPane

class CReferencesPane : public CDockablePane
{
	DECLARE_DYNAMIC(CReferencesPane)
public:
	
	CReferencesPane();
	virtual ~CReferencesPane();

	void newReferenceView(CString name);
	void closeTab(int index);
	void closeAllTab();
	CMFCTabCtrl* getCMFCTabCtrl() { return &m_TabCtrl; }

	CReferenceTable* getCurrentReferenceView();
	std::vector<CReferenceTable*> getCReferenceTable();
	

protected:
	int m_TabCtrlHeight = 20;
	
	std::vector<CString> m_NameArray;
	CMyTabCtrl m_TabCtrl;
	
	CReferenceTable* m_pCurrentReferencesView;
	std::vector<CReferenceTable*> m_pReferenceTable;
	

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
};


