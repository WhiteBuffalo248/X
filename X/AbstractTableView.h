#pragma once

#include "Configuration.h"
#include "StringUtil.h"
#include <vector>
#include <map>


using namespace std;
using namespace Gdiplus;


// CAbstractTableView

class CAbstractTableView : public CWnd
{
	
	DECLARE_DYNAMIC(CAbstractTableView)

public:

	
	CAbstractTableView();
	virtual ~CAbstractTableView();

	bool setTextToClipboard(CString& text);
	// Configuration
	virtual void Initialize();
	virtual void updateColors();
	virtual void updateFonts();
	virtual void initHScorll();
	virtual void initVScorll();

public:	
	enum GuiState
	{
		NoState,
		ReadyToResizeH,
		ReadyToResizeV,
		ResizeColumnState,
		ResizeWindowState,
		HeaderButtonPressed,
		HeaderButtonReordering
	};

	struct ColumnAttribute
	{
		int		index;// 按钮Id号			
		CString title;//按钮标题文本
		CRect	rcRect;// 按钮矩形位置	
		CWnd*	pParent;// 按钮父窗口
		bool	bDisabled;// 标识按钮已经被禁用	
		bool	bPressed;// 标识已经按下按钮			
		bool	bCheck;// 标识按钮已经被选中	
		bool	bHovering;// 标识鼠标进入按钮区域			
	};
	
	struct ColumnOverlap
	{
		bool bHovering;
		bool	bPressInOverlap;
		CRect	OverlapRect;
	};
	
	struct Header
	{
		bool isVisible;
		int height;
		int activeButtonIndex;
		int insertButtonIndex;
	};
	struct HeaderButton
	{
		bool isClickable;
		bool isPressed;
		bool isMouseOver;
	};
	struct Column
	{
		int width;
		bool hidden;
		HeaderButton header;
		CString title;
		UINT index;
	};
	struct ColumnResizingData
	{
		bool splitHandle = false;
		int index;
		int lastPosX;
	};
	struct TitleData
	{
		bool bTitle = false;
		int hight = 0;		
		CString titleText;

	};

	struct splitData
	{
		bool bSplit = false;
		int iwidth = 0;
	};

protected:
	LOGFONT lf;
	// Configuration
	Color mBackgroundColor;
	Color mTextColor;
	Color mSeparatorColor;
	Color mHeaderTextColor;
	Color mSelectionColor;
	CString mViewName;
	
	
private:
	Header m_Header;
	GuiState m_GuiState;
	vector<Column> m_vColumnList;
	vector<int> m_vColumnOrder;
	ColumnResizingData mColResizeData;
	vector<ColumnAttribute> m_vHeadButton;
	map<int, ColumnOverlap> m_mColumnOverlap;
	TitleData mTitleData;
	splitData mSplitData;

	dsint mRowCount;
	dsint mRowHight;
	dsint mTableOffset;
	dsint mPrevTableOffset;

	int resizinglastPosY = 0;
	int mReorderStartX;
	int mNbrOfLineToPrint;
	int mHoveredColumnDisplayIndex;	
	int mCharWidth = 0;

	bool mShouldReload;
	bool mDrawDebugOnly;
	bool mAllowPainting;
	bool mLButtonDown;
	bool bSHIFT = false;
	bool bInHeader = false;
	

	
public:
	int getGuiState() const;
	bool getShiftState();
	bool getRButtonInHeader();
	
	
	// New Columns/New Size
	virtual void addColumnAt(int width, const CString & title, bool isClickable);
	virtual void setRowCount(dsint count);
	virtual void deleteAllColumns();
	// Pure Virtual Methods
	virtual CString paintContent(CDC* painter, Graphics& graphics, dsint rowBase, int rowOffset, int col, int x, int y, int w, int h) = 0;
	// Getter & Setter
	dsint getRowCount() const;
	int getRowHeight() const;
	void setShowHeader(bool show);
	bool getShowHeader() const;
	int getColumnHeight() const;
	int getHeaderHeight() const;
	int getTableHeight() const;
	bool setColumnIndex();
	int getColumnIndex() const;
	int getColumnCount() const;
	void setColumnHidden(int col, bool hidden);
	bool getColumnHidden(int col) const;	
	void setNbrOfLineToPrint(int parNbrOfLineToPrint);
	void setColumnWidth(int index, int width);
	int getColumnWidth(int index) const;
	int getCharWidth() const;
	bool getDrawDebugOnly() const;
	void setDrawDebugOnly(bool value);
	bool getAllowPainting() const;
	void setAllowPainting(bool allow);

	//characteristic
	void setHasSplit(bool bHas = true);
	bool getHasSplit() const;
	void setSplitWidth(UINT width);
	int getSplitWidth() const;
	//title
	void setHasTitle(bool bHas = true);
	bool getHasTitle() const;
	void setTitleText(CString text);
	void setTitleHeight(UINT hight);
	int getTitleHeight() const;

	// Coordinates Utils
	int getIndexOffsetFromY(int y) const;
	int transY(int y) const;
	int getColumnIndexFromX(int x) const;
	int getColumnDisplayIndexFromX(int x);
	int getColumnPosition(int index) const;
	int getViewableRowsCount() const;

	// UI customization
	void loadColumnFromConfig(const CString& viewName);
	void saveColumnToConfig();
	static void setupColumnConfigDefaultValue(map<string, duint> & map, const string & viewName, int columnCount);

	// Table offset management
	dsint getTableOffset() const;
	virtual void setTableOffset(dsint val);

	// Update/Reload/Refresh/Repaint
	virtual void prepareData();
	//slots:
	virtual void reloadData();	

	int	TButtonHitTest(CPoint point, CRect& rtButton);
	int	HitTestOverlap(CPoint point, CRect& rtButton);
	
	void CreateColumn(CDC* painter, UINT index, int startX);

private:
	void defaultFont();

protected:
	DECLARE_MESSAGE_MAP()

public:
	// ScrollBar Management
	virtual dsint sliderMovedHook(int type, dsint value, dsint delta);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);	
	afx_msg void OnMouseLeave();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};


