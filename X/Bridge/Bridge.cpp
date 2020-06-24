#include "stdafx.h"
#include "Bridge.h"
#include "Exports.h"
#include "MainFrm.h"
#include "AddMsgThread.h"
#include "BridgeResult.h"


#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

/************************************************************************************
Global Variables
************************************************************************************/
static Bridge* mBridge;
/************************************************************************************
Class Members
************************************************************************************/
Bridge::Bridge()
{
	//m_pAddMsgThrd = AfxBeginThread(RUNTIME_CLASS(CAddMsgThread));
	InitializeCriticalSection(&csBridge);
	hResultEvent = CreateEventW(nullptr, true, true, nullptr);;
	dwMainThreadId = GetCurrentThreadId();
}

Bridge::~Bridge()
{
	/*
	m_pAddMsgThrd->PostThreadMessage(WM_QUIT, 0, 0);

	HANDLE hp = m_pAddMsgThrd->m_hThread;
	if (hp)
	{
		if (WaitForSingleObject(hp, 5000) != WAIT_OBJECT_0)
		{
			TerminateThread(hp, 0);
			CloseHandle(hp);
		}
	}
	*/
	CloseHandle(hResultEvent);
	EnterCriticalSection(&csBridge);
	DeleteCriticalSection(&csBridge);
}

bool Bridge::CopyToClipboard(CString & text)
{
	if (!text.GetLength())
		return false;
	if (OpenClipboard(NULL))
	{
		EmptyClipboard();
		int len = text.GetLength() + 1;
		HGLOBAL hglbCopy = GlobalAlloc(0, len * sizeof(TCHAR));
		if (hglbCopy == NULL)
		{
			CloseClipboard();
			return false;
		}
		LPTSTR lptstrCopy = (LPTSTR)GlobalLock(hglbCopy);
		memcpy(lptstrCopy, text.GetBuffer(),
			len * sizeof(TCHAR));
		lptstrCopy[len] = (TCHAR)0;    // null character 
		GlobalUnlock(hglbCopy);
		SetClipboardData(CF_UNICODETEXT, hglbCopy);
		CloseClipboard();
		GuiAddStatusBarMessage("The data has been copied to clipboard.\n");
		return true;
	}
	return false;
	
}

void Bridge::setResult(dsint result)
{
	bridgeResult = result;
	SetEvent(hResultEvent);
}

/************************************************************************************
Static Functions
************************************************************************************/
Bridge* Bridge::getBridge()
{
	return mBridge;
}

void Bridge::initBridge()
{
	mBridge = new Bridge();
}

/************************************************************************************
Helper Functions
************************************************************************************/

void Bridge::setDbgStopped()
{
	dbgStopped = true;
}

/************************************************************************************
Message processing
************************************************************************************/

void* Bridge::processMessage(GUIMSG type, void* param1, void* param2)
{
	if (dbgStopped) //there can be no more messages if the debugger stopped = IGNORE
		return nullptr;
	CMainFrame* pMain(nullptr);
	if (AfxGetApp())
		pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;

	if (!pMain->GetSafeHwnd())
		return nullptr;

	switch (type)
	{
	case GUI_DISASSEMBLE_AT:
		mLastCip = (duint)param2;
		//emit disassembleAt((dsint)param1, (dsint)param2)
		pMain->PostMessage(WM_USER + GUI_DISASSEMBLE_AT, (duint)param1, (duint)param2);
		break;

	case GUI_UPDATE_DISASSEMBLY_VIEW:
		//emit repaintGui();
		pMain->PostMessage(WM_USER + GUI_UPDATE_DISASSEMBLY_VIEW);
		break;

	case GUI_ADD_MSG_TO_LOG:
	{
		//emit addMsgToLog(QByteArray(msg, int(strlen(msg)) + 1)); //Speed up performance: don't convert to UCS-2 QString
		
		auto msg = (const char*)param1;
		/*
		int len = msg.GetLength();
		TCHAR* msgBuffer = new TCHAR[len + 1];
		StrCpy(msgBuffer, msg.GetBuffer()); //_tcscpy(msgBuffer, msg.GetBuffer());
		m_pAddMsgThrd->PostThreadMessage(WM_USER + 101, (WPARAM)msgBuffer, len);
		*/		
		
		pMain->SendMessage(WM_USER + GUI_ADD_MSG_TO_LOG, (LPARAM)msg);							
	}
	break;

	case GUI_ADD_MSG_TO_STATUSBAR:
	{
		CString msg(CA2W((const char*)param1, CP_UTF8));
		SendMessage(pMain->getCMyStatuBar()->m_hWnd, WM_USER + SB_SETTEXT, nStatusOutput, (LPARAM)(LPCTSTR)msg);
	}				
	break;

	case GUI_UPDATE_WINDOW_TITLE:		
	{	
		////emit updateWindowTitle(QString((const char*)param1));
		CString title(AfxGetApp()->m_pszAppName);
		title += " ";
		title += CA2W((const char*)param1, CP_UTF8);
		pMain->SetTitle(title);
		pMain->SetWindowTextW(title);
		CString title1(AfxGetApp()->m_pszExeName);
	}
	break;

	case GUI_SET_DEBUG_STATE:
		//emit dbgStateChanged((DBGSTATE)(dsint)param1);
		mIsRunning = DBGSTATE(duint(param1)) == running;
		if (!param2)
			pMain->PostMessage(WM_USER + GUI_SET_DEBUG_STATE, 0, (UINT)param1);						
	break;

	case GUI_DUMP_AT:
		pMain->PostMessage(WM_USER + GUI_DUMP_AT, 0, (duint)param1);
	break;

	case GUI_STACK_DUMP_AT:
		pMain->PostMessage(WM_USER + GUI_STACK_DUMP_AT , (duint)param1, (duint)param2);
		break;

	case GUI_UPDATE_REGISTER_VIEW:
		pMain->PostMessage(WM_USER + GUI_UPDATE_REGISTER_VIEW); //直接返回，不等消息结果		
		break;

	case GUI_UPDATE_BREAKPOINTS_VIEW:
		pMain->PostMessage(WM_USER + GUI_UPDATE_BREAKPOINTS_VIEW);
		break;

	case GUI_FOCUS_VIEW:
		pMain->PostMessage(WM_USER + GUI_FOCUS_VIEW, 0, (duint)param1);
		break;

	case GUI_CLOSE_APPLICATION:
		pMain->PostMessage(WM_CLOSE, 0, 0);//最常用
		break;

	case GUI_OPEN_TRACE_FILE:
	{
		if (param1 == nullptr)
			return nullptr;
		auto msg = (const char*)param1;
		CString filepath(CA2W((const char*)param1, CP_UTF8));
		pMain->SendMessage(WM_USER + GUI_OPEN_TRACE_FILE, (WPARAM)(LPCTSTR)filepath);
	}
	break;

	case GUI_UPDATE_TRACE_BROWSER:
		pMain->SendMessage(WM_USER + GUI_UPDATE_TRACE_BROWSER);
		break;

	case GUI_MENU_ADD:
	{
		//emit menuAddMenu((int)param1, QString((const char*)param2));
		auto msg = (const char*)param2;
		CString tittle(CA2W((const char*)param2, CP_UTF8));
		pMain->SendMessage(WM_USER + GUI_MENU_ADD, (WPARAM)(LPCTSTR)tittle, (int)param1);
	}
	break;

	case GUI_MENU_ADD_ENTRY:
	{
		//emit menuAddMenuEntry((int)param1, QString((const char*)param2));
		auto msg = (const char*)param2;
		CString tittle(CA2W((const char*)param2, CP_UTF8));
		pMain->SendMessage(WM_USER + GUI_MENU_ADD_ENTRY, (WPARAM)(LPCTSTR)tittle, (int)param1);
	}

	case GUI_GET_WINDOW_HANDLE:
		return hWinMain;
	break;

	case GUI_ADD_INFO_LINE:
	{
		if (param1)
		{
			//emit addInfoLine(QString((const char*)param1));
			CString infoText = CA2T((const char*)param2, CP_UTF8);
			pMain->SendMessage(WM_USER + GUI_ADD_INFO_LINE, (WPARAM)(LPCTSTR)infoText);
		}
	}
	break;

	case  GUI_REF_INITIALIZE:
	{
		BridgeResult result;
		CString name = CA2T((const char*)param1, CP_UTF8);
		pMain->SendMessage(WM_USER + GUI_REF_INITIALIZE, (WPARAM)(LPCTSTR)name);
		result.Wait();
	}
	break;

	case GUI_REF_ADDCOLUMN:
	{
		CString name = CA2T((const char*)param2, CP_UTF8);
		pMain->SendMessage(WM_USER + GUI_REF_ADDCOLUMN, (WPARAM)(LPCTSTR)name, (int)param1);
	}
	break;

	case GUI_REF_RELOADDATA:
		pMain->SendMessage(WM_USER + GUI_REF_RELOADDATA);
		break;

	case GUI_REF_SETCELLCONTENT:
	{
		CELLINFO* info = (CELLINFO*)param1;
		//emit referenceSetCellContent(info->row, info->col, QString(info->str));
		
		pMain->SendMessage(WM_USER + GUI_REF_SETCELLCONTENT, 0, (LPARAM)param1);
	}
	break;

	case GUI_REF_SETROWCOUNT:
		//emit referenceSetRowCount((dsint)param1);
		pMain->SendMessage(WM_USER + GUI_REF_SETROWCOUNT, 0, (LPARAM)param1);
		break;

	case GUI_REF_SETPROGRESS:
		//emit referenceSetProgress((int)param1);
		pMain->SendMessage(WM_USER + GUI_REF_SETPROGRESS, 0, (LPARAM)param1);
		break;

	case GUI_REF_SETCURRENTTASKPROGRESS:
	{
		//emit referenceSetCurrentTaskProgress((int)param1, QString((const char*)param2));
		CString labalText = CA2T((const char*)param2, CP_UTF8);
		pMain->SendMessage(WM_USER + GUI_REF_SETCURRENTTASKPROGRESS, (WPARAM)(LPCTSTR)labalText, (LPARAM)param1);
		break;
	}

	case GUI_GET_DISASSEMBLY:
	{
		duint parVA = (duint)param1;
		char* text = (char*)param2;
		if (!text || !parVA || !DbgIsDebugging())
			return 0;
		byte_t wBuffer[16];
		if (!DbgMemRead(parVA, wBuffer, 16))
			return 0;
		QBeaEngine disasm(int(ConfigUint("Disassembler", "MaxModuleSize")));
		Instruction_t instr = disasm.DisassembleAt(wBuffer, 16, 0, parVA);
		CString finalInstruction;
		for (const auto & curToken : instr.tokens.tokens)
			finalInstruction += curToken.text;
		strncpy_s(text, GUI_MAX_DISASSEMBLY_SIZE, CT2A(finalInstruction, CP_UTF8), _TRUNCATE);
		return (void*)1;
	}
	break;

	case GUI_AUTOCOMPLETE_ADDCMD:		
	{
		//emit autoCompleteAddCmd(QString((const char*)param1));
		CString cmd = CA2T((const char*)param1, CP_UTF8);
		pMain->SendMessage(WM_USER + GUI_AUTOCOMPLETE_ADDCMD, (WPARAM)(LPCTSTR)cmd);
	}		
	break;
	
	case GUI_REGISTER_SCRIPT_LANG:
	{
		//emit registerScriptLang((SCRIPTTYPEINFO*)param1);
		pMain->SendMessage(WM_USER + GUI_REGISTER_SCRIPT_LANG, 0, (LPARAM)param1);
	}
	break;

	case GUI_UNREGISTER_SCRIPT_LANG:
		//emit unregisterScriptLang((int)param1);
		pMain->SendMessage(WM_USER + GUI_UNREGISTER_SCRIPT_LANG, 0, (LPARAM)param1);
		break;
		/*
		case GUI_MENU_CLEAR:
		{
		BridgeResult result;
		pMain->SendMessage(WM_USER + GUI_MENU_CLEAR, 0, (LPARAM)param1);
		result.Wait();
		}
		break;

		case GUI_MENU_REMOVE:
		{
		BridgeResult result;
		pMain->SendMessage(WM_USER + GUI_MENU_REMOVE, 0, (LPARAM)param1);
		result.Wait();
		}
		*/
	
	
	}

	return nullptr;
}

/************************************************************************************
Exported Functions
************************************************************************************/
__declspec(dllexport) int _gui_guiinit(int argc, char* argv[])
{
	//return main(argc, argv);//...
	// Init communication with debugger
	Bridge::initBridge();
	return true;
}

__declspec(dllexport) void* _gui_sendmessage(GUIMSG type, void* param1, void* param2)
{
	return Bridge::getBridge()->processMessage(type, param1, param2);
}

__declspec(dllexport) const char* _gui_translate_text(const char* source)
{
	/*
	if (TLS_TranslatedStringMap)
	{
		QByteArray translatedUtf8 = QCoreApplication::translate("DBG", source).toUtf8();
		// Boom... VS does not support "thread_local"... and cannot use "__declspec(thread)" in a DLL... https://blogs.msdn.microsoft.com/oldnewthing/20101122-00/?p=12233
		// Simulating Thread Local Storage with a map...
		DWORD ThreadId = GetCurrentThreadId();
		TranslatedStringStorage & TranslatedString = (*TLS_TranslatedStringMap)[ThreadId];
		TranslatedString.Data[translatedUtf8.size()] = 0; // Set the string terminator first.
		memcpy(TranslatedString.Data, translatedUtf8.constData(), std::min((size_t)translatedUtf8.size(), sizeof(TranslatedString.Data) - 1)); // Then copy the string safely.
		return TranslatedString.Data; // Don't need to free this memory. But this pointer should be used immediately to reduce race condition.
	}
	else // Translators are not initialized yet.
	*/
		return source;
}