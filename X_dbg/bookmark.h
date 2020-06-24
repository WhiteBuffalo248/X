#ifndef _BOOKMARK_H
#define _BOOKMARK_H

#include "_global.h"
#include "addrinfo.h"

struct BOOKMARKSINFO : AddrInfo
{
};

bool BookmarkSet(duint Address, bool Manual);
bool BookmarkGet(duint Address);
void BookmarkCacheSave(JSON Root);
void BookmarkCacheLoad(JSON Root);
void BookmarkClear();
void BookmarkDelRange(duint Start, duint End, bool Manual);


#endif // _BOOKMARK_H





