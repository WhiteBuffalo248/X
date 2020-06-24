// CPUSideBar.cpp : 实现文件
//

#include "stdafx.h"
#include "X.h"
#include "CPUSideBar.h"
#include "DisasmPane.h"
#include <algorithm>

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

using namespace Gdiplus;


// CCPUSideBar

IMPLEMENT_DYNAMIC(CCPUSideBar, CWnd)

CCPUSideBar::CCPUSideBar()
{

	memset(&regDump, 0, sizeof(REGDUMP));
	updateColors();
}

CCPUSideBar::~CCPUSideBar()
{
}

void CCPUSideBar::updateColors()
{
	mCipLabelColor = ConfigColor("SideBarCipLabelColor");
	mCipLabelBackgroundColor = ConfigColor("SideBarCipLabelBackgroundColor");
	mConditionalJumpLineFalseColor = ConfigColor("SideBarConditionalJumpLineFalseColor");
    mUnconditionalJumpLineFalseColor = ConfigColor("SideBarUnconditionalJumpLineFalseColor");
	mConditionalJumpLineTrueColor = ConfigColor("SideBarConditionalJumpLineTrueColor");
	mUnconditionalJumpLineTrueColor = ConfigColor("SideBarUnconditionalJumpLineTrueColor");
}

void CCPUSideBar::updateFonts()
{
	mDefaultFont = (CFont*)mDisas->GetFont();
	
	CSize cSpaceSize = this->GetDC()->GetTextExtent(_T(" "));

	mDefaultFont = mDisas->GetDC()->GetCurrentFont();
	LOGFONT lf;
	mDefaultFont->GetLogFont(&lf);

	TEXTMETRIC tm;	
	this->GetDC()->GetTextMetrics(&tm);

	fontWidth = cSpaceSize.cx;
	fontHeight = cSpaceSize.cy;
}

void CCPUSideBar::reload()
{
	fontHeight = mDisas->getRowHeight();
	Invalidate();
}

void CCPUSideBar::setViewableRows(int rows)
{
	viewableRows = rows;
}

void CCPUSideBar::setSelection(dsint selVA)
{
	if (selVA != selectedVA)
	{
		selectedVA = selVA;
		reload();
	}
}

void CCPUSideBar::changeTopmostAddress(dsint i)
{
	topVA = i;
	DbgGetRegDumpEx(&regDump, sizeof(REGDUMP));
	reload();
}

int CCPUSideBar::getDisassemblyViewHeaderHeight()
{
	CDisasmPane* pParentPane = (CDisasmPane*)GetParent();
	auto pDisassembly = pParentPane->getCDisassembly();
	
	return pDisassembly->getShowHeader() ? pDisassembly->getHeaderHeight() : 0;
}

bool CCPUSideBar::isJump(int i) const
{
	if (i < 0 || i >= mInstrBuffer->GetSize())
		return false;

	const Instruction_t & instr = mInstrBuffer->GetAt(mInstrBuffer->FindIndex(i));
	Instruction_t::BranchType branchType = instr.branchType;
	if (branchType == Instruction_t::Unconditional || branchType == Instruction_t::Conditional)
	{
		duint start = mDisas->getBase();
		duint end = start + mDisas->getSize();
		duint addr = DbgGetBranchDestination(start + instr.rva);

		if (!addr)
			return false;

		return addr >= start && addr < end; //do not draw jumps that go out of the section
	}

	return false;
}

CCPUSideBar::LabelArrow CCPUSideBar::drawLabel(CDC* painter, int Line, const CString & Text)
{
	const int LineCoordinate = fontHeight * (1 + Line);

	const Color & IPLabel = mCipLabelColor;
	const Color & IPLabelBG = mCipLabelBackgroundColor;

	int width = painter->GetTextExtent(Text).cx;
	int x = 1;
	int y = LineCoordinate - fontHeight;

	CRect rect(x, y, x + width, y + fontHeight - 1);

	// Draw rectangle
	//painter->Draw3dRect(rect, IPLabelBG.ToCOLORREF(), IPLabelBG.ToCOLORREF());
	painter->FillSolidRect(rect, IPLabelBG.ToCOLORREF());

	// Draw text inside the rectangle
	painter->SetTextColor(IPLabel.ToCOLORREF());
	painter->DrawText(Text, rect, DT_CENTER | DT_VCENTER);

	// Draw arrow
	/*y = fontHeight * (1 + Line) - 0.5 * fontHeight;

	painter->setPen(QPen(IPLabelBG, 2.0));
	painter->setBrush(QBrush(IPLabelBG));
	drawStraightArrow(painter, rect.right() + 2, y, this->viewport()->width() - x - 11 - (isFoldingGraphicsPresent(Line) != 0 ? mBulletRadius + fontHeight : 0), y);*/

	LabelArrow labelArrow;
	labelArrow.line = Line;
	labelArrow.startX = rect.left + rect.Width() + 2;
	labelArrow.endX = 0;

	return labelArrow;
}

void CCPUSideBar::drawLabelArrows(CDC* painter, const std::vector<LabelArrow> & labelArrows)
{
	if (!labelArrows.empty())
	{
		//painter->setPen(QPen(mCipLabelBackgroundColor, 2.0));
		CPen pen;
		pen.CreatePen(PS_SOLID, 2, mCipLabelBackgroundColor.ToCOLORREF());
		CPen* oldPen = painter->SelectObject(&pen);		
		for (auto i : labelArrows)
		{
			if (i.startX < i.endX)
			{
				int y = fontHeight * (1 + i.line) - 0.5 * fontHeight;
				drawStraightArrow(painter, i.startX, y, i.endX, y);
			}
		}
	}
}

void CCPUSideBar::AllocateJumpOffsets(std::vector<JumpLine> & jumpLines, std::vector<LabelArrow> & labelArrows)
{
	unsigned int* numLines = new unsigned int[viewableRows * 2]; // Low:jump offsets of the vertical jumping line, High:jump offsets of the horizontal jumping line.
	memset(numLines, 0, sizeof(unsigned int) * viewableRows * 2);
	// preprocessing
	for (size_t i = 0; i < jumpLines.size(); i++)
	{
		JumpLine & jmp = jumpLines.at(i);
		jmp.jumpOffset = abs(jmp.destLine - jmp.line);
	}
	// Sort jumpLines so that longer jumps are put at the back.
	std::sort(jumpLines.begin(), jumpLines.end(), [](const JumpLine & op1, const JumpLine & op2)
	{
		return op2.jumpOffset > op1.jumpOffset;
	});
	// Allocate jump offsets
	for (size_t i = 0; i < jumpLines.size(); i++)
	{
		JumpLine & jmp = jumpLines.at(i);
		unsigned int maxJmpOffset = 0;
		if (jmp.line < jmp.destLine)
		{
			for (int j = jmp.line; j <= jmp.destLine && j < viewableRows; j++)
			{
				if (numLines[j] > maxJmpOffset)
					maxJmpOffset = numLines[j];
			}
		}
		else
		{
			for (int j = jmp.line; j >= jmp.destLine && j >= 0; j--)
			{
				if (numLines[j] > maxJmpOffset)
					maxJmpOffset = numLines[j];
			}
		}
		jmp.jumpOffset = maxJmpOffset + 1;
		if (jmp.line < jmp.destLine)
		{
			for (int j = jmp.line; j <= jmp.destLine && j < viewableRows; j++)
				numLines[j] = jmp.jumpOffset;
		}
		else
		{
			for (int j = jmp.line; j >= jmp.destLine && j >= 0; j--)
				numLines[j] = jmp.jumpOffset;
		}
		if (jmp.line >= 0 && jmp.line < viewableRows)
			numLines[jmp.line + viewableRows] = jmp.jumpOffset;
		if (jmp.destLine >= 0 && jmp.destLine < viewableRows)
			numLines[jmp.destLine + viewableRows] = jmp.jumpOffset;
	}
	// set label arrows according to jump offsets
	CRect viewRect;
	GetClientRect(&viewRect);
	auto viewportWidth = viewRect.Width();
	const int JumpPadding = 10;
	for (auto i = labelArrows.begin(); i != labelArrows.end(); i++)
	{
		if (numLines[i->line + viewableRows] != 0)
			i->endX = viewportWidth - numLines[i->line + viewableRows] * JumpPadding - fontHeight; // This expression should be consistent with drawJump
		else
			i->endX = viewportWidth - 1 - 11 - (isFoldingGraphicsPresent(i->line) != 0 ? 0 + fontHeight : 0);
	}
	delete[] numLines;
}

int CCPUSideBar::isFoldingGraphicsPresent(int line)
{
	if (line < 0 || line >= mInstrBuffer->GetCount()) //There couldn't be a folding box out of viewport
		return 0;
	const Instruction_t* instr = &mInstrBuffer->GetAt(mInstrBuffer->FindIndex(line));
	if (instr == nullptr) //Selection out of a memory page
		return 0;
	auto wVA = instr->rva + mDisas->getBase();
	if (mCodeFoldingManager.isFoldStart(wVA)) //Code is already folded
		return 1;
	const dsint SelectionStart = mDisas->getSelectionStart();
	const dsint SelectionEnd = mDisas->getSelectionEnd();
	duint start, end;
	if ((DbgArgumentGet(wVA, &start, &end) || DbgFunctionGet(wVA, &start, &end)) && wVA == start)
	{
		return end - start + 1 != instr->length ? 1 : 0;
	}
	else if (instr->rva >= duint(SelectionStart) && instr->rva < duint(SelectionEnd))
	{
		if (instr->rva == SelectionStart)
			return (SelectionEnd - SelectionStart + 1) != instr->length ? 1 : 0;
	}
	return 0;
}

void CCPUSideBar::drawStraightArrow(CDC* painter, int x1, int y1, int x2, int y2)
{
	//painter->drawLine(x1, y1, x2, y2);
	painter->MoveTo(x1, y1);
	painter->LineTo(x2, y2);

	const int ArrowSizeX = 4;// Width  of arrow tip in pixels
	const int ArrowSizeY = 4;// Height of arrow tip in pixels

							 // X and Y values adjusted so that the arrow itself is symmetrical on 2px
	//painter->drawLine(x2 - 1, y2 - 1, x2 - ArrowSizeX, y2 - ArrowSizeY);// Arrow top
	painter->MoveTo(x2 - 1, y2 - 1);
	painter->LineTo(x2 - ArrowSizeX, y2 - ArrowSizeY);
	//painter->drawLine(x2 - 1, y2, x2 - ArrowSizeX, y2 + ArrowSizeY - 1);// Arrow bottom
	painter->MoveTo(x2 - 1, y2);
	painter->LineTo(x2 - ArrowSizeX, y2 + ArrowSizeY - 1);
}

void CCPUSideBar::drawJump(CDC* painter, Gdiplus::Graphics& graphics, int startLine, int endLine, int jumpoffset, bool conditional, bool isexecute, bool isactive)
{
	CRect viewRect;
	GetClientRect(&viewRect);
	CPen pen;
	COLORREF penCOLORREF;
	Color penColor;
	int nPenStyle = 0;

	Gdiplus::Status nStatus;
	
	if (conditional)
	{
		penCOLORREF = mConditionalJumpLineFalseColor.ToCOLORREF();
		penColor = mConditionalJumpLineFalseColor;
		nPenStyle = PS_DASH;
	}		
	else
	{
		penCOLORREF = mUnconditionalJumpLineFalseColor.ToCOLORREF(); //JMP
		penColor = mUnconditionalJumpLineFalseColor;
		nPenStyle = PS_SOLID;
	}
											// Pixel adjustment to make drawing lines even
	pen.CreatePen(nPenStyle, 1, penCOLORREF);
	Gdiplus::Pen gdiPlusPlusPen(penColor, 1);
	painter->SelectObject(&pen);

	int pixel_y_offs = 0;

	if (isactive) //selected
	{
		pen.DeleteObject();

		// Active/selected jumps use a bold line (2px wide)	

		// Adjust for 2px line
		pixel_y_offs = 0;

		// Use a different color to highlight jumps that will execute
		if (isexecute)
		{
			if (conditional)
			{
				penCOLORREF = mConditionalJumpLineTrueColor.ToCOLORREF();
				penColor = mConditionalJumpLineTrueColor;
			}				
			else
			{
				penCOLORREF = mUnconditionalJumpLineTrueColor.ToCOLORREF();
				penColor = mUnconditionalJumpLineTrueColor;
			}
				
		}

		// Update the painter itself with the new pen style
		pen.CreatePen(nPenStyle, 2, penCOLORREF);
		painter->SelectObject(&pen);

		gdiPlusPlusPen.SetWidth(2);
		gdiPlusPlusPen.SetColor(penColor);
	}

	// Cache variables
	const int viewportWidth = viewRect.Width();
	const int viewportHeight = viewRect.Height();

	const int JumpPadding = 10;
	int x = viewportWidth - jumpoffset * JumpPadding - fontHeight;
	int x_right = viewportWidth - 2;
	int headerheight = getDisassemblyViewHeaderHeight();
	int y_start = fontHeight * (1 + startLine) - 0.5 * fontHeight - pixel_y_offs + headerheight;
	int y_end = fontHeight * (1 + endLine) - 0.5 * fontHeight + headerheight;

	// special handling of self-jumping
	if (startLine == endLine)
	{
		y_start -= fontHeight / 4;
		y_end += fontHeight / 4;
	}

	// Horizontal (<----)
	if (!isFoldingGraphicsPresent(startLine) != 0)
	{
		//painter->drawLine(x_right, y_start, x, y_start);
		painter->MoveTo(x_right, y_start);
		painter->LineTo(x, y_start);
	}		
	else
	{
		//painter->drawLine(x_right - mBulletRadius - fontHeight, y_start, x, y_start);
		painter->MoveTo(x_right - 0 - fontHeight, y_start);
		painter->LineTo(x, y_start);
	}
		

	// Vertical
	//painter->drawLine(x, y_start, x, y_end);
	painter->MoveTo(x, y_start);
	painter->LineTo(x, y_end);

	// Specialized pen for solid lines only, keeping other styles
	//QPen solidLine = painter->pen();
	//solidLine.setStyle(Qt::SolidLine);

	// Draw the arrow
	if (!isactive) //selected
	{
		// Horizontal (---->)
		if (isFoldingGraphicsPresent(endLine) != 0)
		{
			//painter->drawLine(x, y_end, x_right - mBulletRadius - fontHeight, y_end);
			painter->MoveTo(x, y_end);
			painter->LineTo(x_right - 0 - fontHeight, y_end);
		}
			
		else
		{
			//painter->drawLine(x, y_end, x_right, y_end);
			painter->MoveTo(x, y_end);
			painter->LineTo(x_right, y_end);
		}
			

		if (endLine == viewableRows + 6)
		{
			int y = viewportHeight - 1;
			if (y > y_start)
			{
				//painter->setPen(solidLine);
				POINT wPoints[] =
				{
					{x - 3, y - 3},
					{x, y},
					{x + 3, y - 3},
				};
				//painter->Polyline(wPoints, 3);

				Point arrowsPoints[] =
				{
					Point(x - 3, y - 3),
					Point(x, y),
					Point(x + 3, y - 3),
				};
				nStatus = graphics.DrawLines(&gdiPlusPlusPen, arrowsPoints, 3);
			}
		}
		else if (endLine == -6)
		{
			int y = 0;
			//painter->setPen(solidLine);
			POINT wPoints[] =
			{
				{x - 3, y + 3},
				{x, y},
				{x + 3, y + 3},
			};
			//painter->Polyline(wPoints, 3);

			Point arrowsPoints[] =
			{
				Point(x - 3, y + 3),
				Point(x, y ),
				Point(x + 3, y + 3),
			};
			graphics.DrawLines(&gdiPlusPlusPen, arrowsPoints, 3);
		}
		else
		{
			if (isFoldingGraphicsPresent(endLine) != 0)
				x_right -= 0 + fontHeight;
			//painter->setPen(solidLine);
			POINT wPoints[] =
			{
				{x_right - 3, y_end - 3},
				{x_right, y_end},
				{x_right - 3, y_end + 3},
			};
			//painter->Polyline(wPoints, 3);

			Point arrowsPoints[] =
			{
				Point(x_right - 3, y_end - 3),
				Point(x_right, y_end),
				Point(x_right - 3, y_end + 3),
			};
			graphics.DrawLines(&gdiPlusPlusPen, arrowsPoints, 3);
		}
	}
	else
	{
		if (endLine == viewableRows + 6)
		{
			int y = viewportHeight - 1;
			x--;
			//painter->setPen(solidLine);
			POINT wPoints[] =
			{
				{x - 3, y - 3},
				{x, y},
				{x + 3, y - 3},
			};
			//painter->Polyline(wPoints, 3);

			Point arrowsPoints[] =
			{
				Point(x - 3, y - 3),
				Point(x, y ),
				Point(x + 3, y - 3),
			};
			graphics.DrawLines(&gdiPlusPlusPen, arrowsPoints, 3);
		}
		else if (endLine == -6)
		{
			int y = 0;
			//painter->setPen(solidLine);
			POINT wPoints[] =
			{
				{x - 3, y + 3},
				{x, y},
				{x + 3, y + 3},
			};
			//painter->Polyline(wPoints, 3);

			Point arrowsPoints[] =
			{
				Point(x - 3, y + 3),
				Point(x, y ),
				Point(x + 3, y + 3),
			};
			graphics.DrawLines(&gdiPlusPlusPen, arrowsPoints, 3);
		}
		else
		{
			if (isFoldingGraphicsPresent(endLine) != 0)
				drawStraightArrow(painter, x, y_end, x_right - 0 - fontHeight, y_end);
			else
				drawStraightArrow(painter, x, y_end, x_right, y_end);
		}
	}
}


BEGIN_MESSAGE_MAP(CCPUSideBar, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETFOCUS()
	ON_WM_CREATE()
END_MESSAGE_MAP()



// CCPUSideBar 消息处理程序




void CCPUSideBar::OnPaint()
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
	MenDC.SetBkMode(TRANSPARENT);

	Gdiplus::Graphics graphics(MenDC.m_hDC);

	if (!viewRect.IsRectEmpty())
	{
		MenDC.FillSolidRect(&viewRect, RGB(200, 200, 200));

		// Don't draw anything if there aren't any instructions to draw
		if (mInstrBuffer->GetCount() != 0)
		{
			int jumpoffset = 0;
			duint last_va = mInstrBuffer->GetTail().rva + mDisas->getBase();
			duint first_va = mInstrBuffer->GetHead().rva + mDisas->getBase();

			vector<std::pair<CString, duint>> regLabel;
			std::vector<LabelArrow> labelArrows;
			auto appendReg = [&regLabel, last_va, first_va](const CString & name, duint value)
			{
				if (value >= first_va && value <= last_va)
					regLabel.push_back(std::make_pair(name, value));
			};
#ifdef _WIN64
			appendReg(_T("RIP"), regDump.regcontext.cip);
			appendReg(_T("RAX"), regDump.regcontext.cax);
			appendReg(_T("RCX"), regDump.regcontext.ccx);
			appendReg(_T("RDX"), regDump.regcontext.cdx);
			appendReg(_T("RBX"), regDump.regcontext.cbx);
			appendReg(_T("RSP"), regDump.regcontext.csp);
			appendReg(_T("RBP"), regDump.regcontext.cbp);
			appendReg(_T("RSI"), regDump.regcontext.csi);
			appendReg(_T("RDI"), regDump.regcontext.cdi);
			appendReg(_T("R8"), regDump.regcontext.r8);
			appendReg(_T("R9"), regDump.regcontext.r9);
			appendReg(_T("R10"), regDump.regcontext.r10);
			appendReg(_T("R11"), regDump.regcontext.r11);
			appendReg(_T("R12"), regDump.regcontext.r12);
			appendReg(_T("R13"), regDump.regcontext.r13);
			appendReg(_T("R14"), regDump.regcontext.r14);
			appendReg(_T("R15"), regDump.regcontext.r15);
#else //x86
			appendReg(_T("EIP"), regDump.regcontext.cip);
			appendReg(_T("EAX"), regDump.regcontext.cax);
			appendReg(_T("ECX"), regDump.regcontext.ccx);
			appendReg(_T("EDX"), regDump.regcontext.cdx);
			appendReg(_T("EBX"), regDump.regcontext.cbx);
			appendReg(_T("ESP"), regDump.regcontext.csp);
			appendReg(_T("EBP"), regDump.regcontext.cbp);
			appendReg(_T("ESI"), regDump.regcontext.csi);
			appendReg(_T("EDI"), regDump.regcontext.cdi);
#endif //_WIN64

			std::vector<JumpLine> jumpLines;

			for (int line = 0; line < viewableRows; line++)
			{
				if (line >= mInstrBuffer->GetCount()) //at the end of the page it will crash otherwise
					break;

				const Instruction_t & instr = mInstrBuffer->GetAt(mInstrBuffer->FindIndex(line));
				duint instrVA = instr.rva + mDisas->getBase();
				duint instrVAEnd = instrVA + instr.length;

				if (isJump(line)) //handle jumps
				{
					duint baseAddr = mDisas->getBase();
					duint destVA = DbgGetBranchDestination(baseAddr + instr.rva);

					JumpLine jmp;
					jmp.isJumpGoingToExecute = DbgIsJumpGoingToExecute(instrVA);
					jmp.isSelected = (selectedVA == instrVA || selectedVA == destVA);
					jmp.isConditional = instr.branchType == Instruction_t::Conditional;
					jmp.line = line;

					/*
					if(mDisas->currentEIP() != mInstrBuffer->at(line).rva) //create a setting for this
					isJumpGoingToExecute=false;
					*/

					jumpoffset++;

					// Do not draw jumps that leave the memory range
					if (destVA >= mDisas->getBase() + mDisas->getSize() || destVA < mDisas->getBase())
						continue;

					if (destVA <= last_va && destVA >= first_va)
					{
						int destLine = line;
						while (destLine > -1 && destLine < mInstrBuffer->GetCount())
						{
							duint va = mInstrBuffer->GetAt(mInstrBuffer->FindIndex(destLine)).rva + mDisas->getBase();
							if (destVA > instrVA) //jump goes down
							{
								duint vaEnd = va + mInstrBuffer->GetAt(mInstrBuffer->FindIndex(destLine)).length - 1;
								if (vaEnd >= destVA)
									break;
								destLine++;
							}
							else //jump goes up
							{
								if (va <= destVA)
									break;
								destLine--;
							}
						}
						jmp.destLine = destLine;
					}
					else if (destVA > last_va)
						jmp.destLine = viewableRows + 6;
					else if (destVA < first_va)
						jmp.destLine = -6;
					jumpLines.emplace_back(jmp);
				}

				CString regLabelText;
				for (auto i = regLabel.cbegin(); i != regLabel.cend(); i++)
				{
					if (i->second >= instrVA && i->second < instrVAEnd)
					{
						regLabelText += i->first;
						regLabelText += +" ";
					}
				}
				int len = regLabelText.GetLength();
				if (len)
				{
					regLabelText.Delete(len);
					labelArrows.push_back(drawLabel(&MenDC, line, regLabelText));
				}
			}

			if (jumpLines.size())
			{
				AllocateJumpOffsets(jumpLines, labelArrows);
				for (auto i : jumpLines)
					drawJump(&MenDC, graphics, i.line, i.destLine, i.jumpOffset, i.isConditional, i.isJumpGoingToExecute, i.isSelected);
			}
			else
			{
				for (auto i = labelArrows.begin(); i != labelArrows.end(); i++)
					i->endX = viewRect.Width() - 1 - 11 - (isFoldingGraphicsPresent(i->line) != 0 ? 0 + fontHeight : 0);
			}

			drawLabelArrows(&MenDC, labelArrows);
		}		
	}
	dc.BitBlt(0, 0, viewRect.Width(), viewRect.Height(), &MenDC, 0, 0, SRCCOPY);
}


BOOL CCPUSideBar::OnEraseBkgnd(CDC* pDC)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	return TRUE;
	return CWnd::OnEraseBkgnd(pDC);
}


void CCPUSideBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	SetFocus();
	this->GetParent()->Invalidate();

	CWnd::OnLButtonDown(nFlags, point);
}


void CCPUSideBar::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);

	// TODO:  在此处添加消息处理程序代码
	CDisasmPane* tp = (CDisasmPane*) this->GetParent();
	tp->setFocusState(CDisasmPane::FocusState::S);
}


int CCPUSideBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CDisasmPane* pParentPane = (CDisasmPane*)CWnd::FromHandle(lpCreateStruct->hwndParent);
	mDisas = pParentPane->getCDisassembly();
	mInstrBuffer = mDisas->instructionsBuffer();

	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	updateFonts();

	return 0;
}
