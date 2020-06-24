#ifndef HISTORYCONTEXT_H
#define HISTORYCONTEXT_H

#include "_global.h"
#include "TitanEngine/TitanEngine.h"

class HistoryContext
{
public:
	HistoryContext();
	~HistoryContext();

	void restore();

protected:
	TITAN_ENGINE_CONTEXT_t registers;
	struct ChangedData
	{
		duint addr;
		char oldvalue[32];
	};
	std::vector<ChangedData> ChangedLocation;
	bool invalid;
};

void HistoryAdd();
void HistoryClear();

#endif //HISTORY_CONTEXT_H