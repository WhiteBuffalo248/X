#include "stdafx.h"
#include <crtdbg.h>
#include "CTR_DBG2.h"
#include <time.h>




#define  TIME_STR_LENGTH      10
#define  DATE_STR_LENGTH      10

#define  FILE_IO_ERROR        0


// Macros for setting or clearing bits in the CRT debug flag 
#ifdef CRT_DEBUG_TRACE
#define  SET_CRT_DEBUG_FIELD(a)   _CrtSetDbgFlag((a) | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
#define  CLEAR_CRT_DEBUG_FIELD(a) _CrtSetDbgFlag(~(a) & _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
#else
#define  SET_CRT_DEBUG_FIELD(a)   ((void) 0)
#define  CLEAR_CRT_DEBUG_FIELD(a) ((void) 0)
#endif

/*****************************************************************
*  DEBUG C RUNTIME LIBRARY HOOK FUNCTIONS AND DEFINES           *
*****************************************************************/
_CrtMemState checkPt1;
FILE *logFile;                // Used to log allocation information
const char lineStr[] = {"---------------------------------------\
						 --------------------------------------\n"};

/* ERROR HANDLER
-------------
Handling serious errors gracefully is a real test of craftsmanship.
This function is just a stub; it doesn't really handle errors.
*/
void FatalError(int ErrType)
{
	exit(ErrType);
}

// crt_crtreportblocktype.cpp
// compile with: /MDd
void __cdecl Dumper(void *ptr, void *)
{
	int block = _CrtReportBlockType(ptr);
	_RPTW3(_CRT_WARN, L"Dumper found block at %p: type %d, subtype %d\n", ptr,
		_BLOCK_TYPE(block), _BLOCK_SUBTYPE(block));
}

void __cdecl LeakDumper(void *ptr, size_t sz)
{
	int block = _CrtReportBlockType(ptr);
	_RPTW4(_CRT_WARN, L"LeakDumper found block at %p:"
		L" type %d, subtype %d, size %d\n", ptr,
		_BLOCK_TYPE(block), _BLOCK_SUBTYPE(block), sz);
}

/* CLIENT DUMP HOOK FUNCTION
-------------------------
A hook function for dumping a Client block usually reports some
or all of the contents of the block in question.  The function
below also checks the data in several ways, and reports corruption
or inconsistency as an assertion failure.
*/
void __cdecl MyDumpClientHook(
	void * pUserData,
	size_t nBytes
	)
{
	DebugBreak();
}

/* ALLOCATION HOOK FUNCTION
-------------------------
An allocation hook function can have many, many different
uses. This one simply logs each allocation operation in a file.
*/

int MyAllocHook(
	int      nAllocType,
	void   * pvData,
	size_t   nSize,
	int      nBlockUse,
	long     lRequest,
	const unsigned char * szFileName,
	int      nLine
	)
{
	if (nBlockUse == _CRT_BLOCK)   // Ignore internal C runtime library allocations
		return TRUE;
#ifdef _DEBUG
	if (nBlockUse == _AFX_CLIENT_BLOCK)
		return TRUE;
#else
	return TRUE;
#endif // !_DEBUG
	
	_ASSERT((nAllocType > 0) && (nAllocType < 4));
	_ASSERT((nBlockUse >= 0) && (nBlockUse < 5));
	
	char* operation[] = { "", "allocating", "re-allocating", "freeing" };
	char* blockType[] = { "Free", "Normal", "CRT", "Ignore", "Client" };

	fprintf_s(logFile,
		"Memory operation in %S, line %d: %S a %d-byte '%S' block (#%ld)\n",
		szFileName, nLine, operation[nAllocType], nSize,
		blockType[nBlockUse], lRequest);
	

	if (pvData != NULL)
		fprintf_s(logFile, " at %p\n", pvData);

	return TRUE;         // Allow the memory operation to proceed
}

/*
int MyAllocHook(int nAllocType, void *pvData, size_t nSize, int nBlockUse, long lRequest, const unsigned char * szFileName, int nLine)
{
	//_CRT_BLOCK 块是由 C 运行时库函数内部进行的内存分配,必须显式地忽略
	if (nBlockUse == _CRT_BLOCK)
		return TRUE;

	if (nAllocType == _HOOK_ALLOC)
	{
		CString strHint;
		strHint.Format(_T("分配内存  大小:%d; 文件:; 行号:%d\n"), nSize, nLine);
		
	}
	else if (nAllocType == _HOOK_REALLOC)
	{
		CString strHint;
		strHint.Format(_T("重分配内存  大小:%d; 文件:; 行号:%d\n"), nSize, nLine);
		::OutputDebugString(strHint);
	}
	else if (nAllocType == _HOOK_FREE)
	{
		::OutputDebugString(_T("释放内存 \n"));
	}
	return TRUE;
}
*/

/* REPORT HOOK FUNCTION
--------------------
Again, report hook functions can serve a very wide variety of purposes.
This one logs error and assertion failure debug reports in the
log file, along with 'Damage' reports about overwritten memory.

By setting the retVal parameter to zero, we are instructing _CrtDbgReport
to return zero, which causes execution to continue. If we want the function
to start the debugger, we should have _CrtDbgReport return one.
*/
int MyReportHook(
	int   nRptType,
	char *szMsg,
	int  *retVal
	)
{
	char *RptTypes[] = { "Warning", "Error", "Assert" };

	if ((nRptType > 0) || (strstr(szMsg, "HEAP CORRUPTION DETECTED")))
		fprintf(logFile, "%s: %s", RptTypes[nRptType], szMsg);
	retVal = 0;

	return TRUE;         // Allow the report to be made as usual

}

/*****************************************************************
*  INIT FUNCTION                                                *
*****************************************************************/
int Init_Debug_Trace()
{
#ifndef CRT_DEBUG_TRACE
	TRACE("Skipping this for non-debug mode.\n");
	return 2;
#endif

	TRACE("CRT_DEBUG_TRACE.\n");
#ifdef CRT_DEBUG_TRACE
	char timeStr[TIME_STR_LENGTH], dateStr[DATE_STR_LENGTH];         // Used to set up log file


	// Send all reports to STDOUT, since this example is a console app
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
	_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);

	// Set the debug heap to report memory leaks when the process terminates,
	// and to keep freed blocks in the linked list.
	//SET_CRT_DEBUG_FIELD(_CRTDBG_LEAK_CHECK_DF);

	//获取并设置当前模块目录：
	TCHAR currentDir[MAX_PATH] = _T("");
	GetCurrentDirectory(MAX_PATH, currentDir);
	GetModuleFileName(NULL, currentDir, MAX_PATH);
	_tcsrchr(currentDir, _T('\\'))[1] = 0;
	SetCurrentDirectory(currentDir);

	// Open a log file for the hook functions to use 
	fopen_s(&logFile, "MEM-LOG.XML", "w");
	if (logFile == NULL)
		FatalError(FILE_IO_ERROR);
	_strtime_s(timeStr, TIME_STR_LENGTH);
	_strdate_s(dateStr, DATE_STR_LENGTH);
	fprintf_s(logFile,
		"Memory Allocation Log File for Example Program, run at %s on %s.\n",
		timeStr, dateStr);

	

	fputs(lineStr, logFile);

	// Install the hook functions
	//_CrtSetDumpClient(LeakDumper);
	//_CrtDoForAllClientObjects(Dumper, NULL);
	//_CrtSetAllocHook(MyAllocHook);
	//_CrtSetReportHook(MyReportHook);

#endif                     // End of #ifdef _DEBUG

	// Check the debug heap, and dump the new birthday record. --Note that 
	// debug C runtime library functions such as _CrtCheckMemory( ) and 
	// _CrtMemDumpAllObjectsSince( ) automatically disappear in a release build.
	_CrtMemDumpAllObjectsSince(NULL);
	_CrtCheckMemory();
	_CrtMemCheckpoint(&checkPt1);
	return 0;
}

/*****************************************************************
*  END FUNCTION                                                *
*****************************************************************/
int End_Debug_Trace()
{
	

	// Examine the results
	_CrtMemDumpAllObjectsSince(&checkPt1);
	_CrtMemCheckpoint(&checkPt1);
	_CrtMemDumpStatistics(&checkPt1);
	_CrtCheckMemory();

#ifdef CRT_DEBUG_TRACE
	// This fflush needs to be removed...
	fflush(logFile);
	
#endif

	// OK, time to go. Did I forget to turn out any lights? I could check
	// explicitly using _CrtDumpMemoryLeaks( ), but I have set 
	// _CRTDBG_LEAK_CHECK_DF, so the C runtime library debug heap will
	// automatically alert me at exit of any memory leaks.

#ifdef CRT_DEBUG_TRACE
	fclose(logFile);	
#endif
	return 0;
}