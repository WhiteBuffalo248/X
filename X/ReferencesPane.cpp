// ReferencesPane.cpp : 实现文件
//

#include "stdafx.h"
#include "X.h"
#include "ReferencesPane.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

// CReferencesPane

IMPLEMENT_DYNAMIC(CReferencesPane, CDockablePane)

CReferencesPane::CReferencesPane()
{
	m_pCurrentReferencesView = nullptr;
}

CReferencesPane::~CReferencesPane()
{
	for (auto i : m_pReferenceTable)
	{
		if (i)
			delete i;
	}
}

CReferenceTable* CReferencesPane::getCurrentReferenceView()
{
	return m_pCurrentReferencesView;
}

std::vector<CReferenceTable*> CReferencesPane::getCReferenceTable()
{
	return m_pReferenceTable;
}

void CReferencesPane::closeTab(int index)
{
	if (index >= 0 && index < m_pReferenceTable.size())
	{
		CReferenceTable* p = m_pReferenceTable[index];
		delete p;
		
		m_pReferenceTable.erase(m_pReferenceTable.begin() + index);
		m_NameArray.erase(m_NameArray.begin() + index);
	}	
}

void CReferencesPane::closeAllTab()
{
	for (auto tab : m_pReferenceTable)
	{
		delete tab;
	}
	m_pReferenceTable.clear();
	m_NameArray.clear();
}

void CReferencesPane::newReferenceView(CString name)
{
	/*
	if (mCurrentReferenceView) //disconnect previous reference view
	mCurrentReferenceView->disconnectBridge();
	mCurrentReferenceView = new ReferenceView(false, this);
	mCurrentReferenceView->connectBridge();
	connect(mCurrentReferenceView, SIGNAL(showCpu()), this, SIGNAL(showCpu()));
	insertTab(0, mCurrentReferenceView, name);
	setCurrentIndex(0);
	Bridge::getBridge()->setResult(1);
	*/
	m_pCurrentReferencesView = new CReferenceTable;
	m_pReferenceTable.push_back(m_pCurrentReferencesView);
	m_NameArray.push_back(name);
	
	int time = -1;
	for (auto nameMember : m_NameArray)
	{
		if (!nameMember.CompareNoCase(name))
			time++;
	}
	CString tabLabal = name;
	if (time)
		tabLabal.AppendFormat(_T(" +%d"), time);

	CRect viewRect;
	GetClientRect(&viewRect);
	m_pCurrentReferencesView->Create(NULL, tabLabal, WS_CHILD | WS_VISIBLE, CRect(0, 0, viewRect.Width(), viewRect.Height()), &m_TabCtrl, NULL);

	m_TabCtrl.AddTab(m_pCurrentReferencesView, tabLabal);
	int num = m_TabCtrl.GetTabsNum();
	int id = m_TabCtrl.GetTabID(num - 1);
	int index = m_TabCtrl.GetTabByID(id);
	m_TabCtrl.SetActiveTab(m_pReferenceTable.size() - 1);
	
}




BEGIN_MESSAGE_MAP(CReferencesPane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CReferencesPane 消息处理程序




int CReferencesPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	CRect viewRect;
	GetClientRect(&viewRect);
	
	//m_wndReferencesView.Create(NULL, _T("References"), WS_CHILD | WS_VISIBLE, CRect(0, 0, lpCreateStruct->cx, lpCreateStruct->cy), this, NULL);
	//m_TabCtrl.Create(CMFCTabCtrl::STYLE_3D_VS2005, CRect(0, 0, lpCreateStruct->cx, m_TabCtrlHeight), this, 1, CMFCBaseTabCtrl::Location::LOCATION_TOP, 1);
	m_TabCtrl.Create(CMFCTabCtrl::STYLE_3D_SCROLLED, viewRect, this, 1, CMFCTabCtrl::LOCATION_TOP, TRUE);

	

	return 0;
}


void CReferencesPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	CRect viewRect;
	GetClientRect(&viewRect);

	m_TabCtrl.SetWindowPos(NULL, 0, 0, viewRect.Width(), viewRect.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
	
	
}


void CReferencesPane::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CDockablePane::OnPaint()
	CRect viewRect;
	GetClientRect(&viewRect);
	/*
	
	dc.FillSolidRect(&viewRect, RGB(200, 200, 200));
	*/

	CDC MenDC;
	CBitmap MemMap;
	MenDC.CreateCompatibleDC(&dc);
	MemMap.CreateCompatibleBitmap(&dc, viewRect.Width(), viewRect.Height());
	MenDC.SelectObject(&MemMap);
	MenDC.FillSolidRect(&viewRect, RGB(128, 128, 128));
	dc.BitBlt(0, 0, viewRect.Width(), viewRect.Height(), &MenDC, 0, 0, SRCCOPY);
	
}
