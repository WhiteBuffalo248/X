// SearchDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "X.h"
#include "SearchDlg.h"
#include "afxdialogex.h"



#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// CSearchDlg �Ի���

IMPLEMENT_DYNAMIC(CSearchDlg, CDialogEx)

CSearchDlg::CSearchDlg(CAbstractSearchList* abstractSearchList, CAbstractStdTable** curList, CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_Search_Dlg, pParent), pParent(pParent), mAbstractSearchList(abstractSearchList), mCurList(curList)
{
	
}

CSearchDlg::~CSearchDlg()
{
}

void CSearchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_Search_EDIT, m_SearchEdit);
}


BEGIN_MESSAGE_MAP(CSearchDlg, CDialogEx)
	ON_EN_CHANGE(IDC_Search_EDIT, &CSearchDlg::OnEnChangeSearchEdit)
END_MESSAGE_MAP()


// CSearchDlg ��Ϣ�������


BOOL CSearchDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	CString title;
	pParent->GetWindowText(title);
	SetWindowText(title);

	m_SearchEdit.SetCueBanner(_T("�ڴ˼���..."), TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


void CSearchDlg::OnEnChangeSearchEdit()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	// store the first selection value

	mAbstractSearchList->lock();

	//CTableVessel* pTableVessel = (CTableVessel*)pParent;	
	//auto mCurList = pTableVessel->stdCurList();
	CString mLastFirstColValue;
	auto selList = (*mCurList)->getSelection();

	if (!selList.empty() && (*mCurList)->isValidIndex(selList[0], 0))
		mLastFirstColValue = (*mCurList)->getCellContent(selList[0], 0);

	// get the correct previous list instance
	BOOL s = mAbstractSearchList->list()->IsWindowVisible();
	auto mPrevList = mAbstractSearchList->list()->IsWindowVisible() ? mAbstractSearchList->list() : mAbstractSearchList->searchList();

	CString text;
	m_SearchEdit.GetWindowText(text);

	if (text.GetLength())
	{
		mAbstractSearchList->list()->ShowWindow(SW_HIDE);
		mAbstractSearchList->searchList()->ShowWindow(SW_SHOW);
		(*mCurList) = mAbstractSearchList->searchList();

		// filter the list
		auto filterType = CAbstractSearchList::FilterContainsTextCaseInsensitive;
		
		mAbstractSearchList->filter(text, filterType, mSearchStartCol);
	}
	else
	{
		mAbstractSearchList->searchList()->ShowWindow(SW_HIDE);
		mAbstractSearchList->list()->ShowWindow(SW_SHOW);
		(*mCurList) = mAbstractSearchList->list();
	}

	// attempt to restore previous selection
	bool hasSetSingleSelection = false;
	if (!mLastFirstColValue.IsEmpty())
	{
		int rows = (*mCurList)->getRowCount();
		(*mCurList)->setTableOffset(0);
		for (int i = 0; i < rows; i++)
		{
			if ((*mCurList)->getCellContent(i, 0) == mLastFirstColValue)
			{
				if (rows > (*mCurList)->getViewableRowsCount())
				{
					int cur = i - (*mCurList)->getViewableRowsCount() / 2;
					if (!(*mCurList)->isValidIndex(cur, 0))
						cur = i;
					(*mCurList)->setTableOffset(cur);
				}
				(*mCurList)->setSingleSelection(i);
				hasSetSingleSelection = true;
				break;
			}
		}
	}
	if (!hasSetSingleSelection)
		(*mCurList)->setSingleSelection(0);

	//if (!mCurList->getRowCount())
		//emit emptySearchResult();

	// Do not highlight with regex
	// TODO: fully respect highlighting mode

	if (!(((CButton *)GetDlgItem(IDC_Regex_CHECK))->GetCheck()))
		mAbstractSearchList->searchList()->setHighlightText(text);
	else
		mAbstractSearchList->searchList()->setHighlightText(CString());

	// Reload the search list data
	mAbstractSearchList->searchList()->reloadData();

	// setup the same layout of the previous list control
	if (mPrevList != (*mCurList))
	{
		int cols = mPrevList->getColumnCount();
		for (int i = 0; i < cols; i++)
		{
			//(*mCurList)->setColumnOrder(i, mPrevList->getColumnOrder(i));
			(*mCurList)->setColumnHidden(i, mPrevList->getColumnHidden(i));
			(*mCurList)->setColumnWidth(i, mPrevList->getColumnWidth(i));
		}
	}
	mAbstractSearchList->unlock();
}
