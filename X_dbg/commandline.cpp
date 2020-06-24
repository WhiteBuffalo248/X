#include "commandline.h"
#include "threading.h"
#include "filehelper.h"
char commandLine[MAX_SETTING_SIZE];






void CmdLineCacheSave(JSON Root, const String & cacheFile)
{
	EXCLUSIVE_ACQUIRE(LockCmdLine);

	// Write the (possibly empty) command line to a cache file
	FileHelper::WriteAllText(cacheFile, commandLine);

	// return if command line is empty
	if (!strlen(commandLine))
		return;

	// Create a JSON array to store each sub-object with a breakpoint
	const JSON jsonCmdLine = json_object();
	json_object_set_new(jsonCmdLine, "cmdLine", json_string(commandLine));
	json_object_set_new(Root, "commandLine", jsonCmdLine);
}

void copyCommandLine(const char* cmdLine)
{
	strncpy_s(commandLine, cmdLine, _TRUNCATE);
}

void CmdLineCacheLoad(JSON Root)
{
	EXCLUSIVE_ACQUIRE(LockCmdLine);

	// Clear command line
	memset(commandLine, 0, sizeof(commandLine));

	// Get a handle to the root object -> commandLine
	const JSON jsonCmdLine = json_object_get(Root, "commandLine");

	// Return if there was nothing to load
	if (!jsonCmdLine)
		return;

	const char* cmdLine = json_string_value(json_object_get(jsonCmdLine, "cmdLine"));

	copyCommandLine(cmdLine);

	json_decref(jsonCmdLine);
}

bool isCmdLineEmpty()
{
	return !strlen(commandLine);
}

char* getCommandLineArgs()
{
	auto args = *commandLine == '\"' ? strchr(commandLine + 1, '\"') : nullptr;
	args = strchr(args ? args : commandLine, ' ');
	return args ? args + 1 : nullptr;
}