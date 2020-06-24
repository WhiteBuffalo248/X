#include "stdafx.h"
#include "MyRecentFileList.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CMyRecentFileList::~CMyRecentFileList()
{
}

void CMyRecentFileList::UpdateMenu(CCmdUI* pCmdUI)
{
	int iMRU;
	ASSERT(m_arrNames != NULL);

	CMenu* pMenu = pCmdUI->m_pMenu;

	if (pMenu == NULL)
		return;

	ASSERT(pMenu == pCmdUI->m_pMenu);  // make sure preceding code didn't mess with it
	ASSERT(pMenu->m_hMenu);

	// look for a submenu to use instead
	CMenu *pSubMenu;
	if (pMenu)
		pSubMenu = pMenu->GetSubMenu(pCmdUI->m_nIndex);
	if (pSubMenu) {
		ASSERT(pSubMenu->m_hMenu);
		pMenu = pSubMenu;
	}

	ASSERT(pMenu->m_hMenu);

	if (m_arrNames[0].IsEmpty())
	{
		// no MRU files
		for (iMRU = 1; iMRU < m_nSize; iMRU++) {
			//pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID + iMRU, MF_BYCOMMAND);
			// This hopefully will not remove the popup
			//pCmdUI->m_pMenu->RemoveMenu(pCmdUI->m_nID + iMRU, MF_BYCOMMAND);
			pMenu->RemoveMenu(pCmdUI->m_nID + iMRU, MF_BYCOMMAND);
		}
		pCmdUI->SetText(_T("NULL"));
		pCmdUI->Enable(FALSE);
		return;
	}

	for (iMRU = 0; iMRU < m_nSize; iMRU++) {
		//pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID + iMRU, MF_BYCOMMAND);
		// This hopefully will not remove the popup
		//pCmdUI->m_pMenu->RemoveMenu(pCmdUI->m_nID + iMRU, MF_BYCOMMAND);
		pMenu->RemoveMenu(pCmdUI->m_nID + iMRU, MF_BYCOMMAND);
	}

	TCHAR szCurDir[_MAX_PATH];
	GetCurrentDirectory(_MAX_PATH, szCurDir);
	int nCurDir = lstrlen(szCurDir);
	ASSERT(nCurDir >= 0);
	szCurDir[nCurDir] = '\\';
	szCurDir[++nCurDir] = '\0';

	CString strName;
	CString strTemp;
	for (iMRU = 0; iMRU < m_nSize; iMRU++)
	{
		if (!GetDisplayName(strName, iMRU, szCurDir, _MAX_PATH))
			break;

		// double up any '&' characters so they are not underlined
		LPCTSTR lpszSrc = strName;

		LPTSTR lpszDest = strTemp.GetBuffer(strName.GetLength() * 2);

		while (*lpszSrc != 0)

		{
			if (*lpszSrc == '&')
				*lpszDest++ = '&';

			if (_istlead(*lpszSrc))
				*lpszDest++ = *lpszSrc++;

			*lpszDest++ = *lpszSrc++;

		}
		*lpszDest = 0;

		strTemp.ReleaseBuffer();

		// insert mnemonic + the file name
		TCHAR buf[10];
		wsprintf(buf, _T("&%d "), (iMRU + 1 + m_nStart) % 10);
		/*pCmdUI->m_pMenu->InsertMenu(pCmdUI->m_nIndex++,
		MF_STRING | MF_BYPOSITION, pCmdUI->m_nID++,
		CString(buf) + strTemp);*/
		// Note we use our pMenu which may not be pCmdUI->m_pMenu
		pMenu->InsertMenu(pCmdUI->m_nIndex++,
			MF_STRING | MF_BYPOSITION, pCmdUI->m_nID++,
			CString(buf) + strTemp);
	}

	// update end menu count
	pCmdUI->m_nIndex--; // point to last menu added
	pCmdUI->m_nIndexMax = pMenu->GetMenuItemCount();

	pCmdUI->m_bEnableChanged = TRUE;    // all the added items are enabled
}  // VIRecentFileList::UpdateMenu()
