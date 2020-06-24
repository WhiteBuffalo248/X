#ifndef BRIDGE_H
#define BRIDGE_H

#include "Imports.h"










class Bridge
{
	friend class BridgeResult;

public:
	Bridge();
	~Bridge();
	static Bridge* getBridge();
	static void initBridge();
public:
	//Public variables
	bool mIsRunning = false;
	duint mLastCip = 0;
	void* hWinMain = nullptr;

	// Misc functions
	static bool CopyToClipboard(CString & text);
	//result function
	void setResult(dsint result = 0);
	//helper functions
	void setDbgStopped();
	// Message processing function
	void* processMessage(GUIMSG type, void* param1, void* param2);

private:

	CWinThread *m_pAddMsgThrd;
	volatile bool dbgStopped = false;
	CRITICAL_SECTION csBridge;
	HANDLE hResultEvent;
	DWORD dwMainThreadId = 0;
	dsint bridgeResult = 0;
};

#endif // BRIDGE_H