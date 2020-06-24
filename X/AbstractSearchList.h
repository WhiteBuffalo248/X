#pragma once
#include "stdafx.h"
#include "AbstractStdTable.h"

class CAbstractSearchList
{
public:
	enum FilterType
	{
		FilterStartsWithTextCaseSensitive, //unused
		FilterStartsWithTextCaseInsensitive, //unused
		FilterContainsTextCaseSensitive, //unused
		FilterContainsTextCaseInsensitive,
		FilterRegexCaseSensitive,
		FilterRegexCaseInsensitive
	};

	virtual void lock() = 0;
	virtual void unlock() = 0;
	virtual CAbstractStdTable* list() const = 0;
	virtual CAbstractStdTable* searchList() const = 0;
	virtual void filter(const CString & filter, FilterType type, int startColumn) = 0;

	bool rowMatchesFilter(const CString & filter, FilterType type, int row, int startColumn) const
	{
		int count = list()->getColumnCount();
		if (startColumn + 1 > count)
			return false;
		
		switch (type)
		{
		case FilterStartsWithTextCaseSensitive:
			break;
		case FilterStartsWithTextCaseInsensitive:
			for (int i = startColumn; i < count; i++)
			{
				CString content = list()->getCellContent(row, i);
				CString filterText = filter;
				content.MakeLower();
				filterText.MakeLower();
				if (content.Find(filterText) == 0)
					return true;
			}
			break;
		case FilterContainsTextCaseSensitive:
			break;
		case FilterContainsTextCaseInsensitive:
			for (int i = startColumn; i < count; i++)
			{
				CString content = list()->getCellContent(row, i);
				CString filterText = filter;
				content.MakeLower();
				filterText.MakeLower();
				if (content.Find(filterText) != -1)
					return true;				
			}					
			break;
		case FilterRegexCaseSensitive:
			break;
		case FilterRegexCaseInsensitive:
			break;
		default:
			__debugbreak();
		}
		return false;
	}
};
