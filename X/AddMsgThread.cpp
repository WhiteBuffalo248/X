// AddMsgThread.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "X.h"
#include "AddMsgThread.h"
#include "MainFrm.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// CAddMsgThread

IMPLEMENT_DYNCREATE(CAddMsgThread, CWinThread)

CAddMsgThread::CAddMsgThread()
{
}

CAddMsgThread::~CAddMsgThread()
{
}

BOOL CAddMsgThread::InitInstance()
{
	// TODO:    �ڴ�ִ���������̳߳�ʼ��
	return TRUE;
}

int CAddMsgThread::ExitInstance()
{
	// TODO:    �ڴ�ִ���������߳�����
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CAddMsgThread, CWinThread)
	ON_THREAD_MESSAGE(WM_USER + 101, onParserMsg)
END_MESSAGE_MAP()


// CAddMsgThread ��Ϣ�������

void CAddMsgThread::onParserMsg(WPARAM wParam, LPARAM lParam)
{
	CTime t = CTime::GetCurrentTime();
	CString text = t.Format(" %m-%d %H:%M:%S ");

	//CString copyMsg((char *)wParam);
	CString copyMsg((LPCTSTR)wParam);
	CString findStr = _T("\n");

	while (!copyMsg.IsEmpty())
	{
		CString extractMsg;
		CString remainingMsg;
		CString finalMsg = text;
		int len = findStr.GetLength();
		int index = copyMsg.FindOneOf(findStr);

		if (index != -1)
		{
			extractMsg = copyMsg.Left(index);
			remainingMsg = copyMsg.Mid(index + len);
			finalMsg += extractMsg;
			msgArray.push_back(finalMsg);
			copyMsg = remainingMsg;
		}
		else
		{
			finalMsg += copyMsg;
			copyMsg = "";
			msgArray.push_back(finalMsg);
		}
	}

	CMainFrame* pMain(nullptr);
	pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	if (pMain->GetSafeHwnd())
	{
		for (auto i : msgArray)
		{
			if (!pMain->GetSafeHwnd())
				break;
			pMain->SendMessage(WM_USER + SB_SETTEXT, (WPARAM)(LPCTSTR)i, 0);
			SendMessage(pMain->getCMyStatuBar()->m_hWnd, SB_SETTEXT + WM_USER, nStatusOutput, (LPARAM)(LPCTSTR)i);
		}
		msgArray.clear();
	}	
	delete[](char*)wParam;
}


BOOL CAddMsgThread::OnIdle(LONG lCount)
{
	// TODO: �ڴ����ר�ô����/����û���
	
	return CWinThread::OnIdle(lCount);
}
