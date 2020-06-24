#ifndef MISCUTIL_H
#define MISCUTIL_H



bool valueFromCString(CString& sourceText, ULONGLONG* value, int base);
bool valueFromCString(CString& sourceText, ULONG* value, int base);
void ByteReverse(CByteArray* array);
CString getSymbolicNameStr(ULONG addr);


#endif // MISCUTIL_H