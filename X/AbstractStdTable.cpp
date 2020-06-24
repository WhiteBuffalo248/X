#include "stdafx.h"
#include "AbstractStdTable.h"
#include "Bridge.h"
#include "RichTextPainter.h"
#include "MiscUtil.h"
#include "GlobalID.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CAbstractStdTable::CAbstractStdTable()
{
	Initialize();
}


CAbstractStdTable::~CAbstractStdTable()
{
}

CString CAbstractStdTable::paintContent(CDC* painter, Graphics& graphics, dsint rowBase, int rowOffset, int col, int x, int y, int w, int h)
{
	bool isaddr = DbgIsDebugging() && getRowCount() > 0 && col == mAddressColumn;
	bool wIsSelected = isSelected(rowBase, rowOffset);
	CString text = getCellContent(rowBase + rowOffset, col);
	CString lowerText = text;
	CString lowerHeightText = mHighlightText;
	
	//duint wVA = isaddr ? duint(text.toULongLong(&isaddr, 16)) : 0;
	duint wVA;
	if (isaddr)
		isaddr = valueFromCString(text, &wVA, 16);
	else
		wVA = 0;
	
	auto wIsTraced = isaddr && DbgFunctions()->GetTraceRecordHitCount(wVA) != 0;
	Color lineBackgroundColor;
	bool isBackgroundColorSet;
	if (wIsSelected && wIsTraced)
	{
		lineBackgroundColor = mTracedSelectedAddressBackgroundColor;
		isBackgroundColorSet = true;
	}
	else if (wIsSelected)
	{
		lineBackgroundColor = mSelectionColor;
		isBackgroundColorSet = true;
	}
	else if (wIsTraced)
	{
		lineBackgroundColor = mTracedBackgroundColor;
		isBackgroundColorSet = true;
	}
	else
	{
		isBackgroundColorSet = false;
	}

	CRect cellRect(x, y, x + w, y + h);

	painter->FillSolidRect(&cellRect, mBackgroundColor.ToCOLORREF());

	if (isBackgroundColorSet)
		//painter->fillRect(QRect(x, y, w, h), QBrush(lineBackgroundColor));
		painter->FillSolidRect(&cellRect, lineBackgroundColor.ToCOLORREF());


	if (col == mAddressColumn && isaddr)
	{
		char label[MAX_LABEL_SIZE] = "";
		if (bAddressLabel && DbgGetLabelAt(wVA, SEG_DEFAULT, label)) //has label
		{
			char module[MAX_MODULE_SIZE] = "";
			CString labelText = CA2W(label, CP_UTF8);;
			if (DbgGetModuleAt(wVA, module) && labelText.Find(_T("JMP.&")) == 0)
			{
				CString moduleText = CA2W(module, CP_UTF8);;				
				text += " <";
				text += moduleText;
				text += ".";
				text += labelText;
				text += ">";
			}				
			else
			{
				text += " <";
				text += labelText;
				text += ">";
			}				
		}
		BPXTYPE bpxtype = DbgGetBpxTypeAt(wVA);
		bool isbookmark = DbgGetBookmarkAt(wVA);

		duint cip = Bridge::getBridge()->mLastCip;
		if (bCipBase)
		{
			duint base = DbgFunctions()->ModBaseFromAddr(cip);
			if (base)
				cip = base;
		}

		if (DbgIsDebugging() && wVA == cip) //debugging + cip
		{
			painter->FillSolidRect(&cellRect, mCipBackgroundColor.ToCOLORREF());
			if (!isbookmark) //no bookmark
			{
				if (bpxtype & bp_normal) //normal breakpoint
				{
					Color bpColor = mBreakpointBackgroundColor;
					if (!bpColor.GetAlpha()) //we don't want transparent text
						bpColor = mBreakpointColor;
					if (bpColor.ToCOLORREF() == mCipBackgroundColor.ToCOLORREF())
						bpColor = mCipColor;
					painter->SetTextColor(bpColor.ToCOLORREF());
				}
				else if (bpxtype & bp_hardware) //hardware breakpoint only
				{
					Color hwbpColor = mHardwareBreakpointBackgroundColor;
					if (!hwbpColor.GetAlpha()) //we don't want transparent text
						hwbpColor = mHardwareBreakpointColor;
					if (hwbpColor.ToCOLORREF() == mCipBackgroundColor.ToCOLORREF())
						hwbpColor = mCipColor;
					painter->SetTextColor(hwbpColor.ToCOLORREF());
				}
				else //no breakpoint
				{
					//painter->setPen(mCipColor);
					painter->SetTextColor(mCipColor.ToCOLORREF());
				}
			}
			else //bookmark
			{
				Color bookmarkColor = mBookmarkBackgroundColor;
				if (!bookmarkColor.GetAlpha()) //we don't want transparent text
					bookmarkColor = mBookmarkColor;
				if (bookmarkColor.ToCOLORREF() == mCipBackgroundColor.ToCOLORREF())
					bookmarkColor = mCipColor;
				painter->SetTextColor(bookmarkColor.ToCOLORREF());
				//painter->setPen(bookmarkColor);
			}
		}
		else //non-cip address
		{
			if (!isbookmark) //no bookmark
			{
				if (*label) //label
				{
					if (bpxtype == bp_none) //label only : fill label background
					{
						//painter->setPen(mLabelColor); //red -> address + label text
						painter->SetTextColor(mLabelColor.ToCOLORREF());
						painter->FillSolidRect(&cellRect, mLabelBackgroundColor.ToCOLORREF()); //fill label background
					}
					else //label + breakpoint
					{
						if (bpxtype & bp_normal) //label + normal breakpoint
						{
							//painter->setPen(mBreakpointColor);
							painter->SetTextColor(mBreakpointColor.ToCOLORREF());
							painter->FillSolidRect(&cellRect, mBreakpointBackgroundColor.ToCOLORREF()); //fill red
						}
						else if (bpxtype & bp_hardware) //label + hardware breakpoint only
						{
							//painter->setPen(mHardwareBreakpointColor);
							painter->SetTextColor(mHardwareBreakpointColor.ToCOLORREF());
							painter->FillSolidRect(&cellRect, mHardwareBreakpointBackgroundColor.ToCOLORREF()); //fill ?
						}
						else //other cases -> do as normal
						{
							//painter->setPen(mLabelColor); //red -> address + label text
							painter->SetTextColor(mLabelColor.ToCOLORREF());
							painter->FillSolidRect(&cellRect, mLabelBackgroundColor.ToCOLORREF()); //fill label background
						}
					}
				}
				else //no label
				{
					if (bpxtype == bp_none) //no label, no breakpoint
					{
						Color background;
						if (wIsSelected)
						{
							background = mSelectedAddressBackgroundColor;
							painter->SetTextColor(mSelectedAddressColor.ToCOLORREF());
							//painter->setPen(mSelectedAddressColor); //black address (DisassemblySelectedAddressColor)
						}
						else
						{
							background = mAddressBackgroundColor;
							//painter->setPen(mAddressColor); //DisassemblyAddressColor
							painter->SetTextColor(mAddressColor.ToCOLORREF());
						}
						if (background.GetAlpha())
							painter->FillSolidRect(&cellRect, background.ToCOLORREF()); //fill background
					}
					else //breakpoint only
					{
						if (bpxtype & bp_normal) //normal breakpoint
						{
							//painter->setPen(mBreakpointColor);
							painter->SetTextColor(mBreakpointColor.ToCOLORREF());
							painter->FillSolidRect(&cellRect, mBreakpointBackgroundColor.ToCOLORREF()); //fill red
						}
						else if (bpxtype & bp_hardware) //hardware breakpoint only
						{
							painter->SetTextColor(mHardwareBreakpointColor.ToCOLORREF());
							painter->FillSolidRect(&cellRect, mHardwareBreakpointBackgroundColor.ToCOLORREF()); //fill red
						}
						else //other cases (memory breakpoint in disassembly) -> do as normal
						{
							Color background;
							if (wIsSelected)
							{
								background = mSelectedAddressBackgroundColor;
								painter->SetTextColor(mSelectedAddressColor.ToCOLORREF());
								//painter->setPen(mSelectedAddressColor); //black address (DisassemblySelectedAddressColor)
							}
							else
							{
								background = mAddressBackgroundColor;
								//painter->setPen(mAddressColor);
								painter->SetTextColor(mAddressColor.ToCOLORREF());
							}
							if (background.GetAlpha())
								//painter->fillRect(QRect(x, y, w, h), QBrush(background)); //fill background
								painter->FillSolidRect(&cellRect, background.ToCOLORREF()); //fill background
						}
					}
				}
			}
			else //bookmark
			{
				if (*label) //label + bookmark
				{
					if (bpxtype == bp_none) //label + bookmark
					{
						painter->SetTextColor(mLabelColor.ToCOLORREF());
						painter->FillSolidRect(&cellRect, mBookmarkBackgroundColor.ToCOLORREF());
						//painter->setPen(mLabelColor); //red -> address + label text
						//painter->fillRect(QRect(x, y, w, h), QBrush(mBookmarkBackgroundColor)); //fill label background
					}
					else //label + breakpoint + bookmark
					{
						Color color = mBookmarkBackgroundColor;
						if (!color.GetAlpha()) //we don't want transparent text
							color = mAddressColor;
						
						painter->SetTextColor(color.ToCOLORREF());
						if (bpxtype & bp_normal) //label + bookmark + normal breakpoint
						{
							//painter->fillRect(QRect(x, y, w, h), QBrush(mBreakpointBackgroundColor)); //fill red
							painter->FillSolidRect(&cellRect, mBreakpointBackgroundColor.ToCOLORREF());
						}
						else if (bpxtype & bp_hardware) //label + bookmark + hardware breakpoint only
						{
							//painter->fillRect(QRect(x, y, w, h), QBrush(mHardwareBreakpointBackgroundColor)); //fill ?
							painter->FillSolidRect(&cellRect, mHardwareBreakpointBackgroundColor.ToCOLORREF());
						}
					}
				}
				else //bookmark, no label
				{
					if (bpxtype == bp_none) //bookmark only
					{
						//painter->setPen(mBookmarkColor); //black address
						//painter->fillRect(QRect(x, y, w, h), QBrush(mBookmarkBackgroundColor)); //fill bookmark color
						painter->SetTextColor(mBookmarkColor.ToCOLORREF());
						painter->FillSolidRect(&cellRect, mBookmarkBackgroundColor.ToCOLORREF());
					}
					else //bookmark + breakpoint
					{
						Color color = mBookmarkBackgroundColor;
						if (!color.GetAlpha()) //we don't want transparent text
							color = mAddressColor;
						
						painter->SetTextColor(color.ToCOLORREF());
						if (bpxtype & bp_normal) //bookmark + normal breakpoint
						{
							//painter->fillRect(QRect(x, y, w, h), QBrush(mBreakpointBackgroundColor)); //fill red
							painter->FillSolidRect(&cellRect, mBreakpointBackgroundColor.ToCOLORREF());
						}
						else if (bpxtype & bp_hardware) //bookmark + hardware breakpoint only
						{
							//painter->fillRect(QRect(x, y, w, h), QBrush(mHardwareBreakpointBackgroundColor)); //fill red
							painter->FillSolidRect(&cellRect, mHardwareBreakpointBackgroundColor.ToCOLORREF());
						}
						else //other cases (bookmark + memory breakpoint in disassembly) -> do as normal
						{
							//painter->setPen(mBookmarkColor); //black address
							//painter->fillRect(QRect(x, y, w, h), QBrush(mBookmarkBackgroundColor)); //fill bookmark color
							painter->SetTextColor(mBookmarkColor.ToCOLORREF());
							painter->FillSolidRect(&cellRect, mBookmarkBackgroundColor.ToCOLORREF());
						}
					}
				}
			}
		}
		CRect textRect = cellRect;
		textRect.DeflateRect(4, 0);
		painter->DrawText(text, &textRect, DT_CENTER);
		text = "";
	}
	
	else if (mHighlightText.GetLength() && lowerText.MakeLower().Find(lowerHeightText.MakeLower() != -1)) // TODO: case sensitive + regex highlighting
	{
		//super smart way of splitting while keeping the delimiters (thanks to cypher for guidance)
		int index = -2;
		do
		{
			index = lowerText.MakeLower().Find(lowerHeightText.MakeLower(), index + 2);
			if (index != -1)
			{
				text.Insert(index + mHighlightText.GetLength(), TCHAR('\1'));
				text.Insert(index, TCHAR('\1'));
			}
		} while (index != -1);
		//QStringList split = text.split(QChar('\1'), QString::SkipEmptyParts, Qt::CaseInsensitive);
		text.Trim(_T(" "));
		CStringArray textArray;
		int curPos = 0;
		do 
		{
			CString itemText = text.Tokenize(_T("\1"), curPos);
			if (curPos != -1)
				textArray.Add(itemText);
		} while (curPos != -1);

		//create rich text list
		RichTextPainter::CustomRichText_t curRichText;
		curRichText.flags = RichTextPainter::FlagColor;
		curRichText.textColor = mTextColor;
		curRichText.highlightColor = ConfigColor("SearchListViewHighlightColor");
		RichTextPainter::List richText;
		/*
		foreach(QString str, split)
		{
			curRichText.text = str;
			curRichText.highlight = !str.compare(mHighlightText, Qt::CaseInsensitive);
			richText.push_back(curRichText);
		}
		*/
		
		for (int i = 0; i < textArray.GetCount(); i++)
		{
			curRichText.text = textArray[i];
			curRichText.highlight = !textArray[i].CompareNoCase(mHighlightText);
			richText.push_back(curRichText);
		}

		//paint the rich text
		RichTextPainter::paintRichText(painter, x + 1, y, w, h, 4, richText, &lf);
		text = "";
	}
	return text;
}

void CAbstractStdTable::updateColors()
{
	CAbstractTableView::updateColors();

	mCipBackgroundColor = ConfigColor("DisassemblyCipBackgroundColor");
	mCipColor = ConfigColor("DisassemblyCipColor");
	mBreakpointBackgroundColor = ConfigColor("DisassemblyBreakpointBackgroundColor");
	mBreakpointColor = ConfigColor("DisassemblyBreakpointColor");
	mHardwareBreakpointBackgroundColor = ConfigColor("DisassemblyHardwareBreakpointBackgroundColor");
	mHardwareBreakpointColor = ConfigColor("DisassemblyHardwareBreakpointColor");
	mBookmarkBackgroundColor = ConfigColor("DisassemblyBookmarkBackgroundColor");
	mBookmarkColor = ConfigColor("DisassemblyBookmarkColor");
	mLabelColor = ConfigColor("DisassemblyLabelColor");
	mLabelBackgroundColor = ConfigColor("DisassemblyLabelBackgroundColor");
	mSelectedAddressBackgroundColor = ConfigColor("DisassemblySelectedAddressBackgroundColor");
	mSelectedAddressColor = ConfigColor("DisassemblySelectedAddressColor");
	mAddressBackgroundColor = ConfigColor("DisassemblyAddressBackgroundColor");
	mAddressColor = ConfigColor("DisassemblyAddressColor");
	mTracedBackgroundColor = ConfigColor("DisassemblyTracedBackgroundColor");

	auto a = mSelectionColor, b = mTracedBackgroundColor;
	mTracedSelectedAddressBackgroundColor = Color((a.GetRed() + b.GetRed()) / 2, (a.GetGreen() + b.GetGreen()) / 2, (a.GetBlue() + b.GetBlue()) / 2);
}

void CAbstractStdTable::enableMultiSelection(bool enabled)
{
	mIsMultiSelectionAllowed = enabled;
}





/************************************************************************************
Selection Management
************************************************************************************/
bool CAbstractStdTable::isSelected(int base, int offset) const
{
	int wIndex = base + offset;

	if (wIndex >= mSelection.fromIndex && wIndex <= mSelection.toIndex)
		return true;
	else
		return false;
}

void CAbstractStdTable::setSingleSelection(int index)
{
	mSelection.firstSelectedIndex = index;
	mSelection.fromIndex = index;
	mSelection.toIndex = index;
	//emit selectionChangedSignal(index);
}

void CAbstractStdTable::expandSelectionUpTo(int to)
{
	if (to < mSelection.firstSelectedIndex)
	{
		mSelection.fromIndex = to;
		mSelection.toIndex = mSelection.firstSelectedIndex;
		//emit selectionChangedSignal(to);
	}
	else if (to > mSelection.firstSelectedIndex)
	{
		mSelection.fromIndex = mSelection.firstSelectedIndex;
		mSelection.toIndex = to;
		//emit selectionChangedSignal(to);
	}
	else if (to == mSelection.firstSelectedIndex)
	{
		setSingleSelection(to);
	}
}

int CAbstractStdTable::getInitialSelection() const
{
	return mSelection.firstSelectedIndex;
}

vector<int> CAbstractStdTable::getSelection() const
{
	vector<int> selection;
	//selection.reserve(mSelection.toIndex - mSelection.fromIndex);
	//selection.resize(mSelection.toIndex - mSelection.fromIndex);
	for (int i = mSelection.fromIndex; i <= mSelection.toIndex; i++)
	{
		selection.push_back(i);
	}
	return selection;
}



BEGIN_MESSAGE_MAP(CAbstractStdTable, CAbstractTableView)
	ON_COMMAND_RANGE(ID_GLOBAL_COPY_START, ID_GLOBAL_COPY_END, &CAbstractStdTable::OnGlobalCopy)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

void CAbstractStdTable::setupCopyMenu(CMenu* copyMenu)
{
	if (!getColumnCount())
		return;
	//Copy->Whole Line
	copyMenu->AppendMenu(MF_ENABLED, ID_GLOBAL_COPY_LINE, _T("Line"));
	copyMenu->AppendMenu(MF_ENABLED, ID_GLOBAL_COPY_CROPPED_TABLE, _T("Cropped &Table"));
	copyMenu->AppendMenu(MF_ENABLED, ID_GLOBAL_COPY_FULL_TABLE, _T("&Full Table"));
	copyMenu->AppendMenu(MF_SEPARATOR);
	copyMenu->AppendMenu(MF_ENABLED, ID_GLOBAL_COPY_LINE_TO_LOG, _T("Line To Log"));
	copyMenu->AppendMenu(MF_ENABLED, ID_GLOBAL_COPY_CROPPED_TABLE_TO_LOG, _T("Cropped Table To Log"));
	copyMenu->AppendMenu(MF_ENABLED, ID_GLOBAL_COPY_FULL_TABLE_TO_LOG, _T("Full Table To Log"));
	copyMenu->AppendMenu(MF_SEPARATOR);
	//Copy->ColName


	for (int i = 0; i < getColumnCount(); i++)
	{
		if (!getCellContent(getInitialSelection(), i).GetLength()) //skip empty cells
			continue;
		CString title = mCopyTitles.at(i);
		if (!title.GetLength()) //skip empty copy titles
			continue;
		copyMenu->AppendMenu(MF_ENABLED, ID_GLOBAL_COPY_START + i, title);
	}
}

void CAbstractStdTable::OnGlobalCopy(UINT nCopyID)
{
	int col = nCopyID - ID_GLOBAL_COPY_START;
	CString finalText = getCellContent(getInitialSelection(), col);
	//while (finalText.endsWith(" ")) finalText.chop(1);	
	//Bridge::CopyToClipboard(finalText);
	if (!finalText.IsEmpty())
		finalText.TrimRight(_T(" "));
	if (!finalText.IsEmpty())
		setTextToClipboard(finalText);
}



void CAbstractStdTable::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
}

void CAbstractStdTable::OnButtonDown(UINT nFlags, CPoint point, bool& wAccept)
{
	if (getGuiState() == CAbstractTableView::NoState)
	{
		if (point.y > getHeaderHeight())
		{
			int wRowIndex = getTableOffset() + getIndexOffsetFromY(transY(point.y));

			if (wRowIndex < getRowCount())
			{
				//if (mIsMultiSelectionAllowed && (event->modifiers() & Qt::ShiftModifier))
				short i = GetKeyState(VK_SHIFT);
				if (mIsMultiSelectionAllowed && (GetKeyState(VK_SHIFT) < 0))
					expandSelectionUpTo(wRowIndex);
				else
					setSingleSelection(wRowIndex);

				mGuiState = CAbstractStdTable::MultiRowsSelectionState;

				Invalidate();
				wAccept = true;
			}
		}
	}
}

void CAbstractStdTable::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	bool wAccept = false;
	OnButtonDown(nFlags, point, wAccept);
	if (wAccept)
		SetCapture();
	else
		CAbstractTableView::OnLButtonDown(nFlags, point);
}


void CAbstractStdTable::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	bool wAccept = false;
	//OnButtonDown(nFlags, point, wAccept);	
	if (!wAccept)
		CAbstractTableView::OnRButtonDown(nFlags, point);
}



void CAbstractStdTable::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	bool wAccept = true;
	int y = transY(point.y);

	if (mGuiState == CAbstractStdTable::MultiRowsSelectionState)
	{
		//qDebug() << "State = MultiRowsSelectionState";
		
		if (y >= 0 && y <= getTableHeight())
		{
			int wRowIndex = getTableOffset() + getIndexOffsetFromY(y);

			if (wRowIndex < getRowCount())
			{
				if (mIsMultiSelectionAllowed)
					expandSelectionUpTo(wRowIndex);
				else
					setSingleSelection(wRowIndex);

				Invalidate();

				wAccept = false;
			}
		}
		else if (y < 0)
		{
			SendMessage(WM_VSCROLL, SB_LINEUP, 0);
		}
		else if (y > getTableHeight())
		{
			SendMessage(WM_VSCROLL, SB_LINEDOWN, 0);
		}
	}

	if (wAccept)
		CAbstractTableView::OnMouseMove(nFlags, point);
}


void CAbstractStdTable::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	bool wAccept = true;
	ReleaseCapture();
	if (mGuiState == CAbstractStdTable::MultiRowsSelectionState)
	{
		mGuiState = CAbstractStdTable::NoState;
		Invalidate();
		wAccept = false;
	}

	if (wAccept)
		CAbstractTableView::OnLButtonUp(nFlags, point);
}
