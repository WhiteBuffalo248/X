#pragma once
#include "CmdListBox.h"


// CCmdEdit

class CCmdEdit : public CEdit
{
	DECLARE_DYNAMIC(CCmdEdit)

public:
	CCmdEdit();
	virtual ~CCmdEdit();

	static BOOL UninstallMouseHook();
	BOOL InstallMouseHook();

	void showDropDown(BOOL bShowIt = TRUE);

	void autoCompleteAddCmd(const CString cmd);
	void addArrayToList(CStringArray& array);
	void removeList();
	void setPopupPos();
	void registerScriptType(SCRIPTTYPEINFO* info);
	void unregisterScriptType(int id);

	void cmdExecute(CString cmd);
	bool m_bShow = false;

private:
	
	HWND h_cmdScriptTypeComboBoxHwnd;

	std::vector<SCRIPTTYPEINFO> mScriptInfo;
	int mCurrentScriptIndex = 0;
	CStringArray m_cmdArray;
	CCmdListBox m_listBox;
	
	

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChange();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEnSetfocus();
	afx_msg void OnEnKillfocus();
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};


