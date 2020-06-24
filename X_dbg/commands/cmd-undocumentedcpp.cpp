#include "cmd-undocumented.h"

#include "value.h"
#include "variable.h"
#include "symbolinfo.h"
#include "console.h"
#include "debugger.h"












bool cbBadCmd(int argc, char* argv[])
{
	duint value = 0;
	int valsize = 0;
	bool isvar = false;
	bool hexonly = false;
	if (valfromstring(*argv, &value, false, false, &valsize, &isvar, &hexonly, true)) //dump variable/value/register/etc
	{
		varset("$ans", value, true);
		if (valsize)
			valsize *= 2;
		else
			valsize = 1;
		char format_str[deflen] = "";
		auto symbolic = SymGetSymbolicName(value);
		if (symbolic.length())
			symbolic = " " + symbolic;
		if (isvar) // and *cmd!='.' and *cmd!='x') //prevent stupid 0=0 stuff
		{
			if (value > 9 && !hexonly)
			{
				if (!valuesignedcalc()) //signed numbers
#ifdef _WIN64
					sprintf_s(format_str, "%%s=%%.%dllX (%%llud)%%s\n", valsize); // TODO: This and the following statements use "%llX" for a "int"-typed variable. Maybe we can use "%X" everywhere?
#else //x86
					sprintf_s(format_str, "%%s=%%.%dX (%%ud)%%s\n", valsize);
#endif //_WIN64
				else
#ifdef _WIN64
					sprintf_s(format_str, "%%s=%%.%dllX (%%lld)%%s\n", valsize);
#else //x86
					sprintf_s(format_str, "%%s=%%.%dX (%%d)%%s\n", valsize);
#endif //_WIN64
				dprintf_untranslated(format_str, *argv, value, value, symbolic.c_str());
			}
			else
			{
				sprintf_s(format_str, "%%s=%%.%dX%%s\n", valsize);
				dprintf_untranslated(format_str, *argv, value, symbolic.c_str());
			}
		}
		else
		{
			if (value > 9 && !hexonly)
			{
				if (!valuesignedcalc()) //signed numbers
#ifdef _WIN64
					sprintf_s(format_str, "%%s=%%.%dllX (%%llud)%%s\n", valsize);
#else //x86
					sprintf_s(format_str, "%%s=%%.%dX (%%ud)%%s\n", valsize);
#endif //_WIN64
				else
#ifdef _WIN64
					sprintf_s(format_str, "%%s=%%.%dllX (%%lld)%%s\n", valsize);
#else //x86
					sprintf_s(format_str, "%%s=%%.%dX (%%d)%%s\n", valsize);
#endif //_WIN64
#ifdef _WIN64
				sprintf_s(format_str, "%%.%dllX (%%llud)%%s\n", valsize);
#else //x86
				sprintf_s(format_str, "%%.%dX (%%ud)%%s\n", valsize);
#endif //_WIN64
				dprintf_untranslated(format_str, value, value, symbolic.c_str());
			}
			else
			{
#ifdef _WIN64
				sprintf_s(format_str, "%%.%dllX%%s\n", valsize);
#else //x86
				sprintf_s(format_str, "%%.%dX%%s\n", valsize);
#endif //_WIN64
				dprintf_untranslated(format_str, value, symbolic.c_str());
			}
		}
	}
	else //unknown command
	{
		dprintf_untranslated("Unknown command/expression: \"%s\"\n", *argv);
		return false;
	}
	return true;
}

extern char animate_command[deflen];

bool cbInstrAnimateWait(int argc, char* argv[])
{
	while (DbgIsDebugging() && dbgisrunning() && animate_command[0] != 0) //while not locked (NOTE: possible deadlock)
	{
		Sleep(1);
	}
	return true;
}