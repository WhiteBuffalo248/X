#pragma once
//#include "HandleTable.h"
#include "StdTable.h"
#include "AbstractSearchList.h"
#include "SearchDlg.h"


// CTableVessel
class StdTableSearchList;

class CTableVessel : public CWnd
{
	DECLARE_DYNAMIC(CTableVessel)

public:
	CTableVessel();
	virtual ~CTableVessel();
	
	int getCharWidth();
	void setShowHeader(bool show);
	void addColumnAt(int width, CString title, bool isClickable);
	void enableMultiSelection(bool enabled);
	void setAddressColumn(int col, bool cipBase = false);
	void loadColumnFromConfig(const CString & viewName);
	void setRowCount(dsint count);	
	void setCellContent(int r, int c, CString s);
	void reloadData();
	



	

protected:
	CSearchDlg* pSearchDlg;

	CStdTable* stdList();
	CStdTable* stdSearchList();

	StdTableSearchList* mSearchListData;
	CAbstractStdTable* mCurList;

	

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


