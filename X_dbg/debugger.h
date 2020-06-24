#ifndef _DEBUGGER_H
#define _DEBUGGER_H

#include "_global.h"
#include "_plugins.h"
#include "TitanEngine/TitanEngine.h"
#include "command.h"
#include "breakpoint.h"
#include "commandline.h"
#include "command.h"

#include <tlhelp32.h>
#include <psapi.h>


//structures
struct INIT_STRUCT
{
	char* exe;
	char* commandline;
	char* currentfolder;
};

struct ExceptionRange
{
	unsigned int start;
	unsigned int end;
};

#pragma pack(push,8)
typedef struct _THREADNAME_INFO
{
	DWORD dwType; // Must be 0x1000.
	LPCSTR szName; // Pointer to name (in user addr space).
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

//variables
extern PROCESS_INFORMATION* fdProcessInfo;
extern HANDLE hActiveThread;
extern HANDLE hProcessToken;
extern char szProgramDir[MAX_PATH];
extern char szFileName[MAX_PATH];
extern char szSymbolCachePath[MAX_PATH];
extern bool bUndecorateSymbolNames;
extern bool bEnableSourceDebugging;
extern bool bTraceRecordEnabledDuringTrace;
extern bool bSkipInt3Stepping;
extern bool bIgnoreInconsistentBreakpoints;
extern bool bNoForegroundWindow;
extern bool bVerboseExceptionLogging;
extern bool bNoWow64SingleStepWorkaround;
extern duint maxSkipExceptionCount;
extern HANDLE mProcHandle;
extern HANDLE mForegroundHandle;
extern duint mRtrPreviousCSP;
extern HANDLE hDebugLoopThread;
//functions
void dbginit();
void dbgstop();
void cbStep();
void cbRtrStep();
void cbPauseBreakpoint();
void cbUserBreakpoint();
void dbgsetforeground();
void dbgforcebreaktrace();
void dbgtracebrowserneedsupdate();
void dbgclearignoredexceptions();
void DebugUpdateBreakpointsViewAsync();
void cbTraceOverConditionalStep();
void cbTraceIntoConditionalStep();
void cbTraceIntoBeyondTraceRecordStep();
void cbTraceOverBeyondTraceRecordStep();
void cbTraceIntoIntoTraceRecordStep();
void cbTraceOverIntoTraceRecordStep();

void dbgsetispausedbyuser(bool b);
void dbgsetskipexceptions(bool skip);
void DebugUpdateGuiAsync(duint disasm_addr, bool stack);
void DebugUpdateGui(duint disasm_addr, bool stack);
void DebugUpdateStack(duint dumpAddr, duint csp, bool forceDump = false);
void cbMemoryBreakpoint(void* ExceptionAddress);
void cbHardwareBreakpoint(void* ExceptionAddress);
void cbRunToUserCodeBreakpoint(void* ExceptionAddress);
void cbRunToUserCodeBreakpoint(void* ExceptionAddress);
void DebugUpdateGuiSetStateAsync(duint disasm_addr, bool stack, DBGSTATE state = paused);
void GuiSetDebugStateAsync(DBGSTATE state);
void dbgsetdebuggeeinitscript(const char* fileName);
void dbgaddignoredexception(ExceptionRange range);
void dbgsetsteprepeat(bool steppingIn, duint repeat);
void StepIntoWow64(void* traceCallBack);
void dbgsetfreezestack(bool freeze);



bool dbgisrunning();
bool dbgtraceactive();
bool dbgrestartadmin();
bool dbgisdepenabled();
bool cbSetDLLBreakpoints(const BREAKPOINT* bp);
bool cbSetModuleBreakpoints(const BREAKPOINT* bp);
bool dbgcmdnew(const char* name, CBCOMMAND cbCommand, bool debugonly);
bool dbgcmddel(const char* name);
bool dbgsetdllbreakpoint(const char* mod, DWORD type, bool singleshoot);
bool dbgisignoredexception(unsigned int exception);
bool dbgsetcmdline(const char* cmd_line, cmdline_error_t* cmd_line_error);
bool dbggetcmdline(char** cmd_line, cmdline_error_t* cmd_line_error, HANDLE hProcess = NULL);
bool dbggetwintext(std::vector<std::string>* winTextList, const DWORD dwProcessId);
bool dbglistprocesses(std::vector<PROCESSENTRY32>* infoList, std::vector<std::string>* commandList, std::vector<std::string>* winTextList);
bool dbgsettracecondition(const String & expression, duint maxCount);
BOOL ismainwindow(HWND handle);
BOOL CALLBACK chkWindowPidCallback(HWND hWnd, LPARAM lParam);
bool dbghandledllbreakpoint(const char* mod, bool loadDll);
bool dbgsettracelog(const String & expression, const String & text);
bool dbgsettracecmd(const String & expression, const String & text);
bool dbgsettraceswitchcondition(const String & expression);
bool dbgsettracelogfile(const char* fileName);
duint dbgdebuggedbase();
duint dbggettimewastedcounter();
duint dbggetdbgevents();

DWORD WINAPI threadDebugLoop(void* lpParameter);
const char* dbggetdebuggeeinitscript();

cmdline_qoutes_placement_t getqoutesplacement(const char* cmdline);

EXCEPTION_DEBUG_INFO & getLastExceptionInfo();


#endif // _DEBUGGER_H