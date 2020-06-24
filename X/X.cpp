
// X.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "X.h"
#include "MainFrm.h"
#include <Tlhelp32.h>

#include "LoadResourceString.h"
#include "MyRecentFileList.h"
#include "OptionDlg.h"
#include "TraceConditionDlg.h"
#include "Bridge.h"
#include "SettingsX.h"
#include "Configuration.h"
#include "CTR_DBG2.h"

#ifdef _DEBUG_
#define _CRTDBG_MAP_ALLOC
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

static Configuration* mConfiguration;
// CXApp

BEGIN_MESSAGE_MAP(CXApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CXApp::OnAppAbout)	
	ON_COMMAND(ID_OPTIONS_ARG, &CXApp::OnOptionsArg)
	ON_COMMAND(ID_FILE_OPEN, &CXApp::OnExeFileOpen)
	ON_COMMAND(ID_DEBUG_RUN, &CXApp::OnDebugRun)
	ON_COMMAND(ID_DEBUG_RELOAD, &CXApp::OnDebugReload)
	ON_COMMAND(ID_DEBUG_AUTO_TRACE_STEP_PASS, &CXApp::onAnimateOver)
	ON_COMMAND(ID_DEBUG_AUTO_TRACE_STEP_INTO, &CXApp::onAnimateInto)
	ON_COMMAND(ID_DEBUG_TRACE_STEP_INTO, &CXApp::OnTraceStepInto)
	ON_COMMAND(ID_DEBUG_TRACE_STEP_PASS, &CXApp::OnTraceStepPass)
	ON_COMMAND(ID_DEBUG_TEST, &CXApp::OnTestDestroyMeau)
	ON_COMMAND_RANGE(ID_DEBUG_RUN, ID_DEBUG_RUN_USER, &CXApp::OnDbgCmdExec)
	ON_COMMAND_RANGE(ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE16, &CXApp::OnOpenRecentFile)
END_MESSAGE_MAP()


// CXApp 构造

CXApp::CXApp()
{
#ifdef CRT_DEBUG_TRACE
	Init_Debug_Trace();
#endif

#ifdef VLD_DEBUG_TRACE
	int options = VLDGetOptions();
#endif
	TCHAR szFileName[MAX_PATH] = _T("");
	TCHAR m_szAddrExplain[MAX_PATH] = _T("");

	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 如果应用程序是利用公共语言运行时支持(/clr)构建的，则: 
	//     1) 必须有此附加设置，“重新启动管理器”支持才能正常工作。
	//     2) 在您的项目中，您必须按照生成顺序向 System.Windows.Forms 添加引用。
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("X.AppID.NoVersion"));

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

CXApp::~CXApp()
{
#ifdef CRT_DEBUG_TRACE
	//End_Debug_Trace();
#endif
}

// 唯一的一个 CXApp 对象
CXApp theApp;


// CXApp 初始化

BOOL CXApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// 初始化 OLE 库
	if (!AfxOleInit())
	{
	AfxMessageBox(IDP_OLE_INIT_FAILED);
	return FALSE;
	}
	AfxEnableControlContainer();
	
	

	EnableTaskbarInteraction(FALSE);

	// 使用 RichEdit 控件需要 AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	LoadStdProfileSettings(10);  // 加载标准 INI 文件选项(包括 MRU)

	// Initialize all Managers for usage. They are automatically constructed
	// if not yet present
	InitContextMenuManager();////获取框架上下文菜单， 必须调用一次，否则右击DockPane不弹出选项卡菜单
	InitKeyboardManager();
	InitTooltipManager();

	HANDLE handleX = GetModuleHandle(NULL);

	// 若要创建主窗口，此代码将创建新的框架窗口
	// 对象，然后将其设置为应用程序的主窗口对象
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;

	// 创建并加载框架及其资源
	pFrame->LoadFrame(IDR_MAINFRAME);	
	
	// Set some data
	Bridge::getBridge()->hWinMain = (void*)pFrame->GetSafeHwnd();
	
	InitDebugEnvironment();

	//替换默认菜单
	//BOOL s = SetMenu(pFrame->GetSafeHwnd(), pFrame->globalMenu);
	//加载globalMenu菜单
	pFrame->m_wndMenuBar.CreateFromMenu(pFrame->globalMenu);

	DragAcceptFiles(pFrame->GetSafeHwnd(), true);

	pFrame->LoadMDIState(GetRegSectionPath());
	
	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();	
	return TRUE;
}

// CXApp 消息处理程序


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CXApp::OnAppAbout()
{	
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();	
}

static const TCHAR _afxFileSection[] = _T("Recent File List");
static const TCHAR _afxFileEntry[] = _T("File%d");
static const TCHAR _afxPreviewSection[] = _T("Settings");
static const TCHAR _afxPreviewEntry[] = _T("PreviewPages");

void CXApp::LoadStdProfileSettings(UINT nMaxMRU)
{
	ASSERT_VALID(this);
	ASSERT(m_pRecentFileList == NULL);

	if (nMaxMRU != 0)
	{
		// create file MRU since nMaxMRU not zero

		// Here's the important part--overriding CRecentFileList
		m_pRecentFileList = new CMyRecentFileList(0, _afxFileSection, _afxFileEntry, nMaxMRU, _MAX_PATH);

		m_pRecentFileList->ReadList();
	}
	// 0 by default means not set
	m_nNumPreviewPages = GetProfileInt(_afxPreviewSection, _afxPreviewEntry, 0);
}

bool CXApp::InitDebugEnvironment()
{
	
	

	// Init debugger
	CString InitErrorMsg(DbgInit());
	if (!InitErrorMsg.IsEmpty())
	{
		MessageBox(0, InitErrorMsg, LoadResString(IDS_BRIDGE_INIT_ERR), MB_ICONERROR | MB_SYSTEMMODAL);
		return false;
	}
	return true;
}

// CXApp 消息处理程序

void CXApp::PreLoadState()
{
	// TODO: 在此添加专用代码和/或调用基类
	//清除框架状态
	//CleanState();
	CWinAppEx::PreLoadState();
}

BOOL CXApp::SaveAllModified()
{
	// TODO: 在此添加专用代码和/或调用基类
	if (!CWinAppEx::SaveAllModified())
	{
		return FALSE;
	}
	CMDIFrameWndEx* pMainFrame = DYNAMIC_DOWNCAST(CMDIFrameWndEx, m_pMainWnd);
	if (pMainFrame != NULL)
	{
		pMainFrame->SaveMDIState(GetRegSectionPath());
	}
	return TRUE;
	//return CWinAppEx::SaveAllModified();
}

int CXApp::Run()
{
	// TODO:  在此添加专用代码和/或调用基类

	return CWinAppEx::Run();
}

BOOL CXApp::InitApplication()
{
	// TODO:  在此添加专用代码和/或调用基类

	/*
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	int tmp;

	// Get the current bits
	tmp = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

	// Clear the upper 16 bits and OR in the desired freqency
	tmp = (tmp & 0x0000FFFF) | _CRTDBG_CHECK_EVERY_1024_DF;

	// Set the new bits
	_CrtSetDbgFlag(tmp);
	*/	

	//加载通信库
	
	const wchar_t* errormsg = BridgeInit();
	if (errormsg)
	{
		MessageBox(0, errormsg, LoadResString(IDS_BRIDGE_INIT_ERR), MB_ICONERROR | MB_SYSTEMMODAL);
		return false;
	}
	// Init communication with debugger	
	errormsg = BridgeStart();
	if (errormsg)
	{
		MessageBox(0, errormsg, LoadResString(IDS_BRIDGE_START_ERR), MB_ICONERROR | MB_SYSTEMMODAL);
		return false;
	}
	
	// load config file + set config font
	mConfiguration = new Configuration;
	// Set default setttings (when not set)
	SettingsX defaultSettings;
	//	lastException = 0;
	defaultSettings.SaveSettings();
	

	return CWinAppEx::InitApplication();
}

int CXApp::ExitInstance()
{

	//...结束
	mConfiguration->save();
	BridgeEnd();
	delete mConfiguration;
	
	
	//TODO: 处理可能已添加的附加资源
	AfxOleTerm(FALSE);
	return CWinAppEx::ExitInstance();
}

void CXApp::OnExeFileOpen()
{
	//选择要打开的EXE文件
	//TCHAR            szFileName[MAX_PATH] = _T("");	
	OPENFILENAME    ofn;

	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFile = m_szExefilePath;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = _T("Exe Files(*.exe)\0*.exe\0All Files(*.*)\0*.*\0\0");
	ofn.nFilterIndex = 1;
	if (GetOpenFileName(&ofn) == FALSE)
	{
		return;
	}
	/*
	if (!CheckFileMapping())
	return;
	*/
	
	OnOpenRecentFile(0);
}
//检查PE文件
BOOL CXApp::CheckFileMapping()
{
	HANDLE hFile = NULL;
	//打开文件获得文件句柄
	hFile = CreateFile(m_szExefilePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		OutputDebugString(_T("CreateFile Fail!"));
		DWORD dwErrorCode = 0;
		dwErrorCode = GetLastError();
		return FALSE;
	}
	DWORD dwFileSize = 0;
	dwFileSize = GetFileSize(hFile, NULL);
	HANDLE hFileMap = NULL;
	//创建文件映射
	hFileMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (hFileMap == NULL)
	{
		OutputDebugString(_T("CreateFileMapping Fail!"));
		DWORD dwErrorCode = 0;
		dwErrorCode = GetLastError();
		CloseHandle(hFile);
		return FALSE;
	}
	//映射文件
	DWORD pFile = NULL;
	pFile = (DWORD)MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 0);
	if (pFile == NULL)
	{
		OutputDebugString(_T("MapViewOfFile Fail!"));
		DWORD dwErrorCode = 0;
		dwErrorCode = GetLastError();
		CloseHandle(hFile);
		CloseHandle(hFileMap);
		return FALSE;
	}
	//判断PE有效性
	PIMAGE_DOS_HEADER pDos = NULL;
	pDos = (PIMAGE_DOS_HEADER)pFile;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pFile + pDos->e_lfanew);
	//检查MZ PE 两个标志
	if (pDos->e_magic != IMAGE_DOS_SIGNATURE || pNt->Signature != IMAGE_NT_SIGNATURE)
	{
		AfxMessageBox(_T("无效的PE文件"));
		CloseHandle(hFile);
		CloseHandle(hFileMap);
		return FALSE;
	}
	if (pNt->FileHeader.Characteristics & IMAGE_FILE_DLL)
	{
		AfxMessageBox(_T("文件为动态链接库"));
		CloseHandle(hFile);
		CloseHandle(hFileMap);
		return FALSE;
	}
	//ShowPeInfo(pFile, pFile + pDos->e_lfanew, dwFileSize);
	CloseHandle(hFile);
	CloseHandle(hFileMap);
	return TRUE;
}

string UnicodeToANSI(const wstring& str)
{
	char* pElementText;
	int iTextLen;
	// wide char to multi char
	iTextLen = WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, NULL, 0, NULL, NULL);
	pElementText = new char[iTextLen + 1];
	memset((void*)pElementText, 0, sizeof(char) * (iTextLen + 1));
	WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, pElementText, iTextLen, NULL, NULL);
	string strText;
	strText = pElementText;
	delete[] pElementText;
	return strText;
}

void CXApp::OnOpenRecentFile(UINT id)
{
	// TODO:  在此添加命令处理程序代码
	CString debugCMD;
	CString exeFilePath;
	int nIndex = -1;
	if (!id)
	{
		exeFilePath = m_szExefilePath;
		debugCMD.Format(_T("init \"%s\""), m_szExefilePath);
	}
	else
	{
		int size = m_pRecentFileList->GetSize();
		if (size)
		{
			nIndex = id - ID_FILE_MRU_FILE1;
			m_pRecentFileList->GetDisplayName(exeFilePath, nIndex, m_szExefilePath, MAX_PATH);
			if (!exeFilePath.IsEmpty())				
				debugCMD.Format(_T("init \"%s\""), exeFilePath.GetBuffer());
			else
			{
				nIndex = -1;
			}
		}		
	}
	if (PathFileExists(exeFilePath))
	{
		AddToRecentFileList(exeFilePath);
		DbgCmdExec(CT2A(debugCMD, CP_UTF8));
	}		
	else
		if (nIndex != -1)
			if (MessageBox(NULL, _T("文件不存在，是否删除？"), _T("错误"), MB_YESNO) == IDYES)
				m_pRecentFileList->Remove(nIndex);	
}

void CXApp::OnDebugRun()
{
	// TODO: 在此添加命令处理程序代码
	if (DbgIsDebugging())
		DbgCmdExec("run");
	else
		OnDebugReload();
}


void CXApp::OnDebugReload()
{
	// TODO: 在此添加命令处理程序代码
	CString exeFilePath;
	auto last = m_pRecentFileList->GetDisplayName(exeFilePath, 0, m_szExefilePath, MAX_PATH);
	if (!exeFilePath.IsEmpty())
	{
		CString debugCMD;
		debugCMD.Format(_T("init \"%s\""), exeFilePath);
		DbgCmdExec(CT2A(debugCMD, CP_UTF8));
	}		
}

void CXApp::onAnimateInto()
{
	if (DbgIsDebugging())
		DbgFunctions()->AnimateCommand("StepInto;AnimateWait");
}

void CXApp::onAnimateOver()
{
	if (DbgIsDebugging())
		DbgFunctions()->AnimateCommand("StepOver;AnimateWait");
}

void CXApp::OnTraceStepInto()
{
	CTraceConditionDlg traceConditionDlg(_T("跟踪步入条件"), _T("TraceIntoConditional"), NULL);
	traceConditionDlg.DoModal();
}

void CXApp::OnTraceStepPass()
{
	CTraceConditionDlg traceConditionDlg(_T("跟踪步过条件"), _T("TraceOverConditional"), NULL);
	traceConditionDlg.DoModal();
}

void CXApp::OnTestDestroyMeau()
{
	/*
	CMainFrame* pMain = (CMainFrame*)m_pMainWnd;
	pMain->removeMenuEntry(2002);
	*/
	
	CMainFrame* pMain = (CMainFrame*)m_pMainWnd;
	pMain->uninstallPluginMenu();
}

void CXApp::OnOptionsArg()
{
	COptionDlg optionsArgDlg(theApp.m_pMainWnd);
	optionsArgDlg.DoModal();

}

void CXApp::OnDbgCmdExec(UINT id)
{
	string command;
	switch (id)
	{
	case ID_DEBUG_PAUSE:
		command = "pause";
		break;
	case ID_DEBUG_CLOSE:
		command = "stop";
		break;
	case  ID_DEBUG_STEP_PASS:
		command = "StepOver";
		break;
	case  ID_DEBUG_STEP_INTO:
		command = "StepInto";
		break;

	default:
		break;
	}
	DbgCmdExec(command.data());
}


BOOL CXApp::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CWinAppEx::PreTranslateMessage(pMsg);
}


void CXApp::AddToRecentFileList(LPCTSTR lpszPathName)
{
	// TODO: 在此添加专用代码和/或调用基类

	CWinAppEx::AddToRecentFileList(lpszPathName);
}


BOOL CXApp::SaveState(LPCTSTR lpszSectionName, CFrameImpl* pFrameImpl)
{
	// TODO:  在此添加专用代码和/或调用基类
	CMainFrame* pFrame = (CMainFrame*)m_pMainWnd;

	//pFrame->globalMenu.LoadMenu(IDR_MAINFRAME);
	

	return CWinAppEx::SaveState(lpszSectionName, pFrameImpl);
}
