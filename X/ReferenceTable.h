#pragma once
#include "TableVessel.h"
#include "ProgressBar.h"
class CReferenceTable :
	public CTableVessel
{
public:
	CReferenceTable();
	~CReferenceTable();
	void onShowSearch();

	void addColumnAtRef(int width, CString title);
	void setRowCount(dsint count);
	void searchSelectionChanged(int index);
	void referenceSetProgress(int progress);
	void referenceSetCurrentTaskProgress(int progress, CString taskTitle);

protected:
	int m_ProgressCtrlHeight;
	CProgressBar m_ProgressCtrl;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnHandle(UINT id);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};

