// DebugThreadClose.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "X.h"
#include "DebugThreadClose.h"
#include "Bridge.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// CDebugThreadClose
bool bcanClose = false;

IMPLEMENT_DYNCREATE(CDebugThreadClose, CWinThread)

CDebugThreadClose::CDebugThreadClose()
{
}

CDebugThreadClose::~CDebugThreadClose()
{
}

BOOL CDebugThreadClose::InitInstance()
{
	// TODO:    �ڴ�ִ���������̳߳�ʼ��
	DbgExit();
	Bridge::getBridge()->setDbgStopped();
	//emit canClose();
	MSG msg;
	HWND hwnd = GetMainWnd()->GetSafeHwnd();
	BOOL s = PeekMessage(&msg, GetMainWnd()->GetSafeHwnd(), WM_USER + GUI_DISASSEMBLE_AT, WM_USER + GUI_INVALIDATE_SYMBOL_SOURCE, PM_NOREMOVE);
	bcanClose = true;
	return FALSE;
	return TRUE;
}

int CDebugThreadClose::ExitInstance()
{
	// TODO:    �ڴ�ִ���������߳�����
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CDebugThreadClose, CWinThread)
END_MESSAGE_MAP()


// CDebugThreadClose ��Ϣ�������


int CDebugThreadClose::Run()
{
	// TODO: �ڴ����ר�ô����/����û���

	return CWinThread::Run();
	
}
