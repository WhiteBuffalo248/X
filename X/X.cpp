
// X.cpp : ����Ӧ�ó��������Ϊ��
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


// CXApp ����

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

	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// ���Ӧ�ó��������ù�����������ʱ֧��(/clr)�����ģ���: 
	//     1) �����д˸������ã�������������������֧�ֲ�������������
	//     2) ��������Ŀ�У������밴������˳���� System.Windows.Forms ������á�
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: ������Ӧ�ó��� ID �ַ����滻ΪΨһ�� ID �ַ�����������ַ�����ʽ
	//Ϊ CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("X.AppID.NoVersion"));

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}

CXApp::~CXApp()
{
#ifdef CRT_DEBUG_TRACE
	//End_Debug_Trace();
#endif
}

// Ψһ��һ�� CXApp ����
CXApp theApp;


// CXApp ��ʼ��

BOOL CXApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()��  ���򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// ��ʼ�� OLE ��
	if (!AfxOleInit())
	{
	AfxMessageBox(IDP_OLE_INIT_FAILED);
	return FALSE;
	}
	AfxEnableControlContainer();
	
	

	EnableTaskbarInteraction(FALSE);

	// ʹ�� RichEdit �ؼ���Ҫ AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));
	LoadStdProfileSettings(10);  // ���ر�׼ INI �ļ�ѡ��(���� MRU)

	// Initialize all Managers for usage. They are automatically constructed
	// if not yet present
	InitContextMenuManager();////��ȡ��������Ĳ˵��� �������һ�Σ������һ�DockPane������ѡ��˵�
	InitKeyboardManager();
	InitTooltipManager();

	HANDLE handleX = GetModuleHandle(NULL);

	// ��Ҫ���������ڣ��˴��뽫�����µĿ�ܴ���
	// ����Ȼ��������ΪӦ�ó���������ڶ���
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;

	// ���������ؿ�ܼ�����Դ
	pFrame->LoadFrame(IDR_MAINFRAME);	
	
	// Set some data
	Bridge::getBridge()->hWinMain = (void*)pFrame->GetSafeHwnd();
	
	InitDebugEnvironment();

	//�滻Ĭ�ϲ˵�
	//BOOL s = SetMenu(pFrame->GetSafeHwnd(), pFrame->globalMenu);
	//����globalMenu�˵�
	pFrame->m_wndMenuBar.CreateFromMenu(pFrame->globalMenu);

	DragAcceptFiles(pFrame->GetSafeHwnd(), true);

	pFrame->LoadMDIState(GetRegSectionPath());
	
	// Ψһ��һ�������ѳ�ʼ���������ʾ����������и���
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();	
	return TRUE;
}

// CXApp ��Ϣ�������


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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

// �������жԻ����Ӧ�ó�������
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

// CXApp ��Ϣ�������

void CXApp::PreLoadState()
{
	// TODO: �ڴ����ר�ô����/����û���
	//������״̬
	//CleanState();
	CWinAppEx::PreLoadState();
}

BOOL CXApp::SaveAllModified()
{
	// TODO: �ڴ����ר�ô����/����û���
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
	// TODO:  �ڴ����ר�ô����/����û���

	return CWinAppEx::Run();
}

BOOL CXApp::InitApplication()
{
	// TODO:  �ڴ����ר�ô����/����û���

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

	//����ͨ�ſ�
	
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

	//...����
	mConfiguration->save();
	BridgeEnd();
	delete mConfiguration;
	
	
	//TODO: �����������ӵĸ�����Դ
	AfxOleTerm(FALSE);
	return CWinAppEx::ExitInstance();
}

void CXApp::OnExeFileOpen()
{
	//ѡ��Ҫ�򿪵�EXE�ļ�
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
//���PE�ļ�
BOOL CXApp::CheckFileMapping()
{
	HANDLE hFile = NULL;
	//���ļ�����ļ����
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
	//�����ļ�ӳ��
	hFileMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (hFileMap == NULL)
	{
		OutputDebugString(_T("CreateFileMapping Fail!"));
		DWORD dwErrorCode = 0;
		dwErrorCode = GetLastError();
		CloseHandle(hFile);
		return FALSE;
	}
	//ӳ���ļ�
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
	//�ж�PE��Ч��
	PIMAGE_DOS_HEADER pDos = NULL;
	pDos = (PIMAGE_DOS_HEADER)pFile;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pFile + pDos->e_lfanew);
	//���MZ PE ������־
	if (pDos->e_magic != IMAGE_DOS_SIGNATURE || pNt->Signature != IMAGE_NT_SIGNATURE)
	{
		AfxMessageBox(_T("��Ч��PE�ļ�"));
		CloseHandle(hFile);
		CloseHandle(hFileMap);
		return FALSE;
	}
	if (pNt->FileHeader.Characteristics & IMAGE_FILE_DLL)
	{
		AfxMessageBox(_T("�ļ�Ϊ��̬���ӿ�"));
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
	// TODO:  �ڴ���������������
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
			if (MessageBox(NULL, _T("�ļ������ڣ��Ƿ�ɾ����"), _T("����"), MB_YESNO) == IDYES)
				m_pRecentFileList->Remove(nIndex);	
}

void CXApp::OnDebugRun()
{
	// TODO: �ڴ���������������
	if (DbgIsDebugging())
		DbgCmdExec("run");
	else
		OnDebugReload();
}


void CXApp::OnDebugReload()
{
	// TODO: �ڴ���������������
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
	CTraceConditionDlg traceConditionDlg(_T("���ٲ�������"), _T("TraceIntoConditional"), NULL);
	traceConditionDlg.DoModal();
}

void CXApp::OnTraceStepPass()
{
	CTraceConditionDlg traceConditionDlg(_T("���ٲ�������"), _T("TraceOverConditional"), NULL);
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
	// TODO: �ڴ����ר�ô����/����û���

	return CWinAppEx::PreTranslateMessage(pMsg);
}


void CXApp::AddToRecentFileList(LPCTSTR lpszPathName)
{
	// TODO: �ڴ����ר�ô����/����û���

	CWinAppEx::AddToRecentFileList(lpszPathName);
}


BOOL CXApp::SaveState(LPCTSTR lpszSectionName, CFrameImpl* pFrameImpl)
{
	// TODO:  �ڴ����ר�ô����/����û���
	CMainFrame* pFrame = (CMainFrame*)m_pMainWnd;

	//pFrame->globalMenu.LoadMenu(IDR_MAINFRAME);
	

	return CWinAppEx::SaveState(lpszSectionName, pFrameImpl);
}
