#ifndef _EXCEPTION_H
#define _EXCEPTION_H
#define MS_VC_EXCEPTION 0x406D1388



#include "_global.h"
#include "_dbgfunctions.h"
#include "stringutils.h"


static std::unordered_map<String, unsigned int> Constants;




bool ErrorCodeInit(const String & errorFile);
bool ExceptionCodeInit(const String & exceptionFile);
bool ExceptionNameToCode(const char* Name, unsigned int* ErrorCode);
bool ConstantCodeInit(const String & constantFile);
bool NtStatusCodeInit(const String & ntStatusFile);
bool ConstantFromName(const String & name, duint & value);
const String & ErrorCodeToName(unsigned int ErrorCode);
const String & NtStatusCodeToName(unsigned int NtStatusCode);
const String & ExceptionCodeToName(unsigned int ExceptionCode);
std::vector<CONSTANTINFO> ExceptionList();
std::vector<CONSTANTINFO> ConstantList();
std::vector<CONSTANTINFO> ErrorCodeList();
#endif // _EXCEPTION_H