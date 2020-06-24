#pragma once
#include "_global.h"
#include "taskthread.h"

extern StringConcatTaskThread_<void(*)(const std::string &)> * p_stringConcatTaskThread_;





void dputs(_In_z_ const char* Text, bool bSynchronize = false);
void dprintf(_In_z_ _Printf_format_string_ const char* Format, ...);
void dprintf_args(_In_z_ _Printf_format_string_ const char* Format, va_list Args);
void dprintf_untranslated(_In_z_ _Printf_format_string_ const char* Format, ...);
void dprintf_args_untranslated(_In_z_ _Printf_format_string_ const char* Format, va_list Args);