#include "stdafx.h"
#include "ReferenceTable.h"
#include "GlobalID.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CReferenceTable::CReferenceTable()
{
	m_ProgressCtrlHeight = 20;
}


CReferenceTable::~CReferenceTable()
{
}

void CReferenceTable::addColumnAtRef(int width, CString title)
{
	int charwidth = getCharWidth();
	if (width)
		width = charwidth * width + 8;
	else
		width = 0;
	//mSearchBox->setText("");
	CString lowerTitle = title;
	if (lowerTitle.MakeLower() == "&data&")
		title = "Data";
	CTableVessel::addColumnAt(width, title, true);
}

void CReferenceTable::setRowCount(dsint count)
{
	if (!stdList()->getRowCount() && count) //from zero to N rows
		searchSelectionChanged(0);
//	emit mCountTotalLabel->setText(QString("%1").arg(count));
	CString countText;
	countText.Format(_T("%d"), count);
	m_ProgressCtrl.getItemCountStatic()->setStaticText(countText);
	CTableVessel::setRowCount(count);
}

void CReferenceTable::searchSelectionChanged(int index)
{
	DbgValToString("$__disasm_refindex", index);
	DbgValToString("$__dump_refindex", index);
}

void CReferenceTable::referenceSetProgress(int progress)
{
	CString labalText;
	labalText.Format(_T("Total Progress %d%%"), progress);
	m_ProgressCtrl.getTotalProgressCtrl()->setLabalText(labalText);
	m_ProgressCtrl.getTotalProgressCtrl()->SetPos(progress);
	
}

void CReferenceTable::referenceSetCurrentTaskProgress(int progress, CString taskTitle)
{
	CString labalText = taskTitle;
	labalText.AppendFormat(_T(" %d%%"), progress);
	m_ProgressCtrl.getRegionProgressCtrl()->setLabalText(labalText);
	m_ProgressCtrl.getRegionProgressCtrl()->SetPos(progress);
}

void CReferenceTable::onShowSearch()
{
	pSearchDlg->ShowWindow(SW_SHOW);
	pSearchDlg->SetFocus();
}

void CReferenceTable::OnHandle(UINT id)
{
	switch (id)
	{
	case ID_Reference_Search:
		onShowSearch();
		break;
	default:
		break;
	}
}

BEGIN_MESSAGE_MAP(CReferenceTable, CTableVessel)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND_RANGE(ID_Reference_Search, ID_Reference_Search, OnHandle)
END_MESSAGE_MAP()


void CReferenceTable::OnSize(UINT nType, int cx, int cy)
{
	CTableVessel::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码

	stdList()->SetWindowPos(NULL, 0, 0, cx, cy - m_ProgressCtrlHeight, NULL);
	stdSearchList()->SetWindowPos(NULL, 0, 0, cx, cy - m_ProgressCtrlHeight, NULL);
	m_ProgressCtrl.SetWindowPos(NULL, 0, cy - m_ProgressCtrlHeight, cx, m_ProgressCtrlHeight, NULL);
}



int CReferenceTable::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTableVessel::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	m_ProgressCtrl.Create(NULL, _T("progress"), WS_CHILD | WS_VISIBLE, CRect(0, lpCreateStruct->cx - m_ProgressCtrlHeight, lpCreateStruct->cx, m_ProgressCtrlHeight), this, NULL);
	return 0;
}


void CReferenceTable::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: 在此处添加消息处理程序代码
	if (!mCurList->getRowCount())
		return;

	CMenu PopupMenu;
	// 创建弹出菜单
	BOOL bRet = PopupMenu.CreatePopupMenu();

	PopupMenu.AppendMenu(MF_ENABLED, ID_Reference_Search, _T("搜索"));
	int nCmd = PopupMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this); //TPM_RETURNCMD
}
