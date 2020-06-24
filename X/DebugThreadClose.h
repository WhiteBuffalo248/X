#pragma once



// CDebugThreadClose
extern bool bcanClose;

class CDebugThreadClose : public CWinThread
{
	DECLARE_DYNCREATE(CDebugThreadClose)

protected:
	CDebugThreadClose();           // 动态创建所使用的受保护的构造函数
	virtual ~CDebugThreadClose();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual int Run();
};


