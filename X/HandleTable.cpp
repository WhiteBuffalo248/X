#include "stdafx.h"
#include "HandleTable.h"
#include "HandlesPane.h"
#include "MessagesBPDlg.h"
#include "resource.h"
#include "GlobalID.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CHandleTable::CHandleTable()
{
	
	
}


CHandleTable::~CHandleTable()
{	
	
}

int CHandleTable::getGuiState() const
{
	return m_GuiState;
}

void CHandleTable::setTitle(bool has, int height, CString text)
{
	bHasTitle = has;
	iTitleHeight = height;
	cTitleText = text;
}


BEGIN_MESSAGE_MAP(CHandleTable, CTableVessel)
	ON_WM_CONTEXTMENU()
	ON_COMMAND_RANGE(ID_Hnadles_Refresh, ID_TcpConnections_Refresh, OnHandle)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()
	ON_WM_PAINT()
END_MESSAGE_MAP()



void CHandleTable::onMessagesBP()
{
	CMessagesBPDlg::MsgBreakpointData mbpData;

	if (!mCurList->getRowCount())
		return;

	mbpData.wndHandle = mCurList->getCellContent(mCurList->getInitialSelection(), 1);
	mbpData.procVA = mCurList->getCellContent(mCurList->getInitialSelection(), 0);

	CMessagesBPDlg messagesBPDialog(mbpData, this);
	messagesBPDialog.DoModal();
}

void CHandleTable::onFollowInDisasm()
{
	CString wCmd;
	wCmd.Format(_T("disasm %s"), mCurList->getCellContent(mCurList->getInitialSelection(), 0));
	DbgCmdExecDirect(CT2A(wCmd, CP_UTF8));
}

void CHandleTable::onToggleBP()
{
	if (!DbgIsDebugging())
		return;

	if (!mCurList->getRowCount())
		return;
	CString addrText = mCurList->getCellContent(mCurList->getInitialSelection(), 0);
	duint wVA;
	if (!DbgFunctions()->ValFromString(CT2A(addrText, CP_UTF8), &wVA))
		return;
	if (!DbgMemIsValidReadPtr(wVA))
		return;

	BPXTYPE wBpType = DbgGetBpxTypeAt(wVA);
	CString wCmd;

	if ((wBpType & bp_normal) == bp_normal)
	{
		wCmd += "bc ";
		wCmd += ToPtrString(wVA);
	}

	else if (wBpType == bp_none)
	{
		wCmd += "bp ";
		wCmd += ToPtrString(wVA);
	}

	if (DbgCmdExecDirect(CT2A(wCmd, CP_UTF8)))
		reloadData();
}

void CHandleTable::onShowSearch()
{
	pSearchDlg->ShowWindow(SW_SHOW);
	pSearchDlg->SetFocus();
}

void CHandleTable::OnHandle(UINT id)
{
	switch (id)
	{
	case ID_WinProg_Refresh:
	{
		CHandlesPane* pParent = (CHandlesPane*)GetParent();
		if (pParent->GetSafeHwnd())
			pParent->dataRefresh();
	}
	break;

	case ID_WinProg_Message_Break:
		onMessagesBP();
		break;
	case ID_WinProg_Follow_Pro:
		onFollowInDisasm();
		break;
	case ID_WinProg_Toggle_Break:
		onToggleBP();
		break;
	case ID_WinProg_Search:
		onShowSearch();
	default:
		break;
	}
}

void CHandleTable::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: 在此处添加消息处理程序代码
	if (!DbgIsDebugging())
		return;

	CMenu PopupMenu;
	// 创建弹出菜单
	BOOL bRet = PopupMenu.CreatePopupMenu();

	CHandlesPane* pParent = (CHandlesPane*)GetParent();
	HWND hwnd = GetSafeHwnd();
	if (hwnd == pParent->getCHandlesView()->GetSafeHwnd())
	{
		PopupMenu.AppendMenu(MF_ENABLED, ID_Hnadles_Refresh, _T("刷新"));
	}

	if (hwnd == pParent->getCWinProgView()->GetSafeHwnd())
	{
		PopupMenu.AppendMenu(MF_ENABLED, ID_WinProg_Refresh, _T("刷新"));
		if (mCurList->getRowCount())
		{
			CString enambleTitle = mCurList->getCellContent(mCurList->getInitialSelection(), 9) == _T("Enabled") ? _T("禁用窗口") : _T("激活窗口");
			PopupMenu.AppendMenu(MF_ENABLED, ID_WinProg_Enable, enambleTitle);
			PopupMenu.AppendMenu(MF_ENABLED, ID_WinProg_Follow_Pro, _T("在反汇编窗口跟随过程"));
			PopupMenu.AppendMenu(MF_ENABLED, ID_WinProg_Toggle_Break, _T("切换断点"));
			PopupMenu.AppendMenu(MF_ENABLED, ID_WinProg_Message_Break, _T("消息断点"));
			PopupMenu.AppendMenu(MF_ENABLED, ID_WinProg_Search, _T("搜索"));
			PopupMenu.AppendMenu(MF_SEPARATOR);
		}
	}

	if (hwnd == pParent->getCTcpConnectionsView()->GetSafeHwnd())
	{
		PopupMenu.AppendMenu(MF_ENABLED, ID_TcpConnections_Refresh, _T("刷新"));
	}

	int nCmd = PopupMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this); //TPM_RETURNCMD

}

int CHandleTable::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTableVessel::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	return 0;
}

void CHandleTable::OnSize(UINT nType, int cx, int cy)
{
	CTableVessel::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	stdList()->SetWindowPos(NULL, 0, iTitleHeight, cx, cy - iTitleHeight, NULL);
	stdSearchList()->SetWindowPos(NULL, 0, iTitleHeight, cx, cy - iTitleHeight, NULL);
}

void CHandleTable::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(tme);	//结构体缓冲区大小
	tme.dwFlags = TME_HOVER | TME_LEAVE;	//注册消息WM_MOUSEHOVER | WM_MOUSELEAVE
	tme.dwHoverTime = 1; //WM_MOUSEHOVER消息触发间隔时间
	tme.hwndTrack = m_hWnd; //当前窗口句柄
	::TrackMouseEvent(&tme); //注册发送消息

	bool wAccept = false;
	bool vHandle = true;


	switch (m_GuiState)
	{
	case CHandleTable::NoState:
		if (!bLButtonDown)
		{
			CURSORINFO ci;
			ci.cbSize = sizeof(CURSORINFO);
			GetCursorInfo(&ci);
			bool NSCursor = ci.hCursor == LoadCursor(NULL, IDC_SIZENS) ? true : false;

			if (vHandle && (NSCursor == false))
			{
				m_GuiState = GuiState::ReadyToResizeV;
			}
		}
		break;
	case CHandleTable::ReadyToResizeV:
		if (!bLButtonDown)
		{
			if (vHandle == false)
			{
				m_GuiState = CHandleTable::NoState;
			}
		}
		break;
	case CHandleTable::ResizeWindowState:
		if (abs(point.y - iResizinglastPosY) > 0)
		{
			iResizinglastPosY = point.y;
			CHandlesPane* pParent = (CHandlesPane*)GetParent();

			CRect clientRect;
			pParent->GetClientRect(&clientRect);

			HWND hwnd = GetSafeHwnd();

			/*
			if (hwnd == pParent->getCWinProgView()->GetSafeHwnd())
			{
			int upValue = pParent->iHight1 + point.y - LButtonDownPos;
			int downValue = pParent->iHight2 - point.y + LButtonDownPos;


			if (downValue > pParent->leastHeight)
			{
			if (upValue > pParent->leastHeight)
			{
			pParent->iHight1 += point.y - LButtonDownPos;
			pParent->iHight2 -= point.y - LButtonDownPos;
			}
			}
			else
			{
			pParent->iHight2 = pParent->leastHeight;
			if ((upValue + 2 * pParent->leastHeight) < clientRect.Height())
			pParent->iHight1 += point.y - LButtonDownPos;
			}
			}

			else if (hwnd == pParent->getCTcpConnectionsView()->GetSafeHwnd())
			{
			int iHight = pParent->iHight1 + pParent->iHight2 + point.y - LButtonDownPos;

			if (iHight < (pParent->iHight1 + pParent->leastHeight))
			{
			pParent->iHight1 += point.y - LButtonDownPos;
			pParent->iHight2 = pParent->leastHeight;

			if (iHight < (2 * pParent->leastHeight))
			pParent->iHight1 = pParent->leastHeight;
			}

			else if ((iHight + pParent->leastHeight) < clientRect.Height())
			{
			pParent->iHight2 += point.y - LButtonDownPos;
			}
			}
			*/

			//pParent->MyAdjustLayout();

			POINT p = point;
			MapWindowPoints((CWnd*)pParent, &p, 1);
			if (p.y > 0 && p.y < clientRect.Height())
				pParent->drawAdjustLine(p);
		}
		break;
	default:
		break;
	}


	if (!wAccept)
		CTableVessel::OnMouseMove(nFlags, point);
}

void CHandleTable::OnMouseLeave()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_GuiState = CHandleTable::NoState;

	CTableVessel::OnMouseLeave();
}

void CHandleTable::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	bLButtonDown = true;
	if (m_GuiState == CHandleTable::ReadyToResizeV)
	{
		iResizinglastPosY = point.y;
		iLButtonDownPos = iResizinglastPosY;
		m_GuiState = CHandleTable::ResizeWindowState;
		CHandlesPane* pParent = (CHandlesPane*)GetParent();
		pParent->RecordInitDC();
		POINT ParentPoint = point;
		MapWindowPoints((CWnd*)pParent, &ParentPoint, 1);
		pParent->drawAdjustLine(ParentPoint);
		SetCapture();
	}
	CTableVessel::OnLButtonDown(nFlags, point);
}

void CHandleTable::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	bLButtonDown = false;

	CHandlesPane* pParent = (CHandlesPane*)GetParent();
	pParent->m_intiDC.DeleteDC();

	CRect clientRect;
	pParent->GetClientRect(&clientRect);

	HWND hwnd = GetSafeHwnd();

	if (m_GuiState == CHandleTable::ResizeWindowState)
	{
		POINT ParentPoint = point;
		MapWindowPoints((CWnd*)pParent, &ParentPoint, 1);
		int height = ParentPoint.y - iLButtonDownPos;
		int heightSum = pParent->iHight1 + pParent->iHight2;
		int iheight3 = clientRect.Height() - heightSum;
		int moveAvlue = point.y - iLButtonDownPos;

		if (hwnd == pParent->getCWinProgView()->GetSafeHwnd())
		{
			if (height < pParent->leastHeight)
			{
				pParent->iHight2 = heightSum - pParent->leastHeight;
				pParent->iHight1 = pParent->leastHeight;
			}
			else if (height >(heightSum - pParent->leastHeight))
			{
				if (iheight3 > pParent->leastHeight)
				{
					int canMoveAvlue = iheight3 - pParent->leastHeight;
					if (abs(moveAvlue) < canMoveAvlue)
					{
						pParent->iHight1 += moveAvlue;
					}
					else
					{
						pParent->iHight1 = clientRect.Height() - pParent->leastHeight * 2;
					}
				}
				else
				{
					pParent->iHight1 = clientRect.Height() - pParent->leastHeight * 2;
				}
				pParent->iHight2 = pParent->leastHeight;
			}
			else
			{
				pParent->iHight1 += moveAvlue;
				pParent->iHight2 -= moveAvlue;
			}
		}

		if (hwnd == pParent->getCTcpConnectionsView()->GetSafeHwnd())
		{
			if (height < (pParent->iHight1 + pParent->leastHeight))
			{
				if (pParent->iHight1 > pParent->leastHeight)
				{
					int canMoveAvlue = pParent->iHight1 - pParent->leastHeight;
					if (abs(moveAvlue) < canMoveAvlue)
						pParent->iHight1 += moveAvlue;
					else
					{
						pParent->iHight1 = pParent->leastHeight;
					}
				}
				else
				{
					pParent->iHight1 = pParent->leastHeight;
				}
				pParent->iHight2 = pParent->leastHeight;
			}
			else if ((height + pParent->leastHeight) > clientRect.Height())
			{
				pParent->iHight2 = clientRect.Height() - pParent->iHight1 - pParent->leastHeight;
			}
			else
			{
				pParent->iHight2 += moveAvlue;
			}
		}

		pParent->MyAdjustLayout();
	}

	m_GuiState = CHandleTable::NoState;
	ReleaseCapture();
	CTableVessel::OnLButtonUp(nFlags, point);

}

BOOL CHandleTable::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_GuiState == ReadyToResizeV)
	{
		SetCursor(LoadCursor(NULL, IDC_SIZENS));
		return TRUE;
	}

	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CHandleTable::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CWnd::OnPaint()
					   // Paints title
	CRect viewRect;
	GetClientRect(&viewRect);

	LOGFONT defaultfont;
	memset(&defaultfont, 0, sizeof(defaultfont));
	defaultfont.lfWeight = FW_NORMAL;
	_tcscpy_s(defaultfont.lfFaceName, LF_FACESIZE, _T("Rockwell"));
	defaultfont.lfHeight = MulDiv(12, 96, 72);;

	CFont font;
	VERIFY(font.CreateFontIndirect(&defaultfont));
	dc.SelectObject(&font);

	CRect titleRect = viewRect;
	titleRect.bottom = iTitleHeight;
	dc.FillSolidRect(&titleRect, RGB(200, 200, 200));
	dc.DrawEdge(titleRect, EDGE_ETCHED, BF_RECT);
	CRect textRect = titleRect;
	textRect.DeflateRect(2, 2);
	dc.DrawText(cTitleText, textRect, DT_LEFT | DT_VCENTER);

	/*
	CDC MenDC;
	CBitmap MemMap;
	MenDC.CreateCompatibleDC(&dc);
	MemMap.CreateCompatibleBitmap(&dc, viewRect.Width(), viewRect.Height());
	MenDC.SelectObject(&MemMap);
	if (bHasTitle)
	{

	CRect titleRect = viewRect;
	titleRect.bottom = iTitleHeight;
	MenDC.FillSolidRect(&titleRect, RGB(200, 200, 200));
	MenDC.DrawEdge(titleRect, EDGE_ETCHED, BF_RECT);
	CRect textRect = titleRect;
	textRect.DeflateRect(2, 0);
	MenDC.DrawText(cTitleText, textRect, DT_LEFT | DT_VCENTER);
	}

	dc.BitBlt(0, 0, viewRect.Width(), viewRect.Height(), &MenDC, 0, 0, SRCCOPY);
	*/

}
