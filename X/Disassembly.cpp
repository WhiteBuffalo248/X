#include "stdafx.h"
#include "Disassembly.h"
#include "StringUtil.h"
#include "Bridge.h"
#include "QBeaEngine.h"
#include "RichTextPainter.h"
#include "EncodeMap.h"
#include "Configuration.h"
#include "Breakpoints.h"
#include "DisasmPane.h"
#include "GlobalID.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CDisassembly::CDisassembly()
{
	mHighlightingMode = false;

	setHasSplit();
	setSplitWidth(20);

	mCodeFoldingManager = nullptr;
	mMemPage = new MemoryPage(0, 0);
	mInstBuffer.RemoveAll();
	historyClear();
	memset(&mSelection, 0, sizeof(SelectionData));
	int maxModuleSize = (int)ConfigUint("Disassembler", "MaxModuleSize");
	Config()->writeUints();
	mDisasm = new QBeaEngine(maxModuleSize);
	tokenizerConfigUpdatedSlot();

	duint setting;
	if (BridgeSettingGetUint("Gui", "DisableBranchDestinationPreview", &setting))
		mPopupEnabled = !setting;
	else
		mPopupEnabled = true;

	mXrefInfo.refcount = 0;

	addColumnAt(200, L"address", true); //address
	addColumnAt(200, L"bytes", true); //bytes
	addColumnAt(200, L"disassembly", true); //disassembly
	addColumnAt(200, L"comments", true); //comments
	setShowHeader(false); //hide header

	Initialize();
}

CDisassembly::~CDisassembly()
{
	delete mMemPage;
	delete mDisasm;
	if (mXrefInfo.refcount != 0)
		BridgeFree(mXrefInfo.references);
}

void CDisassembly::setSideBarRowSignal(int rowCount)
{
	CWnd* pMainWnd = AfxGetMainWnd();
	if (pMainWnd->GetSafeHwnd())
		pMainWnd->PostMessage(WM_USER + Signal_SetSideBarRow_Msg, 0, LPARAM(getRowCount()));
}

BEGIN_MESSAGE_MAP(CDisassembly, CAbstractTableView)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETFOCUS()
	ON_WM_NCMOUSEMOVE()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_CONTEXTMENU()
	ON_COMMAND_RANGE(ID_Disassembly_Copy_Selection, ID_Disassembly_Search_AllModule_GUID, &CDisassembly::OnHandle)
END_MESSAGE_MAP()

void CDisassembly::updateFonts()
{
	lf = ConfigFont("Disassembly");
}

void CDisassembly::updateColors()
{
	CAbstractTableView::updateColors();
	mBackgroundColor = ConfigColor("DisassemblyBackgroundColor");

	mInstructionHighlightColor = ConfigColor("InstructionHighlightColor");
	mDisassemblyRelocationUnderlineColor = ConfigColor("DisassemblyRelocationUnderlineColor");
	mSelectionColor = ConfigColor("DisassemblySelectionColor");
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
	mTracedAddressBackgroundColor = ConfigColor("DisassemblyTracedBackgroundColor");
	mSelectedAddressColor = ConfigColor("DisassemblySelectedAddressColor");
	mAddressBackgroundColor = ConfigColor("DisassemblyAddressBackgroundColor");
	mAddressColor = ConfigColor("DisassemblyAddressColor");
	mBytesColor = ConfigColor("DisassemblyBytesColor");
	mBytesBackgroundColor = ConfigColor("DisassemblyBytesBackgroundColor");
	mModifiedBytesColor = ConfigColor("DisassemblyModifiedBytesColor");
	mModifiedBytesBackgroundColor = ConfigColor("DisassemblyModifiedBytesBackgroundColor");
	mRestoredBytesColor = ConfigColor("DisassemblyRestoredBytesColor");
	mRestoredBytesBackgroundColor = ConfigColor("DisassemblyRestoredBytesBackgroundColor");
	mByte00Color = ConfigColor("DisassemblyByte00Color");
	mByte00BackgroundColor = ConfigColor("DisassemblyByte00BackgroundColor");
	mByte7FColor = ConfigColor("DisassemblyByte7FColor");
	mByte7FBackgroundColor = ConfigColor("DisassemblyByte7FBackgroundColor");
	mByteFFColor = ConfigColor("DisassemblyByteFFColor");
	mByteFFBackgroundColor = ConfigColor("DisassemblyByteFFBackgroundColor");
	mByteIsPrintColor = ConfigColor("DisassemblyByteIsPrintColor");
	mByteIsPrintBackgroundColor = ConfigColor("DisassemblyByteIsPrintBackgroundColor");
	mAutoCommentColor = ConfigColor("DisassemblyAutoCommentColor");
	mAutoCommentBackgroundColor = ConfigColor("DisassemblyAutoCommentBackgroundColor");
	mMnemonicBriefColor = ConfigColor("DisassemblyMnemonicBriefColor");
	mMnemonicBriefBackgroundColor = ConfigColor("DisassemblyMnemonicBriefBackgroundColor");
	mCommentColor = ConfigColor("DisassemblyCommentColor");
	mCommentBackgroundColor = ConfigColor("DisassemblyCommentBackgroundColor");
	mUnconditionalJumpLineColor = ConfigColor("DisassemblyUnconditionalJumpLineColor");
	mConditionalJumpLineTrueColor = ConfigColor("DisassemblyConditionalJumpLineTrueColor");
	mConditionalJumpLineFalseColor = ConfigColor("DisassemblyConditionalJumpLineFalseColor");
	mLoopColor = ConfigColor("DisassemblyLoopColor");
	mFunctionColor = ConfigColor("DisassemblyFunctionColor");

	auto a = mSelectionColor, b = mTracedAddressBackgroundColor;
	mTracedSelectedAddressBackgroundColor = Color((a.GetRed() + b.GetRed()) / 2, (a.GetGreen() + b.GetGreen()) / 2, (a.GetBlue() + b.GetBlue()) / 2);

	
	mLoopPen.CreatePen(PS_SOLID, 1, mLoopColor.ToCOLORREF());	
	mFunctionPen.CreatePen(PS_SOLID, 1, mFunctionColor.ToCOLORREF());	
	mUnconditionalPen.CreatePen(PS_SOLID, 1, mUnconditionalJumpLineColor.ToCOLORREF());	
	mConditionalTruePen.CreatePen(PS_SOLID, 1, mConditionalJumpLineTrueColor.ToCOLORREF());	
	mConditionalFalsePen.CreatePen(PS_SOLID, 1, mConditionalJumpLineFalseColor.ToCOLORREF());

	CapstoneTokenizer::UpdateColors();
	mDisasm->UpdateConfig();
}

bool CDisassembly::isSelected(CList<Instruction_t>* buffer, int index) const
{
	if (buffer->GetSize() > 0 && index >= 0 && index < buffer->GetSize())
	{
		POSITION tmpPostion = mInstBuffer.FindIndex(index);
		dsint tmpAva = mInstBuffer.GetAt(tmpPostion).rva;
		if (tmpAva >= mSelection.fromIndex && tmpAva <= mSelection.toIndex)
			return true;
		else
			return false;
	}
	else
	{
		return false;
	}
}

void CDisassembly::tokenizerConfigUpdatedSlot()
{
	//mDisasm->UpdateConfig();
	//mPermanentHighlightingMode = ConfigBool("Disassembler", "PermanentHighlightingMode");
	//mNoCurrentModuleText = ConfigBool("Disassembler", "NoCurrentModuleText");
}

bool CDisassembly::isHighlightMode() const
{
	return mHighlightingMode;
}

bool CDisassembly::hightlightToken(const CapstoneTokenizer::SingleToken & token)
{
	mHighlightToken = token;
	mHighlightingMode = false;
	return true;
}

void CDisassembly::DrawBPCircle(CDC* painter, Gdiplus::Graphics& graphics, int line, bool isbp ,bool isbpdisabled)
{
	int wX =  this->GetParent()->GetScrollPos(SB_HORZ);
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	GetScrollInfo(SB_HORZ, &si);

	if (isbp || isbpdisabled)
	{	
		Color mColor;
		if (isbp)
			mColor = 0xFFFF0000;
		if (isbpdisabled)
			mColor = 0xFF00FF00;
		int x = getSplitWidth() / 2;
		int y = getHeaderHeight() + getRowHeight() * line;
		int dim = getSplitWidth() > getRowHeight() ? getRowHeight() : getSplitWidth();
		int airValue = dim / 5;
		//Graphics graphics(painter->m_hDC);
		Pen pen(mColor, 0.5);
	
		graphics.DrawEllipse(&pen, airValue - si.nPos, y + airValue, dim - 2* airValue, dim - 2 * airValue);
		graphics.FillEllipse(&Gdiplus::SolidBrush(mColor), airValue - si.nPos, y + airValue, dim - 2 * airValue, dim - 2 * airValue);

	}
}

/************************************************************************************
Reimplemented Functions
************************************************************************************/
/**
* @brief       This method has been reimplemented. It returns the string to paint or paints it
*              by its own.
*
* @param[in]   painter     Pointer to the painter that allows painting by its own
* @param[in]   rowBase     Index of the top item (Table offset)
* @param[in]   rowOffset   Index offset starting from rowBase
* @param[in]   col         Column index
* @param[in]   x           Rectangle x
* @param[in]   y           Rectangle y
* @param[in]   w           Rectangle width
* @param[in]   h           Rectangle heigth
*
* @return      String to paint.
*/
CString CDisassembly::paintContent(CDC* painter, Graphics& graphics, dsint rowBase, int rowOffset, int col, int x, int y, int w, int h)
{
	UNUSED(rowBase);
// Hook/hack to update the sidebar at the same time as this widget.
// Ensures the two widgets are synced and prevents "draw lag"
	CDisasmPane* pParentPane = (CDisasmPane*)GetParent();
	auto sidebar = pParentPane->getCCPUSideBar();
	if (sidebar)
		sidebar->reload();
	
	if (mHighlightingMode)
	{

	}
	POSITION tmpPosition = mInstBuffer.FindIndex(rowOffset);
	dsint wRVA = mInstBuffer.GetAt(tmpPosition).rva;
	dsint cur_addr = rvaToVa(wRVA);

	if (col == 0)//只需一次
	{
		/*
		duint wVA = mInstBuffer.GetAt(mInstBuffer.FindIndex(rowOffset)).rva + mMemPage->getBase();
		*/
		BPXTYPE bpTyte = DbgGetBpxTypeAt(cur_addr);
		bool bBookMark = DbgGetBookmarkAt(cur_addr);
		bool isbp = DbgGetBpxTypeAt(cur_addr) != bp_none;
		bool isbpdisabled = DbgIsBpDisabled(cur_addr);

		DrawBPCircle(painter, graphics, rowOffset, isbp, isbpdisabled);
	}

	bool wIsSelected = isSelected(&mInstBuffer, rowOffset);	
	auto traceCount = DbgFunctions()->GetTraceRecordHitCount(cur_addr);

	
	CRect tmpRect(x, y, x + w, y + h);

	CBrush	mBrush;

	painter->FillSolidRect(&tmpRect, mBackgroundColor.ToCOLORREF());

	// Highlight if selected
	if (wIsSelected && traceCount)
		painter->FillSolidRect(&tmpRect, mTracedSelectedAddressBackgroundColor.ToCOLORREF());
	
	else if (wIsSelected)
		painter->FillSolidRect(&tmpRect, mSelectionColor.ToCOLORREF());
	
	else if (traceCount)
	{
		// Color depending on how often a sequence of code is executed
		int exponent = 1;
		while (traceCount >>= 1) //log2(traceCount)
			exponent++;
		int colorDiff = (exponent * exponent) / 2;

		// If the user has a light trace background color, substract
		if (mTracedAddressBackgroundColor.GetB() > 160)
			colorDiff *= -1;
		mBrush.DeleteObject();
		mBrush.CreateSolidBrush(RGB(mTracedAddressBackgroundColor.GetR(),
			mTracedAddressBackgroundColor.GetG(), 
			max(0, min(256, mTracedAddressBackgroundColor.GetB()) + colorDiff))
			);		
		painter->FillRect(tmpRect, &mBrush);

	}

	switch (col)
	{
	case 0: // Draw address (+ label)
	{
		char label[MAX_LABEL_SIZE] = "";
		CString addrText = getAddrText(cur_addr, label);
		BPXTYPE bpxtype = DbgGetBpxTypeAt(cur_addr);
		bool isbookmark = DbgGetBookmarkAt(cur_addr);
		if (wRVA == mCipRva && !Bridge::getBridge()->mIsRunning && DbgMemFindBaseAddr(DbgValFromString("cip"), nullptr)) //cip + not running + valid cip
		{
			painter->FillSolidRect(&tmpRect, mCipBackgroundColor.ToCOLORREF());
			if (!isbookmark) //no bookmark
			{
				if (bpxtype & bp_normal) //normal breakpoint
				{
					Color bpColor =	mBreakpointBackgroundColor;
					if (!bpColor.ToCOLORREF()) //we don't want transparent text
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
						painter->SetTextColor(mLabelColor.ToCOLORREF());	////red -> address + label text
						painter->FillSolidRect(&tmpRect, mLabelBackgroundColor.ToCOLORREF());	//fill label background
					}
					else //label + breakpoint
					{
						if (bpxtype & bp_normal) //label + normal breakpoint
						{
							//painter->setPen(mBreakpointColor);
							//painter->fillRect(QRect(x, y, w, h), QBrush(mBreakpointBackgroundColor)); //fill red
							painter->SetTextColor(mBreakpointColor.ToCOLORREF());
							painter->FillSolidRect(&tmpRect, mBreakpointBackgroundColor.ToCOLORREF());
						}
						else if (bpxtype & bp_hardware) //label + hardware breakpoint only
						{
							//painter->setPen(mHardwareBreakpointColor);
							//painter->fillRect(QRect(x, y, w, h), QBrush(mHardwareBreakpointBackgroundColor)); //fill ?
							painter->SetTextColor(mHardwareBreakpointColor.ToCOLORREF());
							painter->FillSolidRect(&tmpRect, mHardwareBreakpointBackgroundColor.ToCOLORREF());
						}
						else //other cases -> do as normal
						{
							//painter->setPen(mLabelColor); //red -> address + label text
							//painter->fillRect(QRect(x, y, w, h), QBrush(mLabelBackgroundColor)); //fill label background
							painter->SetTextColor(mLabelColor.ToCOLORREF());
							painter->FillSolidRect(&tmpRect, mLabelBackgroundColor.ToCOLORREF());
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
							//painter->setPen(mSelectedAddressColor); //black address (DisassemblySelectedAddressColor)
							painter->SetTextColor(mSelectedAddressColor.ToCOLORREF());
							
						}
						else
						{
						
							background = mAddressBackgroundColor;
							//painter->setPen(mAddressColor); //DisassemblyAddressColor
							painter->SetTextColor(mAddressColor.ToCOLORREF());
						}
						if (background.GetAlpha())
						{
							
							//painter->fillRect(QRect(x, y, w, h), QBrush(background)); //fill background
							painter->FillSolidRect(&tmpRect, background.ToCOLORREF());
						}
							
					}
					else //breakpoint only
					{
						if (bpxtype & bp_normal) //normal breakpoint
						{
							//painter->setPen(mBreakpointColor);
							//painter->fillRect(QRect(x, y, w, h), QBrush(mBreakpointBackgroundColor)); //fill red

							painter->SetTextColor(mBreakpointColor.ToCOLORREF());
							/*1
							mBrush.DeleteObject();
							mBrush.CreateSolidBrush(mBreakpointBackgroundColor.ToCOLORREF());							
							painter->FillRect(&tmpRect, &mBrush);
							*/
							//painter->FillSolidRect(&tmpRect, RGB(250, 0, 0));
							painter->FillSolidRect(&tmpRect, mBreakpointBackgroundColor.ToCOLORREF());
						}
						else if (bpxtype & bp_hardware) //hardware breakpoint only
						{
							//painter->setPen(mHardwareBreakpointColor);
							//painter->fillRect(QRect(x, y, w, h), QBrush(mHardwareBreakpointBackgroundColor)); //fill red
							painter->SetTextColor(mHardwareBreakpointColor.ToCOLORREF());
							painter->FillSolidRect(&tmpRect, mHardwareBreakpointBackgroundColor.ToCOLORREF());
						}
						else //other cases (memory breakpoint in disassembly) -> do as normal
						{
							Color background;
							if (wIsSelected)
							{
								
								background = mSelectedAddressBackgroundColor;
								//painter->setPen(mSelectedAddressColor); //black address (DisassemblySelectedAddressColor)
								painter->SetTextColor(mSelectedAddressColor.ToCOLORREF());
							}
							else
							{
								background = mAddressBackgroundColor;
								//painter->setPen(mAddressColor);								
								painter->SetTextColor(mAddressColor.ToCOLORREF());
							}
							if (background.GetAlpha())
							{
								
								//painter->fillRect(QRect(x, y, w, h), QBrush(background)); //fill background
								painter->FillSolidRect(&tmpRect, background.ToCOLORREF());
							}
								
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
						//painter->setPen(mLabelColor); //red -> address + label text
						//painter->fillRect(QRect(x, y, w, h), QBrush(mBookmarkBackgroundColor)); //fill label background
						painter->SetTextColor(mLabelColor.ToCOLORREF());
						painter->FillSolidRect(&tmpRect, mBookmarkBackgroundColor.ToCOLORREF());
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
							painter->FillSolidRect(&tmpRect, mBreakpointBackgroundColor.ToCOLORREF());
						}
						else if (bpxtype & bp_hardware) //label + bookmark + hardware breakpoint only
						{
							//painter->fillRect(QRect(x, y, w, h), QBrush(mHardwareBreakpointBackgroundColor)); //fill ?
							painter->FillSolidRect(&tmpRect, mHardwareBreakpointBackgroundColor.ToCOLORREF());
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
						painter->FillSolidRect(&tmpRect, mBookmarkBackgroundColor.ToCOLORREF());
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
							painter->FillSolidRect(&tmpRect, mBreakpointBackgroundColor.ToCOLORREF());
						}
						else if (bpxtype & bp_hardware) //bookmark + hardware breakpoint only
						{
							//painter->fillRect(QRect(x, y, w, h), QBrush(mHardwareBreakpointBackgroundColor)); //fill red
							painter->FillSolidRect(&tmpRect, mHardwareBreakpointBackgroundColor.ToCOLORREF());
						}
						else //other cases (bookmark + memory breakpoint in disassembly) -> do as normal
						{
							//painter->setPen(mBookmarkColor); //black address
							//painter->fillRect(QRect(x, y, w, h), QBrush(mBookmarkBackgroundColor)); //fill bookmark color
							painter->SetTextColor(mBookmarkColor.ToCOLORREF());
							painter->FillSolidRect(&tmpRect, mBookmarkBackgroundColor.ToCOLORREF());
						}
					}
				}
			}
		}
		//painter->drawText(QRect(x + 4, y, w - 4, h), Qt::AlignVCenter | Qt::AlignLeft, addrText);
		CRect textRect = tmpRect;
		textRect.left += 4;
		textRect.right -= 4;
		painter->DrawText(addrText, &textRect, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX); //DT_END_ELLIPSIS
	}
	break;

	case 1: //draw bytes
	{
		//const Instruction_t & instr = mInstBuffer.at(rowOffset);mInstBuffer.FindIndex(rowOffset);
		const Instruction_t & instr = mInstBuffer.GetAt(mInstBuffer.FindIndex(rowOffset));
		//draw functions
		Function_t funcType;
		FUNCTYPE funcFirst = DbgGetFunctionTypeAt(cur_addr);
		FUNCTYPE funcLast = DbgGetFunctionTypeAt(cur_addr + instr.length - 1);
		if (funcLast == FUNC_END && funcFirst != FUNC_SINGLE)
			funcFirst = funcLast;
		switch (funcFirst)
		{
		case FUNC_SINGLE:
			funcType = Function_single;
			break;
		case FUNC_NONE:
			funcType = Function_none;
			break;
		case FUNC_BEGIN:
			funcType = Function_start;
			break;
		case FUNC_MIDDLE:
			funcType = Function_middle;
			break;
		case FUNC_END:
			funcType = Function_end;
			break;
		}
		int funcsize = paintFunctionGraphic(painter, x, y, funcType, false);

		//painter->setPen(mFunctionPen);
		//pen.CreatePen(PS_SOLID, 2, mFunctionPen.ToCOLORREF());
		//painter->SelectObject(&mFunctionPen);
		painter->SetTextColor(mFunctionColor.ToCOLORREF());

		XREFTYPE refType = DbgGetXrefTypeAt(cur_addr);
		CString indicator;
		if (refType == XREF_JMP)
		{
			indicator = ">";
		}
		else if (refType == XREF_CALL)
		{
			indicator = "$";
		}
		else if (funcType != Function_none)
		{
			indicator = ".";
		}
		else
		{
			indicator = " ";
		}

		//int charwidth = getCharWidth();
		//painter->drawText(QRect(x + funcsize, y, charwidth, h), Qt::AlignVCenter | Qt::AlignLeft, indicator);
		
		LOGFONT tmpFt;
		CFont* font = painter->GetCurrentFont();
		int error = GetLastError();
		font->GetLogFont(&tmpFt);
		TEXTMETRIC tm;
		painter->GetTextMetrics(&tm);
		int charwidth = tm.tmAveCharWidth;
		
		CRect textRect = tmpRect;
		textRect.left += funcsize;
		textRect.right = textRect.left + charwidth;
		painter->DrawText(indicator, tmpRect, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
		funcsize += charwidth;

		//draw jump arrows
		Instruction_t::BranchType branchType = mInstBuffer.GetAt(tmpPosition).branchType;
		int jumpsize = paintJumpsGraphic(painter, graphics, x + funcsize, y - 1, wRVA, branchType != Instruction_t::None && branchType != Instruction_t::Call); //jump line

		//draw bytes
		auto richBytes = getRichBytes(instr);
		RichTextPainter::paintRichText(painter, x, y, getColumnWidth(col), getRowHeight(), jumpsize + funcsize, richBytes, &lf);
	}
	break;

	case 2: //draw disassembly (with colours needed)
	{
		int loopsize = 0;
		int depth = 0;

		while (1) //paint all loop depths
		{
			LOOPTYPE loopType = DbgGetLoopTypeAt(cur_addr, depth);
			if (loopType == LOOP_NONE)
				break;
			Function_t funcType;
			switch (loopType)
			{
			case LOOP_SINGLE:
				funcType = Function_single;
				break;
			case LOOP_BEGIN:
				funcType = Function_start;
				break;
			case LOOP_ENTRY:
				funcType = Function_loop_entry;
				break;
			case LOOP_MIDDLE:
				funcType = Function_middle;
				break;
			case LOOP_END:
				funcType = Function_end;
				break;
			default:
				break;
			}
			loopsize += paintFunctionGraphic(painter, x + loopsize, y, funcType, loopType != LOOP_SINGLE);
			depth++;
		}

		RichTextPainter::List richText;
		//auto & token = mInstBuffer[rowOffset].tokens;
		auto & token = mInstBuffer.GetAt(tmpPosition).tokens;
		if (mHighlightToken.text.GetLength())
			CapstoneTokenizer::TokenToRichText(token, richText, &mHighlightToken);
		else
			CapstoneTokenizer::TokenToRichText(token, richText, 0);
		int xinc = 4;
		RichTextPainter::paintRichText(painter, x + loopsize, y, getColumnWidth(col) - loopsize, getRowHeight(), xinc, richText, &lf);
		token.x = x + loopsize + xinc;
	}
	break;

	case 3: //draw comments
	{
		//draw arguments
		Function_t funcType;
		ARGTYPE argFirst = DbgGetArgTypeAt(cur_addr);
		//ARGTYPE argLast = DbgGetArgTypeAt(cur_addr + mInstBuffer.at(rowOffset).length - 1);
		ARGTYPE argLast = DbgGetArgTypeAt(cur_addr + mInstBuffer.GetAt(tmpPosition).length - 1);
		if (argLast == ARG_END && argFirst != ARG_SINGLE)
			argFirst = argLast;
		switch (argFirst)
		{
		case ARG_SINGLE:
			funcType = Function_single;
			break;
		case ARG_NONE:
			funcType = Function_none;
			break;
		case ARG_BEGIN:
			funcType = Function_start;
			break;
		case ARG_MIDDLE:
			funcType = Function_middle;
			break;
		case ARG_END:
			funcType = Function_end;
			break;
		}
		int argsize = funcType == Function_none ? 3 : paintFunctionGraphic(painter, x, y, funcType, false);

		CString comment;
		bool autoComment = false;
		char label[MAX_LABEL_SIZE] = "";
		if (GetCommentFormat(cur_addr, comment, &autoComment))
		{
			Color backgroundColor;
			if (autoComment)
			{
				painter->SetTextColor(mAutoCommentColor.ToCOLORREF());
				backgroundColor = mAutoCommentBackgroundColor;
			}
			else //user comment
			{
				//painter->setPen(mCommentColor);
				painter->SetTextColor(mCommentColor.ToCOLORREF());
				backgroundColor = mCommentBackgroundColor;
			}
			//int width = mFontMetrics->width(comment);
			int width = lf.lfWidth * (comment.GetLength());
			if (width > w)
				width = w;
			if (width)
			{
				//painter->fillRect(QRect(x + argsize, y, width, h), QBrush(backgroundColor)); //fill comment color
				tmpRect.left = x + argsize;
				tmpRect.top = y;
				tmpRect.right = tmpRect.left + width;
				tmpRect.bottom = tmpRect.top + h;
				/*
				mBrush.DeleteObject();
				mBrush.CreateSolidBrush(backgroundColor.ToCOLORREF());
				painter->FillRect(&tmpRect, &mBrush);
				*/
				painter->FillSolidRect(&tmpRect, backgroundColor.ToCOLORREF());
			}				
			//painter->drawText(QRect(x + argsize, y, width, h), Qt::AlignVCenter | Qt::AlignLeft, comment);
			painter->DrawText(comment, tmpRect, DT_VCENTER | DT_NOPREFIX);
			argsize += width + 3;
		}
		else if (DbgGetLabelAt(cur_addr, SEG_DEFAULT, label)) // label but no comment
		{
			CString labelText(CA2T(label, CP_UTF8));
			Color backgroundColor = mLabelBackgroundColor;
			//painter->setPen(mLabelColor);
			painter->SetTextColor(mLabelColor.ToCOLORREF());
		
			//int width = mFontMetrics->width(labelText);
			int width = lf.lfWidth * (labelText.GetLength());
			if (width > w)
				width = w;
			if (width)
			{
				//painter->fillRect(QRect(x + argsize, y, width, h), QBrush(backgroundColor)); //fill comment color
				tmpRect.left = x + argsize;
				tmpRect.top = y;
				tmpRect.right = tmpRect.left + width;
				tmpRect.bottom = tmpRect.top + h;
				painter->FillSolidRect(&tmpRect, backgroundColor.ToCOLORREF());
			}				
			//painter->drawText(QRect(x + argsize, y, width, h), Qt::AlignVCenter | Qt::AlignLeft, labelText);
			painter->DrawText(labelText, tmpRect, DT_VCENTER | DT_NOPREFIX);
			argsize += width + 3;
		}

		if (mShowMnemonicBrief)
		{
			char brief[MAX_STRING_SIZE] = "";
			CString mnem;
			for (const CapstoneTokenizer::SingleToken & token : mInstBuffer.GetAt(tmpPosition).tokens.tokens)
			{
				if (token.type != CapstoneTokenizer::TokenType::Space && token.type != CapstoneTokenizer::TokenType::Prefix)
				{
					mnem = token.text;
					break;
				}
			}
			if (mnem.IsEmpty())
				mnem = mInstBuffer.GetAt(tmpPosition).instStr;

			int index = mnem.Find(' ');
			if (index != -1)
				mnem.Left(index);
			//DbgFunctions()->GetMnemonicBrief(mnem.toUtf8().constData(), MAX_STRING_SIZE, brief);
			string str;
			str = CT2A(mnem.GetBuffer());
			DbgFunctions()->GetMnemonicBrief(str.data(), MAX_STRING_SIZE, brief);
			
			//painter->setPen(mMnemonicBriefColor);
			painter->SetTextColor(mMnemonicBriefColor.ToCOLORREF());

			CString mnemBrief(CA2T(brief, CP_UTF8));
			if (mnemBrief.GetLength())
			{
				int width = lf.lfWidth * (mnemBrief.GetLength());
				if (width > w)
					width = w;
				if (width)
				{
					//painter->fillRect(QRect(x + argsize, y, width, h), QBrush(mMnemonicBriefBackgroundColor)); //mnemonic brief background color
					tmpRect.left = x + argsize;
					tmpRect.top = y;
					tmpRect.right = tmpRect.left + width;
					tmpRect.bottom = tmpRect.top + h;
					painter->FillSolidRect(&tmpRect, mMnemonicBriefBackgroundColor.ToCOLORREF());
				}					
				//painter->drawText(QRect(x + argsize, y, width, h), Qt::AlignVCenter | Qt::AlignLeft, mnemBrief);
				painter->DrawText(mnemBrief, tmpRect, DT_VCENTER | DT_NOPREFIX);
			}
			break;
		}
	}
	break;
	}
	return _T("");
}

/************************************************************************************
ScrollBar Management
***********************************************************************************/
/**
* @brief       This method has been reimplemented. It realigns the slider on real instructions except
*              when the type is QAbstractSlider::SliderNoAction. This type (QAbstractSlider::SliderNoAction)
*              is used to force the disassembling at a specific RVA.
*
* @param[in]   type      Type of action
* @param[in]   value     Old table offset
* @param[in]   delta     Scrollbar value delta compared to the previous state
*
* @return      Return the value of the new table offset.
*/
dsint CDisassembly::sliderMovedHook(int type, dsint value, dsint delta)
{
	//ShowDisassemblyPopup(0, 0, 0);

	// QAbstractSlider::SliderNoAction is used to disassembe at a specific address
	//if (type == QAbstractSlider::SliderNoAction)
	//	return value + delta;

	// If it's a slider action, disassemble one instruction back and one instruction next in order to be aligned on a real instruction
	if (type == SB_THUMBTRACK)
	{
		dsint wNewValue = 0;

		if (value + delta > 0)
		{
			wNewValue = getInstructionRVA(value + delta, -1);
			wNewValue = getInstructionRVA(wNewValue, 1);
		}

		return wNewValue;
	}

	// For other actions, disassemble according to the delta
	return getInstructionRVA(value, delta);
}

/************************************************************************************
Jumps Graphic
************************************************************************************/
/**
* @brief       This method paints the graphic for jumps.
*
* @param[in]   painter     Pointer to the painter that allows painting by its own
* @param[in]   x           Rectangle x
* @param[in]   y           Rectangle y
* @param[in]   addr        RVA of address to process
*
* @return      Nothing.
*/
int CDisassembly::paintJumpsGraphic(CDC* painter, Graphics& graphics, int x, int y, dsint addr, bool isjmp)
{
	dsint selHeadRVA = mSelection.fromIndex;
	dsint rva = addr;
	duint curVa = rvaToVa(addr);
	duint selVa = rvaToVa(mSelection.firstSelectedIndex);
	Instruction_t instruction = DisassembleAt(selHeadRVA);
	auto branchType = instruction.branchType;

	bool showXref = false;

	GraphicDump wPict = GD_Nothing;
	//Gdiplus::Graphics graphics(painter->m_hDC);

	if (branchType != Instruction_t::None && branchType != Instruction_t::Call)
	{
		dsint base = mMemPage->getBase();
		dsint destVA = DbgGetBranchDestination(rvaToVa(selHeadRVA));

		if (destVA >= base && destVA < base + (dsint)mMemPage->getSize())
		{
			destVA -= base;

			if (destVA < selHeadRVA)
			{
				if (rva == destVA)
					wPict = GD_HeadFromBottom;
				else if (rva > destVA && rva < selHeadRVA)
					wPict = GD_Vert;
				else if (rva == selHeadRVA)
					wPict = GD_FootToTop;
			}
			else if (destVA > selHeadRVA)
			{
				if (rva == selHeadRVA)
					wPict = GD_FootToBottom;
				else if (rva > selHeadRVA && rva < destVA)
					wPict = GD_Vert;
				else if (rva == destVA)
					wPict = GD_HeadFromTop;
			}
		}
	}
	else if (mXrefInfo.refcount > 0)
	{
		duint max = selVa, min = selVa;
		showXref = true;
		int jmpcount = 0;
		for (duint i = 0; i < mXrefInfo.refcount; i++)
		{
			if (mXrefInfo.references[i].type != XREF_JMP)
				continue;
			jmpcount++;
			if (curVa == mXrefInfo.references[i].addr)
				wPict = GD_VertHori;
			if (mXrefInfo.references[i].addr > max)
				max = mXrefInfo.references[i].addr;
			if (mXrefInfo.references[i].addr < min)
				min = mXrefInfo.references[i].addr;
		}
		if (jmpcount)
		{
			if (curVa == selVa)
			{
				if (max == selVa)
				{
					wPict = GD_HeadFromTop;
				}
				else if (min == selVa)
				{
					wPict = GD_HeadFromBottom;
				}
				else if (max > selVa && min < selVa)
				{
					wPict = GD_HeadFromBoth;
				}

			}
			else if (curVa < selVa && curVa == min)
			{
				wPict = GD_FootToBottom;
			}
			else if (curVa > selVa && curVa == max)
			{
				wPict = GD_FootToTop;
			}
			if (wPict == GD_Nothing && curVa > min && curVa < max)
				wPict = GD_Vert;
		}
	}

	GraphicJumpDirection curInstDir = GJD_Nothing;

	if (isjmp)
	{
		duint curInstDestination = DbgGetBranchDestination(curVa);
		if (curInstDestination == 0 || curVa == curInstDestination)
		{
			curInstDir = GJD_Nothing;
		}
		else if (curInstDestination < curVa)
		{
			curInstDir = GJD_Up;
		}
		else
		{
			curInstDir = GJD_Down;
		}
	}

	int halfRow = getRowHeight() / 2 + 1;

	//painter->setPen(mConditionalTruePen);
	CPen pen;
	//pen.CreatePen(PS_SOLID, 2, mConditionalTruePen.ToCOLORREF());
	painter->SelectObject(&mConditionalTruePen);
	// 
	BYTE types[] = {
		PT_MOVETO,  // 移动绘制点到第一个坐标
		PT_LINETO,  // 画线，从第一个坐标到第二个坐标
		PT_LINETO,  // 画线，从第二个坐标到第三个坐标
	};
	if (curInstDir == GJD_Up)
	{
		// 创建4个坐标，其实是三个点，第一个和第四重合，是为了使三角形闭合。
		POINT wPoints[] =
		{
			{ x, y + halfRow + 1 },
			{ x + 2, y + halfRow - 1 },
			{ x + 4, y + halfRow + 1 },
		};
		Point point1(x, y + halfRow + 1);
		Point point2(x + 2, y + halfRow - 1);
		Point point3(x + 4, y + halfRow + 1);
		Point curvePoints[] =
		{
			point1,
			point2,
			point3
		};
		Point* p = curvePoints;
		// 绘制
		//painter->PolyDraw(wPoints, types, 3);
		//painter->Polyline(wPoints, 3);
		Pen  redPen(mUnconditionalJumpLineColor, 1);
		//graphics.DrawPolygon(&redPen, p, 3);
		graphics.DrawLines(&redPen, curvePoints, 3);
	}
	else if (curInstDir == GJD_Down)
	{
		POINT wPoints[] =
		{
			{ x, y + halfRow - 1 },
			{ x + 2, y + halfRow + 1 },
			{ x + 5, y + halfRow - 2 },
		};
		Point point1(x, y + halfRow - 1);
		Point point2(x + 2, y + halfRow + 1);
		Point point3(x + 4, y + halfRow - 1);
		Point curvePoints[] =
		{
			point1,
			point2,
			point3
		};
		// 绘制
		//painter->PolyDraw(wPoints, types, 3);
		Pen  redPen(mUnconditionalJumpLineColor, 1);
		graphics.DrawLines(&redPen, curvePoints, 3);
	}

	x += 8;

	if (showXref)
	{
		//pen.CreatePen(PS_SOLID, 2, mUnconditionalPen.ToCOLORREF());
		painter->SelectObject(&mUnconditionalPen);
	}
	else
	{
		bool bIsExecute = DbgIsJumpGoingToExecute(rvaToVa(instruction.rva));


		if (branchType == Instruction_t::Unconditional) //unconditional
		{
			//painter->setPen(mUnconditionalPen);
			//pen.CreatePen(PS_SOLID, 2, mUnconditionalPen.ToCOLORREF());
			painter->SelectObject(&mUnconditionalPen);

		}
		else
		{
			if (bIsExecute)
			{
				//painter->setPen(mConditionalTruePen);
				//pen.CreatePen(PS_SOLID, 2, mConditionalTruePen.ToCOLORREF());
				painter->SelectObject(&mConditionalTruePen);
			}
			else
			{
				//painter->setPen(mConditionalFalsePen);
				//pen.CreatePen(PS_SOLID, 2, mConditionalFalsePen.ToCOLORREF());
				painter->SelectObject(&mConditionalFalsePen);
			}
		}
	}

	if (wPict == GD_Vert)
	{
		//painter->drawLine(x, y, x, y + getRowHeight());
		painter->MoveTo(x, y);
		painter->LineTo(x, y + getRowHeight());
	}
	else if (wPict == GD_FootToBottom)
	{
		//painter->drawLine(x, y + halfRow, x + 5, y + halfRow);
		painter->MoveTo(x, y + halfRow);
		painter->LineTo(x + 5, y + halfRow);
		//painter->drawLine(x, y + halfRow, x, y + getRowHeight());
		painter->MoveTo(x, y + halfRow);
		painter->LineTo(x, y + getRowHeight());
	}
	else if (wPict == GD_FootToTop)
	{
		//painter->drawLine(x, y + halfRow, x + 5, y + halfRow);
		painter->MoveTo(x, y + halfRow);
		painter->LineTo(x + 5, y + halfRow);
		//painter->drawLine(x, y, x, y + halfRow);
		painter->MoveTo(x, y);
		painter->LineTo(x, y + halfRow);
	}
	else if (wPict == GD_HeadFromBottom)
	{
		POINT wPoints[] =
		{
			{ x + 3, y + halfRow - 2 },
			{ x + 5, y + halfRow },
			{ x + 3, y + halfRow + 2 },
		};
		
		Point Point01(x + 5, y + halfRow);
		Point Point02(x, y + halfRow);
		Point Point03(x, y + getRowHeight());
		Point Points[] =
		{
			Point01,
			Point02,
			Point03
		};

		Point point1(x + 3, y + halfRow - 2);
		Point point2(x + 5, y + halfRow);
		Point point3(x + 3, y + halfRow + 2);
		Point curvePoints[] =
		{
			point1,
			point2,
			point3
		};
		// GDI+绘制		
		Pen  redPen(mUnconditionalJumpLineColor, 1);
		graphics.DrawLines(&redPen, Points, 3);
		graphics.DrawLines(&redPen, curvePoints, 3);
		/*
		//painter->drawLine(x, y + halfRow, x + 5, y + halfRow);
		painter->MoveTo(x, y + halfRow);
		painter->LineTo(x + 5, y + halfRow);
		//painter->drawLine(x, y + halfRow, x, y + getRowHeight());
		painter->MoveTo(x, y + halfRow);
		painter->LineTo(x, y + getRowHeight());
		painter->Polyline(wPoints, 3);
		*/
	}
	else if (wPict == GD_HeadFromTop)
	{
		POINT wPoints[] =
		{
			{ x + 3, y + halfRow - 2 },
			{ x + 5, y + halfRow },
			{ x + 3, y + halfRow + 2 },
		};

		Point Point01(x, y);
		Point Point02(x, y + halfRow);
		Point Point03(x + 5, y + halfRow);
		Point Points[] =
		{
			Point01,
			Point02,
			Point03
		};

		Point point1(x + 3, y + halfRow - 2);
		Point point2(x + 5, y + halfRow);
		Point point3(x + 3, y + halfRow + 2);
		Point curvePoints[] =
		{
			point1,
			point2,
			point3
		};
		// GDI+绘制		
		Pen  redPen(mUnconditionalJumpLineColor, 1);
		graphics.DrawLines(&redPen, Points, 3);
		graphics.DrawLines(&redPen, curvePoints, 3);
		/*
		//painter->drawLine(x, y + halfRow, x + 5, y + halfRow);
		painter->MoveTo(x, y + halfRow);
		painter->LineTo(x + 5, y + halfRow);
		//painter->drawLine(x, y, x, y + halfRow);
		painter->MoveTo(x, y);
		painter->LineTo(x, y + halfRow);
		painter->Polyline(wPoints, 3);
		*/
	}
	else if (wPict == GD_HeadFromBoth)
	{
		POINT wPoints[] =
		{
			{ x + 3, y + halfRow - 2 },
			{ x + 5, y + halfRow },
			{ x + 3, y + halfRow + 2 },
		};

		//painter->drawLine(x, y + halfRow, x + 5, y + halfRow);
		painter->MoveTo(x, y + halfRow);
		painter->LineTo(x + 5, y + halfRow);
		//painter->drawLine(x, y, x, y + getRowHeight());
		painter->MoveTo(x, y);
		painter->LineTo(x, y + getRowHeight());
		painter->Polyline(wPoints, 3);
	}
	else if (wPict == GD_VertHori)
	{
		//painter->drawLine(x, y + halfRow, x + 5, y + halfRow);
		painter->MoveTo(x, y + halfRow);
		painter->LineTo(x + 5, y + halfRow);
		//painter->drawLine(x, y, x, y + getRowHeight());
		painter->MoveTo(x, y);
		painter->LineTo(x, y + getRowHeight());
	}
	return 15;
}

/************************************************************************************
Function Graphic
************************************************************************************/
/**
* @brief       This method paints the graphic for functions/loops.
*
* @param[in]   painter     Pointer to the painter that allows painting by its own
* @param[in]   x           Rectangle x
* @param[in]   y           Rectangle y
* @param[in]   funcType    Type of drawing to make
*
* @return      Width of the painted data.
*/

int CDisassembly::paintFunctionGraphic(CDC* painter, int x, int y, Function_t funcType, bool loop)
{
	//Graphics gdi(GetDC()->m_hDC);
	//Graphics graphics(painter->m_hDC);
	CPen pen;
	if (loop && funcType == Function_none)
		return 0;
	if (loop)
	{
		//painter->setPen(mLoopPen); //thick black line
		//pen.CreatePen(PS_SOLID, 2, mLoopPen.ToCOLORREF());
		painter->SelectObject(&mLoopPen);
	}
	else
	{
		//painter->setPen(mFunctionPen); //thick black line
		//pen.CreatePen(PS_SOLID, 2, mFunctionPen.ToCOLORREF());
		painter->SelectObject(&mFunctionPen);
	}
		
	int height = getRowHeight();
	int x_add = 5;
	int y_add = 4;
	int end_add = 2;
	int line_width = 3;
	if (loop)
	{
		end_add = -1;
		x_add = 4;
	}
	switch (funcType)
	{
	case Function_single:
	{
		if (loop)
			y_add = height / 2 + 1;
		//painter->drawLine(x + x_add + line_width, y + y_add, x + x_add, y + y_add);
		painter->MoveTo(x + x_add + line_width, y + y_add);
		painter->LineTo(x + x_add, y + y_add);
		//painter->drawLine(x + x_add, y + y_add, x + x_add, y + height - y_add - 1);
		painter->MoveTo(x + x_add , y + y_add);
		painter->LineTo(x + x_add, y + height - y_add - 1);
		if (loop)
			y_add = height / 2 - 1;
		//painter->drawLine(x + x_add, y + height - y_add, x + x_add + line_width, y + height - y_add);
		painter->MoveTo(x + x_add, y + height - y_add);
		painter->LineTo(x + x_add + line_width, y + height - y_add);
	}
	break;

	case Function_start:
	{
		if (loop)
			y_add = height / 2 + 1;
		//painter->drawLine(x + x_add + line_width, y + y_add, x + x_add, y + y_add);
		painter->MoveTo(x + x_add + line_width, y + y_add);
		painter->LineTo(x + x_add, y + y_add);
		//painter->drawLine(x + x_add, y + y_add, x + x_add, y + height);
		painter->MoveTo(x + x_add, y + y_add);
		painter->LineTo(x + x_add, y + height);
	}
	break;

	case Function_middle:
	{
		//painter->drawLine(x + x_add, y, x + x_add, y + height);
		painter->MoveTo(x + x_add, y);
		painter->LineTo(x + x_add, y + height);
	}
	break;

	case Function_loop_entry:
	{
		int trisize = 2;
		int y_start = (height - trisize * 2) / 2 + y;
		//painter->drawLine(x + x_add, y_start, x + trisize + x_add, y_start + trisize);
		painter->MoveTo(x + x_add, y_start);
		painter->LineTo(x + trisize + x_add, y_start + trisize);
		//painter->drawLine(x + trisize + x_add, y_start + trisize, x + x_add, y_start + trisize * 2);
		painter->MoveTo(x + trisize + x_add, y_start + trisize);
		painter->LineTo(x + x_add, y_start + trisize * 2);

		//painter->drawLine(x + x_add, y, x + x_add, y_start - 1);
		painter->MoveTo(x + x_add, y);
		painter->LineTo(x + x_add, y_start - 1);
		//painter->drawLine(x + x_add, y_start + trisize * 2 + 2, x + x_add, y + height);
		painter->MoveTo(x + x_add, y_start + trisize * 2 + 2);
		painter->LineTo(x + x_add, y + height);
	}
	break;

	case Function_end:
	{
		if (loop)
			y_add = height / 2 - 1;
		//painter->drawLine(x + x_add, y, x + x_add, y + height - y_add);
		painter->MoveTo(x + x_add, y);
		painter->LineTo(x + x_add, y + height - y_add);
		//painter->drawLine(x + x_add, y + height - y_add, x + x_add + line_width, y + height - y_add);
		painter->MoveTo(x + x_add, y + height - y_add);
		painter->LineTo(x + x_add + line_width, y + height - y_add);
	}
	break;

	case Function_none:
	{

	}
	break;
	}
	return x_add + line_width + end_add;
}


/************************************************************************************
							Instructions Management
***********************************************************************************/
/**
* @brief       Disassembles the instruction at the given RVA.
*
* @param[in]   rva     RVA of instruction to disassemble
*
* @return      Return the disassembled instruction.
*/
Instruction_t CDisassembly::DisassembleAt(dsint rva)
{
	if (mMemPage->getSize() < (duint)rva)
		return Instruction_t();

	CByteArray wBuffer;
	duint base = mMemPage->getBase();
	duint wMaxByteCountToRead = 16 * 2;

	// Bounding
	auto size = getSize();
	if (!size)
		size = rva + wMaxByteCountToRead * 2;

	if (mCodeFoldingManager)
		wMaxByteCountToRead += mCodeFoldingManager->getFoldedSize(rvaToVa(rva), rvaToVa(rva + wMaxByteCountToRead));

	wMaxByteCountToRead = wMaxByteCountToRead > (size - rva) ? (size - rva) : wMaxByteCountToRead;
	if (!wMaxByteCountToRead)
		return Instruction_t();

	wBuffer.SetSize(wMaxByteCountToRead);

	if (!mMemPage->read((BYTE*)wBuffer.GetData(), rva, wBuffer.GetSize()))
		return Instruction_t();

	return mDisasm->DisassembleAt((byte_t*)wBuffer.GetData(), wBuffer.GetSize(), base, rva);

	/* Zydis<->Capstone diff logic.
	* TODO: Remove once transition is completed.

	auto zy_instr = mDisasm->DisassembleAt((byte_t*)wBuffer.data(), wBuffer.size(), base, rva);
	auto cs_instr = mCsDisasm->DisassembleAt((byte_t*)wBuffer.data(), wBuffer.size(), base, rva);

	if(zy_instr.tokens.tokens != cs_instr.tokens.tokens)
	{
	if(zy_instr.instStr.startsWith("lea"))  // cs scales lea mem op incorrectly
	goto _exit;
	if(cs_instr.instStr.startsWith("movabs"))  // cs uses non-standard movabs mnem
	goto _exit;
	if(cs_instr.instStr.startsWith("lock") || cs_instr.instStr.startsWith("rep"))  // cs includes prefix in mnem
	goto _exit;
	if(cs_instr.instStr.startsWith('j') && cs_instr.length == 4)  // cs has AMD style handling of 66 branches
	goto _exit;
	if(cs_instr.instStr.startsWith("prefetchw"))  // cs uses m8 (AMD/intel doc), zy m512
	goto _exit;                               // (doesn't matter, prefetch doesn't really have a size)
	if(cs_instr.instStr.startsWith("xchg"))  // cs/zy print operands in different order (doesn't make any diff)
	goto _exit;
	if(cs_instr.instStr.startsWith("rdpmc") ||
	cs_instr.instStr.startsWith("in") ||
	cs_instr.instStr.startsWith("out") ||
	cs_instr.instStr.startsWith("sti") ||
	cs_instr.instStr.startsWith("cli") ||
	cs_instr.instStr.startsWith("iret")) // cs assumes priviliged, zydis doesn't (CPL is configurable for those)
	goto _exit;
	if(cs_instr.instStr.startsWith("sal"))  // cs says sal, zydis say shl (both correct)
	goto _exit;
	if(cs_instr.instStr.startsWith("xlat"))  // cs uses xlatb form, zydis xlat m8 form (both correct)
	goto _exit;
	if(cs_instr.instStr.startsWith("lcall") ||
	cs_instr.instStr.startsWith("ljmp") ||
	cs_instr.instStr.startsWith("retf")) // cs uses "f" mnem-suffic, zydis has seperate "far" token
	goto _exit;
	if(cs_instr.instStr.startsWith("movsxd"))  // cs has wrong operand size (32) for 0x63 variant (e.g. "63646566")
	goto _exit;
	if(cs_instr.instStr.startsWith('j') && (cs_instr.dump[0] & 0x40) == 0x40)  // cs honors rex.w on jumps, truncating the
	goto _exit;                                                         // target address to 32 bit (must be ignored)
	if(cs_instr.instStr.startsWith("enter"))  // cs has wrong operand size (32)
	goto _exit;
	if(cs_instr.instStr.startsWith("wait"))  // cs says wait, zy says fwait (both ok)
	goto _exit;
	if(cs_instr.dump.length() > 2 &&   // cs ignores segment prefixes if followed by branch hints
	(cs_instr.dump[1] == '\x2e' ||
	cs_instr.dump[2] == '\x3e'))
	goto _exit;
	if(QRegExp("mov .s,.*").exactMatch(cs_instr.instStr) ||
	cs_instr.instStr.startsWith("str") ||
	QRegExp("pop .s").exactMatch(cs_instr.instStr)) // cs claims it's priviliged (it's not)
	goto _exit;
	if(QRegExp("l[defgs]s.*").exactMatch(cs_instr.instStr))  // cs allows LES (and friends) in 64 bit mode (invalid)
	goto _exit;
	if(QRegExp("f[^ ]+ st0.*").exactMatch(zy_instr.instStr))  // zy prints excplitic st0, cs omits (both ok)
	goto _exit;
	if(cs_instr.instStr.startsWith("fstp"))  // CS reports 3 operands but only prints 2 ... wat.
	goto _exit;
	if(cs_instr.instStr.startsWith("fnstsw"))  // CS reports wrong 32 bit operand size (is 16)
	goto _exit;
	if(cs_instr.instStr.startsWith("popaw")) // CS prints popaw, zydis popa (both ok)
	goto _exit;
	if(cs_instr.instStr.startsWith("lsl")) // CS thinks the 2. operand is 32 bit (it's 16)
	goto _exit;
	if(QRegExp("mov [cd]r\\d").exactMatch(cs_instr.instStr)) // CS fails to reject bad DR/CRs (that #UD, like dr4)
	goto _exit;
	if(QRegExp("v?comi(ps|pd|ss|sd).*").exactMatch(zy_instr.instStr)) // CS has wrong operand size
	goto _exit;
	if(QRegExp("v?cmp(ps|pd|ss|sd).*").exactMatch(zy_instr.instStr)) // CS uses pseudo-op notation, Zy prints cond as imm (both ok)
	goto _exit;
	if(cs_instr.dump.length() > 2 &&
	cs_instr.dump[0] == '\x0f' &&
	(cs_instr.dump[1] == '\x1a' || cs_instr.dump[1] == '\x1b')) // CS doesn't support MPX
	goto _exit;

	auto insn_hex = cs_instr.dump.toHex().toStdString();
	auto cs = cs_instr.instStr.toStdString();
	auto zy = zy_instr.instStr.toStdString();

	for(auto zy_it = zy_instr.tokens.tokens.begin(), cs_it = cs_instr.tokens.tokens.begin()
	; zy_it != zy_instr.tokens.tokens.end() && cs_it != cs_instr.tokens.tokens.end()
	; ++zy_it, ++cs_it)
	{
	Zydis zd;
	zd.Disassemble(0, (unsigned char*)zy_instr.dump.data(), zy_instr.length);

	auto zy_tok_text = zy_it->text.toStdString();
	auto cs_tok_text = cs_it->text.toStdString();

	if(zy_tok_text == "bnd")  // cs doesn't support BND prefix
	goto _exit;
	if(zy_it->value.size != cs_it->value.size)  // imm sizes in CS are completely broken
	goto _exit;

	if(!(*zy_it == *cs_it))
	__debugbreak();
	}

	//__debugbreak();
	}

	_exit:
	return zy_instr;
	*/
}

/**
* @brief       Disassembles the instruction count instruction afterc the instruction at the given RVA.
*              Count can be positive or negative.
*
* @param[in]   rva     RVA of reference instruction
* @param[in]   count   Number of instruction
*
* @return      Return the disassembled instruction.
*/
Instruction_t CDisassembly::DisassembleAt(dsint rva, dsint count)
{
	rva = getNextInstructionRVA(rva, count);
	return DisassembleAt(rva);
}
/**
* @brief       Returns the RVA of count-th instructions before the given instruction RVA.
*
* @param[in]   rva         Instruction RVA
* @param[in]   count       Instruction count
*
* @return      RVA of count-th instructions before the given instruction RVA.
*/
dsint CDisassembly::getPreviousInstructionRVA(dsint rva, duint count)
{
	CByteArray wBuffer;
	dsint wBottomByteRealRVA;
	dsint wVirtualRVA;
	dsint wMaxByteCountToRead;

	wBottomByteRealRVA = (dsint)rva - 16 * (count + 3);
	if (mCodeFoldingManager)
	{
		if (mCodeFoldingManager->isFolded(rvaToVa(wBottomByteRealRVA)))
		{
			wBottomByteRealRVA = mCodeFoldingManager->getFoldBegin(wBottomByteRealRVA) - mMemPage->getBase() - 16 * (count + 3);
		}
	}
	wBottomByteRealRVA = wBottomByteRealRVA < 0 ? 0 : wBottomByteRealRVA;

	wVirtualRVA = (dsint)rva - wBottomByteRealRVA;

	wMaxByteCountToRead = wVirtualRVA + 1 + 16;
	wBuffer.SetSize(wMaxByteCountToRead);

	mMemPage->read(wBuffer.GetData(), wBottomByteRealRVA, wBuffer.GetSize());

	dsint addr = mDisasm->DisassembleBack((byte_t*)wBuffer.GetData(), rvaToVa(wBottomByteRealRVA), wBuffer.GetSize(), wVirtualRVA, count);

	addr += rva - wVirtualRVA;

	return addr;
}

/**
* @brief       Returns the RVA of count-th instructions before/after (depending on the sign) the given instruction RVA.
*
* @param[in]   rva         Instruction RVA
* @param[in]   count       Instruction count
*
* @return      RVA of count-th instructions before/after the given instruction RVA.
*/
dsint CDisassembly::getInstructionRVA(dsint index, dsint count)
{
	dsint wAddr = 0;

	if (count == 0)
		wAddr = index;
	if (count < 0)
		wAddr = getPreviousInstructionRVA(index, abs(count));
	else if (count > 0)
		wAddr = getNextInstructionRVA(index, abs(count));


	if (wAddr < 0)
		wAddr = 0;
	else if (wAddr > getRowCount() - 1)
		wAddr = getRowCount() - 1;

	return wAddr;
}

/**
* @brief       Returns the RVA of count-th instructions after the given instruction RVA.
*
* @param[in]   rva         Instruction RVA
* @param[in]   count       Instruction count
* @param[in]   isGlobal    Whether it rejects rva beyond current page
*
* @return      RVA of count-th instructions after the given instruction RVA.
*/
dsint CDisassembly::getNextInstructionRVA(dsint rva, duint count, bool isGlobal)
{
	CByteArray wBuffer;
	dsint wRemainingBytes;
	dsint wMaxByteCountToRead;
	dsint wNewRVA;

	if (!isGlobal)
	{
		if (mMemPage->getSize() < (duint)rva)
			return rva;
		wRemainingBytes = mMemPage->getSize() - rva;

		wMaxByteCountToRead = 16 * (count + 1);
		if (mCodeFoldingManager)
			wMaxByteCountToRead += mCodeFoldingManager->getFoldedSize(rvaToVa(rva), rvaToVa(rva + wMaxByteCountToRead));
		wMaxByteCountToRead = wRemainingBytes > wMaxByteCountToRead ? wMaxByteCountToRead : wRemainingBytes;
	}
	else
	{
		wMaxByteCountToRead = 16 * (count + 1);
		if (mCodeFoldingManager)
			wMaxByteCountToRead += mCodeFoldingManager->getFoldedSize(rvaToVa(rva), rvaToVa(rva + wMaxByteCountToRead));
	}
	//wBuffer.resize(wMaxByteCountToRead);
	wBuffer.SetSize(wMaxByteCountToRead);

	mMemPage->read((BYTE*)wBuffer.GetData(), rva, wBuffer.GetSize());

	wNewRVA = mDisasm->DisassembleNext((byte_t*)wBuffer.GetData(), rvaToVa(rva), wBuffer.GetSize(), 0, count);

	wNewRVA += rva;

	return wNewRVA;
}










/************************************************************************************
Selection Management
************************************************************************************/
void CDisassembly::expandSelectionUpTo(dsint to)
{
	if (to < mSelection.firstSelectedIndex)
	{
		mSelection.fromIndex = to;
	}
	else if (to > mSelection.firstSelectedIndex)
	{
		mSelection.toIndex = to;
	}
	else if (to == mSelection.firstSelectedIndex)
	{
		setSingleSelection(to);
	}
}

void CDisassembly::selectionChangedSlot(dsint Va)
{
	CDisasmPane* pParentPane = (CDisasmPane*)GetParent();
	auto pCPUSideBar = pParentPane->getCCPUSideBar();
	pCPUSideBar->setSelection(Va);

	if (mXrefInfo.refcount != 0)
	{
		BridgeFree(mXrefInfo.references);
		mXrefInfo.refcount = 0;
	}
	if (DbgIsDebugging())
		DbgXrefGet(Va, &mXrefInfo);
}

dsint CDisassembly::getInitialSelection() const
{
	return mSelection.firstSelectedIndex;
}

void CDisassembly::setSingleSelection(dsint index)
{
	mSelection.firstSelectedIndex = index;
	mSelection.fromIndex = index;
	mSelection.toIndex = getInstructionRVA(mSelection.fromIndex, 1) - 1;
	//emit selectionChanged(rvaToVa(index));
	duint value = rvaToVa(index);
	selectionChangedSlot(value);
	CWnd* pMainWnd = AfxGetMainWnd();
	if (pMainWnd->GetSafeHwnd())
		pMainWnd->PostMessage(WM_USER + Signal_SelectionChange_Msg, 0, LPARAM(value));
}

dsint CDisassembly::getSelectionStart() const
{
	return mSelection.fromIndex;
}

dsint CDisassembly::getSelectionEnd() const
{
	return mSelection.toIndex;
}

RichTextPainter::List CDisassembly::getRichBytes(const Instruction_t & instr) const
{
	RichTextPainter::List richBytes;
	std::vector<std::pair<size_t, bool>> realBytes;
	formatOpcodeString(instr, richBytes, realBytes);
	dsint cur_addr = rvaToVa(instr.rva);

	if (!richBytes.empty())
	{
		int index = richBytes.back().text.Find(' ');
		if (index == (richBytes.back().text.GetLength() - 1))
			//richBytes.back().text.chop(1); //remove trailing space if exists
			richBytes.back().text.Delete(index);
	}		

	for (size_t i = 0; i < richBytes.size(); i++)
	{
		auto byteIdx = realBytes[i].first;
		auto isReal = realBytes[i].second;
		RichTextPainter::CustomRichText_t & curByte = richBytes.at(i);
		DBGRELOCATIONINFO relocInfo;
		curByte.highlightColor = mDisassemblyRelocationUnderlineColor;
		if (DbgFunctions()->ModRelocationAtAddr(cur_addr + byteIdx, &relocInfo))
		{
			bool prevInSameReloc = relocInfo.rva < cur_addr + byteIdx - DbgFunctions()->ModBaseFromAddr(cur_addr + byteIdx);
			curByte.highlight = isReal;
			curByte.highlightConnectPrev = i > 0 && prevInSameReloc;
		}
		else
		{
			curByte.highlight = false;
			curByte.highlightConnectPrev = false;
		}

		DBGPATCHINFO patchInfo;
		if (isReal && DbgFunctions()->PatchGetEx(cur_addr + byteIdx, &patchInfo))
		{
			if ((unsigned char)(instr.dump.GetAt(byteIdx)) == patchInfo.newbyte)
			{
				curByte.textColor = mModifiedBytesColor;
				curByte.textBackground = mModifiedBytesBackgroundColor;
			}
			else
			{
				curByte.textColor = mRestoredBytesColor;
				curByte.textBackground = mRestoredBytesBackgroundColor;
			}
		}
		else
		{
			curByte.textColor = mBytesColor;
			curByte.textBackground = mBytesBackgroundColor;
		}
	}

	if (mCodeFoldingManager && mCodeFoldingManager->isFolded(cur_addr))
	{
		RichTextPainter::CustomRichText_t curByte;
		curByte.textColor = mBytesColor;
		curByte.textBackground = mBytesBackgroundColor;
		curByte.highlightColor = mDisassemblyRelocationUnderlineColor;
		curByte.highlightWidth = 1;
		curByte.flags = RichTextPainter::FlagAll;
		curByte.highlight = false;
		curByte.textColor = mBytesColor;
		curByte.textBackground = mBytesBackgroundColor;
		curByte.text = "...";
		richBytes.push_back(curByte);
	}
	return richBytes;
}

void CDisassembly::prepareDataRange(dsint startRva, dsint endRva, const std::function<bool(int, const Instruction_t &)> & disassembled)
{
	dsint wAddrPrev = startRva;
	dsint wAddr = wAddrPrev;

	int i = 0;
	while (true)
	{
		if (wAddr > endRva)
			break;
		wAddrPrev = wAddr;
		auto wInst = DisassembleAt(wAddr);
		wAddr = getNextInstructionRVA(wAddr, 1);
		if (wAddr == wAddrPrev)
			break;
		if (!disassembled(i++, wInst))
			break;
	}
}

/************************************************************************************
Update/Reload/Refresh/Repaint
************************************************************************************/
void CDisassembly::prepareData()
{
	dsint wViewableRowsCount = getViewableRowsCount();
	mInstBuffer.RemoveAll();
	//mInstBuffer.reserve(wViewableRowsCount);

	dsint wAddrPrev = getTableOffset();
	dsint wAddr = wAddrPrev;
	Instruction_t wInst;

	int wCount = 0;

	for (int wI = 0; wI < wViewableRowsCount && getRowCount() > 0; wI++)
	{
		wAddrPrev = wAddr;
		wInst = DisassembleAt(wAddr);
		wAddr = getNextInstructionRVA(wAddr, 1);
		if (wAddr == wAddrPrev)
			break;
		//mInstBuffer.append(wInst);
		mInstBuffer.AddTail(wInst);
		wCount++;
	}

	setNbrOfLineToPrint(wCount);
}

void CDisassembly::reloadData()
{
	//emit selectionChanged(rvaToVa(mSelection.firstSelectedIndex));
	selectionChangedSlot(rvaToVa(mSelection.firstSelectedIndex));

	CDisasmPane* pParentPane = (CDisasmPane*)GetParent();
	pParentPane->getCCPUSideBar()->changeTopmostAddress(getTableOffset());

	CAbstractTableView::reloadData();
}

void CDisassembly::setTableOffset(dsint val)
{
	CDisasmPane* pParentPane = (CDisasmPane*)GetParent();
	pParentPane->getCCPUSideBar()->changeTopmostAddress(val);
	CAbstractTableView::setTableOffset(val);
}

/************************************************************************************
Public Methods
************************************************************************************/
duint CDisassembly::rvaToVa(dsint rva) const
{
	return mMemPage->va(rva);
}

void CDisassembly::disassembleAt(dsint parVA, dsint parCIP, bool history, dsint newTableOffset)
{
	duint wSize;
	auto wBase = DbgMemFindBaseAddr(parVA, &wSize);

	unsigned char test;
	if (!wBase || !wSize || !DbgMemRead(parVA, &test, sizeof(test)))
		return;
	dsint wRVA = parVA - wBase;
	dsint wCipRva = parCIP - wBase;
	HistoryData newHistory;
	//VA history
	if (history)
	{
		//truncate everything right from the current VA
		if (mVaHistory.GetSize() && mCurrentVa < mVaHistory.GetSize() - 1) //mCurrentVa is not the last			
		{
			//mVaHistory.erase(mVaHistory.begin() + mCurrentVa + 1, mVaHistory.end());
			int count = mVaHistory.GetCount();
			for (int i = mCurrentVa; mCurrentVa < count; i++)
				mVaHistory.RemoveAt(mVaHistory.FindIndex(i));
		}
		//NOTE: mCurrentVa always points to the last entry of the list

		//add the currently selected address to the history
		dsint selectionVA = rvaToVa(getInitialSelection()); //currently selected VA
		dsint selectionTableOffset = getTableOffset();
		if (selectionVA && mVaHistory.GetSize() && mVaHistory.GetTail().va != selectionVA) //do not have 2x the same va in a row
		{
			mCurrentVa++;
			newHistory.va = selectionVA;
			newHistory.tableOffset = selectionTableOffset;
			newHistory.windowTitle = "MainWindow::windowTitle";
			//mVaHistory.push_back(newHistory);
			mVaHistory.SetAt(mVaHistory.FindIndex(mVaHistory.GetSize() - 1), newHistory);
		}
	}

	// Set base and size (Useful when memory page changed)
	mMemPage->setAttributes(wBase, wSize);
	mDisasm->getEncodeMap()->setMemoryRegion(wBase);

	if (mRvaDisplayEnabled && mMemPage->getBase() != mRvaDisplayPageBase)
		mRvaDisplayEnabled = false;

	setRowCount(wSize);
	setSideBarRowSignal(wSize);

	setSingleSelection(wRVA);               // Selects disassembled instruction
	dsint wInstrSize = getInstructionRVA(wRVA, 1) - wRVA - 1;
	expandSelectionUpTo(wRVA + wInstrSize);

	//set CIP rva
	mCipRva = wCipRva;

	if (newTableOffset == -1) //nothing specified
	{
		// Update table offset depending on the location of the instruction to disassemble
		if (mInstBuffer.GetSize() > 0 && wRVA >= (dsint)mInstBuffer.GetHead().rva && wRVA < (dsint)mInstBuffer.GetTail().rva)
		{
			int wI;
			bool wIsAligned = false;

			// Check if the new RVA is aligned on an instruction from the cache (buffer)
			for (wI = 0; wI < mInstBuffer.GetSize(); wI++)
			{
				if (mInstBuffer.GetAt(mInstBuffer.FindIndex(wI)).rva == wRVA)
				{
					wIsAligned = true;
					break;
				}
			}

			if (wIsAligned == true)
			{
				//updateViewport();
			}
			else
			{
				setTableOffset(wRVA);
			}
		}
		else if (mInstBuffer.GetSize() > 0 && wRVA == (dsint)mInstBuffer.GetTail().rva)
		{
			setTableOffset(mInstBuffer.GetHead().rva + mInstBuffer.GetHead().length);
		}
		else
		{
			setTableOffset(wRVA);
		}

		if (history)
		{
			//new disassembled address
			newHistory.va = parVA;
			newHistory.tableOffset = getTableOffset();
			newHistory.windowTitle = "MainWindow::windowTitle";
			if (mVaHistory.GetSize())
			{
				if (mVaHistory.GetTail().va != parVA) //not 2x the same va in history
				{
					if (mVaHistory.GetSize() >= 1024) //max 1024 in the history
					{
						mCurrentVa--;
						//mVaHistory.erase(mVaHistory.begin()); //remove the oldest element
						mVaHistory.RemoveHead();
					}
					mCurrentVa++;
					//mVaHistory.push_back(newHistory); //add a va to the history
					mVaHistory.SetAt(mVaHistory.FindIndex(mVaHistory.GetSize() - 1), newHistory);
				}
			}
			else //the list is empty
				//mVaHistory.push_back(newHistory);
				//mVaHistory.SetAt(mVaHistory.FindIndex(mVaHistory.GetSize() - 1), newHistory);
				mVaHistory.AddTail(newHistory);
		}
	}
	else //specified new table offset
		;//setTableOffset(newTableOffset);

	/*
	//print history
	if(history)
	{
	QString strList = "";
	for(int i=0; i<mVaHistory.size(); i++)
	strList += QString().sprintf("[%d]:%p,%p\n", i, mVaHistory.at(i).va, mVaHistory.at(i).tableOffset);
	MessageBoxA(GuiGetWindowHandle(), strList.toUtf8().constData(), QString().sprintf("mCurrentVa=%d", mCurrentVa).toUtf8().constData(), MB_ICONINFORMATION);
	}
	*/
	//emit disassembledAt(parVA, parCIP, history, newTableOffset);
	//disassembledAt(parVA, parCIP, history, newTableOffset);

	return;
}

CList<Instruction_t>* CDisassembly::instructionsBuffer()
{
	return &mInstBuffer;
}

void CDisassembly::disassembleAt(dsint parVA, dsint parCIP)
{
	if (mCodeFoldingManager)
	{
		mCodeFoldingManager->expandFoldSegment(parVA);
		mCodeFoldingManager->expandFoldSegment(parCIP);
	}
	disassembleAt(parVA, parCIP, true, -1);
}

CString CDisassembly::getAddrText(dsint cur_addr, char label[MAX_LABEL_SIZE], bool getLabel)
{
	CString addrText = _T("");
	if (mRvaDisplayEnabled) //RVA display
	{
		dsint rva = cur_addr - mRvaDisplayBase;
		if (rva == 0)
		{
#ifdef _WIN64
			addrText = "$ ==>            ";
#else
			addrText = "$ ==>    ";
#endif //_WIN64
		}
		else if (rva > 0)
		{
#ifdef _WIN64
			//addrText = "$+" + CString("%1").arg(rva, -15, 16, QChar(' ')).toUpper();
			addrText.AppendFormat(_T("%16X"), rva);
#else
			//addrText = "$+" + CString("%1").arg(rva, -7, 16, QChar(' ')).toUpper();
			addrText.AppendFormat(_T("%8X"), rva);
#endif //_WIN64
		}
		else if (rva < 0)
		{
#ifdef _WIN64
			//addrText = "$-" + CString("%1").arg(-rva, -15, 16, QChar(' ')).toUpper();
			addrText += _T("$-");
			addrText.AppendFormat(_T("%16X"), rva);
#else
			//addrText = "$-" + CString("%1").arg(-rva, -7, 16, QChar(' ')).toUpper();
			addrText += _T("$-");
			addrText.AppendFormat(_T("%8X"), rva);
#endif //_WIN64
		}
	}
	addrText += ToPtrString(cur_addr);
	char label_[MAX_LABEL_SIZE] = "";
	if (getLabel && DbgGetLabelAt(cur_addr, SEG_DEFAULT, label_)) //has label
	{
		char module[MAX_MODULE_SIZE] = "";
		//		string tmpLabel_ = label_;
		//		string tmpModule = module;
		CString tmpStr(label_);
		if (DbgGetModuleAt(cur_addr, module) && (tmpStr.Find(_T("JMP.&")) != 0) && !mNoCurrentModuleText)
		{
			//addrText += " <" + QString(module) + "." + QString(label_) + ">";
			addrText += " <";
			addrText += CA2T(module, CP_UTF8);
			addrText += ".";
			addrText += CA2T(label_, CP_UTF8);
			addrText += ">";

		}
		else
		{
			//addrText += " <" + QString(label_) + ">";
			addrText += " <";
			addrText += CA2T(label_, CP_UTF8);
			addrText += ">";
		}
	}
	else
		*label_ = 0;
	if (label)
		strcpy_s(label, MAX_LABEL_SIZE, label_);
	return addrText;
}

const duint CDisassembly::getBase() const
{
	return mMemPage->getBase();
}

duint CDisassembly::getSize() const
{
	return mMemPage->getSize();
}

void CDisassembly::historyClear()
{
	mVaHistory.RemoveAll(); //clear history for new targets
	mCurrentVa = 0;
}

void CDisassembly::disassembleClear()
{
	mHighlightingMode = false;
	mHighlightToken = CapstoneTokenizer::SingleToken();
	historyClear();
	mMemPage->setAttributes(0, 0);
	mDisasm->getEncodeMap()->setMemoryRegion(0);
	setRowCount(0);
	setTableOffset(0);
	reloadData();
}

void CDisassembly::onCopySelection(bool copyBytes)
{
	CString selectionString;
	CString selectionHtmlString;
	/*
	CTextStream stream(&selectionString);
	QTextStream htmlStream(&selectionHtmlString);
	pushSelectionInto(copyBytes, stream, &htmlStream);
	Bridge::CopyToClipboard(selectionString, selectionHtmlString);
	*/
	const int addressLen = getColumnWidth(1) / getCharWidth() - 1;
	const int bytesLen = getColumnWidth(2) / getCharWidth() - 1;
	const int disassemblyLen = getColumnWidth(3) / getCharWidth() - 1;

	prepareDataRange(getSelectionStart(), getSelectionEnd(), [&](int i, const Instruction_t & inst)
	{
		if (i)
			selectionString += "\r\n";
		duint cur_addr = rvaToVa(inst.rva);
		CString address = getAddrText(cur_addr, 0, addressLen > sizeof(duint) * 2 + 1);
		CString bytes;
		CString bytesHtml;
		if (copyBytes)
			RichTextPainter::htmlRichText(getRichBytes(inst), bytesHtml, bytes);
		CString disassembly;
		CString htmlDisassembly;

		for (const auto & token : inst.tokens.tokens)
				disassembly += token.text;

		CString fullComment;
		CString comment;
		bool autocomment;
		if (GetCommentFormat(cur_addr, comment, &autocomment))
		{
			fullComment += " ";
			fullComment += comment;
		}
		selectionString += address;
		if (copyBytes)
		{
			selectionString += " | ";
			selectionString += bytes;
		}

		selectionString += " | ";
		selectionString += disassembly;
		selectionString += " | ";
		selectionString += fullComment;		
		return true;
	});
	/*
	if (htmlStream)
	*htmlStream << "</table>";
	*/
	Bridge::CopyToClipboard(selectionString);

}

void CDisassembly::onFindStrings(UINT id)
{
	int refFindType = 0;
	if (id == ID_Disassembly_Search_CurrentRegion_String)
		refFindType = 0;
	else if (id == ID_Disassembly_Search_CurrentModule_String)
		refFindType = 1;
	else if (id == ID_Disassembly_Search_AllModule_String)
		refFindType = 2;
	/*
	else if (id == mFindStringsFunction)
	{
		duint start, end;
		if (DbgFunctionGet(rvaToVa(getInitialSelection()), &start, &end))
			DbgCmdExec(QString("strref %1, %2, 0").arg(ToPtrString(start)).arg(ToPtrString(end - start)).toUtf8().constData());
		return;
	}
	*/
	

	auto addrText = ToHexString(rvaToVa(getInitialSelection()));
	CString cmd;
	cmd.Format(_T("strref %s, 0, %d"), addrText, refFindType);
	DbgCmdExec(CT2A(cmd, CP_UTF8));
	
	//emit displayReferencesWidget();
	CWnd* pMainWnd = AfxGetMainWnd();
	if (pMainWnd->GetSafeHwnd())
		pMainWnd->PostMessage(WM_USER + Signal_DisplayReferencesPane_Msg);
}


int CDisassembly::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CAbstractTableView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	loadColumnFromConfig(_T("CPUDisassembly"));
	return 0;
}

void CDisassembly::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	SetFocus();

	this->GetParent()->Invalidate();
	bool wAccept = false;

	if (DbgIsDebugging())
	{
		if (getGuiState() == NoState)
		{
			if (point.y > getHeaderHeight())
			{
				SetCapture();
				dsint wIndex = getIndexOffsetFromY(transY(point.y));
				if (point.x < getSplitWidth())
				{
					// calculate virtual address of clicked line
					duint wVA = mInstBuffer.GetAt(mInstBuffer.FindIndex(wIndex)).rva + mMemPage->getBase();
					CString wCmd;
					// create --> disable --> delete --> create --> ...
					switch (Breakpoints::BPState(bp_normal, wVA))
					{
					case bp_enabled:
						// breakpoint exists and is enabled --> disable breakpoint
						wCmd = "bd ";
						break;
					case bp_disabled:
						// is disabled --> delete or enable
						if (Breakpoints::BPTrival(bp_normal, wVA))
							wCmd = "bc ";
						else
							wCmd = "be ";
						break;
					case bp_non_existent:
						// no breakpoint was found --> create breakpoint
						wCmd = "bp ";
						break;
					}
					wCmd += ToPtrString(wVA);
					DbgCmdExec(CT2A(wCmd, CP_UTF8));
				}
				else
				{
					if (mInstBuffer.GetSize() > wIndex && wIndex >= 0)
					{
						dsint wRowIndex = mInstBuffer.GetAt(mInstBuffer.FindIndex(wIndex)).rva;
						dsint wInstrSize = mInstBuffer.GetAt(mInstBuffer.FindIndex(wIndex)).length - 1;
						//if (!(event->modifiers() & Qt::ShiftModifier)) //SHIFT pressed
						if (GetKeyState(VK_SHIFT) >= 0)
							setSingleSelection(wRowIndex);
						if (getSelectionStart() > wRowIndex) //select up
						{
							setSingleSelection(getInitialSelection());
							expandSelectionUpTo(getInstructionRVA(getInitialSelection(), 1) - 1);
							expandSelectionUpTo(wRowIndex);
						}
						else //select down
						{
							setSingleSelection(getInitialSelection());
							expandSelectionUpTo(wRowIndex + wInstrSize);
						}

						mGuiState = CDisassembly::MultiRowsSelectionState;

						Invalidate();

						wAccept = true;
					}
				}				
			}
		}
	}
	if (!wAccept)
		CAbstractTableView::OnLButtonDown(nFlags, point);
}

void CDisassembly::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nChar)
	{
	case 0x10:
		bSHIFT = true;
	default:
		break;
	}

	CAbstractTableView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CDisassembly::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nChar)
	{
	case 0x10:
		bSHIFT = false;
	default:
		break;
	}

	CAbstractTableView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CDisassembly::OnSetFocus(CWnd* pOldWnd)
{
	CAbstractTableView::OnSetFocus(pOldWnd);

	// TODO:  在此处添加消息处理程序代码
	CDisasmPane* tp = (CDisasmPane*) this->GetParent();
	tp->setFocusState(CDisasmPane::FocusState::D);
}

void CDisassembly::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CRect wndRect;
	GetWindowRect(&wndRect);

	CAbstractTableView::OnNcMouseMove(nHitTest, point);
}


void CDisassembly::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	bool wAccept = true;
	int y = point.y;

	CRect viewRect;
	GetClientRect(&viewRect);

	if (mGuiState == CDisassembly::MultiRowsSelectionState)
	{
		if ((transY(y) >= 0) && (transY(y) <= this->getTableHeight()))
		{
			int wI = getIndexOffsetFromY(transY(y));

			if (mMemPage->getSize() > 0)
			{
				// Bound
				wI = wI >= mInstBuffer.GetSize() ? mInstBuffer.GetSize() - 1 : wI;
				wI = wI < 0 ? 0 : wI;

				if (wI >= mInstBuffer.GetSize())
					return;
				dsint wRowIndex = mInstBuffer.GetAt(mInstBuffer.FindIndex(wI)).rva;
				dsint wInstrSize = getInstructionRVA(wRowIndex, 1) - wRowIndex - 1;

				if (wRowIndex < getRowCount())
				{
					setSingleSelection(getInitialSelection());
					expandSelectionUpTo(getInstructionRVA(getInitialSelection(), 1) - 1);
					if (wRowIndex > getInitialSelection()) //select down
						expandSelectionUpTo(wRowIndex + wInstrSize);
					else
						expandSelectionUpTo(wRowIndex);

					CString selStart = ToPtrString(rvaToVa(getSelectionStart()));
					CString selEnd = ToPtrString(rvaToVa(getSelectionEnd()));
					CString info = _T("Disassembly");
					char mod[MAX_MODULE_SIZE] = "";
					if (DbgFunctions()->ModNameFromAddr(rvaToVa(getSelectionStart()), mod, true))
					{
						info += mod;
						info += "";
					}
					Invalidate();

					wAccept = false;
				}
			}
		}
		
		else if (y > viewRect.Height())
		{
			SendMessage(WM_VSCROLL, SB_LINEDOWN, 0);
		}
		else if (transY(y) < 0)
		{
			SendMessage(WM_VSCROLL, SB_LINEUP, 0);
		}
	}
	else if (mGuiState == CDisassembly::NoState)
	{
		if (!mHighlightingMode && mPopupEnabled)
		{
			bool popupShown = false;
			if (y > getHeaderHeight() && getColumnIndexFromX(point.x) == 2)
			{
				int rowOffset = getIndexOffsetFromY(transY(y));
				if (rowOffset < mInstBuffer.GetSize())
				{
					CapstoneTokenizer::SingleToken token;
					auto & instruction = mInstBuffer.GetAt(mInstBuffer.FindIndex(rowOffset));
					if (CapstoneTokenizer::TokenFromX(instruction.tokens, token, point.x, &lf))
					{
						duint addr = token.value.value;
						bool isCodePage = DbgFunctions()->MemIsCodePage(addr, false);
						if (!isCodePage && instruction.branchDestination)
						{
							addr = instruction.branchDestination;
							isCodePage = DbgFunctions()->MemIsCodePage(addr, false);
						}
						if (isCodePage && (addr - mMemPage->getBase() < mInstBuffer.GetHead().rva || addr - mMemPage->getBase() > mInstBuffer.GetTail().rva))
						{
							//ShowDisassemblyPopup(addr, point.x, y);
							popupShown = true;
						}
					}
				}
			}
			if (popupShown == false)
			{
				//ShowDisassemblyPopup(0, 0, 0); // hide popup
			}				
		}
	}

	if (wAccept == true)
		CAbstractTableView::OnMouseMove(nFlags, point);
	
}


BOOL CDisassembly::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	return CAbstractTableView::OnSetCursor(pWnd, nHitTest, message);
}


void CDisassembly::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	bool wAccept = true;
	ReleaseCapture();
	if (mGuiState == CDisassembly::MultiRowsSelectionState)
	{
		mGuiState = CDisassembly::NoState;
		Invalidate();
		wAccept = false;
	}
	/*
	if ((event->button() & Qt::BackButton) != 0)
	{
		wAccept = true;
		historyPrevious();
	}
	else if ((event->button() & Qt::ForwardButton) != 0)
	{
		wAccept = true;
		historyNext();
	}
	*/
	

	if (wAccept == true)
		CAbstractTableView::OnLButtonUp(nFlags, point);
	
}


BOOL CDisassembly::OnEraseBkgnd(CDC* pDC)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	return CAbstractTableView::OnEraseBkgnd(pDC);
}


void CDisassembly::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: 在此处添加消息处理程序代码
	if (!DbgIsDebugging())
		return;
	CMenu PopupMenu;
	// 创建弹出菜单
	BOOL bRet = PopupMenu.CreatePopupMenu();
	PopupMenu.AppendMenu(MF_ENABLED, ID_Disassembly_Copy_Selection, _T("复制"));

	CMenu SearchSecondLevelPopupMenu;
	// 创建二级弹出菜单
	bRet = SearchSecondLevelPopupMenu.CreateMenu();
	

	CMenu SearchThirdLevelPopupMenu;
	// 创建二级弹出菜单
	bRet = SearchThirdLevelPopupMenu.CreateMenu();
	SearchThirdLevelPopupMenu.AppendMenu(MF_ENABLED, ID_Disassembly_Search_CurrentModule_Command, _T("命令"));
	SearchThirdLevelPopupMenu.AppendMenu(MF_ENABLED, ID_Disassembly_Search_CurrentModule_Constant, _T("常量"));
	SearchThirdLevelPopupMenu.AppendMenu(MF_ENABLED, ID_Disassembly_Search_CurrentModule_String, _T("字符串"));
	SearchThirdLevelPopupMenu.AppendMenu(MF_ENABLED, ID_Disassembly_Search_CurrentModule_Intermodular_Calls, _T("模块间调用"));
	SearchThirdLevelPopupMenu.AppendMenu(MF_ENABLED, ID_Disassembly_Search_CurrentModule_Pattern, _T("Pattern"));
	SearchThirdLevelPopupMenu.AppendMenu(MF_ENABLED, ID_Disassembly_Search_CurrentModule_GUID, _T("GUID"));
	SearchThirdLevelPopupMenu.AppendMenu(MF_ENABLED, ID_Disassembly_Search_CurrentModule_Names, _T("名称"));

	SearchSecondLevelPopupMenu.AppendMenu(MF_POPUP, (UINT)SearchThirdLevelPopupMenu.m_hMenu, _T("当前模块"));

	PopupMenu.AppendMenu(MF_POPUP, (UINT)SearchSecondLevelPopupMenu.m_hMenu, _T("搜索"));

	// 显示菜单
	int nCmd = PopupMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this);
}

void CDisassembly::OnHandle(UINT id)
{
	switch (id)
	{
	case ID_Disassembly_Copy_Selection:
		onCopySelection(true);
		break;
	case  ID_Disassembly_Search_CurrentModule_String:
		onFindStrings(ID_Disassembly_Search_CurrentModule_String);
		break;
	default:
		break;
	}
}