#pragma once



// CDebugThreadClose
extern bool bcanClose;

class CDebugThreadClose : public CWinThread
{
	DECLARE_DYNCREATE(CDebugThreadClose)

protected:
	CDebugThreadClose();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CDebugThreadClose();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual int Run();
};


