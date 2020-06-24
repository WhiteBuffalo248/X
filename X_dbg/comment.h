#ifndef _COMMENT_H
#define _COMMENT_H

#include "_global.h"
#include "addrinfo.h"

struct COMMENTSINFO : AddrInfo
{
	std::string text;
};

bool CommentSet(duint Address, const char* Text, bool Manual);
bool CommentDelete(duint Address);
bool CommentGet(duint Address, char* Text);
void CommentCacheSave(JSON Root);
void CommentCacheLoad(JSON Root);
void CommentClear();
void CommentDelRange(duint Start, duint End, bool Manual);

#endif // _COMMENT_H