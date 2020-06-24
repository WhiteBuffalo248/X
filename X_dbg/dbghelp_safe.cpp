
#include "_global.h"
#include "dbghelp_safe.h"








static CRITICAL_SECTION criticalSection;

struct Lock
{
	explicit Lock(bool weak)
	{
		if (weak)
			success = !!TryEnterCriticalSection(&criticalSection);
		else
		{
			EnterCriticalSection(&criticalSection);
			success = true;
		}
	}

	~Lock()
	{
		if (success)
			LeaveCriticalSection(&criticalSection);
	}

	bool success;
};

#define WEAK_ACQUIRE() Lock __lock(true); if(!__lock.success) return 0;
#define STRONG_ACQUIRE() Lock __lock(false);

void SafeDbghelpInitialize()
{
	InitializeCriticalSection(&criticalSection);
}

void SafeDbghelpDeinitialize()
{
	DeleteCriticalSection(&criticalSection);
}

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
)
{
	STRONG_ACQUIRE();
	return StackWalk64(MachineType, hProcess, hThread, StackFrame, ContextRecord, ReadMemoryRoutine, FunctionTableAccessRoutine, GetModuleBaseRoutine, TranslateAddress);
}