#pragma once
#include "afxadv.h"
class CMyRecentFileList :
	public CRecentFileList
{
public:
	CMyRecentFileList(UINT nStart, LPCTSTR lpszSection,
		LPCTSTR lpszEntryFormat, int nSize,
		int nMaxDispLen = AFX_ABBREV_FILENAME_LEN)
		: CRecentFileList(nStart, lpszSection, lpszEntryFormat, nSize,
			nMaxDispLen) {}
	~CMyRecentFileList();
	virtual void UpdateMenu(CCmdUI* pCmdUI);
};

