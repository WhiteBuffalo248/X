
/**
@file exhandlerinfo.cpp

@brief ???
*/

#include "exhandlerinfo.h"
#include "thread.h"
#include "debugger.h"
#include "memory.h"

bool IsVistaOrLater()
{
	static bool vistaOrLater = []()
	{
		OSVERSIONINFOEXW osvi = { 0 };
		osvi.dwOSVersionInfoSize = sizeof(osvi);
#pragma warning( disable : 4996 )
		return GetVersionExW((LPOSVERSIONINFOW)&osvi) && osvi.dwMajorVersion > 5;
	}();
	return vistaOrLater;
}

#define MAX_HANDLER_DEPTH 10

bool ExHandlerGetSEH(std::vector<duint> & Entries)
{
#ifdef _WIN64
	return false; // TODO: 64-bit
#endif
	static duint nextSEH = 0;
	NT_TIB tib;
	if (ThreadGetTib((duint)GetTEBLocation(hActiveThread), &tib))
	{
		EXCEPTION_REGISTRATION_RECORD sehr;
		duint addr_ExRegRecord = (duint)tib.ExceptionList;
		int MAX_DEPTH = MAX_HANDLER_DEPTH;
		while (addr_ExRegRecord != 0xFFFFFFFF && MAX_DEPTH)
		{
			Entries.push_back(addr_ExRegRecord);
			if (!MemRead(addr_ExRegRecord, &sehr, sizeof(EXCEPTION_REGISTRATION_RECORD)))
				break;
			addr_ExRegRecord = (duint)sehr.Next;
			MAX_DEPTH--;
		}
	}
	return true;
}
