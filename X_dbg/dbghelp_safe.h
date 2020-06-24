#ifndef _DBGHELP_SAFE_H
#define _DBGHELP_SAFE_H

#ifdef __GNUC__
#include "dbghelp\dbghelp.h"
#else
#include <dbghelp.h>
#endif //__GNUC__

void SafeDbghelpInitialize();
void SafeDbghelpDeinitialize();

BOOL
SafeStackWalk64(
__in DWORD MachineType,
__in HANDLE hProcess,
__in HANDLE hThread,
__inout LPSTACKFRAME64 StackFrame,
__inout PVOID ContextRecord,
__in_opt PREAD_PROCESS_MEMORY_ROUTINE64 ReadMemoryRoutine,
__in_opt PFUNCTION_TABLE_ACCESS_ROUTINE64 FunctionTableAccessRoutine,
__in_opt PGET_MODULE_BASE_ROUTINE64 GetModuleBaseRoutine,
__in_opt PTRANSLATE_ADDRESS_ROUTINE64 TranslateAddress
);

#endif //_DBGHELP_SAFE_H