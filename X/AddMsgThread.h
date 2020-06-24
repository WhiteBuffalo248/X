#pragma once



// CAddMsgThread

class CAddMsgThread : public CWinThread
{
	std::vector<CString> msgArray;

	DECLARE_DYNCREATE(CAddMsgThread)

protected:
	CAddMsgThread();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CAddMsgThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	
protected:	
	DECLARE_MESSAGE_MAP()
	afx_msg void onParserMsg(WPARAM wParam, LPARAM lParam);
	
public:
	virtual BOOL OnIdle(LONG lCount);
};


