#ifndef _GLOBAL_H
#define _GLOBAL_H

#include "bridgemain.h"


//GUI typedefs
typedef int(*GUIGUIINIT)(int, char**);
typedef const char* (*GUITRANSLATETEXT)(const char* source);
typedef void* (*GUISENDMESSAGE)(GUIMSG type, void* param1, void* param2);



//GUI functions
extern GUIGUIINIT _gui_guiinit;
extern GUITRANSLATETEXT _gui_translate_text;
extern GUISENDMESSAGE _gui_sendmessage;

//DBG typedefs
typedef const char* (*DBGDBGINIT)();
typedef bool(*DBGISDEBUGGING)();
typedef bool(*DBGMEMISVALIDREADPTR)(duint addr);
typedef bool(*DBGADDRINFOGET)(duint addr, SEGMENTREG segment, BRIDGE_ADDRINFO* addrinfo);
typedef bool(*DBGMEMREAD)(duint addr, void* dest, duint size, duint* read);
typedef bool(*DBGMEMWRITE)(duint addr, const void* src, duint size, duint* written);
typedef duint(*DBGSENDMESSAGE)(DBGMSG type, void* param1, void* param2);
typedef duint(*DBGGETBRANCHDESTINATION)(duint addr);
typedef bool(*DBGISJUMPGOINGTOEXECUTE)(duint addr);
typedef bool(*DBGGETREGDUMP)(REGDUMP* regdump);
typedef bool(*DBGDBGCMDEXEC)(const char* cmd);
typedef bool(*DBGDBGCMDEXECDIRECT)(const char* cmd);
typedef BPXTYPE(*DBGBPGETTYPEAT)(duint addr);
typedef duint(*DBGMEMFINDBASEADDR)(duint addr, duint* size);
typedef bool(*DBGVALFROMSTRING)(const char* string, duint* value);
typedef bool(*DBGENCODETYPESET)(duint addr, duint size, ENCODETYPE type);
typedef int(*DBGGETBPLIST)(BPXTYPE type, BPMAP* bplist);
typedef void(*DBGDBGEXITSIGNAL)();
typedef bool(*DBGVALTOSTRING)(const char* string, duint value);

//DBG functions
extern DBGDBGINIT _dbg_dbginit;
extern DBGISDEBUGGING _dbg_isdebugging;
extern DBGMEMISVALIDREADPTR _dbg_memisvalidreadptr;
extern DBGADDRINFOGET _dbg_addrinfoget;
extern DBGMEMREAD _dbg_memread;
extern DBGMEMWRITE _dbg_memwrite;
extern DBGSENDMESSAGE _dbg_sendmessage;
extern DBGGETBRANCHDESTINATION _dbg_getbranchdestination;
extern DBGISJUMPGOINGTOEXECUTE _dbg_isjumpgoingtoexecute;
extern DBGGETREGDUMP _dbg_getregdump;
extern DBGDBGCMDEXEC _dbg_dbgcmdexec;
extern DBGDBGCMDEXECDIRECT _dbg_dbgcmddirectexec;
extern DBGBPGETTYPEAT _dbg_bpgettypeat;
extern DBGMEMFINDBASEADDR _dbg_memfindbaseaddr;
extern DBGVALFROMSTRING _dbg_valfromstring;
extern DBGENCODETYPESET _dbg_encodetypeset;
extern DBGGETBPLIST _dbg_getbplist;
extern DBGDBGEXITSIGNAL _dbg_dbgexitsignal;
extern DBGVALTOSTRING _dbg_valtostring;


#endif // _GLOBAL_H