#pragma once
#include "AbstractStdTable.h"
#include <functional>

class CStdTable :
	public CAbstractStdTable
{
public:
	CStdTable();
	~CStdTable();

public:
	// Sorting
	struct SortBy
	{
		typedef std::function<bool(const CString &, const CString &)> t;
		static bool AsText(const CString & a, const CString & b);
		static bool AsInt(const CString & a, const CString & b);
		static bool AsHex(const CString & a, const CString & b);
	};

public:
	// Data Management
	void addColumnAt(int width, CString title, bool isClickable, CString copyTitle = _T(""), SortBy::t sortFn = SortBy::AsText);
	void setRowCount(dsint count) override;
	void setCellContent(int r, int c, CString s);
	CString getCellContent(int r, int c);
	void setCellUserdata(int r, int c, duint userdata);
	duint getCellUserdata(int r, int c);
	bool isValidIndex(int r, int c);

protected:
	struct CellData
	{
		CString text;
		duint userdata = 0;
	};
	std::vector<std::vector<CellData>> mData; //listof(row) where row = (listof(col) where col = CellData)
	std::vector<SortBy::t> mColumnSortFunctions;

};

