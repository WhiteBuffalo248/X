
// X.h : X 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号
#include "../bridge/bridgemain.h"



// CXApp:
// 有关此类的实现，请参阅 X.cpp
//

class CXApp : public CWinAppEx
{
public:
	CXApp();
	~CXApp();
public:
	_CrtMemState s1;
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;
	TCHAR	m_szExefilePath[MAX_PATH];
	void LoadStdProfileSettings(UINT nMaxMRU);
	bool InitDebugEnvironment();
	BOOL	CheckFileMapping(void);

	// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual void PreLoadState();
	virtual BOOL SaveAllModified();	
	virtual int Run();
	virtual BOOL InitApplication();

	// 实现	

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnAppAbout();
	afx_msg void OnExeFileOpen();
	afx_msg void OnOpenRecentFile(UINT id);
	afx_msg void OnDebugRun();
	afx_msg void OnDebugReload();
	afx_msg void OnOptionsArg();
	afx_msg void OnDbgCmdExec(UINT id); 
	afx_msg void OnTraceStepInto();
	afx_msg void OnTraceStepPass();
	afx_msg void onAnimateInto();
	afx_msg void onAnimateOver();
	afx_msg void OnTestDestroyMeau();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void AddToRecentFileList(LPCTSTR lpszPathName);
	virtual BOOL SaveState(LPCTSTR lpszSectionName = NULL, CFrameImpl* pFrameImpl = NULL);
};

extern CXApp theApp;
