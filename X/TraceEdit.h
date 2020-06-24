#pragma once


// CTraceEdit

class CTraceEdit : public CEdit
{
	DECLARE_DYNAMIC(CTraceEdit)

public:
	CTraceEdit();
	virtual ~CTraceEdit();

	CString addHistoryClear();
	void addLineToHistory(CString parLine);

private:
private:
	int mCmdHistoryMaxSize = 1000;
	CStringArray mCmdHistory;
	int mCmdIndex;
	bool bSixPressed;

protected:
	DECLARE_MESSAGE_MAP()
};


