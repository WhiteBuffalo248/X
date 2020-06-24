
// X.h : X Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������
#include "../bridge/bridgemain.h"



// CXApp:
// �йش����ʵ�֣������ X.cpp
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

	// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual void PreLoadState();
	virtual BOOL SaveAllModified();	
	virtual int Run();
	virtual BOOL InitApplication();

	// ʵ��	

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
