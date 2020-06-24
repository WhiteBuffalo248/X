#ifndef _MEMORY_H
#define _MEMORY_H

#include "_global.h"
#include "addrinfo.h"
#include "patternfind.h"







struct SimplePage;
void MemUpdateMap();
void MemUpdateMapAsync();
bool MemIsCanonicalAddress(duint Address);
bool MemIsValidReadPtr(duint Address, bool cache = false);
bool MemIsValidReadPtrUnsafe(duint Address, bool cache = false);
bool MemIsCodePage(duint Address, bool Refresh);
duint MemFindBaseAddr(duint Address, duint* Size = nullptr, bool Refresh = false, bool FindReserved = false);
bool MemReadUnsafe(duint BaseAddress, void* Buffer, duint Size, duint* NumberOfBytesRead = nullptr);
bool MemRead(duint BaseAddress, void* Buffer, duint Size, duint* NumberOfBytesRead = nullptr, bool cache = false);
bool MemWrite(duint BaseAddress, const void* Buffer, duint Size, duint* NumberOfBytesWritten = nullptr);
bool MemPatch(duint BaseAddress, const void* Buffer, duint Size, duint* NumberOfBytesWritten = nullptr);
bool MemReadUnsafePage(HANDLE hProcess, LPVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesRead);
bool MemoryReadSafePage(HANDLE hProcess, LPVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesRead);
bool MemGetPageInfo(duint Address, MEMPAGE* PageInfo, bool Refresh = false);
bool MemDecodePointer(duint* Pointer, bool vistaPlus);
void MemInitRemoteProcessCookie(ULONG cookie);
void MemReadDumb(duint BaseAddress, void* Buffer, duint Size);
duint MemAllocRemote(duint Address, duint Size, DWORD Type = MEM_RESERVE | MEM_COMMIT, DWORD Protect = PAGE_EXECUTE_READWRITE);
bool MemSetPageRights(duint Address, const char* Rights);
bool MemGetPageRights(duint Address, char* Rights);
bool MemPageRightsToString(DWORD Protect, char* Rights);
bool MemPageRightsFromString(DWORD* Protect, const char* Rights);
bool MemFindInPage(const SimplePage & page, duint startoffset, const std::vector<PatternByte> & pattern, std::vector<duint> & results, duint maxresults);
bool MemFindInMap(const std::vector<SimplePage> & pages, const std::vector<PatternByte> & pattern, std::vector<duint> & results, duint maxresults, bool progress = true);

extern std::map<Range, MEMPAGE, RangeCompare> memoryPages;
extern bool bListAllPages;
extern bool bQueryWorkingSet;


struct SimplePage
{
	duint address;
	duint size;

	SimplePage(duint address, duint size)
	{
		this->address = address;
		this->size = size;
	}
};



#endif // _MEMORY_H