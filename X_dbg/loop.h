#ifndef _LOOP_H
#define _LOOP_H

#include "addrinfo.h"

struct LOOPSINFO
{
	duint modhash;
	duint start;
	duint end;
	duint parent;
	int depth;
	bool manual;
	duint instructioncount;

	std::string mod() const
	{
		return ModNameFromHash(modhash);
	}
};

bool LoopAdd(duint Start, duint End, bool Manual, duint InstructionCount = 0);
bool LoopOverlaps(int Depth, duint Start, duint End, int* FinalDepth);
bool LoopDelete(int Depth, duint Address);
bool LoopGet(int Depth, duint Address, duint* Start = nullptr, duint* End = nullptr, duint* InstructionCount = nullptr);
void LoopCacheSave(JSON Root);
void LoopCacheLoad(JSON Root);
void LoopClear();


#endif //_LOOP_H