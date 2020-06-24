// HandlesPane.cpp : 实现文件
//

#include "stdafx.h"
#include "X.h"
#include "HandlesPane.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// CHandlesPane

IMPLEMENT_DYNAMIC(CHandlesPane, CDockablePane)

CHandlesPane::CHandlesPane()
{
	leastHeight = 40;

	iHight1 = Config()->getUint("DefaultFrameData", "HandleHight1");
	iHight2 = Config()->getUint("DefaultFrameData", "HandleHight2");

	m_wndHandlesView.addColumnAt(200, _T("Type"), false);
	m_wndHandlesView.addColumnAt(200, _T("Type number"), true);
	m_wndHandlesView.addColumnAt(222, _T("Handle"), true);
	m_wndHandlesView.addColumnAt(200, _T("Access"), true);
	m_wndHandlesView.addColumnAt(200, _T("Name"), true);
	
	m_wndHandlesView.loadColumnFromConfig(_T("Handle"));
	

	m_wndWinProgView.addColumnAt(200, _T("Proc"), false);
	m_wndWinProgView.addColumnAt(200, _T("Handle"), true);
	m_wndWinProgView.addColumnAt(222, _T("Title"), true);
	m_wndWinProgView.addColumnAt(200, _T("Class"), true);
	m_wndWinProgView.addColumnAt(200, _T("Thread"), true);
	m_wndWinProgView.addColumnAt(200, _T("Style"), false);
	m_wndWinProgView.addColumnAt(200, _T("StyleEx"), true);
	m_wndWinProgView.addColumnAt(222, _T("Parent"), true);
	m_wndWinProgView.addColumnAt(200, _T("Size"), true);
	m_wndWinProgView.addColumnAt(200, _T("Enable"), true);
	m_wndWinProgView.setTitle(true, 20, _T("Window"));
	m_wndWinProgView.loadColumnFromConfig(_T("Window"));
	

	m_wndTcpConnectionsView.addColumnAt(200, _T("Remote address"), false);
	m_wndTcpConnectionsView.addColumnAt(200, _T("Local address"), true);
	m_wndTcpConnectionsView.addColumnAt(222, _T("State"), true);
	m_wndTcpConnectionsView.setTitle(true, 20, _T("TcpConnection"));
	m_wndTcpConnectionsView.loadColumnFromConfig(_T("TcpConnection"));
	
}

CHandlesPane::~CHandlesPane()
{
	auto cfg = Config();
	cfg->setUint("DefaultFrameData", "HandleHight1", iHight1);
	cfg->setUint("DefaultFrameData", "HandleHight2", iHight2);
}

//Enum functions
//Enumerate handles and update handles table
void CHandlesPane::enumHandles()
{
	BridgeList<HANDLEINFO> handles;
	if (DbgFunctions()->EnumHandles(&handles))
	{
		auto count = handles.Count();
		m_wndHandlesView.setRowCount(count);
		for (auto i = 0; i < count; i++)
		{
			const HANDLEINFO & handle = handles[i];
			char name[MAX_STRING_SIZE] = "";
			char typeName[MAX_STRING_SIZE] = "";
			DbgFunctions()->GetHandleName(handle.Handle, name, sizeof(name), typeName, sizeof(typeName));
			m_wndHandlesView.setCellContent(i, 0, (CString)CA2T(typeName, CP_UTF8));
			m_wndHandlesView.setCellContent(i, 1, ToHexString(handle.TypeNumber));
			m_wndHandlesView.setCellContent(i, 2, ToHexString(handle.Handle));
			m_wndHandlesView.setCellContent(i, 3, ToHexString(handle.GrantedAccess));
			m_wndHandlesView.setCellContent(i, 4, (CString)CA2T(name, CP_UTF8));
		}
	}
	else
		m_wndHandlesView.setRowCount(0);
	m_wndHandlesView.reloadData();
	// refresh values also when in mSearchList
	//mHandlesTable->refreshSearchList();
}

//Enumerate windows and update windows table
void CHandlesPane::enumWindows()
{
	BridgeList<WINDOW_INFO> windows;
	if (DbgFunctions()->EnumWindows(&windows))
	{
		auto count = windows.Count();
		m_wndWinProgView.setRowCount(count);
		for (auto i = 0; i < count; i++)
		{
			m_wndWinProgView.setCellContent(i, 0, ToPtrString(windows[i].wndProc));
			m_wndWinProgView.setCellContent(i, 1, ToHexString(windows[i].handle));
			m_wndWinProgView.setCellContent(i, 2, (CString)CA2T(windows[i].windowTitle, CP_UTF8));
			m_wndWinProgView.setCellContent(i, 3, (CString)CA2T(windows[i].windowClass, CP_UTF8));
			char threadname[MAX_THREAD_NAME_SIZE];
			if (DbgFunctions()->ThreadGetName(windows[i].threadId, threadname))
				m_wndWinProgView.setCellContent(i, 4, (CString)CA2T(threadname, CP_UTF8));
			else if (Config()->getBool("Gui", "PidInHex"))
				m_wndWinProgView.setCellContent(i, 4, ToHexString(windows[i].threadId));
			else
			{
				CString threadId;
				threadId.Format(_T("%d"), windows[i].threadId);
				m_wndWinProgView.setCellContent(i, 4, threadId);
			}


			//Style
			m_wndWinProgView.setCellContent(i, 5, ToHexString(windows[i].style));
			//StyleEx
			m_wndWinProgView.setCellContent(i, 6, ToHexString(windows[i].styleEx));
			m_wndWinProgView.setCellContent(i, 7, ToHexString(windows[i].parent) + (windows[i].parent == ((duint)GetDesktopWindow()) ? _T(" (Desktop window)") : _T("")));
			//Size
			//QString sizeText = QString("(%1,%2);%3x%4").arg(windows[i].position.left).arg(windows[i].position.top)
			//	.arg(windows[i].position.right - windows[i].position.left).arg(windows[i].position.bottom - windows[i].position.top);
			CString sizeText;
			sizeText.Format(_T("(%d,%d);%dx%d"), windows[i].position.left, windows[i].position.right, \
				windows[i].position.right - windows[i].position.left, windows[i].position.bottom - windows[i].position.top);
			m_wndWinProgView.setCellContent(i, 8, sizeText);
			m_wndWinProgView.setCellContent(i, 9, windows[i].enabled != FALSE ? _T("Enabled") : _T("Disabled"));
		}
	}
	else
		m_wndWinProgView.setRowCount(0);
	m_wndWinProgView.reloadData();
	// refresh values also when in mSearchList
	//mWindowsTable->refreshSearchList();
}

//Enumerate TCP connections and update TCP connections table
void CHandlesPane::enumTcpConnections()
{
	BridgeList<TCPCONNECTIONINFO> connections;
	if (DbgFunctions()->EnumTcpConnections(&connections))
	{
		auto count = connections.Count();
		m_wndTcpConnectionsView.setRowCount(count);
		CString Text;
		for (auto i = 0; i < count; i++)
		{
			const TCPCONNECTIONINFO & connection = connections[i];

			Text.Format(_T("%s:%d"), CA2T(connection.RemoteAddress, CP_UTF8), connection.RemotePort);
			m_wndTcpConnectionsView.setCellContent(i, 0, Text);
			Text.Format(_T("%s:%d"), CA2T(connection.LocalAddress, CP_UTF8), connection.LocalPort);
			m_wndTcpConnectionsView.setCellContent(i, 1, Text);
			m_wndTcpConnectionsView.setCellContent(i, 2, (CString)CA2T(connection.StateText, CP_UTF8));
		}
	}
	else
		m_wndTcpConnectionsView.setRowCount(0);
	m_wndTcpConnectionsView.reloadData();
	// refresh values also when in mSearchList
	//mTcpConnectionsTable->refreshSearchList();
}



BEGIN_MESSAGE_MAP(CHandlesPane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CHandlesPane 消息处理程序


void CHandlesPane::dataRefresh()
{
	if (DbgIsDebugging())
	{
		enumHandles();
		enumWindows();
		enumTcpConnections();
		//enumHeaps();
		//enumPrivileges();
	}
	else
	{
		m_wndHandlesView.setRowCount(0);
		m_wndHandlesView.reloadData();
		m_wndTcpConnectionsView.setRowCount(0);
		m_wndTcpConnectionsView.reloadData();
		m_wndWinProgView.setRowCount(0);
		m_wndWinProgView.reloadData();

		//mHeapsTable->setRowCount(0);
		//mHeapsTable->reloadData();
		//mPrivilegesTable->setRowCount(0);
		//mPrivilegesTable->reloadData();
	}
}

void CHandlesPane::MyAdjustLayout()
{
	CRect clientRect;
	GetClientRect(&clientRect);
	m_wndHandlesView.SetWindowPos(NULL, 0, 0, clientRect.Width(), iHight1, SWP_NOZORDER);
	m_wndHandlesView.Invalidate();
	m_wndWinProgView.SetWindowPos(NULL, 0, iHight1, clientRect.Width(), iHight2, SWP_NOZORDER);
	m_wndWinProgView.Invalidate();
	m_wndTcpConnectionsView.SetWindowPos(NULL, 0, iHight1 + iHight2, clientRect.Width(), clientRect.Height() - iHight1 - iHight2, SWP_NOZORDER);
	m_wndTcpConnectionsView.Invalidate();
}

void CHandlesPane::RecordInitDC()
{
	CRect viewRect;
	GetClientRect(&viewRect);
	m_intiDC.CreateCompatibleDC(GetDCEx(NULL, DCX_CACHE));
	CBitmap intMap;
	intMap.CreateCompatibleBitmap(GetDCEx(NULL, DCX_CACHE), viewRect.Width(), viewRect.Height());
	m_intiDC.SelectObject(&intMap);
	m_intiDC.BitBlt(0, 0, viewRect.Width(), viewRect.Height(), GetDCEx(NULL, DCX_CACHE), 0, 0, SRCCOPY);
}

void CHandlesPane::drawAdjustLine(CPoint point)
{
	CRect viewRect;
	GetClientRect(&viewRect);
	
	CDC* pDC = GetDCEx(NULL, DCX_CACHE);
	pDC->BitBlt(0, 0, viewRect.Width(), viewRect.Height(), &m_intiDC, 0, 0, SRCCOPY);

	CPen pen;
	pen.CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
	pDC->SelectObject(&pen);

	pDC->MoveTo(viewRect.left, point.y);
	pDC->LineTo(viewRect.right, point.y);
}

int CHandlesPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	m_wndHandlesView.Create(NULL, _T("Handle"), WS_CHILD | WS_VISIBLE, CRect(0, 0, lpCreateStruct->cx, lpCreateStruct->cy), this, NULL);
	m_wndWinProgView.Create(NULL, _T("Window"), WS_CHILD | WS_VISIBLE, CRect(0, 0, lpCreateStruct->cx, lpCreateStruct->cy), this, NULL);
	m_wndTcpConnectionsView.Create(NULL, _T("TcpConnection"), WS_CHILD | WS_VISIBLE, CRect(0, 0, lpCreateStruct->cx, lpCreateStruct->cy), this, NULL);
	return 0;
}


void CHandlesPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	/*
	int hight = cy / 3;
	m_wndHandlesView.SetWindowPos(NULL, 0, 0, cx, hight, SWP_NOZORDER);
	m_wndWinProgView.SetWindowPos(NULL, 0, hight, cx, hight, SWP_NOZORDER);
	m_wndTcpConnectionsView.SetWindowPos(NULL, 0, 2 * hight, cx, cy - 2 *hight, SWP_NOZORDER);
	*/
	MyAdjustLayout();
	
}


void CHandlesPane::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CDockablePane::OnPaint()
	CRect viewRect;
	GetClientRect(&viewRect);
	CDC MenDC;
	CBitmap MemMap;
	MenDC.CreateCompatibleDC(&dc);
	MemMap.CreateCompatibleBitmap(&dc, viewRect.Width(), viewRect.Height());
	MenDC.SelectObject(&MemMap);
	MenDC.FillSolidRect(&viewRect, RGB(128, 128, 128));
	dc.BitBlt(0, 0, viewRect.Width(), viewRect.Height(), &MenDC, 0, 0, SRCCOPY);
}
