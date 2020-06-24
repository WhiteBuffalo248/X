#pragma once
#include "_global.h"
#include "symbolsourcebase.h"

bool SymbolFromAddressExact(duint address, SymbolInfo & symInfo);
bool SymbolFromAddressExactOrLower(duint address, SymbolInfo & symInfo);