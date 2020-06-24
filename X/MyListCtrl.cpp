// MyListCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "X.h"
#include "MyListCtrl.h"


#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
//#pragma comment(lib,"gdiplus.lib")


// CMyListCtrl

IMPLEMENT_DYNAMIC(CMyListCtrl, CListCtrl)

CMyListCtrl::CMyListCtrl()
{
	m_bDrawCirclePoint = false;
	m_bSelected = false;
	m_BreakPointItem[0] = 1;
	
	for (int i = 1; i < 21; i++)
	{
		m_BreakPointItem[i] = -1;
	}	
}

CMyListCtrl::~CMyListCtrl()
{
}

BEGIN_MESSAGE_MAP(CMyListCtrl, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CMyListCtrl::OnNMCustomdraw)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, &CMyListCtrl::OnLvnItemchanged)
	ON_NOTIFY_REFLECT(LVN_ENDSCROLL, &CMyListCtrl::OnLvnEndScroll)
	ON_NOTIFY(HDN_ITEMCHANGEDA, 0, &CMyListCtrl::OnHdnItemchanged)
	ON_NOTIFY(HDN_ITEMCHANGEDW, 0, &CMyListCtrl::OnHdnItemchanged)
	ON_WM_PAINT()
	ON_NOTIFY_REFLECT(NM_CLICK, &CMyListCtrl::OnNMClick)
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

void CMyListCtrl::Init()
{	
	//gdi+用到的两个变量   装载gdi+  
	GdiplusStartupInput m_gdiplusStartupInput;
	ULONG_PTR m_pGdiToken;
	GdiplusStartup(&m_pGdiToken, &m_gdiplusStartupInput, NULL);
	m_Width = 20;
	LIST_ITEM_HEIGHT = 21;
	m_BkColor = RGB(200, 200, 200);
	m_SelectColor = RGB(128, 255, 255);
	m_CutLineColor = RGB(0, 255, 255);
	LOGFONT logfont;
	memset(&logfont, 0, sizeof(logfont));
	logfont.lfWeight = FW_NORMAL;
	logfont.lfCharSet = GB2312_CHARSET;
	_tcscpy_s(logfont.lfFaceName, LF_FACESIZE, _T("宋体"));
	logfont.lfHeight = (LIST_ITEM_HEIGHT-1);
	CFont font;
	font.CreateFontIndirect(&logfont);
	SetFont(&font);
	font.Detach();	
}
// CMyListCtrl 消息处理程序
void CMyListCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	LPNMLVCUSTOMDRAW lpnmcd = (LPNMLVCUSTOMDRAW) pNMHDR;
	if (lpnmcd ->nmcd.dwDrawStage == CDDS_PREPAINT)
	{
		*pResult =  CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTPAINT;
		return;
	}
	
	else if (lpnmcd ->nmcd.dwDrawStage == (CDDS_SUBITEM | CDDS_ITEMPREPAINT))
	{
		int iItem = lpnmcd->nmcd.dwItemSpec;
		int iSubItem = lpnmcd->iSubItem;
		if (iItem >= 0 && iSubItem >= 0)
		{
			CRect rSubItem;
			CRect BoundRect;
			HDC hDC = lpnmcd->nmcd.hdc;
			GetSubItemRect(iItem, iSubItem, LVIR_LABEL,rSubItem);
			GetSubItemRect(iItem, iSubItem, LVIR_BOUNDS, BoundRect);
			if (iSubItem == 0)
			{
				rSubItem.left = BoundRect.left + m_Width;
			}
			m_bSelected = false;
			if (GetItemState(iItem, LVIS_SELECTED))
			{
				m_bSelected = true;
			}

			bool bFocus = false;
			CWnd *pWndFocus = GetFocus();
			if (IsChild(pWndFocus) || pWndFocus == this)
			{
				bFocus = true;
			}
			rSubItem.NormalizeRect();
			CDC dc;
			HDC dc1 = lpnmcd->nmcd.hdc;
			dc.Attach(lpnmcd->nmcd.hdc);
			DrawSubItem(&dc,iItem,iSubItem,rSubItem,m_bSelected,bFocus);
			dc.Detach();
			*pResult =  CDRF_SKIPDEFAULT;
			return;
		}
	}

	else if (lpnmcd->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
	{
		CRect rSubItem, rectClient;
		int nColumnCount = GetHeaderCtrl()->GetItemCount();
		if (nColumnCount > 0)
		{
			GetSubItemRect(lpnmcd->nmcd.dwItemSpec, nColumnCount - 1, LVIR_LABEL, rSubItem);
			GetClientRect(&rectClient);
			rSubItem.left = rSubItem.right;
			rSubItem.right = rectClient.right;
			rSubItem.NormalizeRect();
			m_bSelected = false;
			if (GetItemState(lpnmcd->nmcd.dwItemSpec, LVIS_SELECTED))
			{
				m_bSelected = true;
			}

			bool bFocus = false;
			HWND hWndFocus = ::GetFocus();
			if (::IsChild(m_hWnd, hWndFocus) || m_hWnd == hWndFocus)
			{
				bFocus = true;
			}
			CDC dc;
			HDC dc2 = lpnmcd->nmcd.hdc;
			dc.Attach(lpnmcd->nmcd.hdc);
			draw_row_bg(&dc, rSubItem, m_bSelected, bFocus, (int)lpnmcd->nmcd.dwItemSpec);
			dc.Detach();
		}		
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
		return;
	}

	else if (lpnmcd ->nmcd.dwDrawStage == CDDS_POSTPAINT)
	{
		DrawRemainSpace(lpnmcd);
		*pResult =  CDRF_SKIPDEFAULT;
		return;
	}
	*pResult = 0;
}

void CMyListCtrl::draw_row_bg(CDC *pDC, RECT rc, bool bSelected, bool bFocus,int nRow)
{ 

	bool bOdd = (nRow % 2 == 0 ? true : false);
	//bool bOdd = true;
	CRect rect = rc;
	if (rect.Width() == 0)
	{
		return;
	}
	draw_row_bg(pDC, rc, bSelected,bFocus,bOdd);
}

void CMyListCtrl::draw_row_bg(CDC *pDC, RECT rc, bool bSelected, bool bFocus, bool bOdd)
{
	CRect rect = rc;
	CRect r;
	CRect wr;
	GetClientRect(&r);	
	GetHeaderCtrl()->GetWindowRect(&wr);
	if (rect.Width() == 0)
	{
		return;
	}
	bFocus = true;
	int nSave = pDC->SaveDC();
	
	if (bSelected)
	{
		if (bFocus)
		{
			CBrush selectBrush;
			selectBrush.CreateSolidBrush(m_SelectColor);
			pDC->FillRect(&rc, &selectBrush);
		}
		else
		{
			CBrush selectNoFocusBrush;
			selectNoFocusBrush.CreateSolidBrush(m_BkColor);
			pDC->FillRect(&rc, &selectNoFocusBrush);
		}
	}	
	else if (bOdd)
	{
		CBrush oddBrush;
		oddBrush.CreateSolidBrush(RGB(200, 200, 200));
		pDC->FillRect(&rc, &oddBrush);
	}
	else
	{
		CBrush normalBrush;
		normalBrush.CreateSolidBrush(RGB(200, 200, 200));
		pDC->FillRect(&rc, &normalBrush);
	}	

	// 画竖线
	CPen pen;
	pen.CreatePen(PS_SOLID, 1, m_CutLineColor);

	pDC->SelectObject(&pen);
	pDC->MoveTo(rc.right - 1, rc.top);
	pDC->LineTo(rc.right - 1, rc.bottom);

	r.right = m_Width;
	r.top = long(wr.Height());
	bool s = false;
	
		s = true;
		CBrush normalBrush;
	//	normalBrush.CreateSolidBrush(RGB(255, 255, 255));
	//	pDC->FillRect(&r, &normalBrush);
	
	// 画选中的底部分割线
	if (bSelected)
	{
		CPen bottomPen;
		bottomPen.CreatePen(PS_SOLID, 1, RGB(100, 100, 100));

		pDC->SelectObject(&bottomPen);
		pDC->MoveTo(rc.left, rc.bottom - 1);
		pDC->LineTo(rc.right, rc.bottom - 1);
	}	
	pDC->RestoreDC(nSave);
}
// 画剩余部分
void CMyListCtrl::DrawRemainSpace(LPNMLVCUSTOMDRAW lpnmcd)
{

	int nTop = lpnmcd->nmcd.rc.top;
	int nRight = lpnmcd->nmcd.rc.left;
	int nCount = GetItemCount();
	if (nCount > 0)
	{
		CRect rcItem;
		GetItemRect(nCount - 1, &rcItem, LVIR_LABEL);
		nTop = rcItem.bottom;
		nRight = rcItem.right;
	}
	CRect rectClient;
	GetClientRect(&rectClient);
	if (nTop < lpnmcd->nmcd.rc.bottom) // 有剩余
	{
		
		CRect rcRemain = lpnmcd->nmcd.rc;
		rcRemain.top = nTop;
		rcRemain.right = rectClient.right;
		int nRemainItem = rcRemain.Height() / LIST_ITEM_HEIGHT;
		if (rcRemain.Height() % LIST_ITEM_HEIGHT)
		{
			nRemainItem++;
		}

		int pos = GetScrollPos(SB_HORZ);
		CDC dc;
		dc.Attach(lpnmcd->nmcd.hdc);
		int nColumnCount = GetHeaderCtrl()->GetItemCount();
		CRect  rcSubItem;
		CRect rcItem;
		for (int i = 0; i < nRemainItem; ++i)
		{

			rcItem.top = rcRemain.top + i * LIST_ITEM_HEIGHT;
			rcItem.left = rcRemain.left;
			rcItem.right = rcRemain.right;
			rcItem.bottom = rcItem.top + LIST_ITEM_HEIGHT;
			for (int j = 0; j < nColumnCount; ++j)
			{
				GetHeaderCtrl()->GetItemRect(j, &rcSubItem);
				rcSubItem.top = rcItem.top;
				rcSubItem.bottom = rcItem.bottom;
				rcSubItem.OffsetRect(-pos, 0);
				if(rcSubItem.right < rcRemain.left || rcSubItem.left > rcRemain.right)
					continue;
				draw_row_bg(&dc, rcSubItem, false, false, i + nCount);			
			}
			if (rcSubItem.right<rectClient.right)
			{
				rcSubItem.left=rcSubItem.right;
				rcSubItem.right=rectClient.right;
				draw_row_bg(&dc, rcSubItem, false, false, i+nCount);	
			}
		}
		dc.Detach();
	}
}

void CMyListCtrl::PreSubclassWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	Init();
	CListCtrl::PreSubclassWindow();
}

void CMyListCtrl::DrawSubItem(CDC *pDC, int nItem, int nSubItem, CRect &rSubItem, bool bSelected, bool bFocus)
{


	//pDC->SetBkMode(TRANSPARENT);
//	COLORREF crBkColor = ::GetSysColor(COLOR_3DFACE);
//	pDC->FillRect(rSubItem, &CBrush(RGB(100, 100, 100)));
//	pDC->FillRect(rSubItem, &CBrush(m_BkColor));
//	ASSERT(GetTextBkColor() == crBkColor);
//	pDC->SetBkColor(m_BkColor);
	pDC->SetBkMode(TRANSPARENT);
	
	CFont font;
	font.CreateFont(12,   // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("宋体"));
	pDC->SelectObject(&font);	
	CString strText;
	strText = GetItemText(nItem, nSubItem);	
	draw_row_bg(pDC, rSubItem, bSelected, bFocus, nItem);
	pDC->DrawText(strText, strText.GetLength(), &rSubItem, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
}

void CMyListCtrl::DrawBPCircle(int nitem)
{
	
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CListCtrl::OnPaint()

	RECT rcItem;
	for (int i = 1; i < 21; i++)
	{ 
		if (GetItemRect(m_BreakPointItem[i], &rcItem, LVIR_BOUNDS) && m_BreakPointItem[i] == nitem)
		{
			rcItem.right = m_Width;
			int dim =int( ((rcItem.right - rcItem.left) < (rcItem.bottom - rcItem.top) ? (rcItem.right - rcItem.left) : (rcItem.bottom - rcItem.top)) * 0.55);
			int x = rcItem.left + ((rcItem.right - rcItem.left) - dim) / 2;
			int y = rcItem.top + ((rcItem.bottom - rcItem.top) - dim) / 2;
			CRgn rgn;
			rgn.CreateEllipticRgn(rcItem.left + 5, rcItem.top + 5, rcItem.right - 5, rcItem.bottom - 5);

			//pDC->SetMapMode(MM_ANISOTROPIC);
			//CSize s = pDC->SetWindowExt(2000, 2000);
			//CSize ss = pDC->SetViewportExt(rcItem.right - rcItem.left, rcItem.bottom - rcItem.top);
			//pDC->SetViewportOrg(rcItem.right / 2, rcItem.bottom / 2);

			CBrush normalBrush;
			normalBrush.CreateSolidBrush(RGB(0, 0, 0));
			//HBRUSH oldBrush = (HBRUSH)pDC->SelectObject(normalBrush);
			//pDC->Ellipse(&r);
			//pDC->SelectObject(oldBrush);
			
			HDC hdc = GetDC()->m_hDC;
			//HDC hdc1 = pDC->m_hDC;
			Graphics graphics(GetDC()->m_hDC);
			Pen pen(Color(255, 255, 0x00, 0x00), 0.5);
			//graphics.SetSmoothingMode(SmoothingMode::SmoothingModeHighQuality);
			graphics.DrawEllipse(&pen, x, y, dim, dim);
			graphics.FillEllipse(&SolidBrush(Color(255, 255, 0, 0)), x, y, dim, dim);
		}
	}	
}

BOOL CMyListCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return FALSE;
	return CListCtrl::OnEraseBkgnd(pDC);
}


void CMyListCtrl::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	Default();
	Invalidate();
	*pResult = 0;
}


void CMyListCtrl::OnLvnEndScroll(NMHDR *pNMHDR, LRESULT *pResult)
{
	// 此功能要求 Internet Explorer 5.5 或更高版本。
	// 符号 _WIN32_IE 必须是 >= 0x0560。
	LPNMLVSCROLL pStateChanged = reinterpret_cast<LPNMLVSCROLL>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	//Invalidate();
	RECT rcClientRect;
	GetClientRect(&rcClientRect);
	RedrawWindow(&rcClientRect,NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
	*pResult = 0;
}


void CMyListCtrl::OnHdnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	Default();
	Invalidate();
	*pResult = 0;
}

void CMyListCtrl::DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/)
{

	// TODO:  添加您的代码以绘制指定项
}

void CMyListCtrl::OnSize(UINT nType, int cx, int cy)
{
	CListCtrl::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	//	CRect r;
	//	GetClientRect(&r);
	//	SetColumnWidth(4, cy - r.Width());
	//SetColumnWidth(3, LVSCW_AUTOSIZE);
}

void CMyListCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CListCtrl::OnPaint()
	CRect ListRect;
	CRect HeaderRect;
	GetClientRect(&ListRect);
	GetDlgItem(0)->GetWindowRect(&HeaderRect);
	CDC MenDC;
	CBitmap MemMap;	
	MenDC.CreateCompatibleDC(&dc);
	MemMap.CreateCompatibleBitmap(&dc, ListRect.Width(), ListRect.Height());
	MenDC.SelectObject(&MemMap);

	CRect ViewRect;
	GetViewRect(&ViewRect);
	ViewRect.right += m_Width;
	CRect TmpRect{ 0,0,0,0};
	UINT TopIndex = GetTopIndex();
	if (GetItemRect(TopIndex, &TmpRect, LVIR_BOUNDS))
	{
		TmpRect.right = TmpRect.left + m_Width;
		TmpRect.bottom = ListRect.bottom;
		MenDC.FillSolidRect(&TmpRect, RGB(250, 250, 250));
	}
		
	HDC hdc = GetDC()->m_hDC;
	//HDC hdc1 = pDC->m_hDC;
	//Graphics graphics(GetDC()->m_hDC);
	Graphics graphics(MenDC.m_hDC);	
	graphics.SetSmoothingMode(SmoothingMode::SmoothingModeHighQuality);
	Pen pen(Color(255, 255, 0x00, 0x00), 0.5);
	RECT rcItem;
	for (int i = 1; i < 21; i++)
	{
		if (GetItemRect(m_BreakPointItem[i], &rcItem, LVIR_BOUNDS))
		{
			rcItem.right = rcItem.left + m_Width;
			int dim =int( ((rcItem.right - rcItem.left) < (rcItem.bottom - rcItem.top) ? (rcItem.right - rcItem.left) : (rcItem.bottom - rcItem.top)) * 0.55);
			int x = rcItem.left + ((rcItem.right - rcItem.left) - dim) / 2;
			int y = rcItem.top + ((rcItem.bottom - rcItem.top) - dim) / 2;
			CRgn rgn;
			rgn.CreateEllipticRgn(rcItem.left + 5, rcItem.top + 5, rcItem.right - 5, rcItem.bottom - 5);

			//pDC->SetMapMode(MM_ANISOTROPIC);
			//CSize s = pDC->SetWindowExt(2000, 2000);
			//CSize ss = pDC->SetViewportExt(rcItem.right - rcItem.left, rcItem.bottom - rcItem.top);
			//pDC->SetViewportOrg(rcItem.right / 2, rcItem.bottom / 2);

			graphics.DrawEllipse(&pen, x, y, dim, dim);
			graphics.FillEllipse(&SolidBrush(Color(255, 255, 0, 0)), x, y, dim, dim);
		}
	}
	DefWindowProc(WM_PAINT, (WPARAM)MenDC.m_hDC, (LPARAM)0);
	dc.BitBlt(0, HeaderRect.Height(), ListRect.Width(), ListRect.Height(), &MenDC, 0, HeaderRect.Height(), SRCCOPY);
	MenDC.DeleteDC();
	MemMap.DeleteObject();
}


void CMyListCtrl::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	DWORD dwPos = GetMessagePos();
	CPoint point(LOWORD(dwPos), HIWORD(dwPos));

	ScreenToClient(&point);

	LVHITTESTINFO lvinfo;
	lvinfo.pt = point;
	lvinfo.flags = LVHT_ABOVE;

	int nItem = SubItemHitTest(&lvinfo);
	if (nItem != -1)
	{
		//CString strtemp;
		//strtemp.Format("单击的是第%d行第%d列", lvinfo.iItem, lvinfo.iSubItem);
		//AfxMessageBox(strtemp);

		m_bDrawCirclePoint = true;
		int n = m_BreakPointItem[0];
		for (int i = 1; i <= n; i++)
		{
			if (m_BreakPointItem[i] == nItem)
			{
				for (int j = i; j <= n; j++)
				{
					if (j != 20)
						m_BreakPointItem[j] = m_BreakPointItem[j+1];
					else
						m_BreakPointItem[j] = -1;
				}
				m_BreakPointItem[0] --;
				Invalidate(TRUE);
				return;
			}
		}
		m_BreakPointItem[m_BreakPointItem[0]++] = nItem;
		CRect TmpRect;
		GetItemRect(nItem, &TmpRect, LVIR_BOUNDS);
		TmpRect.right = m_Width;
		InvalidateRect(&TmpRect,TRUE);
		if (m_BreakPointItem[0] >= 20) m_BreakPointItem[0] = 20;
	}
	*pResult = 0;
}


BOOL CMyListCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return CListCtrl::OnSetCursor(pWnd, nHitTest, message);
}
