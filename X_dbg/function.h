#ifndef _FUNCTION_H
#define _FUNCTION_H
#include "addrinfo.h"

struct FUNCTIONSINFO
{
	duint modhash;
	duint start;
	duint end;
	bool manual;
	duint instructioncount;

	std::string mod() const
	{
		return ModNameFromHash(modhash);
	}
};

bool FunctionGet(duint Address, duint* Start = nullptr, duint* End = nullptr, duint* InstrCount = nullptr);
void FunctionCacheSave(JSON Root);
void FunctionCacheLoad(JSON Root);
void FunctionClear();
bool FunctionOverlaps(duint Start, duint End);
bool FunctionDelete(duint Address);
bool FunctionAdd(duint Start, duint End, bool Manual, duint InstructionCount = 0);
void FunctionDelRange(duint Start, duint End, bool DeleteManual = false);

#endif // _FUNCTION_H