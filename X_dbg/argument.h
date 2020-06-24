#ifndef _ARGUMENT_H
#define _ARGUMENT_H

#include "addrinfo.h"

struct ARGUMENTSINFO
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
bool ArgumentAdd(duint Start, duint End, bool Manual, duint InstructionCount = 0);
bool ArgumentOverlaps(duint Start, duint End);
bool ArgumentDelete(duint Address);
bool ArgumentGet(duint Address, duint* Start = nullptr, duint* End = nullptr, duint* InstrCount = nullptr);
void ArgumentCacheSave(JSON Root);
void ArgumentCacheLoad(JSON Root);
void ArgumentClear();

#endif // _ARGUMENT_H