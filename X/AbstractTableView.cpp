// AbstractTableView.cpp : 实现文件
//

#include "stdafx.h"
#include "X.h"
#include "AbstractTableView.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

// CAbstractTableView

IMPLEMENT_DYNAMIC(CAbstractTableView, CWnd)

CAbstractTableView::CAbstractTableView()
{
	m_Header.isVisible = false;
	m_Header.height = 20;
	mRowHight = 20;
	
	m_Header.activeButtonIndex = -1;
	m_Header.insertButtonIndex = 0;
	mNbrOfLineToPrint = 0;
	mDrawDebugOnly = false;
	mShouldReload = true;
	mRowCount = 0;
	mTableOffset = 0;
	mPrevTableOffset = mTableOffset + 1;
	m_GuiState = NoState;
	mLButtonDown = false;
	bSHIFT = false;
	defaultFont();
}

CAbstractTableView::~CAbstractTableView()
{
//	if (ConfigBool("Gui", "SaveColumnOrder"))
		saveColumnToConfig();
}

bool CAbstractTableView::setTextToClipboard(CString& text)
{
	if (OpenClipboard())
	{
		EmptyClipboard();
		int len = text.GetLength() + 1;
		HGLOBAL hglbCopy = GlobalAlloc(0, len * sizeof(TCHAR));
		if (hglbCopy == NULL)
		{
			CloseClipboard();
			return false;
		}
		LPTSTR lptstrCopy = (LPTSTR)GlobalLock(hglbCopy);
		memcpy(lptstrCopy, text.GetBuffer(),
			len * sizeof(TCHAR));
		lptstrCopy[len] = (TCHAR)0;    // null character 
		GlobalUnlock(hglbCopy);
		SetClipboardData(CF_UNICODETEXT, hglbCopy);
		CloseClipboard();
		GlobalFree(hglbCopy);
		return true;
	}
	return false;
}

BEGIN_MESSAGE_MAP(CAbstractTableView, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_SETCURSOR()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_NCMOUSEMOVE()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

/************************************************************************************
Configuration
************************************************************************************/

void CAbstractTableView::Initialize()
{
	//gdi+用到的两个变量   装载gdi+  
	GdiplusStartupInput m_gdiplusStartupInput;
	ULONG_PTR m_pGdiToken;
	GdiplusStartup(&m_pGdiToken, &m_gdiplusStartupInput, NULL);
	// Required to be called by each constructor because
	// of VTable changes
	//
	// Init all other updates once
	updateColors();
	updateFonts();
	//updateShortcuts();
}

void CAbstractTableView::defaultFont()
{
	memset(&lf, 0, sizeof(lf));
	lf.lfWeight = FW_NORMAL;
	lf.lfCharSet = GB2312_CHARSET;
	_tcscpy_s(lf.lfFaceName, LF_FACESIZE, _T("微软雅黑"));
	int i = GetDeviceCaps(::GetDC(this->GetSafeHwnd()), LOGPIXELSY);
	lf.lfHeight = MulDiv(13, 96, 72);

	/*
	font.CreateFont(12,   // nHeight
	0,                         // nWidth
	0,                         // nEscapement
	0,                         // nOrientation
	FW_NORMAL,                 // nWeight
	FALSE,                     // bItalic
	FALSE,                     // bUnderline
	0,                         // cStrikeOut
	DEFAULT_CHARSET,              // nCharSet
	OUT_DEFAULT_PRECIS,        // nOutPrecision
	CLIP_DEFAULT_PRECIS,       // nClipPrecision
	DEFAULT_QUALITY,           // nQuality
	DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
	_T("新宋体"));
	*/
	/*
	memset(&mFontMetrics, 0, sizeof(mFontMetrics));
	mFontMetrics.lfWeight = FW_NORMAL;
	mFontMetrics.lfCharSet = GB2312_CHARSET;
	_tcscpy_s(mFontMetrics.lfFaceName, LF_FACESIZE, _T("微软雅黑"));
	mFontMetrics.lfHeight = -18;
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));       // zero out structure
	lf.lfHeight = 12;                      // request a 12-pixel-height font
	_tcscpy_s(lf.lfFaceName, LF_FACESIZE, _T("微软雅黑"));//_tcscpy_s(lf.lfFaceName, _T("Arial"));        // request a face name "Arial"
	VERIFY(font.CreateFontIndirect(&lf));  // create the font
	*/
}

void CAbstractTableView::updateFonts()
{
	lf = ConfigFont("AbstractTableView");
}

void CAbstractTableView::updateColors()
{
	mBackgroundColor = ConfigColor("AbstractTableViewBackgroundColor");
	mTextColor = ConfigColor("AbstractTableViewTextColor");
	mSeparatorColor = ConfigColor("AbstractTableViewSeparatorColor");
	mHeaderTextColor = ConfigColor("AbstractTableViewHeaderTextColor");
	mSelectionColor = ConfigColor("AbstractTableViewSelectionColor");
}

void CAbstractTableView::initHScorll()
{
	int totalWidth = 0;
	for (int i = 0; i < getColumnCount(); i++)
		if (!getColumnHidden(i))
			totalWidth += getColumnWidth(i);
	CRect viewRect;
	GetClientRect(&viewRect);
	int width = viewRect.Width();

	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	GetScrollInfo(SB_HORZ, &si);
	si.fMask = SIF_RANGE | SIF_PAGE;
	si.nMin = 0;

	if (totalWidth > width)
		si.nMax = totalWidth;
	else
		si.nMax = 0;
	si.nPage = width;
	SetScrollInfo(SB_HORZ, &si);
}

void CAbstractTableView::initVScorll()
{
	if (GetSafeHwnd())
	{
		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_PAGE | SIF_RANGE;
		si.nMin = 0;
		si.nMax = getRowCount();
		si.nPage = getViewableRowsCount();
		SetScrollInfo(SB_VERT, &si);
	}
}

void CAbstractTableView::loadColumnFromConfig(const CString& viewName)
{
	int columnCount = getColumnCount();
	
	CString tmpStr;
	string s;
	for (int i = 0; i < columnCount; i++)
	{	
		tmpStr.Format(_T("%sColumnWidth%d"), viewName, i);
		s = CT2A(tmpStr);
		duint width = ConfigUint("Gui", s);
		tmpStr.Format(_T("%sColumnHidden%d"), viewName, i);
		s = CT2A(tmpStr);
		duint hidden = ConfigUint("Gui", s);
		tmpStr.Format(_T("%sColumnOrder%d"), viewName, i);
		s = CT2A(tmpStr);
		duint order = ConfigUint("Gui", s);
		if (width != 0)
			setColumnWidth(i, width);
		if (hidden != 2)
			setColumnHidden(i, !!hidden);
		if (order != 0)
			m_vColumnOrder[i] = order - 1;
	}
	mViewName = viewName;
	//connect(Bridge::getBridge(), SIGNAL(close()), this, SLOT(closeSlot()));
}

void CAbstractTableView::saveColumnToConfig()
{
	if (mViewName.GetLength() == 0)
		return;
	int columnCount = getColumnCount();
	auto cfg = Config();
	CString tmpStr;
	string s;
	for (int i = 0; i < columnCount; i++)
	{
		tmpStr.Format(_T("%sColumnWidth%d"), mViewName.GetBuffer(), i);
		s = CT2A(tmpStr);
		cfg->setUint("Gui", s, getColumnWidth(i));
		tmpStr.Format(_T("%sCColumnHidden%d"), mViewName.GetBuffer(), i);
		s = CT2A(tmpStr);
		cfg->setUint("Gui", s, getColumnWidth(i) ? 1 : 0);
		tmpStr.Format(_T("%sColumnOrder%d"), mViewName.GetBuffer(), i);
		s = CT2A(tmpStr);
		cfg->setUint("Gui", s, m_vColumnOrder[i] + 1);
	}
}

BOOL CAbstractTableView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  在此添加专用代码和/或调用基类

	return CWnd::PreCreateWindow(cs);
}

void CAbstractTableView::CreateColumn(CDC* painter, UINT index, int startX)
{
	//int wX = -horizontalScrollBar()->value();
	//int nWidht = GetSystemMetrics(SM_CXVSCROLL);
	//int nHeight = GetSystemMetrics(SM_CYVSCROLL);
	
	CRect clientRect;
	GetClientRect(&clientRect);
	CRect colunmRect;
	colunmRect.left = startX;
	colunmRect.top = clientRect.top + getTitleHeight();
	colunmRect.right = colunmRect.left + getColumnWidth(index);
	colunmRect.bottom = colunmRect.top + getColumnHeight();
	CRect txtRect = colunmRect;

	txtRect.DeflateRect(2, 2);

	CBrush tmpBrush;

	tmpBrush.CreateSolidBrush(RGB(250, 250, 200));
	
	painter->FillRect(&txtRect, &tmpBrush);
	/*
	CPen pen;
	UINT penWidth;
	penWidth = m_vColumnList[index].header.isPressed ? 2 : 1;

	pen.CreatePen(PS_SOLID, penWidth, RGB(0, 0, 150));
	painter->SelectObject(&pen);

	painter->MoveTo(getColumnWidth(index) + startX - 1, clientRect.top);
	painter->LineTo(getColumnWidth(index) + startX - 1, getHeaderHeight() - 1);
	painter->MoveTo(getColumnWidth(index) + startX - 1, getHeaderHeight() - 1);
	painter->LineTo(startX - 1, getHeaderHeight() - 1);
	*/
	COLORREF clrLT = m_vColumnList[index].header.isPressed ? ::GetSysColor(COLOR_3DDKSHADOW) : ::GetSysColor(COLOR_3DLIGHT);
	COLORREF clrRB = m_vColumnList[index].header.isPressed ? ::GetSysColor(COLOR_3DLIGHT) : ::GetSysColor(COLOR_3DDKSHADOW);
	painter->Draw3dRect(colunmRect, clrLT, clrRB);
	if (m_vColumnList[index].header.isPressed)
		txtRect.OffsetRect(1, 1);
	painter->DrawText(m_vColumnList[index].title, txtRect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	
}



void CAbstractTableView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO:  在此处添加消息处理程序代码
	// 不为绘图消息调用 CWnd::OnPaint()	
	CRect viewRect;
	GetClientRect(&viewRect);
	
	CDC MenDC;
	CBitmap MemMap;
	MenDC.CreateCompatibleDC(&dc);
	MemMap.CreateCompatibleBitmap(&dc, viewRect.Width(), viewRect.Height());
	MenDC.SelectObject(&MemMap);

	CFont font;
	VERIFY(font.CreateFontIndirect(&lf));
	MenDC.SelectObject(&font);
	
	TEXTMETRIC tm;
	MenDC.GetTextMetrics(&tm);
	mCharWidth = tm.tmAveCharWidth;

	MenDC.SetBkMode(TRANSPARENT);

	Graphics graphics(MenDC.m_hDC);
	graphics.SetSmoothingMode(SmoothingMode::SmoothingModeHighQuality);

	MenDC.FillSolidRect(&viewRect, mBackgroundColor.ToCOLORREF());

	// Reload data if needed
	if (mPrevTableOffset != mTableOffset || mShouldReload == true)
	{
		prepareData();
		mPrevTableOffset = mTableOffset;
		mShouldReload = false;
	}

	
	if (getColumnCount()) //make sure the last column is never smaller than the window
	{
		//int width = clientRect.Width() + GetSystemMetrics(SM_CXVSCROLL);
		int width = viewRect.Width();

		int totalWidth = 0;
		int lastWidth = totalWidth;
		int last = 0;
		for (int i = 0; i < getColumnCount(); i++)
		{
			if (getColumnHidden(m_vColumnOrder[i]))
				continue;
			last = m_vColumnOrder[i];
			lastWidth = getColumnWidth(last);
			totalWidth += lastWidth;
		}
		lastWidth = totalWidth - lastWidth;		
		lastWidth = width > lastWidth ? width - lastWidth : 0;
		if (totalWidth < width)
			setColumnWidth(last, lastWidth);		
		else
			setColumnWidth(last, getColumnWidth(last));
	}

	int scrollValue = GetScrollPos(SB_HORZ);
	int x = -scrollValue;
	int y = 0;

	// Paints title
	if (getHasTitle())
	{
		CRect titleRect = viewRect;
		titleRect.bottom = getTitleHeight();
		MenDC.FillSolidRect(&titleRect, RGB(200, 200, 200));
		MenDC.DrawEdge(titleRect, EDGE_ETCHED, BF_RECT);
		CRect textRect = titleRect;
		textRect.DeflateRect(2, 0);
		MenDC.DrawText(mTitleData.titleText, textRect, DT_LEFT | DT_VCENTER);
	}

	// Paints header	
	if (m_Header.isVisible == true)
	{
		int count = getColumnCount();
		for (int j = 0; j < count; j++)
		{			
			int i = m_vColumnOrder[j];
			if (getColumnHidden(i))
				continue;
			int width = getColumnWidth(i);
			CreateColumn(&MenDC, i, x);
			x += width;
		}
	}

	int totalWidth = 0;
	for (int i = 0; i < getColumnCount(); i++)
	{
		if (getColumnHidden(m_vColumnOrder[i]))
			continue;
		totalWidth += getColumnWidth(i);
	}

	x = - scrollValue + getSplitWidth();
	y = getHeaderHeight();

	// Iterate over all columns and cells
	CPen linePen;
	linePen.CreatePen(PS_SOLID, 1, mSeparatorColor.ToCOLORREF());

	//画断点分割线
	if (getHasSplit())
	{
		if (getSplitWidth() > scrollValue)
		{
			MenDC.SelectObject(&linePen);
			MenDC.MoveTo(getSplitWidth() - scrollValue - 1, y);
			MenDC.LineTo(getSplitWidth() - scrollValue - 1, viewRect.Height());
		}		
	}

	int wViewableRowsCount = getViewableRowsCount();

	for (int k = 0; k < getColumnCount(); k++)
	{
		int j = m_vColumnOrder[k];
		if (getColumnHidden(j))
			continue;	
		int w = getColumnWidth(j);
		if (j == 0)
			w -= getSplitWidth();
		for (int i = 0; i < wViewableRowsCount; i++)
		{
			//  Paints cell contents
			if (i < mNbrOfLineToPrint)
			{
				// Don't draw cells if the flag is set, and no process is running
				if (!mDrawDebugOnly || DbgIsDebugging())
				{
					CString wStr = paintContent(&MenDC, graphics, mTableOffset, i, j, x, y, w, getRowHeight());

					if (wStr.GetLength())
					{
						//wPainter.setPen(mTextColor);
						//wPainter.drawText(QRect(x + 4, y, getColumnWidth(j) - 4, getRowHeight()), Qt::AlignVCenter | Qt::AlignLeft, wStr);
						CRect textRect;
						textRect.left = x + 4;
						textRect.top = y;
						textRect.right = textRect.left + getColumnWidth(j) - 4;
						textRect.bottom = textRect.top + getRowHeight();
						MenDC.SetTextColor(mTextColor.ToCOLORREF());
						MenDC.DrawText(wStr, &textRect, DT_VCENTER | DT_LEFT);						
					}
				}
			}

			if (getColumnCount() > 1)
			{
				// Paints cell right borders
				//wPainter.setPen(mSeparatorColor);
				//wPainter.drawLine(x + getColumnWidth(j) - 1, y, x + getColumnWidth(j) - 1, y + getRowHeight() - 1);				
				MenDC.SelectObject(&linePen);
				MenDC.MoveTo(x + w - 1, y);
				MenDC.LineTo(x + w - 1, y + getRowHeight());
			}

			// Update y for the next iteration
			y += getRowHeight();
		}

		y = getHeaderHeight();
		x += getColumnWidth(j);
		if (j == 0)
			x -= getSplitWidth();
	}
	dc.BitBlt(0, 0, viewRect.Width(), viewRect.Height(), &MenDC, 0, 0, SRCCOPY);
	/*
	DefWindowProc(WM_PAINT, (WPARAM)MenDC.m_hDC, (LPARAM)0);
	dc.BitBlt(0, 0, ViewRect.Width(), ViewRect.Height(), &MenDC, 0, 0, SRCCOPY);
	MenDC.DeleteDC();
	MemMap.DeleteObject();
	*/
}

int CAbstractTableView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	CFont font;
	VERIFY(font.CreateFontIndirect(&lf));
	CDC* testDC = GetDC();
	testDC->SelectObject(&font);
	
	return 0;
}

// CAbstractTableView 消息处理程序
int CAbstractTableView::TButtonHitTest(CPoint point, CRect& rtButton)
{

	for (int j = 0; j < getColumnCount(); j++)
	{
		if (m_vHeadButton[j].rcRect.PtInRect(point))
		{
			return m_vHeadButton[j].index;
		}
	}
	return -1;
}

int CAbstractTableView::HitTestOverlap(CPoint point, CRect& rtButton)
{

	for (UINT j = 0; j < m_mColumnOverlap.size(); j++)
	{
		if (m_mColumnOverlap[j].OverlapRect.PtInRect(point))
		{
			return j;
		}
	}
	return -1;
}

// 鼠标按下按钮事件
void CAbstractTableView::OnLButtonDown(UINT nFlags, CPoint point)
{
	mLButtonDown = true;
	if (m_GuiState == ReadyToResizeH)
	{
		int wColIndex = getColumnDisplayIndexFromX(point.x);
		int wDisplayIndex = getColumnDisplayIndexFromX(point.x);
		int wStartPos = getColumnPosition(wDisplayIndex); // Position X of the start of column
		
		if (point.x <= (wStartPos + 2))
			mColResizeData.index = wColIndex - 1;
		else
			mColResizeData.index = wColIndex;

		mColResizeData.lastPosX = point.x;
		m_GuiState = ResizeColumnState;
		SetCapture();		
	}

	else if (m_Header.isVisible && getColumnCount() && (point.y <= getHeaderHeight()) && (point.y > getTitleHeight()))
	{
		mReorderStartX = point.x;

		int wColIndex = getColumnIndexFromX(point.x);
		if (m_vColumnList[wColIndex].header.isClickable)
		{
			//qDebug() << "Button " << wColIndex << "has been pressed.";
			//emit headerButtonPressed(wColIndex);

			m_vColumnList[wColIndex].header.isPressed = true;
			m_vColumnList[wColIndex].header.isMouseOver = true;

			m_Header.activeButtonIndex = wColIndex;
			Invalidate();
			//m_GuiState = HeaderButtonPressed;

			//updateViewport();
		}
	}
}

// 鼠标松开按钮事件
void CAbstractTableView::OnLButtonUp(UINT nFlags, CPoint point)
{
	mLButtonDown = false;

	m_GuiState = CAbstractTableView::NoState;

	// Release all buttons
	for (int i = 0; i < getColumnCount(); i++)
	{
		m_vColumnList[i].header.isPressed = false;
	}
	Invalidate();
	ReleaseCapture();

}

void CAbstractTableView::OnMouseHover(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	
	CWnd::OnMouseHover(nFlags, point);

}

// 鼠标进入按钮区域事件
/*
void CAbstractTableView::OnMouseMove(UINT nFlags, CPoint point)
{

if (point.y <= getTitleHeight() && m_GuiState != ResizeColumnState)
{
SendMessage(WM_MOUSELEAVE);
return;
}



TRACKMOUSEEVENT tme;
tme.cbSize = sizeof(tme);	//结构体缓冲区大小
tme.dwFlags = TME_HOVER | TME_LEAVE;	//注册消息WM_MOUSEHOVER | WM_MOUSELEAVE
tme.dwHoverTime = 1; //WM_MOUSEHOVER消息触发间隔时间
tme.hwndTrack = m_hWnd; //当前窗口句柄
::TrackMouseEvent(&tme); //注册发送消息

if (getColumnCount() <= 1)
return;
int wColIndex = getColumnIndexFromX(point.x);
int wDisplayIndex = getColumnDisplayIndexFromX(point.x);
int wStartPos = getColumnPosition(wDisplayIndex); // Position X of the start of column
int wEndPos = wStartPos + getColumnWidth(wColIndex); // Position X of the end of column
bool wHandle = ((wColIndex != 0) && (point.x >= wStartPos) && (point.x <= (wStartPos + 2))) || ((point.x <= wEndPos) && (point.x >= (wEndPos - 2)));
CRect viewRect;
GetClientRect(&viewRect);
if (wColIndex == getColumnCount() - 1 && point.x > viewRect.Width()) //last column
wHandle = false;

switch (m_GuiState)
{
case NoState:
{
if (!mLButtonDown)
{
CURSORINFO ci;
ci.cbSize = sizeof(CURSORINFO);
GetCursorInfo(&ci);

bool wHasCursor = ci.hCursor == LoadCursor(NULL, IDC_SIZEWE) ? true : false;

if ((wHandle == true) && (wHasCursor == false))
{
mColResizeData.splitHandle = true;
m_GuiState = ReadyToResize;
}
if ((wHandle == false) && (wHasCursor == true))
{
mColResizeData.splitHandle = false;
m_GuiState = NoState;
}
}
}
break;

case ReadyToResize:
{
if (!mLButtonDown)
{
if ((wHandle == false) && (m_GuiState == ReadyToResize))
{
mColResizeData.splitHandle = false;
m_GuiState = NoState;
}
}
}
break;

case ResizeColumnState:
{
int delta = point.x - mColResizeData.lastPosX;
bool bCanResize = (getColumnWidth(m_vColumnOrder[mColResizeData.index]) + delta) >= 20;
if (bCanResize)
{
int wNewSize = getColumnWidth(m_vColumnOrder[mColResizeData.index]) + delta;
setColumnWidth(m_vColumnOrder[mColResizeData.index], wNewSize);
mColResizeData.lastPosX = point.x;
CRect repaintRect;
GetClientRect(&repaintRect);
repaintRect.left = mColResizeData.lastPosX;
initHScorll();
Invalidate();
}
}
break;

default:
break;
}




CWnd::OnMouseMove(nFlags, point);
}
*/


void CAbstractTableView::OnMouseMove(UINT nFlags, CPoint point)
{
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(tme);	//结构体缓冲区大小
	tme.dwFlags = TME_HOVER | TME_LEAVE;	//注册消息WM_MOUSEHOVER | WM_MOUSELEAVE
	tme.dwHoverTime = 1; //WM_MOUSEHOVER消息触发间隔时间
	tme.hwndTrack = m_hWnd; //当前窗口句柄
	::TrackMouseEvent(&tme); //注册发送消息

	bool hHandle = false;
	if (point.y > getTitleHeight() + 1)
	{
		if (getColumnCount() > 1)
		{
			int wColIndex = getColumnIndexFromX(point.x);
			int wDisplayIndex = getColumnDisplayIndexFromX(point.x);
			int wStartPos = getColumnPosition(wDisplayIndex); // Position X of the start of column
			int wEndPos = wStartPos + getColumnWidth(wColIndex); // Position X of the end of column
			hHandle = ((wColIndex != 0) && (point.x >= wStartPos) && (point.x <= (wStartPos + 2))) || ((point.x <= wEndPos) && (point.x >= (wEndPos - 2)));
			CRect viewRect;
			GetClientRect(&viewRect);
			if (wColIndex == getColumnCount() - 1 && point.x > viewRect.Width()) //last column
				hHandle = false;
		}
	}
	switch (m_GuiState)
	{
	case NoState:
	{
		if (hHandle)
		{
			if (!mLButtonDown)
			{
				CURSORINFO ci;
				ci.cbSize = sizeof(CURSORINFO);
				GetCursorInfo(&ci);
				bool WECursor = ci.hCursor == LoadCursor(NULL, IDC_SIZEWE) ? true : false;

				if (hHandle && (WECursor == false))
				{
					m_GuiState = GuiState::ReadyToResizeH;
				}
			}
		}
	}
	break;
	case ReadyToResizeH:
	{
		if (!mLButtonDown)
		{
			if (hHandle == false)
			{
				m_GuiState = NoState;
			}
		}
	}
	break;
	case ResizeColumnState:
	{
		int delta = point.x - mColResizeData.lastPosX;
		bool bCanResize = (getColumnWidth(m_vColumnOrder[mColResizeData.index]) + delta) >= 20;
		if (bCanResize)
		{
			int wNewSize = getColumnWidth(m_vColumnOrder[mColResizeData.index]) + delta;
			setColumnWidth(m_vColumnOrder[mColResizeData.index], wNewSize);
			mColResizeData.lastPosX = point.x;
			CRect repaintRect;
			GetClientRect(&repaintRect);
			repaintRect.left = mColResizeData.lastPosX;
			initHScorll();
			Invalidate();
		}
	}
	break;

	case ResizeWindowState:
		if (abs(point.y - resizinglastPosY) > 0)
		{
			//:SendMessage(GetParent()->GetSafeHwnd(), WM_SIZE, (WPARAM)this, point.y);
		}
			
		break;

	default:
		return CWnd::OnMouseMove(nFlags, point);

	}
}

void CAbstractTableView::OnMouseLeave()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	m_GuiState = CAbstractTableView::NoState;

	CWnd::OnMouseLeave();
}


BOOL CAbstractTableView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (m_GuiState == ReadyToResizeH)
	{
		SetCursor(LoadCursor(NULL, IDC_SIZEWE));
		return TRUE;
	}

	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}


/************************************************************************************
New Columns/New Size
************************************************************************************/
/**
* @brief       This mehtod adds a new column to the table.
*
* @param[in]   width           Width of the column in pixel
* @param[in]   isClickable     Boolean that tells whether the header is clickable or not
* @return      Nothing.
*/
void CAbstractTableView::addColumnAt(int width, const CString & title, bool isClickable)
{
	HeaderButton wHeaderButton;
	Column wColumn;
	int wCurrentCount;
	// Fix invisible columns near the edge of the screen
	if (width < 20)
		width = 20;
	wHeaderButton.isPressed = false;
	wHeaderButton.isClickable = isClickable;
	wHeaderButton.isMouseOver = false;
	wColumn.header = wHeaderButton;
	wColumn.width = width;
	wColumn.hidden = false;
	wColumn.title = title;
	wCurrentCount = m_vColumnList.size();
	m_vColumnList.push_back(wColumn);
	m_vColumnOrder.push_back(wCurrentCount);
	/*
	UINT index = getColumnIndex();
	CRect tmpRect;
	CRect ClientRect;
	GetClientRect(&ClientRect);
	if (index)
	{
		tmpRect.left = m_vHeadButton[index - 1].rcRect.right;
		tmpRect.right = m_vHeadButton[index - 1].rcRect.right + width;
	}
	else
	{
		tmpRect.left = 0;
		tmpRect.right = width;
	}
	tmpRect.top = 0;
	tmpRect.bottom = getHeaderHeight();
	if (setColumnIndex())
	{
		ColumnAttribute tmpButton;
		tmpButton.index = index;
		tmpButton.title = title;
		tmpButton.rcRect = tmpRect;
		tmpButton.bDisabled = isClickable;
		tmpButton.bPressed = FALSE;
		tmpButton.bCheck = FALSE;
		tmpButton.bHovering = FALSE;
		m_vHeadButton.push_back(tmpButton);
	}
	ColumnOverlap tmpColumnOverlap;
	tmpColumnOverlap.bHovering = false;
	tmpColumnOverlap.bPressInOverlap = false;
	tmpRect.left = m_vHeadButton[index].rcRect.right - 3;
	tmpRect.top = m_vHeadButton[index].rcRect.top + 0;
	tmpRect.right = m_vHeadButton[index].rcRect.right + 3;
	tmpRect.bottom = m_vHeadButton[index].rcRect.bottom;
	tmpColumnOverlap.OverlapRect = tmpRect;
	m_mColumnOverlap.insert(make_pair(index, tmpColumnOverlap));
	*/
}

void CAbstractTableView::setRowCount(dsint count)
{
	//updateScrollBarRange(count);
	if (mRowCount != count)
		mShouldReload = true;
	mRowCount = count;

	initVScorll();
}

void CAbstractTableView::deleteAllColumns()
{
	m_vColumnList.clear();
	m_vColumnOrder.clear();
}

/************************************************************************************
Getter & Setter
************************************************************************************/


dsint CAbstractTableView::getRowCount() const
{
	return mRowCount;	
}

int CAbstractTableView::getColumnHeight() const
{
	if (m_Header.isVisible == true)
		//return m_Header.height;
		return lf.lfHeight + 4;
	else
		return 0;
}

void CAbstractTableView::setHasSplit(bool bHas)
{
	mSplitData.bSplit = bHas;
}

bool CAbstractTableView::getHasSplit() const
{
	return mSplitData.bSplit;
}

void  CAbstractTableView::setSplitWidth(UINT width)
{
	mSplitData.iwidth = width;
}
int  CAbstractTableView::getSplitWidth() const
{
	return mSplitData.iwidth;
}
//title
void  CAbstractTableView::setHasTitle(bool bHas)
{
	mTitleData.bTitle = bHas;
}

bool  CAbstractTableView::getHasTitle() const
{
	return mTitleData.bTitle;
}

void  CAbstractTableView::setTitleText(CString text)
{
	mTitleData.titleText = text;
}
void  CAbstractTableView::setTitleHeight(UINT hight)
{
	mTitleData.hight = hight;
}

int CAbstractTableView::getTitleHeight() const
{
	return mTitleData.hight;
}

int CAbstractTableView::getHeaderHeight() const
{
	return getTitleHeight() + getColumnHeight();
}

int CAbstractTableView::getRowHeight() const
{
	//return mRowHight;
	//return lf.lfHeight | 1;
	int x = lf.lfHeight;
	int y = lf.lfHeight | 1;
	return lf.lfHeight;
}

int CAbstractTableView::getTableHeight() const
{
	if (!GetSafeHwnd())
		return 0;
	CRect viewRect;
	GetClientRect(&viewRect);
	return viewRect.Height() - getHeaderHeight();
}

int CAbstractTableView::getColumnIndex() const
{
	return m_Header.insertButtonIndex;
}

bool CAbstractTableView::setColumnIndex()
{
	if (m_Header.insertButtonIndex < 6)
	{
		m_Header.insertButtonIndex++;
		return true;
	}
	else
	{
		return false;
	}
}

void CAbstractTableView::setColumnWidth(int index, int width)
{
	m_vColumnList[index].width = width;
	if (GetSafeHwnd())
		initVScorll();
}

void CAbstractTableView::setShowHeader(bool show)
{
	m_Header.isVisible = show;
}

bool CAbstractTableView::getShowHeader() const
{
	return m_Header.isVisible;
}

int CAbstractTableView::getColumnCount() const
{
	return m_vColumnList.size();
}

void CAbstractTableView::setColumnHidden(int col, bool hidden)
{
	if (col < getColumnCount() && col >= 0)
		m_vColumnList[col].hidden = hidden;
}

bool CAbstractTableView::getColumnHidden(int col) const
{
	if (col < 0)
		return true;
	else if (col < getColumnCount())
		return m_vColumnList[col].hidden;
	else
		return true;
}

int CAbstractTableView::getColumnWidth(int index) const
{
	if (index < 0)
		return -1;
	else if (index < getColumnCount())
		return m_vColumnList[index].width;
	return 0;
}

int CAbstractTableView::getGuiState() const
{
	return m_GuiState;
}

bool CAbstractTableView::getShiftState()
{
	return bSHIFT;
}

bool  CAbstractTableView::getRButtonInHeader()
{
	return bInHeader;
}

void CAbstractTableView::setupColumnConfigDefaultValue(map<string, duint> & map, const string & viewName, int columnCount)
{
	string str;
	char buffer[50];
	for (int i = 0; i < columnCount; i++)
	{		
		sprintf_s(buffer, 50, "%sColumnWidth%d", viewName.data(), i);
		str = buffer;
		map.insert(pair<string, duint>(str, 0));

		sprintf_s(buffer, 50, "%sColumnHidden%d", viewName.data(), i);
		str = buffer;
		map.insert(pair<string, duint>(str, 2));

		sprintf_s(buffer, 50, "%sColumnOrder%d", viewName.data(), i);
		str = buffer;
		map.insert(pair<string, duint>(str, 0));
	}
}

void CAbstractTableView::setNbrOfLineToPrint(int parNbrOfLineToPrint)
{
	mNbrOfLineToPrint = parNbrOfLineToPrint;
}

int CAbstractTableView::getCharWidth() const
{
	if (mCharWidth == 0)
	{
		TEXTMETRIC  tm;
		GetTextMetrics(::GetDC(GetSafeHwnd()), &tm);
		return tm.tmAveCharWidth;
	}
	else
		return mCharWidth;
}

/************************************************************************************
Content drawing control
************************************************************************************/

bool CAbstractTableView::getDrawDebugOnly() const
{
	return mDrawDebugOnly;
}

void CAbstractTableView::setDrawDebugOnly(bool value)
{
	mDrawDebugOnly = value;
}

void CAbstractTableView::setAllowPainting(bool allow)
{
	mAllowPainting = allow;
}

bool CAbstractTableView::getAllowPainting() const
{
	return mAllowPainting;
}

/************************************************************************************
Coordinates Utils
************************************************************************************/

/**
* @brief       Returns the index offset (relative to the table offset) corresponding to the given y coordinate.
*
* @param[in]   y      Pixel offset starting from the top of the table (without the header)
*
* @return      row index offset.
*/
int CAbstractTableView::getIndexOffsetFromY(int y) const
{
	return (y / getRowHeight());
}

/**
* @brief       Substracts the header height from the given y.
*
* @param[in]   y      y coordinate
*
* @return      y - getHeaderHeight().
*/
int CAbstractTableView::transY(int y) const
{
	return y - getHeaderHeight();
}

/**
* @brief       Returns the index of the column corresponding to the given x coordinate.
*
* @param[in]   x      Pixel offset starting from the left of the table
*
* @return      Column index.
*/
int CAbstractTableView::getColumnIndexFromX(int x) const
{
	//int wX = -horizontalScrollBar()->value();
	int wX = - GetScrollPos(SB_HORZ);
	int wColIndex = 0;

	while (wColIndex < getColumnCount())
	{
		int col = m_vColumnOrder[wColIndex];
		if (getColumnHidden(col))
		{
			wColIndex++;
			continue;
		}
		wX += getColumnWidth(col);

		if (x <= wX)
		{
			return m_vColumnOrder[wColIndex];
		}
		else if (wColIndex < getColumnCount())
		{
			wColIndex++;
		}
	}
	return getColumnCount() > 0 ? m_vColumnOrder[getColumnCount() - 1] : -1;
}

/**
* @brief       Returns the displayed index of the column corresponding to the given x coordinate.
*
* @param[in]   x      Pixel offset starting from the left of the table
*
* @return      Displayed index.
*/
int CAbstractTableView::getColumnDisplayIndexFromX(int x)
{
	//int wX = -horizontalScrollBar()->value();
	int wX = -GetScrollPos(SB_HORZ);
	int wColIndex = 0;

	while (wColIndex < getColumnCount())
	{
		int col = m_vColumnOrder[wColIndex];
		if (getColumnHidden(col))
		{
			wColIndex++;
			continue;
		}
		wX += getColumnWidth(col);

		if (x <= wX)
		{
			return wColIndex;
		}
		else if (wColIndex < getColumnCount())
		{
			wColIndex++;
		}
	}
	return getColumnCount() - 1;
}

/**
* @brief       Returns the x coordinate of the beginning of the column at index index.
*
* @param[in]   index      Column index.
*
* @return      X coordinate of the column index.
*/
int CAbstractTableView::getColumnPosition(int index) const
{
	//int posX = -horizontalScrollBar()->value();
	int posX = -GetScrollPos(SB_HORZ);

	if ((index >= 0) && (index < getColumnCount()))
	{
		for (int i = 0; i < index; i++)
			if (!getColumnHidden(m_vColumnOrder[i]))
				posX += getColumnWidth(m_vColumnOrder[i]);
		return posX;
	}
	else
	{
		return -1;
	}
}

/**
* @brief       Returns the number of viewable rows in the current window (Partially viewable rows are aslo counted).
*
* @return      Number of viewable rows.
*/
int CAbstractTableView::getViewableRowsCount() const
{
	//int wTableHeight = this->viewport()->height() - getHeaderHeight();
	CRect tmpRect;
	GetClientRect(&tmpRect);
	int wTableHeight = tmpRect.Height() - getHeaderHeight();
	int wCount = wTableHeight / getRowHeight();
	wCount += (wTableHeight % getRowHeight()) > 0 ? 1 : 0;
	return wCount;
}

/************************************************************************************
Table offset management
************************************************************************************/
dsint CAbstractTableView::getTableOffset() const
{
	return mTableOffset;
}

void CAbstractTableView::setTableOffset(dsint val)
{
	dsint wMaxOffset = getRowCount() - getViewableRowsCount() + 1;
	wMaxOffset = wMaxOffset > 0 ? getRowCount() : 0;
	if (val > wMaxOffset)
		return;

	// If val is within the last ViewableRows, then set RVA to rowCount - ViewableRows + 1
	if (wMaxOffset && val >= (getRowCount() - getViewableRowsCount() + 1))
		mTableOffset = (getRowCount() - getViewableRowsCount()) + 1;
	else
		mTableOffset = val;
	SCROLLINFO si;
	si.nPos = val;
	si.fMask = SIF_POS;
	SetScrollInfo(SB_VERT, &si);
	/*
	emit tableOffsetChanged(val);

#ifdef _WIN64
	int wNewValue = scaleFromUint64ToScrollBarRange(mTableOffset);
	verticalScrollBar()->setValue(wNewValue);
	verticalScrollBar()->setSliderPosition(wNewValue);
#else
	verticalScrollBar()->setValue(val);
	verticalScrollBar()->setSliderPosition(val);
#endif
	*/
}


/************************************************************************************
Update/Reload/Refresh/Repaint
************************************************************************************/

/**
* @brief       This method is called when data have to be reloaded (e.g. When table offset changes).
*
* @return      Nothing.
*/
void CAbstractTableView::prepareData()
{
	int wViewableRowsCount = getViewableRowsCount();
	dsint wRemainingRowsCount = getRowCount() - mTableOffset;
	mNbrOfLineToPrint = (dsint)wRemainingRowsCount > (dsint)wViewableRowsCount ? (int)wViewableRowsCount : (int)wRemainingRowsCount;
}

void CAbstractTableView::reloadData()
{
	mShouldReload = true;
	//emit tableOffsetChanged(mTableOffset);
	//this->viewport()->update();
	if (this->GetSafeHwnd())
		Invalidate(TRUE);
}

void CAbstractTableView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO:  在此处添加消息处理程序代码
	mShouldReload = true;
	initVScorll();
	initHScorll();
}


/************************************************************************************
ScrollBar Management
***********************************************************************************/
void CAbstractTableView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	
	dsint nDelta = 0;
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	GetScrollInfo(SB_VERT, &si);
	switch (nSBCode) 
	{
	case SB_TOP:
		nDelta = si.nMin - si.nPos;
		break;
	case SB_BOTTOM:
		nDelta = si.nMax - si.nPos;
		break;

	case SB_LINEUP:
		nDelta = -1;
		break;

	case SB_LINEDOWN:
		nDelta = 1;
		break;

	case SB_PAGEUP:
		nDelta -= si.nPage;
		break;	

	case SB_PAGEDOWN:
		nDelta = si.nPage;
		break;

	case SB_THUMBTRACK:
		//si.nPos = si.nTrackPos;
		nDelta = si.nTrackPos - si.nPos;
		break;

	default:
		return;
	}	
		
	if (nDelta != 0) 
	{
		// TableBounding
		/*
		dsint wValue = mTableOffset + nDelta;
		dsint wMax = getRowCount() - getViewableRowsCount() + 1;		
		wValue = wValue > wMax ? wMax : wValue;
		wValue = wValue < 0 ? 0 : wValue;
		*/
		// Call the hook (Usefull for disassembly)
		mTableOffset = sliderMovedHook(nSBCode, mTableOffset, nDelta);
		//mTableOffset = wValue;
		// SliderBounding
		int wSliderPos = si.nPos + nDelta;
		wSliderPos = wSliderPos > GetScrollLimit(SB_VERT) ? GetScrollLimit(SB_VERT) : wSliderPos;
		wSliderPos = wSliderPos < 0 ? 0 : wSliderPos;

		int nLastPos = si.nPos;
		si.nPos = wSliderPos;
		
		si.fMask = SIF_POS;
		SetScrollInfo(SB_VERT, &si);
		if (nLastPos != GetScrollPos(SB_VERT))
			Invalidate();
		//SetScrollPos(SB_VERT, si.nPos, TRUE);		
		//ScrollWindow(0, -nDelta);
	}
	
	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CAbstractTableView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	int nDelta;

	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	GetScrollInfo(SB_HORZ, &si);

	switch (nSBCode)
	{
	case SB_LINELEFT:
		nDelta = -20;
		break;

	case SB_LINERIGHT:
		nDelta = 20;
		break;

	case SB_THUMBTRACK:
		nDelta = si.nTrackPos - si.nPos;
		break;	

	default:
		return;
	}

	

	if (nDelta != 0)
	{
		/*
		m_nHScrollPos += nDelta;
		SetScrollPos(SB_HORZ, m_nHScrollPos, TRUE);
		Invalidate();
		*/
		int nScrollPos = si.nPos + nDelta;
		nScrollPos = nScrollPos > GetScrollLimit(SB_HORZ) ? GetScrollLimit(SB_HORZ) : nScrollPos;
		nScrollPos = nScrollPos < 0 ? 0 : nScrollPos;

		int nLastPos = si.nPos;
		si.nPos = nScrollPos;

		si.fMask = SIF_POS;
		SetScrollInfo(SB_HORZ, &si);
		if (nLastPos != GetScrollPos(SB_HORZ))
			Invalidate();
	}
	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

/**
* @brief       This virtual method is called at the end of the vertSliderActionSlot(...) method.
*              It allows changing the table offset according to the action type, the old table offset
*              and delta between the old and the new table offset.
*
* @param[in]   type      Type of action (Refer to the QAbstractSlider::SliderAction enum)
* @param[in]   value     Old table offset
* @param[in]   delta     Scrollbar value delta compared to the previous state
*
* @return      Return the value of the new table offset.
*/
dsint CAbstractTableView::sliderMovedHook(int type, dsint value, dsint delta)
{
	//Q_UNUSED(type);
	dsint wValue = value + delta;
	dsint wMax = getRowCount() - getViewableRowsCount() + 1;

	// Bounding
	wValue = wValue > wMax ? wMax : wValue;
	wValue = wValue < 0 ? 0 : wValue;

	return wValue;
}

BOOL CAbstractTableView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	/*
	int numDegrees = zDelta / 8;
	int numSteps = numDegrees / 15;

	if (numSteps > 0)
	{
		for (int i = 0; i < numSteps; i++)
			SendMessage(WM_VSCROLL, SB_LINEUP, 0);
	}

	else
	{
		for (int i = 0; i <  numSteps * -1; i++)
			SendMessage(WM_VSCROLL, SB_LINEDOWN, 0);
	}
	*/
	
	switch (zDelta)
	{	
	case 120:
		SendMessage(WM_VSCROLL, SB_LINEUP, 0);
		break;
	case -120:
		SendMessage(WM_VSCROLL, SB_LINEDOWN, 0);
		break;
	default:
		break;
	}
	

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}


void CAbstractTableView::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CWnd::OnNcMouseMove(nHitTest, point);
}





void CAbstractTableView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (point.y > getTitleHeight() && point.y < getHeaderHeight())
		bInHeader = true;
	else
		bInHeader = false;
	CWnd::OnRButtonDown(nFlags, point);
}
