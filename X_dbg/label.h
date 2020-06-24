#ifndef _LABEL_H
#define _LABEL_H

#include "addrinfo.h"

struct LABELSINFO : AddrInfo
{
	std::string text;
};

bool LabelFromString(const char* Text, duint* Address);
bool LabelGet(duint Address, char* Text);
void LabelCacheSave(JSON root);
void LabelCacheLoad(JSON root);
void LabelClear();
bool LabelSet(duint Address, const char* Text, bool Manual, bool Temp = false);
bool LabelDelete(duint Address);
void LabelDelRange(duint Start, duint End, bool Manual);

#endif // _LABEL_H