#ifndef _THREAD_H
#define _THREAD_H

#include "_global.h"
#include "ntdll/ntdll.h"









int ThreadGetCount();
bool ThreadGetTib(duint TEBAddress, NT_TIB* Tib);
void ThreadGetList(THREADLIST* list);
void ThreadGetList(std::vector<THREADINFO> & list);
DWORD ThreadGetId(HANDLE Thread);
bool ThreadGetName(DWORD ThreadId, char* Name);
int ThreadGetSuspendCount(HANDLE Thread);
THREADPRIORITY ThreadGetPriority(HANDLE Thread);
DWORD ThreadGetLastErrorTEB(ULONG_PTR ThreadLocalBase);
DWORD ThreadGetLastError(DWORD ThreadId);
NTSTATUS ThreadGetLastStatusTEB(ULONG_PTR ThreadLocalBase);
NTSTATUS ThreadGetLastStatus(DWORD ThreadId);
ULONG64 ThreadQueryCycleTime(HANDLE hThread);
ULONG_PTR ThreadGetLocalBase(DWORD ThreadId);
void ThreadClear();
HANDLE ThreadGetHandle(DWORD ThreadId);
void ThreadCreate(CREATE_THREAD_DEBUG_INFO* CreateThread);
void ThreadExit(DWORD ThreadId);
bool ThreadIsValid(DWORD ThreadId);
bool ThreadSetName(DWORD ThreadId, const char* name);
void ThreadUpdateWaitReasons();

#endif // _THREAD_H