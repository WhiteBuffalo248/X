// TraceEdit.cpp : 实现文件
//

#include "stdafx.h"
#include "X.h"
#include "TraceEdit.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// CTraceEdit

IMPLEMENT_DYNAMIC(CTraceEdit, CEdit)

CTraceEdit::CTraceEdit()
{

}

CTraceEdit::~CTraceEdit()
{
}


BEGIN_MESSAGE_MAP(CTraceEdit, CEdit)
END_MESSAGE_MAP()

void CTraceEdit::addLineToHistory(CString parLine)
{
	int size = mCmdHistory.GetCount();
	if (mCmdHistory.GetCount() > mCmdHistoryMaxSize)
		for (int i = mCmdHistoryMaxSize; i > size; i--)
			mCmdHistory.RemoveAt(i);
		

	if (mCmdHistory.IsEmpty() || mCmdHistory.GetAt(0) != parLine)
		mCmdHistory[0] = parLine;

	mCmdIndex = -1;
}

CString CTraceEdit::addHistoryClear()
{
	CString editText;
	GetWindowTextW(editText);
	if (editText.GetLength())
	{
		addLineToHistory(editText);
		Clear();
	}
	return editText;
}



// CTraceEdit 消息处理程序


