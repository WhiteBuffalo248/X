#include "stdafx.h"
#include "MiscUtil.h"
#include "StringUtil.h"
#include "ldconvert/ldconvert.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CString ToLongDoubleString(void* buffer)
{
	char str[32];
	ld2str(buffer, str);
	return CString(str);
}

CString EscapeCh(TCHAR ch)
{
	switch (ch)
	{
	case '\0':
		return _T("\\0");
	case '\t':
		return _T("\\t");
	case '\f':
		return _T("\\f");
	case '\v':
		return _T("\\v");
	case '\n':
		return _T("\\n");
	case '\r':
		return _T("\\r");
	case '\\':
		return _T("\\\\");
	case '\"':
		return _T("\\\"");
	case '\a':
		return _T("\\a");
	case '\b':
		return _T("\\b");
	default:
		//return QString(1, ch);
		return CString(ch);
	}
}

#define HEXLOOKUP "0123456789ABCDEF"
CString GetDataTypeString(void* buffer, duint size, ENCODETYPE type)
{
	CByteArray tmpBA;
	CString tmpStr;
	int len;
	const char* tmpBuffer;
	switch (type)
	{
	case enc_byte:
		return ToIntegralString<unsigned char>(buffer);
	case enc_word:
		return ToIntegralString<unsigned short>(buffer);
	case enc_dword:
		return ToIntegralString<unsigned int>(buffer);
	case enc_fword:
		tmpBuffer = (const char*) buffer;
		len = 6;
		tmpBA.SetSize(len);
		for (int i = 0; i < len; i++)
			//tmpBA[i] = tmpBuffer[i];
			tmpBA.SetAt(i, tmpBuffer[i]);
		//CByteArray tmpBA1 = ByteReverse(tmpBA); // ×Ö·û´®Ê×Î²»¥»»;	
		for (int i = 0; i < len; i++)
		{			
			//ch = tmpBA1[i];	//tmpStr.SetAt(i,tmpBA1.GetAt(i)); // CByteArray ×ª CString
			//tmpStr += ch;		
			//auto ch = tmpBA[len - i - 1];
			auto ch = (BYTE)tmpBA.GetAt(len - i - 1);
			tmpStr += HEXLOOKUP[(ch >> 4) & 0xF];
			tmpStr += HEXLOOKUP[ch & 0xF];
		}		
		return tmpStr;
	case enc_qword:
		return ToIntegralString<unsigned long long int>(buffer);
	case enc_tbyte:
		//return QString(ByteReverse(QByteArray((const char*)buffer, 10)).toHex());
		tmpBuffer = (const char*)buffer;
		len = 10;
		tmpBA.SetSize(len);
		for (int i = 0; i < len; i++)
			//tmpBA[i] = tmpBuffer[i];
			tmpBA.SetAt(i, tmpBuffer[i]);
		//CByteArray tmpBA1 = ByteReverse(tmpBA); // ×Ö·û´®Ê×Î²»¥»»;	
		for (int i = 0; i < len; i++)
		{
			//ch = tmpBA1[i];	//tmpStr.SetAt(i,tmpBA1.GetAt(i)); // CByteArray ×ª CString
			//tmpStr += ch;		
			//auto ch = tmpBA[len - i - 1];
			auto ch = (BYTE)tmpBA.GetAt(len - i - 1);
			tmpStr += HEXLOOKUP[(ch >> 4) & 0xF];
			tmpStr += HEXLOOKUP[ch & 0xF];
		}
		return tmpStr;
	case enc_oword:
		//return QString(ByteReverse(QByteArray((const char*)buffer, 16)).toHex());
		tmpBuffer = (const char*)buffer;
		len = 16;
		tmpBA.SetSize(len);
		for (int i = 0; i < len; i++)
			//tmpBA[i] = tmpBuffer[i];
			tmpBA.SetAt(i, tmpBuffer[i]);
		//CByteArray tmpBA1 = ByteReverse(tmpBA); // ×Ö·û´®Ê×Î²»¥»»;	
		for (int i = 0; i < len; i++)
		{
			//ch = tmpBA1[i];	//tmpStr.SetAt(i,tmpBA1.GetAt(i)); // CByteArray ×ª CString
			//tmpStr += ch;		
			//auto ch = tmpBA[len - i - 1];
			auto ch = (BYTE)tmpBA.GetAt(len - i - 1);
			tmpStr += HEXLOOKUP[(ch >> 4) & 0xF];
			tmpStr += HEXLOOKUP[ch & 0xF];
		}
		return tmpStr;
	case enc_mmword:
	case enc_xmmword:
	case enc_ymmword:
		//return QString(QByteArray((const char*)buffer, size).toHex());
		tmpBuffer = (const char*)buffer;
		len = size;
		tmpBA.SetSize(len);
		for (int i = 0; i < len; i++)
			//tmpBA[i] = tmpBuffer[i];
			tmpBA.SetAt(i, tmpBuffer[i]);
		//CByteArray tmpBA1 = ByteReverse(tmpBA); // ×Ö·û´®Ê×Î²»¥»»;	
		for (int i = 0; i < len; i++)
		{
			//ch = tmpBA1[i];	//tmpStr.SetAt(i,tmpBA1.GetAt(i)); // CByteArray ×ª CString
			//tmpStr += ch;		
			//auto ch = tmpBA[len - i - 1];
			auto ch = (BYTE)tmpBA.GetAt(len - i - 1);
			tmpStr += HEXLOOKUP[(ch >> 4) & 0xF];
			tmpStr += HEXLOOKUP[ch & 0xF];
		}
		return tmpStr;
	case enc_real4:
		return ToFloatString(buffer);
	case enc_real8:
		return ToDoubleString(buffer);
	case enc_real10:
		return ToLongDoubleString(buffer);
	case enc_ascii:
		return EscapeCh(*(const char*)buffer);
	case enc_unicode:
		return EscapeCh(*(const wchar_t*)buffer);
	default:
		return ToIntegralString<unsigned char>(buffer);
	}
}

bool GetCommentFormat(duint addr, CString & comment, bool* autoComment)
{
	//comment.clear();
	comment.Empty();
	char commentData[MAX_COMMENT_SIZE] = "";
	if (!DbgGetCommentAt(addr, commentData))
		return false;
	auto a = *commentData == '\1';
	if (autoComment)
		*autoComment = a;
	if (!strstr(commentData, "{"))
	{
		comment = commentData + a;
		return true;
	}
	char commentFormat[MAX_SETTING_SIZE] = "";
	if (DbgFunctions()->StringFormatInline(commentData + a, MAX_SETTING_SIZE, commentFormat))
		comment = commentFormat;
	else
		comment = commentData + a;
	return true;
}