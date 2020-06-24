#include "stdafx.h"
#include "MiscUtil.h"
#include "StringUtil.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

bool valueFromCString(CString& sourceText, ULONGLONG* value, int base)
{
	if (!sourceText.IsEmpty() || value)
	{
		TCHAR* endptr = NULL;
		*value = _tcstoull(sourceText, &endptr, base);
		if (endptr && endptr[0] == sourceText[sourceText.GetLength()])
			return true;
	}
	return false;
}

bool valueFromCString(CString& sourceText, ULONG* value, int base)
{
	if (!sourceText.IsEmpty() || value)
	{
		TCHAR* endptr = NULL;
		*value = _tcstoul(sourceText, &endptr, base);
		if (endptr && endptr[0] == sourceText[sourceText.GetLength()])
			return true;
	}	
	return false;
}








void ByteReverse(CByteArray* array)
{
	int length = array->GetSize();
	for (int i = 0; i < length / 2; i++)
	{
		//char temp = array[i];
		//array[i] = array[length - i - 1];
		//array[length - i - 1] = temp;
		char temp = array->GetAt(i);
		array->SetAt(i, array->GetAt(length - i - 1));
		array->SetAt(length - i - 1, temp);
	}
}

CString getSymbolicNameStr(ULONG addr)
{
	char labelText[MAX_LABEL_SIZE] = "";
	char moduleText[MAX_MODULE_SIZE] = "";
	char string[MAX_STRING_SIZE] = "";
	bool bHasString = DbgGetStringAt(addr, string);
	bool bHasLabel = DbgGetLabelAt(addr, SEG_DEFAULT, labelText);
	
	//bool bHasModule = (DbgGetModuleAt(addr, moduleText) && !QString(labelText).startsWith("JMP.&"));
	CString labelTextC = CA2T(labelText, CP_UTF8);
	int index = labelTextC.Find(_T("JMP.&"));
	bool bHasModule = (DbgGetModuleAt(addr, moduleText) && index != 0);

	CString addrText = DbgMemIsValidReadPtr(addr) ? ToPtrString(addr) : ToHexString(addr);
	CString finalText;
	if (bHasString)
	{
		finalText = addrText;
		finalText += " ";
		finalText += CA2T(string, CP_UTF8);
	}
	else if (bHasLabel && bHasModule) //<module.label>
	{
		//finalText = QString("<%1.%2>").arg(moduleText).arg(labelText);
		finalText += CA2T(moduleText, CP_UTF8);
		finalText += ".";
		finalText += CA2T(labelText, CP_UTF8);
	}		
	else if (bHasModule) //module.addr
	{
		//finalText = QString("%1.%2").arg(moduleText).arg(addrText);
		finalText += CA2T(moduleText, CP_UTF8);
		finalText += ".";
		finalText += addrText;
	}
		
	else if (bHasLabel) //<label>
	{
		finalText += "<";
		finalText += CA2T(labelText, CP_UTF8);
		finalText += ">";
	}
	else
	{
		finalText = addrText;
		if (addr == (addr & 0xFF))
		{
			if (iswprint(addr) || iswspace(addr))
				finalText.AppendFormat(_T(" '%s'"), EscapeCh(addr));
		}
		else if (addr == (addr & 0xFFF)) //UNICODE?
		{
			if (iswprint(addr) || iswspace(addr))
				finalText.AppendFormat(_T(" L'%s'"), EscapeCh(addr));
		}
	}
	return finalText;
}