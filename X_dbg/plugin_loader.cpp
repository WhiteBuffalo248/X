/**
@file plugin_loader.cpp

@brief Implements the plugin loader.
*/

#include "plugin_loader.h"
#include "threading.h"
#include "console.h"
#include "debugger.h"
#include "expressionfunctions.h"

#include <algorithm>
#include <shlwapi.h>

//structures
struct PLUG_MENU
{
	int pluginHandle; //plugin handle
	int hEntryMenu; //GUI entry/menu handle (unique)
	int hParentMenu; //parent GUI menu handle
};

struct PLUG_MENUENTRY : PLUG_MENU
{
	int hEntryPlugin; //plugin entry handle (unique per plugin)
};

/**
\brief The current plugin handle.
*/
static int curPluginHandle = 0;
/**
\brief List of plugins.
*/
static std::vector<PLUG_DATA> pluginList;
/**
\brief Saved plugin directory
*/
static std::wstring pluginDirectory;
/**
\brief List of plugin callbacks.
*/
static std::vector<PLUG_CALLBACK> pluginCallbackList[CB_LAST];
/**
\brief List of plugin menu entries.
*/
static std::vector<PLUG_MENUENTRY> pluginMenuEntryList;
/**
\brief List of plugin menus.
*/
static std::vector<PLUG_MENU> pluginMenuList;

static PLUG_DATA pluginData;

/**
\brief List of plugin commands.
*/
static std::vector<PLUG_COMMAND> pluginCommandList;

/**
\brief List of plugin exprfunctions.
*/
static std::vector<PLUG_EXPRFUNCTION> pluginExprfunctionList;

/**
\brief Unregister all plugin callbacks of a certain type.
\param pluginHandle Handle of the plugin to unregister a callback from.
\param cbType The type of the callback to unregister.
*/
bool pluginunregistercallback(int pluginHandle, CBTYPE cbType)
{
	EXCLUSIVE_ACQUIRE(LockPluginCallbackList);
	auto & cbList = pluginCallbackList[cbType];
	for (auto it = cbList.begin(); it != cbList.end();)
	{
		if (it->pluginHandle == pluginHandle)
		{
			cbList.erase(it);
			return true;
		}
		else
			++it;
	}
	return false;
}

/**
\brief Register a plugin callback.
\param pluginHandle Handle of the plugin to register a callback for.
\param cbType The type of the callback to register.
\param cbPlugin The actual callback function.
*/
void pluginregistercallback(int pluginHandle, CBTYPE cbType, CBPLUGIN cbPlugin)
{
	pluginunregistercallback(pluginHandle, cbType); //remove previous callback
	PLUG_CALLBACK cbStruct;
	cbStruct.pluginHandle = pluginHandle;
	cbStruct.cbType = cbType;
	cbStruct.cbPlugin = cbPlugin;
	EXCLUSIVE_ACQUIRE(LockPluginCallbackList);
	pluginCallbackList[cbType].push_back(cbStruct);
}

/**
\brief Loads a plugin from the plugin directory.
\param pluginName Name of the plugin.
\param loadall true on unload all.
\return true if it succeeds, false if it fails.
*/
bool pluginload(const char* pluginName, bool loadall)
{
	//no empty plugin names allowed
	if (!pluginName)
		return false;

	char name[MAX_PATH] = "";
	strncpy_s(name, pluginName, _TRUNCATE);

	if (!loadall)
#ifdef _WIN64
		strncat_s(name, ".dp64", _TRUNCATE);
#else
		strncat_s(name, ".dp32", _TRUNCATE);
#endif

	wchar_t currentDir[deflen] = L"";
	if (!loadall)
	{
		GetCurrentDirectoryW(deflen, currentDir);
		SetCurrentDirectoryW(pluginDirectory.c_str());
	}
	char searchName[deflen] = "";
#ifdef _WIN64
	sprintf_s(searchName, "%s\\%s", StringUtils::Utf16ToUtf8(pluginDirectory.c_str()).c_str(), name);
#else
	sprintf_s(searchName, "%s\\%s", StringUtils::Utf16ToUtf8(pluginDirectory.c_str()).c_str(), name);
#endif // _WIN64

	//Check to see if this plugin is already loaded
	if (!loadall)
	{
		EXCLUSIVE_ACQUIRE(LockPluginList);
		for (auto it = pluginList.begin(); it != pluginList.end(); ++it)
		{
			if (_stricmp(it->plugname, name) == 0 && it->isLoaded)
			{
				dprintf(QT_TRANSLATE_NOOP("DBG", "[PLUGIN] %s already loaded\n"), name);
				SetCurrentDirectoryW(currentDir);
				return false;
			}
		}
	}

	//check if the file exists
	if (!loadall && !PathFileExistsW(StringUtils::Utf8ToUtf16(searchName).c_str()))
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "[PLUGIN] Cannot find plugin: %s\n"), name);
		return false;
	}

	//setup plugin data
	memset(&pluginData, 0, sizeof(pluginData));
	pluginData.initStruct.pluginHandle = curPluginHandle;
	pluginData.hPlugin = LoadLibraryW(StringUtils::Utf8ToUtf16(searchName).c_str()); //load the plugin library
	int err = GetLastError();
	if (!pluginData.hPlugin)
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "[PLUGIN] Failed to load plugin: %s\n"), name);
		SetCurrentDirectoryW(currentDir);
		return false;
	}
	pluginData.pluginit = (PLUGINIT)GetProcAddress(pluginData.hPlugin, "pluginit");
	if (!pluginData.pluginit)
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "[PLUGIN] Export \"pluginit\" not found in plugin: %s\n"), name);
		for (int i = CB_INITDEBUG; i < CB_LAST; i++)
			pluginunregistercallback(curPluginHandle, CBTYPE(i));
		FreeLibrary(pluginData.hPlugin);
		SetCurrentDirectoryW(currentDir);
		return false;
	}
	pluginData.plugstop = (PLUGSTOP)GetProcAddress(pluginData.hPlugin, "plugstop");
	pluginData.plugsetup = (PLUGSETUP)GetProcAddress(pluginData.hPlugin, "plugsetup");

	strncpy_s(pluginData.plugpath, searchName, MAX_PATH);
	strncpy_s(pluginData.plugname, name, MAX_PATH);

	//init plugin
	if (!pluginData.pluginit(&pluginData.initStruct))
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "[PLUGIN] pluginit failed for plugin: %s\n"), name);
		for (int i = CB_INITDEBUG; i < CB_LAST; i++)
			pluginunregistercallback(curPluginHandle, CBTYPE(i));
		FreeLibrary(pluginData.hPlugin);
		SetCurrentDirectoryW(currentDir);
		return false;
	}
	if (pluginData.initStruct.sdkVersion < PLUG_SDKVERSION) //the plugin SDK is not compatible
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "[PLUGIN] %s is incompatible with this SDK version\n"), pluginData.initStruct.pluginName);
		for (int i = CB_INITDEBUG; i < CB_LAST; i++)
			pluginunregistercallback(curPluginHandle, CBTYPE(i));
		FreeLibrary(pluginData.hPlugin);
		SetCurrentDirectoryW(currentDir);
		return false;
	}

	dprintf(QT_TRANSLATE_NOOP("DBG", "[PLUGIN] %s v%d Loaded!\n"), pluginData.initStruct.pluginName, pluginData.initStruct.pluginVersion);

	//auto-register callbacks for certain export names
	auto cbPlugin = CBPLUGIN(GetProcAddress(pluginData.hPlugin, "CBALLEVENTS"));
	if (cbPlugin)
	{
		for (int i = CB_INITDEBUG; i < CB_LAST; i++)
			pluginregistercallback(curPluginHandle, CBTYPE(i), cbPlugin);
	}
	auto regExport = [](const char* exportname, CBTYPE cbType)
	{
		auto cbPlugin = CBPLUGIN(GetProcAddress(pluginData.hPlugin, exportname));
		if (cbPlugin)
			pluginregistercallback(curPluginHandle, cbType, cbPlugin);
	};
	regExport("CBINITDEBUG", CB_INITDEBUG);
	regExport("CBSTOPDEBUG", CB_STOPDEBUG);
	regExport("CBCREATEPROCESS", CB_CREATEPROCESS);
	regExport("CBEXITPROCESS", CB_EXITPROCESS);
	regExport("CBCREATETHREAD", CB_CREATETHREAD);
	regExport("CBEXITTHREAD", CB_EXITTHREAD);
	regExport("CBSYSTEMBREAKPOINT", CB_SYSTEMBREAKPOINT);
	regExport("CBLOADDLL", CB_LOADDLL);
	regExport("CBUNLOADDLL", CB_UNLOADDLL);
	regExport("CBOUTPUTDEBUGSTRING", CB_OUTPUTDEBUGSTRING);
	regExport("CBEXCEPTION", CB_EXCEPTION);
	regExport("CBBREAKPOINT", CB_BREAKPOINT);
	regExport("CBPAUSEDEBUG", CB_PAUSEDEBUG);
	regExport("CBRESUMEDEBUG", CB_RESUMEDEBUG);
	regExport("CBSTEPPED", CB_STEPPED);
	regExport("CBATTACH", CB_ATTACH);
	regExport("CBDETACH", CB_DETACH);
	regExport("CBDEBUGEVENT", CB_DEBUGEVENT);
	regExport("CBMENUENTRY", CB_MENUENTRY);
	regExport("CBWINEVENT", CB_WINEVENT);
	regExport("CBWINEVENTGLOBAL", CB_WINEVENTGLOBAL);
	regExport("CBLOADDB", CB_LOADDB);
	regExport("CBSAVEDB", CB_SAVEDB);
	regExport("CBFILTERSYMBOL", CB_FILTERSYMBOL);
	regExport("CBTRACEEXECUTE", CB_TRACEEXECUTE);
	regExport("CBANALYZE", CB_ANALYZE);
	regExport("CBADDRINFO", CB_ADDRINFO);
	regExport("CBVALFROMSTRING", CB_VALFROMSTRING);
	regExport("CBVALTOSTRING", CB_VALTOSTRING);
	regExport("CBMENUPREPARE", CB_MENUPREPARE);

	SectionLocker<LockPluginMenuList, false> menuLock; //exclusive lock

	//add plugin menu
	int hNewMenu = GuiMenuAdd(GUI_PLUGIN_MENU, pluginData.initStruct.pluginName);
	if (hNewMenu == -1)
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "[PLUGIN] GuiMenuAdd(GUI_PLUGIN_MENU) failed for plugin: %s\n"), pluginData.initStruct.pluginName);
		pluginData.hMenu = -1;
	}
	else
	{
		PLUG_MENU newMenu;
		newMenu.hEntryMenu = hNewMenu;
		newMenu.hParentMenu = GUI_PLUGIN_MENU;
		newMenu.pluginHandle = pluginData.initStruct.pluginHandle;
		pluginMenuList.push_back(newMenu);
		pluginData.hMenu = newMenu.hEntryMenu;
	}

	//add disasm plugin menu
	hNewMenu = GuiMenuAdd(GUI_DISASM_MENU, pluginData.initStruct.pluginName);
	if (hNewMenu == -1)
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "[PLUGIN] GuiMenuAdd(GUI_DISASM_MENU) failed for plugin: %s\n"), pluginData.initStruct.pluginName);
		pluginData.hMenuDisasm = -1;
	}
	else
	{
		PLUG_MENU newMenu;
		newMenu.hEntryMenu = hNewMenu;
		newMenu.hParentMenu = GUI_DISASM_MENU;
		newMenu.pluginHandle = pluginData.initStruct.pluginHandle;
		pluginMenuList.push_back(newMenu);
		pluginData.hMenuDisasm = newMenu.hEntryMenu;
	}

	//add dump plugin menu
	hNewMenu = GuiMenuAdd(GUI_DUMP_MENU, pluginData.initStruct.pluginName);
	if (hNewMenu == -1)
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "[PLUGIN] GuiMenuAdd(GUI_DUMP_MENU) failed for plugin: %s\n"), pluginData.initStruct.pluginName);
		pluginData.hMenuDump = -1;
	}
	else
	{
		PLUG_MENU newMenu;
		newMenu.hEntryMenu = hNewMenu;
		newMenu.hParentMenu = GUI_DUMP_MENU;
		newMenu.pluginHandle = pluginData.initStruct.pluginHandle;
		pluginMenuList.push_back(newMenu);
		pluginData.hMenuDump = newMenu.hEntryMenu;
	}

	//add stack plugin menu
	hNewMenu = GuiMenuAdd(GUI_STACK_MENU, pluginData.initStruct.pluginName);
	if (hNewMenu == -1)
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "[PLUGIN] GuiMenuAdd(GUI_STACK_MENU) failed for plugin: %s\n"), pluginData.initStruct.pluginName);
		pluginData.hMenuStack = -1;
	}
	else
	{
		PLUG_MENU newMenu;
		newMenu.hEntryMenu = hNewMenu;
		newMenu.hParentMenu = GUI_STACK_MENU;
		newMenu.pluginHandle = pluginData.initStruct.pluginHandle;
		pluginMenuList.push_back(newMenu);
		pluginData.hMenuStack = newMenu.hEntryMenu;
	}
	menuLock.Unlock();

	//add the plugin to the list
	SectionLocker<LockPluginList, false> pluginLock; //exclusive lock
	pluginList.push_back(pluginData);
	pluginLock.Unlock();

	//setup plugin
	if (pluginData.plugsetup)
	{
		PLUG_SETUPSTRUCT setupStruct;
		setupStruct.hwndDlg = GuiGetWindowHandle();
		setupStruct.hMenu = pluginData.hMenu;
		setupStruct.hMenuDisasm = pluginData.hMenuDisasm;
		setupStruct.hMenuDump = pluginData.hMenuDump;
		setupStruct.hMenuStack = pluginData.hMenuStack;
		pluginData.plugsetup(&setupStruct);
	}
	pluginData.isLoaded = true;
	curPluginHandle++;

	if (!loadall)
		SetCurrentDirectoryW(currentDir);
	return true;
}

/**
\brief Loads plugins from a specified directory.
\param pluginDir The directory to load plugins from.
*/
void pluginloadall(const char* pluginDir)
{
	//reserve menu space
	pluginMenuList.reserve(1024);
	pluginMenuEntryList.reserve(1024);
	//load new plugins
	wchar_t currentDir[deflen] = L"";
	pluginDirectory = StringUtils::Utf8ToUtf16(pluginDir);
	GetCurrentDirectoryW(deflen, currentDir);
	SetCurrentDirectoryW(pluginDirectory.c_str());
	char searchName[deflen] = "";
#ifdef _WIN64
	sprintf_s(searchName, "%s\\*.dp64", pluginDir);
#else
	sprintf_s(searchName, "%s\\*.dp32", pluginDir);
#endif // _WIN64
	WIN32_FIND_DATAW foundData;
	HANDLE hSearch = FindFirstFileW(StringUtils::Utf8ToUtf16(searchName).c_str(), &foundData);
	if (hSearch == INVALID_HANDLE_VALUE)
	{
		SetCurrentDirectoryW(currentDir);
		return;
	}
	do
	{
		pluginload(StringUtils::Utf16ToUtf8(foundData.cFileName).c_str(), true);
	} while (FindNextFileW(hSearch, &foundData));
	FindClose(hSearch);
	SetCurrentDirectoryW(currentDir);
}

/**
\brief Unloads a plugin from the plugin directory.
\param pluginName Name of the plugin.
\param unloadall true on unload all.
\return true if it succeeds, false if it fails.
*/
bool pluginunload(const char* pluginName, bool unloadall)
{
	char name[MAX_PATH] = "";
	strncpy_s(name, pluginName, _TRUNCATE);

	if (!unloadall)
#ifdef _WIN64
		strncat_s(name, ".dp64", _TRUNCATE);
#else
		strncat_s(name, ".dp32", _TRUNCATE);
#endif

	auto found = pluginList.end();
	{
		EXCLUSIVE_ACQUIRE(LockPluginList);
		found = std::find_if(pluginList.begin(), pluginList.end(), [&name](const PLUG_DATA & a)
		{
			return _stricmp(a.plugname, name) == 0;
		});
	}

	if (found != pluginList.end())
	{
		auto currentPlugin = *found;
		if (currentPlugin.plugstop)
			currentPlugin.plugstop();
		plugincmdunregisterall(currentPlugin.initStruct.pluginHandle);
		pluginexprfuncunregisterall(currentPlugin.initStruct.pluginHandle);

		//remove the callbacks
		{
			EXCLUSIVE_ACQUIRE(LockPluginCallbackList);
			for (auto & cbList : pluginCallbackList)
			{
				for (auto it = cbList.begin(); it != cbList.end();)
				{
					if (it->pluginHandle == currentPlugin.initStruct.pluginHandle)
						it = cbList.erase(it);
					else
						++it;
				}
			}
		}
		{
			EXCLUSIVE_ACQUIRE(LockPluginList);
			pluginmenuclear(currentPlugin.hMenu, true);

			if (!unloadall)
			{
				//remove from main pluginlist. We do this so unloadall doesn't try to unload an already released plugin
				pluginList.erase(found);
			}
		}

		FreeLibrary(currentPlugin.hPlugin);
		dprintf(QT_TRANSLATE_NOOP("DBG", "[PLUGIN] %s unloaded\n"), name);
		return true;
	}
	dprintf(QT_TRANSLATE_NOOP("DBG", "[PLUGIN] %s not found\n"), name);
	return false;
}

/**
\brief Unloads all plugins.
*/
void pluginunloadall()
{
	EXCLUSIVE_ACQUIRE(LockPluginList);
	for (const auto & plugin : pluginList)
		pluginunload(plugin.plugname, true);
	pluginList.clear();
}

/**
\brief Call all registered callbacks of a certain type.
\param cbType The type of callbacks to call.
\param [in,out] callbackInfo Information describing the callback. See plugin documentation for more information on this.
*/
void plugincbcall(CBTYPE cbType, void* callbackInfo)
{
	if (pluginCallbackList[cbType].empty())
		return;
	SHARED_ACQUIRE(LockPluginCallbackList);
	auto cbList = pluginCallbackList[cbType]; //copy for thread-safety reasons
	SHARED_RELEASE();
	for (const auto & currentCallback : cbList)
		currentCallback.cbPlugin(cbType, callbackInfo);
}

/**
\brief Call the registered CB_MENUENTRY callbacks for a menu entry.
\param hEntry The menu entry that triggered the event.
*/
void pluginmenucall(int hEntry)
{
	if (hEntry == -1)
		return;

	SectionLocker<LockPluginMenuList, true> menuLock; //shared lock
	auto i = pluginMenuEntryList.begin();
	while (i != pluginMenuEntryList.end())
	{
		const auto currentMenu = *i;
		++i;
		if (currentMenu.hEntryMenu == hEntry && currentMenu.hEntryPlugin != -1)
		{
			PLUG_CB_MENUENTRY menuEntryInfo;
			menuEntryInfo.hEntry = currentMenu.hEntryPlugin;
			SectionLocker<LockPluginCallbackList, true> callbackLock; //shared lock
			const auto & cbList = pluginCallbackList[CB_MENUENTRY];
			for (auto j = cbList.begin(); j != cbList.end();)
			{
				auto currentCallback = *j++;
				if (currentCallback.pluginHandle == currentMenu.pluginHandle)
				{
					menuLock.Unlock();
					callbackLock.Unlock();
					currentCallback.cbPlugin(currentCallback.cbType, &menuEntryInfo);
					return;
				}
			}
		}
	}
}

/**
\brief Calls the registered CB_WINEVENT callbacks.
\param [in,out] message the message that triggered the event. Cannot be null.
\param [out] result The result value. Cannot be null.
\return The value the plugin told it to return. See plugin documentation for more information.
*/
bool pluginwinevent(MSG* message, long* result)
{
	PLUG_CB_WINEVENT winevent;
	winevent.message = message;
	winevent.result = result;
	winevent.retval = false; //false=handle event, true=ignore event
	plugincbcall(CB_WINEVENT, &winevent);
	return winevent.retval;
}

/**
\brief Calls the registered CB_WINEVENTGLOBAL callbacks.
\param [in,out] message the message that triggered the event. Cannot be null.
\return The value the plugin told it to return. See plugin documentation for more information.
*/
bool pluginwineventglobal(MSG* message)
{
	PLUG_CB_WINEVENTGLOBAL winevent;
	winevent.message = message;
	winevent.retval = false; //false=handle event, true=ignore event
	plugincbcall(CB_WINEVENTGLOBAL, &winevent);
	return winevent.retval;
}

/**
\brief Checks if any callbacks are registered
\param cbType The type of the callback.
\return true if no callbacks are registered.
*/
bool plugincbempty(CBTYPE cbType)
{
	return pluginCallbackList[cbType].empty();
}

/**
\brief Unregister all plugin commands.
\param pluginHandle Handle of the plugin to remove the commands from.
*/
void plugincmdunregisterall(int pluginHandle)
{
	SHARED_ACQUIRE(LockPluginCommandList);
	auto commandList = pluginCommandList; //copy for thread-safety reasons
	SHARED_RELEASE();
	auto i = commandList.begin();
	while (i != commandList.end())
	{
		auto currentCommand = *i;
		if (currentCommand.pluginHandle == pluginHandle)
		{
			i = commandList.erase(i);
			dbgcmddel(currentCommand.command);
		}
		else
			++i;
	}
}

/**
\brief Unregister all plugin expression functions.
\param pluginHandle Handle of the plugin to remove the commands from.
*/
void pluginexprfuncunregisterall(int pluginHandle)
{
	SHARED_ACQUIRE(LockPluginExprfunctionList);
	auto commandList = pluginExprfunctionList; //copy for thread-safety reasons
	SHARED_RELEASE();
	auto i = commandList.begin();
	while (i != commandList.end())
	{
		auto currentExprfunction = *i;
		if (currentExprfunction.pluginHandle == pluginHandle)
		{
			i = commandList.erase(i);
			ExpressionFunctions::Unregister(currentExprfunction.name);
		}
		else
			++i;
	}
}

/// <summary>
/// Helper function that recursively clears the menus and their items.
/// </summary>
/// <param name="hMenu">Handle of the menu to clear.</param>
static void pluginmenuclear_helper(int hMenu)
{
	//delete menu entries
	for (auto i = pluginMenuEntryList.size() - 1; i != -1; i--)
		if (hMenu == pluginMenuEntryList.at(i).hParentMenu) //we found an entry that has the menu as parent
			pluginMenuEntryList.erase(pluginMenuEntryList.begin() + i);
	//delete the menus
	std::vector<int> menuClearQueue;
	for (auto i = pluginMenuList.size() - 1; i != -1; i--)
	{
		if (hMenu == pluginMenuList.at(i).hParentMenu) //we found a menu that has the menu as parent
		{
			menuClearQueue.push_back(pluginMenuList.at(i).hEntryMenu);
			pluginMenuList.erase(pluginMenuList.begin() + i);
		}
	}
	//recursively clear the menus
	for (auto & hMenu : menuClearQueue)
		pluginmenuclear_helper(hMenu);
}

/**
\brief Clears a plugin menu.
\param hMenu The menu to clear.
\return true if it succeeds, false otherwise.
*/
bool pluginmenuclear(int hMenu, bool erase)
{
	EXCLUSIVE_ACQUIRE(LockPluginMenuList);
	pluginmenuclear_helper(hMenu);
	for (auto it = pluginMenuList.begin(); it != pluginMenuList.end(); ++it)
	{
		const auto & currentMenu = *it;
		if (currentMenu.hEntryMenu == hMenu)
		{
			if (erase)
			{
				it = pluginMenuList.erase(it);
				GuiMenuRemove(hMenu);
			}
			else
				GuiMenuClear(hMenu);
			return true;
		}
	}
	return false;
}

/**
\brief Add a plugin menu entry to a menu.
\param hMenu The menu to add the entry to.
\param hEntry The handle you like to have the entry. This should be a unique value in the scope of the plugin that registered the \p hMenu. Cannot be -1.
\param title The menu entry title.
\return true if the \p hEntry was unique and the entry was successfully added, false otherwise.
*/
bool pluginmenuaddentry(int hMenu, int hEntry, const char* title)
{
	if (!title || !strlen(title) || hEntry == -1)
		return false;
	EXCLUSIVE_ACQUIRE(LockPluginMenuList);
	int pluginHandle = -1;
	//find plugin handle
	for (const auto & currentMenu : pluginMenuList)
	{
		if (currentMenu.hEntryMenu == hMenu)
		{
			pluginHandle = currentMenu.pluginHandle;
			break;
		}
	}
	if (pluginHandle == -1) //not found
		return false;
	//search if hEntry was previously used
	for (const auto & currentMenu : pluginMenuEntryList)
		if (currentMenu.pluginHandle == pluginHandle && currentMenu.hEntryPlugin == hEntry)
			return false;
	int hNewEntry = GuiMenuAddEntry(hMenu, title);
	if (hNewEntry == -1)
		return false;
	PLUG_MENUENTRY newMenu;
	newMenu.hEntryMenu = hNewEntry;
	newMenu.hParentMenu = hMenu;
	newMenu.hEntryPlugin = hEntry;
	newMenu.pluginHandle = pluginHandle;
	pluginMenuEntryList.push_back(newMenu);
	return true;
}

/**
\brief Add a menu separator to a menu.
\param hMenu The menu to add the separator to.
\return true if it succeeds, false otherwise.
*/
bool pluginmenuaddseparator(int hMenu)
{
	SHARED_ACQUIRE(LockPluginMenuList);
	for (const auto & currentMenu : pluginMenuList)
	{
		if (currentMenu.hEntryMenu == hMenu)
		{
			GuiMenuAddSeparator(hMenu);
			return true;
		}
	}
	return false;
}

/**
\brief Add a new plugin (sub)menu.
\param hMenu The menu handle to add the (sub)menu to.
\param title The title of the (sub)menu.
\return The handle of the new (sub)menu.
*/
int pluginmenuadd(int hMenu, const char* title)
{
	if (!title || !strlen(title))
		return -1;
	EXCLUSIVE_ACQUIRE(LockPluginMenuList);
	int nFound = -1;
	for (unsigned int i = 0; i < pluginMenuList.size(); i++)
	{
		if (pluginMenuList.at(i).hEntryMenu == hMenu)
		{
			nFound = i;
			break;
		}
	}
	if (nFound == -1) //not a valid menu handle
		return -1;
	int hMenuNew = GuiMenuAdd(hMenu, title);
	PLUG_MENU newMenu;
	newMenu.pluginHandle = pluginMenuList.at(nFound).pluginHandle;
	newMenu.hEntryMenu = hMenuNew;
	newMenu.hParentMenu = hMenu;
	pluginMenuList.push_back(newMenu);
	return hMenuNew;
}

/**
\brief Sets an icon for a menu.
\param hMenu The menu handle.
\param icon The icon (can be all kinds of formats).
*/
void pluginmenuseticon(int hMenu, const ICONDATA* icon)
{
	SHARED_ACQUIRE(LockPluginMenuList);
	for (const auto & currentMenu : pluginMenuList)
	{
		if (currentMenu.hEntryMenu == hMenu)
		{
			GuiMenuSetIcon(hMenu, icon);
			break;
		}
	}
}