
// MainFrm.h : CMainFrame 类的接口
//

#pragma once
#include "ChildView.h"
#include "TrueColorToolBar.h"
#include "MyStatuBar.h"
#include "MyToolBar.h"
#include "DisasmPane.h"
#include "PeInfoPane.h"
#include "LogPane.h"
#include "MemoryPane.h"
#include "CPUStackPane.h"
#include "RegistersPane.h"
#include "BreakPointsManagePane.h"
#include "InstructTracePane.h"
#include "HandlesPane.h"
#include "ReferencesPane.h"
#include "CloseDlg.h"
#include "CmdBar.h"


//class CMainFrame : public CFrameWnd
class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();
	virtual ~CMainFrame();
// 特性
public:
	UINT	m_nToolBarButtonLook;
	CMenu tmpMenu;

	struct MenuInfo
	{
	public:
		MenuInfo() {};
		MenuInfo(CWnd* parent, CMenu* mMenu, int hMenu, int hParentMenu, bool globalMenu, bool bNew)
			: parent(parent), mMenu(mMenu), hMenu(hMenu), hParentMenu(hParentMenu), globalMenu(globalMenu), bNew(bNew)
		{
		}

		CWnd* parent;
		CMenu* mMenu;
		int hMenu;
		int hParentMenu;
		bool globalMenu;
		bool bNew;
	};

	//menu api
	struct MenuEntryInfo
	{
		//QAction* mAction;
		int hEntry;
		int hParentMenu;
		CString hotkey;
		CString hotkeyId;
		bool hotkeyGlobal;
	};

	int hEntryMenuPool;
	CList<MenuEntryInfo> mEntryList;
	CList<MenuInfo> mMenuList;
	CMenu globalMenu;
	
	bool bRequestClose = false;
	
	

// 操作
public:
	void dbgStateChanged(DBGSTATE state);
	void addMsgToLog(const char* msg);
	void ActivatePane(CDockablePane* pane);

	void initMenuApi();
	const MenuInfo* findMenu(int hMenu);
	void addMenuToList(CWnd* parent, CMenu* menu, int hMenu, int hParentMenu = -1);
	void addMenu(int hMenu, CString title);
	void addMenuEntry(int hMenu, CString title);
	void removeMenuEntry(int hEntryMenu);
	void clearMenu(int hMenu, bool erase);
	void uninstallPluginMenu();
	
	virtual	CMyStatuBar*	getCMyStatuBar(){ return &m_wndStatusBar; }
	virtual CMyToolBar*		getCMyToolBar() { return &m_wndToolBar; }
	virtual CDisasmPane*	getCDisasmPane() { return &m_wndDisasmPane; }
	virtual CLogPane*		getCLogPane() { return &m_wndLogPane; }
	virtual CHandlesPane*	getCHandlesPane() { return &m_wndHandlesPane; }
	
// 重写
public:
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE,
		CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	

// 实现
public:
	
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
public:
	CMFCMenuBar				m_wndMenuBar;

protected:  // 控件条嵌入成员
		
//	CTrueColorToolBar		m_wndToolBar;
	
	CMyStatuBar				m_wndStatusBar;
	CMFCToolBar				m_wndTabViewBar;
	CMyToolBar				m_wndToolBar;
	
	CDisasmPane				m_wndDisasmPane;
	CPeInfoPane				m_wndPeInfo;
	CLogPane				m_wndLogPane;
	CMemoryPane				m_wndMemoryPane;
	CCPUStackPane			m_wndCPUStackPane;
	CRegistersPane			m_wndRegistersPane;
	CBreakPointsManagePane	m_wndBreakPointsManagePane;
	CInstructTracePane		m_wndInstructTracePane;
	CHandlesPane			m_wndHandlesPane;
	CReferencesPane			m_wndReferencesPane;
	
	

	CMFCReBar				m_cmdReBar;
	CCmdBar					m_cmdBar;
	

private:
	CCloseDlg m_closeDlg;


// 生成的消息映射函数
protected:
	BOOL CreateDockingWindows();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg	void OnToolBarButtonLook(UINT id);
	afx_msg void OnPluginHandle(UINT id);
	afx_msg void OnLoadScyllaToPlugin();
	afx_msg	void OnUpdateToolBarButtonLook(CCmdUI* pCmdUI);
	afx_msg LRESULT OnToolbarReset(WPARAM, LPARAM);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()	
public:
	afx_msg void OnChildActivate();
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
};


