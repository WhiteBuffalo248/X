#include "cmd-watch-control.h"
#include "threading.h"
#include "watch.h"
#include "variable.h"








bool cbCheckWatchdog(int argc, char* argv[])
{
	EXCLUSIVE_ACQUIRE(LockWatch);
	bool watchdogTriggered = false;
	for (auto j = watchexpr.begin(); j != watchexpr.end(); ++j)
	{
		std::pair<unsigned int, WatchExpr*> i = *j;
		i.second->watchdogTriggered = false;
		duint intVal = i.second->getIntValue();
		watchdogTriggered |= i.second->watchdogTriggered;
	}
	EXCLUSIVE_RELEASE();
	if (watchdogTriggered)
		GuiUpdateWatchViewAsync();
	varset("$result", watchdogTriggered ? 1 : 0, false);
	return true;
}