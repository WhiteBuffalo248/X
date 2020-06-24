#include "stdafx.h"
#include "StdTable.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CStdTable::CStdTable()
{
}


CStdTable::~CStdTable()
{
}


/************************************************************************************
Sorting
************************************************************************************/
bool CStdTable::SortBy::AsText(const CString & a, const CString & b)
{
	auto i = a.Compare(b);
	if (i < 0)
		return true;
	if (i > 0)
		return false;
	return duint(&a) < duint(&b);
}

bool CStdTable::SortBy::AsInt(const CString & a, const CString & b)
{
	if (_ttoll(a) < _ttoll(b))
		return true;
	if (_ttoll(a) > _ttoll(b))
		return false;
	return duint(&a) < duint(&b);
}

bool CStdTable::SortBy::AsHex(const CString & a, const CString & b)
{
	string sa;
	sa = CW2A(a);
	string sb;
	sb = CW2A(b);

	LONGLONG lla = strtoll(sa.data(), NULL, 16);
	LONGLONG llb = strtoll(sb.data(), NULL, 16);
	if (lla < llb)
		return true;
	if (lla > llb)
		return false;
	return duint(&a) < duint(&b);
}

/************************************************************************************
Data Management
************************************************************************************/
void CStdTable::addColumnAt(int width, CString title, bool isClickable, CString copyTitle, SortBy::t sortFn)
{
	CAbstractTableView::addColumnAt(width, title, isClickable);

	//append empty column to list of rows
	for (size_t i = 0; i < mData.size(); i++)
		mData[i].push_back(CellData());

	//Append copy title
	if (!copyTitle.GetLength())
		mCopyTitles.push_back(title);
	else
		mCopyTitles.push_back(copyTitle);

	//append column sort function
	mColumnSortFunctions.push_back(sortFn);
}

void CStdTable::setCellContent(int r, int c, CString s)
{
	if (isValidIndex(r, c))
		mData[r][c].text = s;
}

CString CStdTable::getCellContent(int r, int c)
{
	if (isValidIndex(r, c))
		return mData[r][c].text;
	else
		return CString("");
}

void CStdTable::setCellUserdata(int r, int c, duint userdata)
{
	if (isValidIndex(r, c))
		mData[r][c].userdata = userdata;
}

duint CStdTable::getCellUserdata(int r, int c)
{
	return isValidIndex(r, c) ? mData[r][c].userdata : 0;
}

void CStdTable::setRowCount(dsint count)
{
	int wRowToAddOrRemove = count - int(mData.size());
	for (int i = 0; i < abs(wRowToAddOrRemove); i++)
	{
		if (wRowToAddOrRemove > 0)
		{
			mData.push_back(std::vector<CellData>());
			for (int j = 0; j < getColumnCount(); j++)
				mData[mData.size() - 1].push_back(CellData());
		}
		else
			mData.pop_back();
	}
	CAbstractTableView::setRowCount(count);
}

bool CStdTable::isValidIndex(int r, int c)
{
	if (r < 0 || c < 0 || r >= int(mData.size()))
		return false;
	return c < int(mData.at(r).size());
}
