#include "stdafx.h"
#include "MyByteArray.h"

int CMyByteArray::m_defoultConstructionCount = 0;
int CMyByteArray::m_myConstructionCount = 0;
int CMyByteArray::m_equConstructionCount = 0;

CMyByteArray::CMyByteArray()
{
	m_defoultConstructionCount++;
}


CMyByteArray::~CMyByteArray()
{
}
