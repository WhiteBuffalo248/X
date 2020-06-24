#pragma once
#include "StdTable.h"
#include "QBeaEngine.h"
class CBreakPointsManage :
	public CStdTable
{
public:
	CBreakPointsManage();
	~CBreakPointsManage();

public:
	void updateColors() override;
	void updateBreakpoints();
	virtual CString paintContent(CDC* painter, Graphics& graphics, dsint rowBase, int rowOffset, int col, int x, int y, int w, int h);
	void removeBreakpoint();
	void toggleBreakpoint();

	void resetHitCountBreakpoint();
	void enableAllBreakpoints();
	void disableAllBreakpoints();
	void removeAllBreakpoints();
	void addDllBreakpoint();
	void addExceptionBreakpoint();
	void OnBreakPointsManageHandle(UINT nID);
	void followEvent();


private:
	enum
	{
		ColType,
		ColAddr,
		ColModLabel,
		ColState,
		ColDisasm,
		ColHits,
		ColSummary
	};

	duint mCip = 0;
	int mExceptionMaxLength;

	QBeaEngine* mDisasm;

	std::vector<BRIDGEBP> mBps;
	std::vector<std::pair<RichTextPainter::List, RichTextPainter::List>> mRich;
	std::unordered_map<duint, const char*> mExceptionMap;
	//CStringList mExceptionList;
	std::list <CString> mExceptionList;

	const int bpIndex(int i)
	{
		return mData.at(i).at(ColAddr).userdata;
	}

	CString bpTypeName(BPXTYPE type)
	{
		switch (type)
		{
		case bp_normal:
			return _T("Software");
		case bp_hardware:
			return _T("Hardware");
		case bp_memory:
			return _T("Memory");
		case bp_dll:
			return _T("DLL");
		case bp_exception:
			return _T("Exception");
		default:
			return CString();
		}
	}

	bool isValidBp(int sel = -1)
	{
		if (sel == -1)
			sel = getInitialSelection();
		if (!DbgIsDebugging() || mBps.empty() || !isValidIndex(sel, ColType))
			return false;
		auto & bp = mBps.at(bpIndex(sel));
		return bp.addr != 0 || bp.active;
	}

	const BRIDGEBP & selectedBp(int index = -1)
	{
		if (index == -1)
			index = getInitialSelection();
		return mBps.at(bpIndex(index));
	}

	Color mDisasmBackgroundColor;
	Color mDisasmSelectionColor;
	Color mCipBackgroundColor;
	Color mCipColor;
	Color mSummaryParenColor;
	Color mSummaryKeywordColor;
	Color mSummaryStringColor;
public:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};

