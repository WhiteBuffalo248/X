#pragma once



// CAddMsgThread

class CAddMsgThread : public CWinThread
{
	std::vector<CString> msgArray;

	DECLARE_DYNCREATE(CAddMsgThread)

protected:
	CAddMsgThread();           // 动态创建所使用的受保护的构造函数
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


