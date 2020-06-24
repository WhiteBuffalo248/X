// TestPane.cpp : 实现文件
//

#include "stdafx.h"
#include "X.h"
#include "DisasmPane.h"
using namespace Gdiplus;

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

// CDisasmPane
IMPLEMENT_DYNAMIC(CDisasmPane, CDockablePane)

CDisasmPane::CDisasmPane()
{
	m_guiState = GuiState::NoState;
	m_mouseButtonState = MouseButtonState::MouseButtonNoState;
	m_focusState = FocusState::D;
	m_ResizingX = false;
	m_ResizingY = false;
	m_bOnsize = false;
	m_splitWidth = 1;
	m_splitHight = 1;
	m_defaultWidth = 20;
	m_defaultHight = 40;
	m_space = 2;
	
	iWidth = Config()->getUint("DefaultFrameData", "SideBarWidth");	
	iHight = Config()->getUint("DefaultFrameData", "OutputWndHight");

	if (iWidth < m_defaultWidth || iWidth > GetSystemMetrics(SM_CXSCREEN))
		iWidth = m_defaultWidth;
	if (iHight < m_defaultHight || iHight > GetSystemMetrics(SM_CYSCREEN))
		iHight = m_defaultHight;
}

CDisasmPane::~CDisasmPane()
{
	auto cfg = Config();
	cfg->setUint("DefaultFrameData", "SideBarWidth", iWidth);
	cfg->setUint("DefaultFrameData", "OutputWndHight", iHight);
}

BEGIN_MESSAGE_MAP(CDisasmPane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_MOUSELEAVE()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEHOVER()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_CONTEXTMENU()	
END_MESSAGE_MAP()

void CDisasmPane::ActivePane()
{
	
	CMFCBaseTabCtrl* pParentTab = DYNAMIC_DOWNCAST(CMFCBaseTabCtrl, GetParent());
	if (pParentTab == NULL)
	{
	ASSERT(FALSE);
	return;
	}

	ASSERT_VALID(pParentTab);
	pParentTab->SetActiveTab(pParentTab->GetTabFromHwnd(GetSafeHwnd()));
	
	
	
}

// CDisasmPane 消息处理程序
int CDisasmPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	m_wndDisasmView.Create(NULL, NULL, WS_CHILD | WS_VISIBLE,// | WS_BORDER,
		CRect(lpCreateStruct->cx / 5, 0, lpCreateStruct->cx * 4 / 5, lpCreateStruct->cy * 4 / 5), this, NULL);

	m_wndCPUSideBar.Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, lpCreateStruct->cx / 5, lpCreateStruct->cy * 4 / 5), this, NULL);
	
	m_wndOutputBox.Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0, lpCreateStruct->cy * 4 / 5, lpCreateStruct->cx, lpCreateStruct->cy / 5), this, NULL);
	
	return 0;
}

void CDisasmPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);	
	// TODO:  在此处添加消息处理程序代码
	
	CRect viewRect;
	GetClientRect(&viewRect);

	cx = viewRect.Width();
	cy = viewRect.Height();

	m_bOnsize = true;
	int nNoSplitWidth = cx - 4 * m_splitWidth - m_space;
	int nNoSplitHight = cy - 4 * m_splitHight - m_space;

	m_bOnsize = false;
	
	m_wndCPUSideBar.SetWindowPos(NULL, m_splitWidth, m_splitHight, iWidth, nNoSplitHight - iHight, SWP_NOACTIVATE);
	m_wndDisasmView.SetWindowPos(NULL, iWidth + 3 * m_splitWidth + m_space, m_splitHight, nNoSplitWidth - iWidth, nNoSplitHight - iHight, SWP_NOACTIVATE);
	if (m_guiState != GuiState::ResizeStateH)
		m_wndOutputBox.SetWindowPos(NULL, m_splitWidth, nNoSplitHight - iHight + 3 * m_splitHight + m_space, cx - 2 * m_splitWidth, iHight, SWP_NOACTIVATE);
		
	/*
	HDWP hdwp = BeginDeferWindowPos(3);
	hdwp = DeferWindowPos(hdwp, m_wndCPUSideBar.m_hWnd, NULL, m_splitWidth, m_splitHight, w, nNoSplitHight - h, SWP_NOACTIVATE | SWP_NOZORDER | SWP_DRAWFRAME);
	hdwp = DeferWindowPos(hdwp, m_wndDisasmView.m_hWnd, NULL, w + 4 * m_splitWidth, m_splitHight, nNoSplitWidth - w, nNoSplitHight - h, SWP_NOACTIVATE | SWP_NOZORDER | SWP_DRAWFRAME);
	if (m_guiState != GuiState::ResizeStateH)
	hdwp = DeferWindowPos(hdwp, m_wndOutputBox.m_hWnd, NULL, m_splitWidth, nNoSplitHight - h + 4 * m_splitHight, cx - 2 * m_splitWidth, h, SWP_NOACTIVATE | SWP_NOZORDER | SWP_DRAWFRAME);
	EndDeferWindowPos(hdwp);
	*/
	CRect breakViewRect;
	m_wndCPUSideBar.GetClientRect(&breakViewRect);
	CRect outputBoxRect;
	m_wndOutputBox.GetClientRect(&outputBoxRect);
	m_resizeDate.hResizeRect.left = breakViewRect.Width();
	
	Invalidate();
}

void CDisasmPane::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO:  在此处添加消息处理程序代码
					   // 不为绘图消息调用 CDockablePane::OnPaint()
	CRect viewRect;
	GetClientRect(&viewRect);
	
	/*
	CRect breakWindowRect;
	CRect disasmWindowRect;
	CRect dutputWindowRect;

	m_wndCPUSideBar.GetWindowRect(&breakWindowRect);
	ScreenToClient(&breakWindowRect);
	m_wndDisasmView.GetWindowRect(&disasmWindowRect);
	ScreenToClient(&disasmWindowRect);
	m_wndOutputBox.GetWindowRect(&dutputWindowRect);
	ScreenToClient(&dutputWindowRect);
	*/

	dc.FillSolidRect(&viewRect, RGB(250, 250, 250));
	CPen pen;
	pen.CreatePen(PS_SOLID, m_splitWidth, RGB(128, 128, 128));
	CPen focusPen;
	focusPen.CreatePen(PS_SOLID, m_splitWidth, RGB(255, 0, 128));	

	CWnd* wnd[] =
	{
		CWnd::FromHandle(m_wndCPUSideBar.m_hWnd),
		CWnd::FromHandle(m_wndDisasmView.m_hWnd),
		CWnd::FromHandle(m_wndOutputBox.m_hWnd),
	};
	int size = sizeof(wnd) / sizeof(wnd[0]);
	CRect windowRect;
	for (int i = 0; i < size; i++)
	{

		wnd[i]->GetWindowRect(&windowRect);
		ScreenToClient(&windowRect);
		POINT points[] =
		{
			{ windowRect.left - 1, windowRect.top - 1 },
			{ windowRect.right, windowRect.top - 1 },
			{ windowRect.right, windowRect.bottom },
			{ windowRect.left - 1, windowRect.bottom },
			{ windowRect.left - 1, windowRect.top - 1 },
		};		
		dc.SelectObject(&pen);
		if (i == m_focusState)
			dc.SelectObject(&focusPen);
		dc.Polyline(points, 5);
	}

	if (m_ResizingX || m_ResizingY)
	{
		pen.DeleteObject();
		pen.CreatePen(PS_SOLID, 1, RGB(0, 250, 0));
		dc.SelectObject(&pen);

		if (m_ResizingX)
		{
			dc.MoveTo(m_splitWidth + iWidth + m_space, m_splitHight);
			dc.LineTo(m_splitWidth + iWidth + m_space, viewRect.Height() - iHight - 4 * m_splitHight);
		}

		else
		{
			dc.MoveTo(m_splitHight, viewRect.Height() - 3 * m_splitHight - iHight - m_space / 2);
			dc.LineTo(viewRect.Width() - 2 * m_splitWidth, viewRect.Height() - 3 * m_splitHight - iHight - m_space / 2);
		} 
	}	
}

void CDisasmPane::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	CRect viewRect;
	GetClientRect(viewRect);
	
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(tme);	//结构体缓冲区大小
	tme.dwFlags = TME_HOVER | TME_LEAVE;	//注册消息WM_MOUSEHOVER | WM_MOUSELEAVE
	tme.dwHoverTime = 1; //WM_MOUSEHOVER消息触发间隔时间
	tme.hwndTrack = m_hWnd; //当前窗口句柄

	::TrackMouseEvent(&tme); //注册发送消息

	CRect sideRect;
	m_wndCPUSideBar.GetClientRect(&sideRect);
	bool hHandle = ((point.x <= (3 * m_splitHight + m_space + sideRect.Width())) && (point.y >= m_splitHight) && (point.y < (m_splitHight + sideRect.Height())));
	bool vHandle = ((point.y >= m_splitHight + sideRect.Height()) && (point.y <= (3 * m_splitHight + m_space + sideRect.Height())));
	switch (m_guiState)
	{
	case NoState:	
		if (hHandle || vHandle)
		{
			if (m_mouseButtonState == MouseButtonState::MouseButtonNoState)
			{
				CURSORINFO ci;
				ci.cbSize = sizeof(CURSORINFO);
				GetCursorInfo(&ci);
				bool WECursor = ci.hCursor == LoadCursor(NULL, IDC_SIZEWE) ? true : false;
				bool NSCursor = ci.hCursor == LoadCursor(NULL, IDC_SIZENS) ? true : false;

				if (hHandle && (WECursor == false))
					m_guiState = GuiState::ReadyToResizeH;
				else if (vHandle && (NSCursor == false))
					m_guiState = GuiState::ReadyToResizeV;
			}
		}
		else
		{
			CDockablePane::OnMouseMove(nFlags, point);
		}
		return;

	case ResizeStateH:
		if ((point.x > m_defaultWidth) && (point.x < viewRect.Width() - 2 * m_defaultWidth))
		{
			if (abs(point.x - m_resizeDate.lastPosX) > 9) //间隔9减少重绘次数，减少闪烁
			{
				iWidth += point.x - m_resizeDate.lastPosX;
				m_resizeDate.lastPosX = point.x;

				int i = viewRect.Height(); //i高节节为高度，低字节为宽度
				i <<= sizeof(int) * 8 / 2;
				i += viewRect.Width();
				SendMessage(WM_SIZE, 0, i);
			}
		}	
		return;

	case ResizeStateV:
		if ((point.y > m_defaultHight) && (point.y < viewRect.Height() - m_defaultHight))
		{
			if (abs(point.y - m_resizeDate.lastPosY) > 0) // 不重绘窗体，只画光标调整线
			{
				iHight += m_resizeDate.lastPosY - point.y;
				m_resizeDate.lastPosY = point.y;

				drawAdjustLine(point);

				//SendMessage(WM_SIZE, 0, 0);
			}
		}
		return;
	}	
	
}

void CDisasmPane::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	m_mouseButtonState = MouseButtonState::LButtonDown;

	switch (m_guiState)
	{
	case CDisasmPane::ReadyToResizeH:
		m_ResizingX = true;
		Invalidate();
		m_guiState = GuiState::ResizeStateH;
		m_resizeDate.lastPosX = point.x;
		SetCapture();
		break;
	case CDisasmPane::ReadyToResizeV:
	{
		int cx = GetSystemMetrics(SM_CXSCREEN);
		int cy = GetSystemMetrics(SM_CYSCREEN);

		m_ResizingY = true;
		Invalidate();

		CRect viewRect;
		GetClientRect(&viewRect);

		m_intDC.CreateCompatibleDC(GetDCEx(NULL, DCX_CACHE));
		CBitmap intMap;
		intMap.CreateCompatibleBitmap(GetDCEx(NULL, DCX_CACHE), viewRect.Width(), viewRect.Height());
		m_intDC.SelectObject(&intMap);
		m_intDC.BitBlt(0, 0, viewRect.Width(), viewRect.Height(), GetDCEx(NULL, DCX_CACHE), 0, 0, SRCCOPY);

		m_guiState = GuiState::ResizeStateV;
		m_resizeDate.lastPosY = point.y;
		SetCapture();
	}
		break;
	
	default:
		CDockablePane::OnLButtonDown(nFlags, point);
		break;
	}
	
}

void CDisasmPane::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	CRect viewRect;
	GetClientRect(viewRect);

	switch (m_guiState)
	{
	case CDisasmPane::ResizeStateH:
	{
		if ((point.x > m_defaultWidth) && (point.x < viewRect.Width() - 2 * m_defaultWidth))
		{
			iWidth += point.x - m_resizeDate.lastPosX;
			m_resizeDate.lastPosX = point.x;			
		}
		else
		{
			iWidth = point.x < m_defaultWidth ? m_defaultWidth : viewRect.Width() - 2 * m_defaultWidth;
		}
		SendMessage(WM_SIZE, 0, 0);
	}
		break;
	case CDisasmPane::ResizeStateV:
	{
		if ((point.y > m_defaultHight) && (point.y < viewRect.Height() - m_defaultHight))
		{
			iHight += m_resizeDate.lastPosY - point.y;
			m_resizeDate.lastPosY = point.y;			
		}
		else
		{
			iHight = point.y < m_defaultHight ? viewRect.Height() - m_defaultHight : m_defaultHight;
		}
		SendMessage(WM_SIZE, 0, 0);
	}
		break;
	default:
		CDockablePane::OnLButtonUp(nFlags, point);
		break;
	}

	m_intDC.DeleteDC();
	m_mouseButtonState = MouseButtonState::MouseButtonNoState;
	m_guiState = GuiState::NoState;
	m_ResizingX = false;
	m_ResizingY = false;
	Invalidate();
	ReleaseCapture();	
}

void CDisasmPane::OnMouseHover(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	CDockablePane::OnMouseHover(nFlags, point);
}

void CDisasmPane::OnMouseLeave()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (m_mouseButtonState == MouseButtonState::MouseButtonNoState)
	{
		m_guiState = GuiState::NoState;		
	}

	CDockablePane::OnMouseLeave();
}

BOOL CDisasmPane::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (m_guiState == GuiState::ReadyToResizeH)
	{
		SetCursor(LoadCursor(NULL, IDC_SIZEWE));
		return TRUE;
	}
	if (m_guiState == GuiState::ReadyToResizeV)
	{
		SetCursor(LoadCursor(NULL, IDC_SIZENS));
		return TRUE;
	}

	return CDockablePane::OnSetCursor(pWnd, nHitTest, message);
}

CDisasmPane::FocusState CDisasmPane::getFocusState()
{
	return m_focusState;
}

void CDisasmPane::setFocusState(FocusState fs)
{
	m_focusState = fs;
	Invalidate();
}

void CDisasmPane::drawAdjustLine(CPoint point)
{	
	CRect viewRect;
	GetClientRect(&viewRect);
	CRect windowRect = viewRect;
	ClientToScreen(windowRect);
	POINT p = point;
	::ClientToScreen(this->m_hWnd, &p);
	
	CDC* pDC = GetDCEx(NULL, DCX_CACHE);
	pDC->BitBlt(0, 0, viewRect.Width(), viewRect.Height(), &m_intDC, 0, 0, SRCCOPY);

	CPen pen;
	pen.CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
	pDC->SelectObject(&pen);

	pDC->MoveTo(viewRect.left, m_resizeDate.lastPosY);
	pDC->LineTo(viewRect.right, m_resizeDate.lastPosY);
}

void CDisasmPane::MyAdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect viewRect;
	GetClientRect(&viewRect);

	int cx = viewRect.Width();
	int cy = viewRect.Height();

	m_bOnsize = true;
	int nNoSplitWidth = cx - 4 * m_splitWidth - m_space;
	int nNoSplitHight = cy - 4 * m_splitHight - m_space;

	m_bOnsize = false;
	/*
	m_wndCPUSideBar.SetWindowPos(NULL, m_splitWidth, m_splitHight, w, nNoSplitHight - h, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOREDRAW);
	m_wndDisasmView.SetWindowPos(NULL, w + 3 * m_splitWidth + m_space, m_splitHight, nNoSplitWidth - w, nNoSplitHight - h, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOREDRAW);
	if (m_guiState != GuiState::ResizeStateH)
		m_wndOutputBox.SetWindowPos(NULL, m_splitWidth, nNoSplitHight - h + 3 * m_splitHight + m_space, cx - 2 * m_splitWidth, h, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOREDRAW);
	*/
}

BOOL CDisasmPane::SaveBitmapToFile(HBITMAP   hBitmap, CString szfilename)
{
	HDC     hDC;
	//当前分辨率下每象素所占字节数          
	int     iBits;
	//位图中每象素所占字节数          
	WORD     wBitCount;
	//定义调色板大小，     位图中像素字节大小     ，位图文件大小     ，     写入文件字节数              
	DWORD     dwPaletteSize = 0, dwBmBitsSize = 0, dwDIBSize = 0, dwWritten = 0;
	//位图属性结构              
	BITMAP     Bitmap;
	//位图文件头结构          
	BITMAPFILEHEADER     bmfHdr;
	//位图信息头结构              
	BITMAPINFOHEADER     bi;
	//指向位图信息头结构                  
	LPBITMAPINFOHEADER     lpbi;
	//定义文件，分配内存句柄，调色板句柄              
	HANDLE     fh, hDib, hPal, hOldPal = NULL;

	//计算位图文件每个像素所占字节数              
	hDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	iBits = GetDeviceCaps(hDC, BITSPIXEL)     *     GetDeviceCaps(hDC, PLANES);
	DeleteDC(hDC);
	if (iBits <= 1)
		wBitCount = 1;
	else  if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else
		wBitCount = 24;

	GetObject(hBitmap, sizeof(Bitmap), (LPSTR)&Bitmap);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = Bitmap.bmWidth;
	bi.biHeight = Bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrImportant = 0;
	bi.biClrUsed = 0;

	dwBmBitsSize = ((Bitmap.bmWidth *wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;

	//为位图内容分配内存              
	hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;

	//     处理调色板                  
	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC = ::GetDC(NULL);
		hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}
	//     获取该调色板下新的像素值              
	GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight,
		(LPSTR)lpbi + sizeof(BITMAPINFOHEADER) + dwPaletteSize,
		(BITMAPINFO *)lpbi, DIB_RGB_COLORS);

	//恢复调色板                  
	if (hOldPal)
	{
		::SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}

	//创建位图文件                  
	fh = CreateFile(szfilename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (fh == INVALID_HANDLE_VALUE)         return     FALSE;
	//     设置位图文件头              
	bmfHdr.bfType = 0x4D42;     //     "BM"              
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;
	//     写入位图文件头              
	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	//     写入位图文件其余内容              
	WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
	//清除                  
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);
	return     TRUE;
}

void CDisasmPane::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CDisassembly* pWndDisasm = (CDisassembly*)&m_wndDisasmView;
	ASSERT_VALID(pWndDisasm);

	if (pWnd != pWndDisasm)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}
	/*
	if (point != CPoint(-1, -1))
	{
		// Select clicked item:
		CPoint ptTree = point;
		pWndDisasm->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndDisasm->HitTest(ptTree, &flags);
		if (hTreeItem != NULL)
		{
			pWndDisasm->SelectItem(hTreeItem);
		}
	}
	*/
	pWndDisasm->SetFocus();
	//如果子类实现了OnContextMenu，则由子类弹出
	//theApp.GetContextMenuManager()->ShowPopupMenu(IDR_DisasmPane_Popup, point.x, point.y, this, TRUE);
}

