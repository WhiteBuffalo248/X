#pragma once

#include "StdTable.h"

class WordEditDialog;
class XrefBrowseDialog;

class CCPUInfoBox : public CStdTable
{
public:
	explicit CCPUInfoBox();
	int getHeight();
	void addInfoLine(const CString & infoLine);
	void setInfoLine(int line, CString text);
	CString getInfoLine(int line);
	void disasmSelectionChanged(dsint parVA);


private:
	dsint curAddr;
	dsint curRva;
	dsint curOffset;
	
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};