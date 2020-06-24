#pragma once
#include "MyStatic.h"
#include "CmdEdit.h"
#include "CmdScriptTypeComboBox.h"

// CCmdBar

class CCmdBar : public CWnd
{
	DECLARE_DYNAMIC(CCmdBar)
	friend CCmdEdit;
public:
	CCmdBar();
	virtual ~CCmdBar();
	CCmdEdit* getCCmdEdit() { return &m_cmdEdit; }
	CComboBox* getCComboBox() { return &m_CmdScriptTypeComboBox; }

	

private:
	
	CMyStatic m_commondTittle;
	CCmdEdit m_cmdEdit;
	CCmdScriptTypeComboBox m_CmdScriptTypeComboBox;
	

	


protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};


