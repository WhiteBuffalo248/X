// TableVessel.cpp : 实现文件
//

#include "stdafx.h"
#include "X.h"
#include "TableVessel.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


class StdTableSearchList : public CAbstractSearchList
{
public:
	friend class CTableVessel;

	StdTableSearchList()
	{
		mList = new CStdTable();
		mSearchList = new CStdTable();
	}
	~StdTableSearchList()
	{
		delete mList;
		delete mSearchList;
	}

	void lock() override { }
	void unlock() override { }
	CAbstractStdTable* list() const override { return mList; }
	CAbstractStdTable* searchList() const override { return mSearchList; }

	void filter(const CString & filter, FilterType type, int startColumn) override
	{
		mSearchList->setRowCount(0);
		int rows = mList->getRowCount();
		int columns = mList->getColumnCount();
		for (int i = 0, j = 0; i < rows; i++)
		{
			if (rowMatchesFilter(filter, type, i, startColumn))
			{
				mSearchList->setRowCount(j + 1);
				for (int k = 0; k < columns; k++)
				{
					mSearchList->setCellContent(j, k, mList->getCellContent(i, k));
					mSearchList->setCellUserdata(j, k, mList->getCellUserdata(i, k));
				}
				j++;
			}
		}
	}

private:
	CStdTable* mList;
	CStdTable* mSearchList;
};
// CTableVessel

IMPLEMENT_DYNAMIC(CTableVessel, CWnd)

CTableVessel::CTableVessel()
{
	mSearchListData = new StdTableSearchList();
	mCurList = stdList();
	enableMultiSelection(true);
	setAddressColumn(0);
	setShowHeader(true);
}

CTableVessel::~CTableVessel()
{
	delete mSearchListData;
	delete pSearchDlg;
}

int CTableVessel::getCharWidth()
{
	return stdList()->getCharWidth();
}

void CTableVessel::setShowHeader(bool show)
{
	stdList()->setShowHeader(true);
	stdSearchList()->setShowHeader(true);
}

void CTableVessel::addColumnAt(int width, CString title, bool isClickable)
{
	stdList()->addColumnAt(width, title, isClickable);
	stdSearchList()->addColumnAt(width, title, isClickable);
}

void CTableVessel::enableMultiSelection(bool enabled)
{
	stdList()->enableMultiSelection(enabled);
	stdSearchList()->enableMultiSelection(enabled);
}

void CTableVessel::setAddressColumn(int col, bool cipBase)
{
	stdList()->setAddressColumn(col, cipBase);
	stdSearchList()->setAddressColumn(col, cipBase);
}

void CTableVessel::loadColumnFromConfig(const CString & viewName)
{
	stdList()->loadColumnFromConfig(viewName);
	stdSearchList()->loadColumnFromConfig(viewName);
}

void CTableVessel::setRowCount(dsint count)
{
	//mSearchBox->setText("");
	stdList()->setRowCount(count);
}

void CTableVessel::setCellContent(int r, int c, CString s)
{
	//mSearchBox->setText("");

	stdList()->setCellContent(r, c, s);
}

void CTableVessel::reloadData()
{
	//mSearchBox->setText("");
	mCurList->reloadData();
	mCurList->SetFocus();
}

CStdTable* CTableVessel::stdList()
{
	return mSearchListData->mList;
}

CStdTable* CTableVessel::stdSearchList()
{
	return mSearchListData->mSearchList;
}

BEGIN_MESSAGE_MAP(CTableVessel, CWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()



// CTableVessel 消息处理程序

int CTableVessel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	stdList()->Create(NULL, _T("Handle"), WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL, CRect(0, 0, lpCreateStruct->cx, lpCreateStruct->y), this, NULL);
	stdSearchList()->Create(NULL, _T("SearchHandle"), WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL, CRect(0, 0, lpCreateStruct->cx, lpCreateStruct->y), this, NULL);
	pSearchDlg = new CSearchDlg(mSearchListData, &mCurList, this);
	pSearchDlg->Create(IDD_Search_Dlg, this);

	stdList()->ShowWindow(SW_SHOW);
	stdSearchList()->ShowWindow(SW_HIDE);

	return 0;
}