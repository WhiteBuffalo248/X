#pragma once
#include "afxwin.h"
#include "AbstractSearchList.h"

// CSearchDlg 对话框

class CSearchDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSearchDlg)

public:
	CSearchDlg(CAbstractSearchList* abstractSearchList, CAbstractStdTable** curList, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSearchDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_Search_Dlg };
#endif
/*
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
void filter(const CString & filter, FilterType type, int startColumn);
bool rowMatchesFilter(const CString & filter, FilterType type, int row, int startColumn) const;
*/


private:
	CAbstractSearchList* mAbstractSearchList;
	CAbstractStdTable** mCurList;
	CWnd* pParent;
	int mSearchStartCol = 0;

protected:
	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnEnChangeSearchEdit();
	CEdit m_SearchEdit;
};
