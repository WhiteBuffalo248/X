
#include "_plugins.h"
#include "console.h"
#include "plugin_loader.h"

///debugger plugin exports (wrappers)
PLUG_IMPEXP void _plugin_registercallback(int pluginHandle, CBTYPE cbType, CBPLUGIN cbPlugin)
{
	pluginregistercallback(pluginHandle, cbType, cbPlugin);
}

PLUG_IMPEXP void _plugin_logprintf(const char* format, ...)
{
	va_list args;

	va_start(args, format);
	dprintf_args_untranslated(format, args);
	va_end(args);
}

PLUG_IMPEXP bool _plugin_menuaddentry(int hMenu, int hEntry, const char* title)
{
	return pluginmenuaddentry(hMenu, hEntry, title);
}

PLUG_IMPEXP bool _plugin_menuaddseparator(int hMenu)
{
	return pluginmenuaddseparator(hMenu);
}

PLUG_IMPEXP int _plugin_menuadd(int hMenu, const char* title)
{
	return pluginmenuadd(hMenu, title);
}

PLUG_IMPEXP void _plugin_menuseticon(int hMenu, const ICONDATA* icon)
{
	pluginmenuseticon(hMenu, icon);
}