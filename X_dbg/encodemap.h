#pragma once

#include "_global.h"
#include "jansson/jansson_x64dbg.h"

void* EncodeMapGetBuffer(duint addr, duint* size, bool create = false);
ENCODETYPE EncodeMapGetType(duint addr, duint codesize);
duint EncodeMapGetSize(duint addr, duint codesize);
duint GetEncodeTypeSize(ENCODETYPE type);
void EncodeMapCacheSave(JSON Root);
void EncodeMapCacheLoad(JSON Root);
void EncodeMapClear();
void EncodeMapDelRange(duint Start, duint End);
void EncodeMapDelSegment(duint addr);
void EncodeMapReleaseBuffer(void* buffer);
bool EncodeMapSetType(duint addr, duint size, ENCODETYPE type, bool* created = nullptr);