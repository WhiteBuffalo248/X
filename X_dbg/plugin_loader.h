#ifndef _PLUGIN_LOADER_H
#define _PLUGIN_LOADER_H

#include "_global.h"
#include "_plugins.h"




//typedefs
typedef bool(*PLUGINIT)(PLUG_INITSTRUCT* initStruct);
typedef bool(*PLUGSTOP)();
typedef void(*PLUGSETUP)(PLUG_SETUPSTRUCT* setupStruct);

//structures
struct PLUG_CALLBACK
{
	int pluginHandle;
	CBTYPE cbType;
	CBPLUGIN cbPlugin;
};

struct PLUG_COMMAND
{
	int pluginHandle;
	char command[deflen];
};

struct PLUG_EXPRFUNCTION
{
	int pluginHandle;
	char name[deflen];
};

struct PLUG_FORMATFUNCTION
{
	int pluginHandle;
	char name[deflen];
};

struct PLUG_DATA
{
	char plugpath[MAX_PATH];
	char plugname[MAX_PATH];
	bool isLoaded;
	HINSTANCE hPlugin;
	PLUGINIT pluginit;
	PLUGSTOP plugstop;
	PLUGSETUP plugsetup;
	int hMenu;
	int hMenuDisasm;
	int hMenuDump;
	int hMenuStack;
	PLUG_INITSTRUCT initStruct;
};

//plugin management functions
bool pluginload(const char* pluginname, bool loadall = false);
void pluginloadall(const char* pluginDir);
bool pluginunload(const char* pluginname, bool unloadall = false);
void pluginunloadall();
void plugincmdunregisterall(int pluginHandle);
void pluginexprfuncunregisterall(int pluginHandle);

void plugincbcall(CBTYPE cbType, void* callbackInfo);
void pluginmenucall(int hEntry);
bool pluginmenuclear(int hMenu, bool erase);
void pluginregistercallback(int pluginHandle, CBTYPE cbType, CBPLUGIN cbPlugin);

bool pluginunregistercallback(int pluginHandle, CBTYPE cbType);

bool pluginwinevent(MSG* message, long* result);
bool pluginwineventglobal(MSG* message);
bool plugincbempty(CBTYPE cbType);
bool pluginmenuaddentry(int hMenu, int hEntry, const char* title);
bool pluginmenuaddseparator(int hMenu);
int pluginmenuadd(int hMenu, const char* title);
void pluginmenuseticon(int hMenu, const ICONDATA* icon);


#endif // _PLUGIN_LOADER_H