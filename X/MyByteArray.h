#pragma once
#include "afxcoll.h"
class CMyByteArray :
	public CByteArray
{
public:
	CMyByteArray();
	CMyByteArray(const CMyByteArray& by) {
		m_myConstructionCount++;
		this->Copy(by);
	}
	CMyByteArray& operator = (const CMyByteArray& by) {
		m_equConstructionCount++;
		this->Copy(by);
		return *this;
	}
	~CMyByteArray();
	static int m_defoultConstructionCount;
	static int m_myConstructionCount;
	static int m_equConstructionCount;
};

