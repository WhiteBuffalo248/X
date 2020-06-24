
#include "symcache.h"
#include "threading.h"
#include "addrinfo.h"

#include <algorithm>






bool SymbolFromAddressExact(duint address, SymbolInfo & symInfo)
{
	if (address == 0)
		return false;

	SHARED_ACQUIRE(LockModules);
	MODINFO* modInfo = ModInfoFromAddr(address);
	if (modInfo)
	{
		duint rva = address - modInfo->base;

		// search in symbols
		if (modInfo->symbols->isOpen())
		{
			if (modInfo->symbols->findSymbolExact(rva, symInfo))
				return true;
		}

		// search in module exports
		if (modInfo->exports.size())
		{
			auto found = std::lower_bound(modInfo->exportsByRva.begin(), modInfo->exportsByRva.end(), rva, [&modInfo](size_t index, duint rva)
			{
				return modInfo->exports.at(index).rva < rva;
			});
			found = found != modInfo->exportsByRva.end() && rva >= modInfo->exports.at(*found).rva ? found : modInfo->exportsByRva.end();
			if (found != modInfo->exportsByRva.end())
			{
				auto & modExport = modInfo->exports.at(*found);
				symInfo.rva = modExport.rva;
				symInfo.size = 0;
				symInfo.disp = 0;
				symInfo.decoratedName = modExport.name;
				symInfo.publicSymbol = true;
				return true;
			}
		}
	}
	return false;
}

bool SymbolFromAddressExactOrLower(duint address, SymbolInfo & symInfo)
{
	if (address == 0)
		return false;

	SHARED_ACQUIRE(LockModules);
	MODINFO* modInfo = ModInfoFromAddr(address);
	if (modInfo)
	{
		if (modInfo->symbols->isOpen() == false)
			return false;

		duint rva = address - modInfo->base;
		return modInfo->symbols->findSymbolExactOrLower(rva, symInfo);
	}

	return false;
}