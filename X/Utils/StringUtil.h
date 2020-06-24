#pragma once
#include <sstream>
#include <iomanip>
#include "Imports.h"


#ifdef _WIN64
#define ArchValue(x32value, x64value) x64value
#else
#define ArchValue(x32value, x64value) x32value
#endif //_WIN64


inline CString ToPtrString(duint Address)
{
	//
	// This function exists because of how QT handles
	// variables in strings.
	//
	// QString::arg():
	// ((int32)0xFFFF0000) == 0xFFFFFFFFFFFF0000 with sign extension
	//

	CString temp;
#ifdef _WIN64
	//sprintf_s(temp, "%016llX", Address);
	temp.Format(_T("%016llX"), Address);
#else
	//sprintf_s(temp, "%08X", Address);
	temp.Format(_T("%08X"), Address);
#endif // _WIN64
	if (temp.IsEmpty())
		_CrtDbgBreak();
	return temp;
}

inline CString ToHexString(duint Value)
{
	CString temp;
#ifdef _WIN64
	//sprintf_s(temp, "%llX", Value);
	temp.Format(_T("%llX"), Value);
#else
	//sprintf_s(temp, "%X", Value);
	temp.Format(_T("%X"), Value);
#endif // _WIN64
	if (temp.IsEmpty())
		_CrtDbgBreak();
	return temp;
}

inline CString ToLongLongHexString(unsigned long long Value)
{
	CString temp;
	//sprintf_s(temp, "%llX", Value);
	temp.Format(_T("%llX"), Value);
	return temp;
}

template<typename T>
inline CString ToIntegralString(void* buffer)
{
	auto value = *(T*)buffer;
	return ToLongLongHexString(value);
}

inline CString ToByteString(unsigned char Value)
{
	char temp[4];
	sprintf_s(temp, "%02X", Value);
	return CString(temp);
}

inline CString ToWordString(unsigned short Value)
{
	char temp[8];
	sprintf_s(temp, "%04X", Value);
	return CString(temp);
}

template<typename T>
inline CString ToFloatingString(void* buffer)
{
	auto value = *(T*)buffer;
	std::stringstream wFloatingStr;
	wFloatingStr << std::setprecision(std::numeric_limits<T>::digits10) << value;
	//return CString::fromStdString(wFloatingStr.str());
	CString tmpStr;
	tmpStr = wFloatingStr.str().c_str();
	return tmpStr;	
}

inline CString ToFloatString(void* buffer)
{
	return ToFloatingString<float>(buffer);
}

inline CString ToDoubleString(void* buffer)
{
	return ToFloatingString<double>(buffer);
}

CString EscapeCh(TCHAR ch);

CString ToLongDoubleString(void* buffer);

CString GetDataTypeString(void* buffer, duint size, ENCODETYPE type);

bool GetCommentFormat(duint addr, CString & comment, bool* autoComment = nullptr);