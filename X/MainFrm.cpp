
// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "X.h"
#include "GlobalID.h"
#include "MainFrm.h"
#include "MSMLinksBarButton.h"
#include "DebugThreadClose.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_COMMAND(IDS_INDICATOR_OUTPUTINFO, NULL)
	ON_REGISTERED_MESSAGE(AFX_WM_RESETTOOLBAR, OnToolbarReset)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_COMMAND_RANGE(User_Defined_Start + ID_Disasm_DockPane, User_Defined_Start + ID_InstructTrace_DockPane, OnToolBarButtonLook)
	ON_COMMAND_RANGE(ID_BUTTON32798, ID_BUTTON32810, OnToolBarButtonLook)
	ON_COMMAND(ID_Plugin_Load_Scylla, OnLoadScyllaToPlugin)
	ON_COMMAND_RANGE(ID_Plugin_Start, ID_Plugin_Start + 2000, OnPluginHandle)
	ON_UPDATE_COMMAND_UI_RANGE(User_Defined_Start + ID_Disasm_DockPane, User_Defined_Start + ID_InstructTrace_DockPane, OnUpdateToolBarButtonLook)
	ON_WM_CREATE()
	ON_WM_SIZE()
	
	ON_WM_CHILDACTIVATE()
	ON_WM_MDIACTIVATE()
	ON_WM_NCACTIVATE()
	ON_WM_DROPFILES()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_WM_MOVE()
	ON_WM_MOVING()
END_MESSAGE_MAP()

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
	//读取主题
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008);
	//当弹出菜单出现在主框架外时，强制框架绘制菜单阴影
	CMFCPopupMenu::SetForceShadow(TRUE);
	//激活选项卡文档菜单项
	m_bCanConvertControlBarToMDIChild = TRUE;	
}

void CMainFrame::OnLoadScyllaToPlugin()
{
	DbgCmdExec("StartScylla");
}

CMainFrame::~CMainFrame()
{
	
}

void CMainFrame::OnPluginHandle(UINT id)
{
	DbgMenuEntryClicked(id);
}

void CMainFrame::initMenuApi()
{
	//256 entries are reserved
	hEntryMenuPool = ID_Plugin_Start;
	/*
	mEntryList.reserve(1024);
	mMenuList.reserve(1024);
	*/	
}

const CMainFrame::MenuInfo* CMainFrame::findMenu(int hMenu)
{
	if (hMenu == -1)
		return 0;
	int nFound = -1;
	for (int i = 0; i < mMenuList.GetSize(); i++)
	{
		if (hMenu == mMenuList.GetAt(mMenuList.FindIndex(i)).hMenu)
		{
			nFound = i;
			break;
		}
	}
	return nFound == -1 ? 0 : &mMenuList.GetAt(mMenuList.FindIndex(nFound));
}

void CMainFrame::addMenuToList(CWnd* parent, CMenu* menu, int hMenu, int hParentMenu)
{
	if (!findMenu(hMenu))
		mMenuList.AddTail(MenuInfo(parent, menu, hMenu, hParentMenu, hMenu == GUI_PLUGIN_MENU, false));
	Bridge::getBridge()->setResult();
}

void CMainFrame::addMenu(int hMenu, CString title)
{
	const MenuInfo* menu = findMenu(hMenu);
	if (!menu && hMenu != -1)
	{
		Bridge::getBridge()->setResult(-1);
		return;
	}
	int hMenuNew = hEntryMenuPool++;
	CWnd* parent = hMenu == -1 ? this : menu->parent;

	// 创建弹出菜单
	CMenu* wMenu = new CMenu;
	BOOL bRet = wMenu->CreatePopupMenu();
	
	mMenuList.AddTail(MenuInfo(parent, wMenu, hMenuNew, hMenu, !menu || menu->globalMenu, true));
	BOOL s;
	if (hMenu == -1) //top-level
		GetMenu()->AppendMenu(MF_POPUP, (UINT)wMenu->m_hMenu, title);
	else //deeper level
	{
		s = menu->mMenu->AppendMenu(MF_POPUP, (UINT)wMenu->m_hMenu, title);
	}

	Bridge::getBridge()->setResult(hMenuNew);
}

void CMainFrame::addMenuEntry(int hMenu, CString title)
{
	const MenuInfo* menu = findMenu(hMenu);
	if (!menu && hMenu != -1)
	{
		Bridge::getBridge()->setResult(-1);
		return;
	}
	
	MenuEntryInfo newInfo;
	int hEntryNew = hEntryMenuPool++;
	newInfo.hEntry = hEntryNew;
	newInfo.hParentMenu = hMenu;
	CWnd* parent = hMenu == -1 ? this : menu->parent;
	/*
	QAction* wAction = new QAction(title, parent);
	wAction->setObjectName(QString().sprintf("ENTRY|%d", hEntryNew));
	wAction->setShortcutContext((!menu || menu->globalMenu) ? Qt::ApplicationShortcut : Qt::WidgetShortcut);
	parent->addAction(wAction);
	connect(wAction, SIGNAL(triggered()), this, SLOT(menuEntrySlot()));
	newInfo.mAction = wAction;
	*/	
	mEntryList.AddTail(newInfo);
	BOOL s;
	if (hMenu == -1) //top level
		s = menu->mMenu->AppendMenu(MF_STRING, (UINT)hEntryNew, title);
	else //deeper level
	{
		s = menu->mMenu->AppendMenu(MF_STRING, (UINT)hEntryNew, title);
	}
	
	
	Bridge::getBridge()->setResult(hEntryNew);
	
}

void CMainFrame::clearMenu(int hMenu, bool erase)
{
	//this recursively removes the entries from mEntryList and mMenuList
	POSITION pos = mMenuList.GetHeadPosition();
	for (int i = 0; i < mMenuList.GetCount(); i++)
	{
		auto curMenu = mMenuList.GetAt(mMenuList.FindIndex(i));
		if (hMenu == curMenu.hMenu)
		{
			if (erase)
			{
				auto parentMenu = findMenu(curMenu.hParentMenu);
				if (parentMenu)
				{
					parentMenu->mMenu->DestroyMenu(); //remove the QMenu from the parent
				}
				mMenuList.RemoveAt(mMenuList.FindIndex(i));
			}
			else
			{
				curMenu.mMenu->DestroyMenu(); //clear the QMenu
			}
			break;
		}
	}
	Bridge::getBridge()->setResult();
}

void CMainFrame::uninstallPluginMenu()
{
	for (int i = 0; i < mMenuList.GetSize(); i++)
	{
		CMenu* pMenu = mMenuList.GetAt(mMenuList.FindIndex(i)).mMenu;
		
		if (mMenuList.GetAt(mMenuList.FindIndex(i)).bNew)
		{
			pMenu->DestroyMenu();
			delete pMenu;
			TRACE("HEAP: Free Heap block %p is freed \n", &pMenu);
		}
	}

	CMenu defaultMenu;
	defaultMenu.LoadMenu(IDR_MAINFRAME);
	m_wndMenuBar.CreateFromMenu(defaultMenu);
	globalMenu.DestroyMenu();
}

void CMainFrame::removeMenuEntry(int hEntryMenu)
{
	//find and remove the hEntryMenu from the mEntryList
	POSITION pos = mEntryList.GetHeadPosition();
	for (int i = 0; i < mEntryList.GetCount(); i++)
	{
		auto entry = mEntryList.GetNext(pos);
		if (entry.hEntry == hEntryMenu)
		{
			auto parentMenu = findMenu(entry.hParentMenu);
			if (parentMenu)
			{
				parentMenu->mMenu->DestroyMenu();
				
				mEntryList.RemoveAt(pos);
			}
			Bridge::getBridge()->setResult();
			return;
		}
	}
	//if hEntryMenu is not in mEntryList, clear+erase it from mMenuList
	clearMenu(hEntryMenu, true);
}

void CMainFrame::dbgStateChanged(DBGSTATE state)
{
	CString text;
	COLORREF textColor;
	COLORREF textBackgroundColor;
	switch (state)
	{
	case initialized:
		text = _T("初始化中");
		textColor = 0x0000FF;
		textBackgroundColor = 0xC0C0C0;
		break;
	case paused:
		text = _T("暂停");
		textColor = 0x0000FF;
		textBackgroundColor = 0x00FFFF;
		break;
	case running:
		text = _T("运行中");
		textColor = 0x000000;
		textBackgroundColor = 0x00FF00;
		break;
	case stopped:
		SetWindowText(AfxGetApp()->m_pszExeName);
		SetTitle(AfxGetApp()->m_pszExeName);
		m_wndDisasmPane.getCDisassembly()->disassembleClear();
		
		m_wndMemoryPane.getCCPUDump()->HexDumpClear();
		m_wndCPUStackPane.getCCPUStack()->HexDumpClear();
		text = _T("终止");
		textColor = 0x000000;
		textBackgroundColor = 0x0000FF;
		break;
	default:
		break;		
	}
	m_wndToolBar.getLabelAttribute().m_text = text;
	m_wndToolBar.getLabelAttribute().m_textColor = textColor;
	m_wndToolBar.getLabelAttribute().m_backgroundColor = textBackgroundColor;
	
	int index = m_wndToolBar.CommandToIndex(ID_DEBUG_STATUS);	
	m_wndToolBar.InvalidateButton(index);	
}

void CMainFrame::addMsgToLog(const char* msg)
{
	//多字节字符集转UNICODE
	CString msg1(CA2W(msg, CP_UTF8));
	CTime t = CTime::GetCurrentTime();
	CString timeText = t.Format(_T(" %m-%d %H:%M:%S "));	

	int curPos = 0;
	CString	resToken = msg1.Tokenize(_T("\n"), curPos);
	while (resToken != _T(""))
	{
		CString singleMsg = timeText;
		singleMsg += resToken;
		
		m_wndLogPane.getCOutputBox()->m_wndOutput.SetCurSel(
			m_wndLogPane.getCOutputBox()->m_wndOutput.AddString(singleMsg));
		m_wndStatusBar.SetPaneText(nStatusOutput, singleMsg);
	
		resToken = msg1.Tokenize(_T("\n"), curPos);
	}
	
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	lpCreateStruct->hMenu = NULL;
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	CString csTittle;
#ifdef _WIN64
	csTittle = _T("X-64");
#else
	csTittle = _T("X");	
#endif
	SetWindowText(csTittle);

	//当工具栏按钮为其他（如32位）位图时，防止菜单栏跟随其大小
	CMFCToolBar::SetMenuSizes(CSize(22, 22), CSize(16, 16));	
	
	//CleanState()或清注册表 更新选项卡状态
	CMDITabInfo	mdiTabParams;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_SCROLLED; // 其他可用样式...
	mdiTabParams.m_tabLocation = CMFCTabCtrl::LOCATION_TOP; //选项卡位于页面顶部
	mdiTabParams.m_bActiveTabCloseButton = TRUE;      // 指定是否在活动选项卡的标签上显示“ 关闭”按钮
	mdiTabParams.m_bTabIcons = TRUE;    // 设置为 TRUE 将在 MDI 选项卡上启用文档图标
	mdiTabParams.m_bAutoColor = TRUE;    // 指定是否为MDI选项卡着色
	mdiTabParams.m_bDocumentMenu = FALSE; // 如果为TRUE，显示选项卡区域右边缘的已打开文档列表; 否则，选项卡窗口将显示选项卡区域右边缘的滚动按钮。默认值为FALSE
	mdiTabParams.m_bEnableTabSwap = FALSE; //指定用户是否可以通过拖动交换选项卡的位置 默认值是true
	mdiTabParams.m_nTabBorderSize = 0;		//指定每个选项卡窗口的边框大小
	EnableMDITabbedGroups(TRUE, mdiTabParams);

	
	
	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("未能创建菜单栏\n");
		return -1;      // 未能创建
	}

	// Setup menu API
	initMenuApi();
	globalMenu.LoadMenu(IDR_MAINFRAME);
	globalMenu.AppendMenu(MF_SEPARATOR);
	CString buildData(__DATE__);
	buildData += ' ';
	buildData += __TIME__;
	globalMenu.AppendMenu(MF_STRING, globalMenu.GetMenuItemCount(), buildData);
	CMenu* pluginMenu = globalMenu.GetSubMenu(4);
	BOOL s = pluginMenu->AppendMenu(MF_STRING, ID_Plugin_Load_Scylla, _T("Scylla"));
	addMenuToList(this, pluginMenu, GUI_PLUGIN_MENU);

	

	//m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// 防止菜单栏在激活时获得焦点
	CMFCPopupMenu::SetForceMenuFocus(FALSE);	

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	
	m_wndToolBar.EnableLargeIcons(FALSE);
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC,
		CRect(1,1,1,1), 1) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME, 0, 0, TRUE /* Locked */))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndTabViewBar.Create(this, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, 2) ||
		!m_wndTabViewBar.LoadToolBar(IDR_TabView_TOOLBAR, 0, 0, TRUE /* Locked */))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	
	
	
	m_wndToolBar.SetBorders();
	CString strToolBarName;
	BOOL bNameValid;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	m_wndTabViewBar.SetWindowText(_T("Table View"));
	
	
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);//

	m_wndTabViewBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);//
	
	// 创建停靠窗口
	
	if (!CreateDockingWindows())
	{
		TRACE0("未能创建停靠窗口\n");
		return -1;
	}
		

	m_cmdBar.Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(10, 10, lpCreateStruct->cx, 32), this, NULL);
	
	if (!m_cmdReBar.Create(this, RBS_AUTOSIZE, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM) ||
		!m_cmdReBar.AddBar(&m_cmdBar))
	{
		TRACE0("Failed to create rebar\n");
		return -1;      // fail to create
	}

	
	EnableDocking(CBRS_ALIGN_ANY);	

	//停靠
	m_cmdReBar.EnableDocking(CBRS_BOTTOM);
	DockPane(&m_cmdReBar);
	
	m_wndMenuBar.EnableDocking(CBRS_TOP);
	DockPane(&m_wndMenuBar);	
	
	m_wndToolBar.EnableDocking(CBRS_TOP);
	DockPane(&m_wndToolBar);

	m_wndTabViewBar.EnableDocking(CBRS_TOP);
	DockPane(&m_wndTabViewBar);
	
	//固定
	//AddPane(&m_wndMenuBar);
	//AddPane(&m_wndToolBar);
	
	
	m_wndPeInfo.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndPeInfo);
	m_wndDisasmPane.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndDisasmPane);
	m_wndLogPane.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndLogPane);
	m_wndMemoryPane.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMemoryPane);
	m_wndCPUStackPane.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndCPUStackPane);
	m_wndRegistersPane.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndRegistersPane);
	m_wndBreakPointsManagePane.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndBreakPointsManagePane);
	m_wndInstructTracePane.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndInstructTracePane);
	m_wndHandlesPane.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndHandlesPane);
	m_wndReferencesPane.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndReferencesPane);
	
	

	// 基于持久值设置视觉管理器和样式	
	OnApplicationLook(theApp.m_nAppLook);
	// 启用 Visual Studio 2005 样式停靠窗口行为
	CDockingManager::SetDockingMode(DT_SMART);
	// 启用 Visual Studio 2005 ID_VIEW_CUSTOMIZE
	EnableAutoHidePanes(CBRS_ALIGN_ANY);
	// 启用工具栏和停靠窗口菜单替换
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改CREATESTRUCT来修改窗口类或样式
	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;
	CString strCaption;

	bNameValid = strCaption.LoadString(ID_PeInfo_DockPane);
	ASSERT(bNameValid);
	if (!m_wndPeInfo.Create(strCaption, this, CRect(100, 100, 200, 200), TRUE, ID_PeInfo_DockPane, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“PE信息”窗口\n");
		return FALSE; // 未能创建
	}

	bNameValid = strCaption.LoadString(ID_Disasm_DockPane);
	ASSERT(bNameValid);	
	if (!m_wndDisasmPane.Create(strCaption, this, CRect(40, 40, 200, 200), TRUE, ID_Disasm_DockPane, WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“反汇编”面板\n");
		return FALSE; // 未能创建
	}

	bNameValid = strCaption.LoadString(ID_Log_DockPane);
	ASSERT(bNameValid);
	if (!m_wndLogPane.Create(strCaption, this, CRect(40, 40, 200, 200), TRUE, ID_Log_DockPane, WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“足迹”面板\n");
		return FALSE; // 未能创建
	}

	bNameValid = strCaption.LoadString(ID_Memory_DockPane);
	ASSERT(bNameValid);
	if (!m_wndMemoryPane.Create(strCaption, this, CRect(40, 40, 200, 200), TRUE, ID_Memory_DockPane, WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“内存”面板\n");
		return FALSE; // 未能创建
	}

	bNameValid = strCaption.LoadString(ID_CallStack_DockPane);
	ASSERT(bNameValid);
	if (!m_wndCPUStackPane.Create(strCaption, this, CRect(40, 40, 200, 200), TRUE, ID_CallStack_DockPane, WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“栈”面板\n");
		return FALSE; // 未能创建
	}

	bNameValid = strCaption.LoadString(ID_Registers_DockPane);
	ASSERT(bNameValid);
	if (!m_wndRegistersPane.Create(strCaption, this, CRect(40, 40, 200, 200), TRUE, ID_Registers_DockPane, WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“栈”面板\n");
		return FALSE; // 未能创建
	}

	bNameValid = strCaption.LoadString(ID_BreakPointManage_DockPane);
	ASSERT(bNameValid);
	if (!m_wndBreakPointsManagePane.Create(strCaption, this, CRect(40, 40, 200, 200), TRUE, ID_BreakPointManage_DockPane, WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“断点管理”面板\n");
		return FALSE; // 未能创建
	}
	
	bNameValid = strCaption.LoadString(ID_InstructTrace_DockPane);
	ASSERT(bNameValid);
	if (!m_wndInstructTracePane.Create(strCaption, this, CRect(40, 40, 200, 200), TRUE, ID_InstructTrace_DockPane, WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“指令跟踪”面板\n");
		return FALSE; // 未能创建
	}

	bNameValid = strCaption.LoadString(ID_HandlesView_DockPane);
	ASSERT(bNameValid);
	if (!m_wndHandlesPane.Create(strCaption, this, CRect(40, 40, 200, 200), TRUE, ID_HandlesView_DockPane, WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“句柄”面板\n");
		return FALSE; // 未能创建
	}

	bNameValid = strCaption.LoadString(ID_References_DockPane);
	ASSERT(bNameValid);
	if (!m_wndReferencesPane.Create(strCaption, this, CRect(40, 40, 200, 200), TRUE, ID_References_DockPane, WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“句柄”面板\n");
		return FALSE; // 未能创建
	}


	//SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

void CMainFrame::ActivatePane(CDockablePane* pane)
{
	CString strPaneName;
	pane->GetPaneName(strPaneName);
	CMDIChildWnd* activateWnd = MDIGetActive();
	if (activateWnd != NULL)
		if (activateWnd->GetTitle() == strPaneName) //已激活，直接返回
			return;

	bool isInTab = false;
	HWND hwndT = ::GetWindow(m_hWndMDIClient, GW_CHILD);

	while (hwndT != NULL)
	{
		//CMDIChildWndEx* pFrame = DYNAMIC_DOWNCAST(CMDIChildWndEx, CWnd::FromHandle(hwndT));
		TCHAR szWndTitle[256];
		::GetWindowText(hwndT, szWndTitle, sizeof(szWndTitle) / sizeof(szWndTitle[0]));
		CString wndTitle = szWndTitle;
		if (wndTitle == strPaneName)
		{
			isInTab = true;
			::SendMessage(m_hWndMDIClient, WM_MDIACTIVATE, (WPARAM)hwndT, 0);
			break;
		}
		hwndT = ::GetWindow(hwndT, GW_HWNDNEXT);
	}
	// 如果不在Tab选项卡中
	if (!isInTab)
	{
		if (pane->IsVisible())
		{
			pane->SetFocus();
		}
		else
		{
			pane->ShowPane(!pane->IsVisible(), FALSE, TRUE);
			pane->SetFocus();
		}
	}	
}

void CMainFrame::OnToolBarButtonLook(UINT id)
{
	
	HWND D = m_wndDisasmPane.GetSafeHwnd();
	CWnd* KK = m_wndDisasmPane.FromHandle(m_wndDisasmPane.GetSafeHwnd());
	CMFCTabCtrl * tabCtrl = &this->GetMDITabs();
	CMFCBaseTabCtrl *pTab = (CMFCBaseTabCtrl *)this;
	
	
	m_nToolBarButtonLook = id - ID_BUTTON32798 + ID_Disasm_DockPane;
	
	switch (m_nToolBarButtonLook)
	{
	case ID_Disasm_DockPane:
		ActivatePane(&m_wndDisasmPane);
		break;

	case ID_PeInfo_DockPane:
		ActivatePane(&m_wndPeInfo);		
		break;

	case ID_CallStack_DockPane:
		ActivatePane(&m_wndCPUStackPane);
		break;

	case ID_Memory_DockPane:
		ActivatePane(&m_wndMemoryPane);
		break;

	case ID_Log_DockPane:
		ActivatePane(&m_wndLogPane);
		break;

	case ID_Registers_DockPane:
		ActivatePane(&m_wndRegistersPane);
		break;

	case ID_BreakPointManage_DockPane:
		ActivatePane(&m_wndBreakPointsManagePane);
		break;

	case ID_InstructTrace_DockPane:
		ActivatePane(&m_wndInstructTracePane);
		break;

	case ID_HandlesView_DockPane:
		ActivatePane(&m_wndHandlesPane);
		break;

	case ID_References_DockPane:
		ActivatePane(&m_wndReferencesPane);
		break;

	default:
		break;
	}
	
}

void CMainFrame::OnUpdateToolBarButtonLook(CCmdUI* pCmdUI)
{
	//pCmdUI->SetRadio(m_nToolBarButtonLook == pCmdUI->m_nID);
	/*
	switch (pCmdUI->m_nID)
	{
	case ID_Disasm_DockPane:
		pCmdUI->SetCheck(m_wndDisasmPane.IsVisible());
		break;

	case ID_PeInfo_DockPane:
		pCmdUI->SetCheck(m_wndPeInfo.IsVisible());
		break;

	case ID_CallStack_DockPane:
		pCmdUI->SetCheck(m_wndCPUStackPane.IsVisible());
		break;

	case ID_Memory_DockPane:
		pCmdUI->SetCheck(m_wndMemoryPane.IsVisible());
		break;

	case ID_Log_DockPane:
		pCmdUI->SetCheck(m_wndLogPane.IsVisible());
		break;

	case ID_Registers_DockPane:
		pCmdUI->SetCheck(m_wndRegistersPane.IsVisible());
		break;

	case ID_BreakPointManage_DockPane:
		pCmdUI->SetCheck(m_wndBreakPointsManagePane.IsVisible());
		break;

	default:
		break;
	}
	*/
	
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG

// CMainFrame 消息处理程序
BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// 否则，执行默认处理
	return CMDIFrameWndEx::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CMDIFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext);
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CMDIFrameWndEx::OnSize(nType, cx, cy);

	// TODO:  在此处添加消息处理程序代码
	if (m_cmdBar.GetSafeHwnd() && m_cmdBar.getCCmdEdit()->GetSafeHwnd())
		m_cmdBar.getCCmdEdit()->setPopupPos();
}

BOOL CMainFrame::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 在此添加专用代码和/或调用基类
	UINT dbgMessage = message - WM_USER;
	switch (dbgMessage)
	{
	case GUI_SET_DEBUG_STATE:
		dbgStateChanged((DBGSTATE)lParam);
		return TRUE;

	case GUI_UPDATE_REGISTER_VIEW:		
		m_wndRegistersPane.getCRegisters()->updateRegistersSlot();
		return TRUE;

	case GUI_DISASSEMBLE_AT:
		m_wndDisasmPane.getCDisassembly()->disassembleAt((dsint)wParam, (dsint)lParam);

	case GUI_UPDATE_DISASSEMBLY_VIEW:		
		m_wndDisasmPane.getCDisassembly()->reloadData();
		return TRUE;

	case GUI_DUMP_AT:
		m_wndMemoryPane.getCCPUDump()->printDumpAt((dsint)lParam);
		return TRUE;

	case GUI_STACK_DUMP_AT:
		m_wndCPUStackPane.getCCPUStack()->stackDumpAt((duint)wParam, (duint)lParam);
		return TRUE;

	case GUI_ADD_MSG_TO_LOG:
		addMsgToLog((const char*)wParam);
		return TRUE;

	case SB_SETTEXT:
		m_wndLogPane.getCOutputBox()->m_wndOutput.SetCurSel(
			m_wndLogPane.getCOutputBox()->m_wndOutput.AddString((LPCTSTR&)wParam));
		return TRUE;

	case GUI_UPDATE_BREAKPOINTS_VIEW:
		m_wndBreakPointsManagePane.getCBreakPointsManage()->updateBreakpoints();
		return TRUE;

	case GUI_FOCUS_VIEW:
		switch ((duint)lParam)
		{
		case GUI_DISASSEMBLY:
			ActivatePane(&m_wndDisasmPane);
			break;
		case GUI_DUMP:
			ActivatePane(&m_wndMemoryPane);
			break;
		default:
			break;
		}
		return TRUE;

	case GUI_OPEN_TRACE_FILE:
		m_wndInstructTracePane.getCInstructTrace()->open((CString&)wParam);
		return TRUE;

	case GUI_UPDATE_TRACE_BROWSER:
		m_wndInstructTracePane.getCInstructTrace()->update();
		return TRUE;

	case GUI_MENU_ADD:
		addMenu(lParam, (CString&)wParam);
		return TRUE;

	case GUI_MENU_ADD_ENTRY:
		addMenuEntry(lParam, (CString&)wParam);
		return TRUE;

	case GUI_ADD_INFO_LINE:
		m_wndDisasmPane.getCCPUInfoBox()->addInfoLine((CString&)wParam);
		return TRUE;

	case Signal_SelectionChange_Msg:
		m_wndDisasmPane.getCCPUInfoBox()->disasmSelectionChanged((dsint)lParam);
		return TRUE;

	case Signal_DisplayReferencesPane_Msg:
		ActivatePane(&m_wndReferencesPane);
		return TRUE;

	case Signal_SetSideBarRow_Msg:
		m_wndDisasmPane.getCCPUSideBar()->setViewableRows(lParam);
		return TRUE;

	case GUI_REF_INITIALIZE:
		m_wndReferencesPane.newReferenceView((CString&)wParam);
		Bridge::getBridge()->setResult(1);
		return TRUE;

	case GUI_REF_ADDCOLUMN:
		if (IsWindow(m_wndReferencesPane.getCurrentReferenceView()->GetSafeHwnd()))
			m_wndReferencesPane.getCurrentReferenceView()->addColumnAtRef((int)lParam, (CString&)wParam);
		return TRUE;

	case GUI_REF_RELOADDATA:
		if (IsWindow(m_wndReferencesPane.getCurrentReferenceView()->GetSafeHwnd()))
			m_wndReferencesPane.getCurrentReferenceView()->reloadData();
		return TRUE;

	case GUI_REF_SETCELLCONTENT:
		{
			CELLINFO* info = (CELLINFO*)lParam;
			if (IsWindow(m_wndReferencesPane.getCurrentReferenceView()->GetSafeHwnd()))
				m_wndReferencesPane.getCurrentReferenceView()->setCellContent(info->row, info->col, (CString)CA2T(info->str, CP_UTF8));
			return TRUE;
		}
		
	case GUI_REF_SETROWCOUNT:
		if (IsWindow(m_wndReferencesPane.getCurrentReferenceView()->GetSafeHwnd()))
				m_wndReferencesPane.getCurrentReferenceView()->setRowCount((dsint)lParam);
		return TRUE;

	case GUI_REF_SETPROGRESS:
		if (IsWindow(m_wndReferencesPane.getCurrentReferenceView()->GetSafeHwnd()))
			m_wndReferencesPane.getCurrentReferenceView()->referenceSetProgress((dsint)lParam);
		return TRUE;

	case GUI_REF_SETCURRENTTASKPROGRESS:
	{
		if (IsWindow(m_wndReferencesPane.getCurrentReferenceView()->GetSafeHwnd()))
			m_wndReferencesPane.getCurrentReferenceView()->referenceSetCurrentTaskProgress((dsint)lParam, (CString&)wParam);
		return TRUE;
	}

	case GUI_AUTOCOMPLETE_ADDCMD:
		m_cmdBar.getCCmdEdit()->autoCompleteAddCmd((CString&)wParam);
		return TRUE;

	case GUI_REGISTER_SCRIPT_LANG:		
		m_cmdBar.getCCmdEdit()->registerScriptType((SCRIPTTYPEINFO*)lParam);
		return TRUE;

	case GUI_UNREGISTER_SCRIPT_LANG:
		m_cmdBar.getCCmdEdit()->unregisterScriptType((int)lParam);
		return TRUE;
		
	case GUI_MENU_CLEAR:
		clearMenu(wParam, (bool)lParam);
		return TRUE;

	case GUI_MENU_REMOVE:
		removeMenuEntry(lParam);
		return TRUE;
		
	

	default:
		return CMDIFrameWndEx::OnWndMsg(message, wParam, lParam, pResult);
	}
}

afx_msg LRESULT CMainFrame::OnToolbarReset(WPARAM wp, LPARAM lp)
{
	UINT uiToolBarId = (UINT)wp;
	CString text = (LPCTSTR)lp;

	if (uiToolBarId == IDR_MAINFRAME)
	{
		/*
		for (UINT uiCmd = ID_Disasm_DockPane; uiCmd <= ID_BreakPointManage_DockPane; uiCmd++)
		{
			CString title;
			title.LoadString(uiCmd);
			//m_wndToolBar.ReplaceButton(uiCmd, CToolbarLabel(uiCmd, title, 0x000000, 0x00FFFF));
			m_wndToolBar.SetToolBarBtnText(m_wndToolBar.CommandToIndex(uiCmd), title, TRUE, FALSE);
		}		
		*/

		/*
		CMenu hMenu;
		hMenu.CreateMenu();

		for (UINT uiCmd = ID_Disasm_DockPane; uiCmd <= ID_InstructTrace_DockPane; uiCmd++)
		{
			BOOL bNameValid;
			CString title;
			bNameValid = title.LoadString(uiCmd);
			ASSERT(bNameValid);
			hMenu.AppendMenu(MF_STRING, uiCmd + User_Defined_Start, title);
		}

		CMFCToolBarMenuButton btnMenuRoot(-1, hMenu, 0, _T("视图"));
		btnMenuRoot.m_bText = TRUE;
		btnMenuRoot.m_bImage = FALSE;
		btnMenuRoot.m_nStyle |= TBBS_DROPDOWN;
		btnMenuRoot.m_nStyle &= ~TBBS_BUTTON;

		m_wndToolBar.ReplaceButton(ID_VIEW_DROP, btnMenuRoot);
		*/
		

		text.LoadString(ID_DEBUG_STATUS);
		m_wndToolBar.ReplaceButton(ID_DEBUG_STATUS,
			CToolbarLabel(ID_DEBUG_STATUS, text, 0x000000, 0x00FFFF));
	}

	if (uiToolBarId == IDR_TabView_TOOLBAR)
	{
		for (UINT uiCmd = 0; uiCmd <= (ID_References_DockPane - ID_Disasm_DockPane); uiCmd++)
		{
			CString title;
			title.LoadString(ID_Disasm_DockPane + uiCmd);
			m_wndTabViewBar.SetToolBarBtnText(uiCmd, title, TRUE, TRUE);
		}
	}
	return 0;
}


void CMainFrame::OnChildActivate()
{
	CMDIFrameWndEx::OnChildActivate();

	// TODO: 在此处添加消息处理程序代码
}


void CMainFrame::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
	CMDIFrameWndEx::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);

	// TODO: 在此处添加消息处理程序代码
}


BOOL CMainFrame::OnNcActivate(BOOL bActive)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return CMDIFrameWndEx::OnNcActivate(bActive);
}


void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值	
	UINT nCount;
	TCHAR szPath[MAX_PATH] = { 0 };

	nCount = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
	if (nCount)
	{
		for (UINT nIndex = 0; nIndex < nCount; ++nIndex)
		{
			DragQueryFile(hDropInfo, nIndex, szPath, _countof(szPath));
			CString cmd;
			cmd.Format(_T("init \"%s\""), szPath);
			DbgCmdExec(CT2A(cmd, CP_UTF8));
			break;
		}
	}

	DragFinish(hDropInfo);
	
	
	CMDIFrameWndEx::OnDropFiles(hDropInfo);
}


void CMainFrame::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值	

	static volatile bool bExecuteThread = true, bExecuteTimer = true;

	if (bExecuteThread)
	{
		if (DbgIsDebugging())
		{
			if (MessageBox(_T("调试器正在运行中，坚持退出？"), _T("提示："), MB_ICONQUESTION | MB_OKCANCEL) != IDOK)
				return;
		}
		m_closeDlg.Create(IDD_Close_Dlg, this);
		//m_closeDlg.ShowWindow(SW_SHOW);
		//m_closeDlg.GetDlgItem(IDC_HistText_STATIC)->SetWindowText(_T("调试器退出中..."));
		bExecuteThread = false;
		//CloseSnowman(mSnowmanView);
		Sleep(100);
		CWinThread* pDebugThread = AfxBeginThread(RUNTIME_CLASS(CDebugThreadClose));
		//emit Bridge::getBridge()->close();
	}

	
	if (!bExecuteThread && bExecuteTimer)
	{
		bExecuteTimer = false;
		SetTimer(1, 100, NULL);	
		return;
	}

	KillTimer(1);
	
	
	/*
	int i = 500;
	
	while (i)
	{
		if (bcanClose)
		{
			bNormalExit = true;
			break;
		}
		Sleep(10);
		i--;
	}
	*/

	delete Bridge::getBridge();
	if (!bRequestClose)
	{
		m_closeDlg.GetDlgItem(IDC_HistText_STATIC)->SetWindowText(_T("超时或强制退出中..."));
		Sleep(500);
	}
	m_closeDlg.DestroyWindow();	

	uninstallPluginMenu();

	CMDIFrameWndEx::OnClose();
}


void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
	case  1:
		if (bcanClose)
		{
			MSG msg;
			if (!PeekMessage(&msg, this->GetSafeHwnd(), WM_USER + GUI_DISASSEMBLE_AT, WM_USER + GUI_INVALIDATE_SYMBOL_SOURCE, PM_NOREMOVE))
			{
				bRequestClose = true;
				PostMessage(WM_CLOSE);
			}
		}	
	break;		
	
	default:
		CMDIFrameWndEx::OnTimer(nIDEvent);
		break;
	}	
}


void CMainFrame::OnMove(int x, int y)
{
	CMDIFrameWndEx::OnMove(x, y);

	// TODO: 在此处添加消息处理程序代码
	if (m_cmdBar.GetSafeHwnd())
		m_cmdBar.getCCmdEdit()->setPopupPos();
}


void CMainFrame::OnMoving(UINT fwSide, LPRECT pRect)
{
	CMDIFrameWndEx::OnMoving(fwSide, pRect);

	// TODO: 在此处添加消息处理程序代码
}


BOOL CMainFrame::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CMDIFrameWndEx::OnNotify(wParam, lParam, pResult);
}


void CMainFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	// TODO: 在此添加专用代码和/或调用基类

	CMDIFrameWndEx::OnUpdateFrameTitle(bAddToTitle);
}
