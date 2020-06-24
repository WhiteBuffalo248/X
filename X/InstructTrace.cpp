#include "stdafx.h"
#include "InstructTrace.h"
#include "TraceFileReader.h"
#include "RichTextPainter.h"
#include "QBeaEngine.h"
#include "GlobalID.h"
#include "InstructTraceBrowserDlg.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CInstructTrace::CInstructTrace()
{
	mTraceFile = nullptr;
	mSelection.firstSelectedIndex = 0;
	mSelection.fromIndex = 0;
	mSelection.toIndex = 0;
	setRowCount(0);
	mRvaDisplayBase = 0;
	mRvaDisplayEnabled = false;

	int maxModuleSize = (int)ConfigUint("Disassembler", "MaxModuleSize");
	mDisasm = new QBeaEngine(maxModuleSize);
	mHighlightingMode = false;
	mPermanentHighlightingMode = false;

	Initialize();

	addColumnAt(getCharWidth() * 2 * 8 + 8, _T("序号"), true); //index
	addColumnAt(getCharWidth() * 2 * sizeof(dsint) + 8, _T("地址"), true); //address
	addColumnAt(getCharWidth() * 2 * 12 + 8, _T("字节"), true); //bytes
	addColumnAt(getCharWidth() * 40, _T("反汇编"), true); //disassembly
	addColumnAt(1000, _T("释义"), true); //comments

	setShowHeader(false); //show header

	loadColumnFromConfig(_T("InstructTrace"));
}


CInstructTrace::~CInstructTrace()
{
	
	delete mDisasm;
	delete mTraceFile;
}

void CInstructTrace::updateColors()
{
	CAbstractTableView::updateColors();
	//CapstoneTokenizer::UpdateColors(); //Already called in disassembly
	mDisasm->UpdateConfig();
	mBackgroundColor = ConfigColor("DisassemblyBackgroundColor");

	mInstructionHighlightColor = ConfigColor("InstructionHighlightColor");
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
	mAutoCommentColor = ConfigColor("DisassemblyAutoCommentColor");
	mAutoCommentBackgroundColor = ConfigColor("DisassemblyAutoCommentBackgroundColor");
	mCommentColor = ConfigColor("DisassemblyCommentColor");
	mCommentBackgroundColor = ConfigColor("DisassemblyCommentBackgroundColor");
}

void CInstructTrace::update()
{
	if (mTraceFile && mTraceFile->Progress() == 100) // && this->isVisible()
	{
		mTraceFile->purgeLastPage();
		setRowCount(mTraceFile->Length());
		reloadData();
	}
}

CString CInstructTrace::getAddrText(dsint cur_addr, char label[MAX_LABEL_SIZE], bool getLabel)
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
		if (DbgGetModuleAt(cur_addr, module) && (tmpStr.Find(_T("JMP.&")) != 0))
		{
			//addrText += " <" + QString(module) + "." + QString(label_) + ">";
			addrText += " <";
			addrText += module;
			addrText += ".";
			addrText += label_;
			addrText += ">";

		}
		else
		{
			//addrText += " <" + QString(label_) + ">";
			addrText += " <";
			addrText += label_;
			addrText += ">";
		}
	}
	else
		*label_ = 0;
	if (label)
		strcpy_s(label, MAX_LABEL_SIZE, label_);
	return addrText;
}

void CInstructTrace::parseFinished()
{
	if (mTraceFile->isError())
	{
		MessageBox(_T("Error when opening run trace file"), _T("Error"), MB_ICONERROR | MB_OK);
		delete mTraceFile;
		mTraceFile = nullptr;
		setRowCount(0);
	}
	else
	{
		if (mTraceFile->HashValue() && DbgIsDebugging())
			if (DbgFunctions()->DbGetHash() != mTraceFile->HashValue())
			{
				CString text;
				text.Format(_T("Checksum is different for current trace file and the debugee. This probably means you have opened a wrong trace file. This trace file is recorded for \"%s\""),
					mTraceFile->ExePath().GetBuffer());
				MessageBox(_T("Error when opening run trace file"), _T("Error"), MB_ICONWARNING | MB_OK);
			}
		setRowCount(mTraceFile->Length());
		//mMRUList->addEntry(mFileName);
		//mMRUList->save();
	}
	reloadData();
}



void CInstructTrace::open(const CString & fileName)
{
	if (mTraceFile != nullptr)
	{
		mTraceFile->Close();
		delete mTraceFile;
	}
	mTraceFile = new CTraceFileReader(this);
	//connect(mTraceFile, SIGNAL(parseFinished()), this, SLOT(parseFinishedSlot()));
	mFileName = fileName;
	mTraceFile->Open(fileName);
}

CString CInstructTrace::paintContent(CDC* painter, Graphics& graphics, dsint rowBase, int rowOffset, int col, int x, int y, int w, int h)
{
	if (!mTraceFile || mTraceFile->Progress() != 100)
	{
		return CString();
	}
	if (mTraceFile->isError())
	{
		GuiAddLogMessage("An error occured when reading trace file.\r\n");
		mTraceFile->Close();
		delete mTraceFile;
		mTraceFile = nullptr;
		setRowCount(0);
		return CString();
	}

	CPen pen;
	CRect colRect(x, y, x + w, y + h);
	CRect viewRect;
	GetClientRect(&viewRect);

	if (mHighlightingMode)
	{
		pen.DeleteObject();
		pen.CreatePen(PS_SOLID, 1, mInstructionHighlightColor.ToCOLORREF());
		CRect tmpRect = viewRect;
		tmpRect.DeflateRect(1, 1);
		
		painter->DrawEdge(&tmpRect, EDGE_ETCHED, BF_RECT);
	}

	duint index = rowBase + rowOffset;
	duint cur_addr;
	cur_addr = mTraceFile->Registers(index).regcontext.cip;
	bool wIsSelected = (index >= mSelection.fromIndex && index <= mSelection.toIndex);
	if (wIsSelected)
	{
		//painter->fillRect(QRect(x, y, w, h), QBrush(mSelectionColor));
		painter->FillSolidRect(&colRect, mSelectionColor.ToCOLORREF());
	}
	if (index >= mTraceFile->Length())
		return CString();
	switch (col)
	{
	case 0: //index
	{
		return getIndexText(index);
	}

	case 1: //address
	{
		CString addrText;
		char label[MAX_LABEL_SIZE] = "";
		if (!DbgIsDebugging())
		{
			addrText = ToPtrString(cur_addr);
			goto NotDebuggingLabel;
		}
		else
			addrText = getAddrText(cur_addr, label, true);
		BPXTYPE bpxtype = DbgGetBpxTypeAt(cur_addr);
		bool isbookmark = DbgGetBookmarkAt(cur_addr);
		//todo: cip
		{
			if (!isbookmark) //no bookmark
			{
				if (*label) //label
				{
					if (bpxtype == bp_none) //label only : fill label background
					{
						//painter->setPen(mLabelColor); //red -> address + label text
						//painter->fillRect(QRect(x, y, w, h), QBrush(mLabelBackgroundColor)); //fill label background
						painter->SetTextColor(mLabelColor.ToCOLORREF());
						painter->FillSolidRect(&colRect, mLabelBackgroundColor.ToCOLORREF());						
					}
					else //label + breakpoint
					{
						if (bpxtype & bp_normal) //label + normal breakpoint
						{
							//painter->setPen(mBreakpointColor);
							//painter->fillRect(QRect(x, y, w, h), QBrush(mBreakpointBackgroundColor)); //fill red
							painter->SetTextColor(mBreakpointColor.ToCOLORREF());
							painter->FillSolidRect(&colRect, mBreakpointBackgroundColor.ToCOLORREF());
						}
						else if (bpxtype & bp_hardware) //label + hardware breakpoint only
						{
							//painter->setPen(mHardwareBreakpointColor);
							//painter->fillRect(QRect(x, y, w, h), QBrush(mHardwareBreakpointBackgroundColor)); //fill ?
							painter->SetTextColor(mHardwareBreakpointColor.ToCOLORREF());
							painter->FillSolidRect(&colRect, mHardwareBreakpointBackgroundColor.ToCOLORREF());
						}
						else //other cases -> do as normal
						{
							//painter->setPen(mLabelColor); //red -> address + label text
							//painter->fillRect(QRect(x, y, w, h), QBrush(mLabelBackgroundColor)); //fill label background
							painter->SetTextColor(mLabelColor.ToCOLORREF());
							painter->FillSolidRect(&colRect, mLabelBackgroundColor.ToCOLORREF());
						}
					}
				}
				else //no label
				{
					if (bpxtype == bp_none) //no label, no breakpoint
					{
					NotDebuggingLabel:
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
							//painter->fillRect(QRect(x, y, w, h), QBrush(background)); //fill background
							painter->FillSolidRect(&colRect, background.ToCOLORREF());
					}
					else //breakpoint only
					{
						if (bpxtype & bp_normal) //normal breakpoint
						{
							//painter->setPen(mBreakpointColor);
							//painter->fillRect(QRect(x, y, w, h), QBrush(mBreakpointBackgroundColor)); //fill red
							painter->SetTextColor(mBreakpointColor.ToCOLORREF());
							painter->FillSolidRect(&colRect, mBreakpointBackgroundColor.ToCOLORREF());
						}
						else if (bpxtype & bp_hardware) //hardware breakpoint only
						{
							//painter->setPen(mHardwareBreakpointColor);
							//painter->fillRect(QRect(x, y, w, h), QBrush(mHardwareBreakpointBackgroundColor)); //fill red
							painter->SetTextColor(mHardwareBreakpointColor.ToCOLORREF());
							painter->FillSolidRect(&colRect, mHardwareBreakpointBackgroundColor.ToCOLORREF());
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
								//painter->fillRect(QRect(x, y, w, h), QBrush(background)); //fill background
								painter->FillSolidRect(&colRect, background.ToCOLORREF());
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
						painter->FillSolidRect(&colRect, mBookmarkBackgroundColor.ToCOLORREF());
					}
					else //label + breakpoint + bookmark
					{
						Color color = mBookmarkBackgroundColor;
						if (!color.GetAlpha()) //we don't want transparent text
							color = mAddressColor;
						//painter->setPen(color);
						painter->SetTextColor(color.ToCOLORREF());
						if (bpxtype & bp_normal) //label + bookmark + normal breakpoint
						{
							//painter->fillRect(QRect(x, y, w, h), QBrush(mBreakpointBackgroundColor)); //fill red
							painter->FillSolidRect(&colRect, mBreakpointBackgroundColor.ToCOLORREF());
						}
						else if (bpxtype & bp_hardware) //label + bookmark + hardware breakpoint only
						{
							//painter->fillRect(QRect(x, y, w, h), QBrush(mHardwareBreakpointBackgroundColor)); //fill ?
							painter->FillSolidRect(&colRect, mHardwareBreakpointBackgroundColor.ToCOLORREF());
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
						painter->FillSolidRect(&colRect, mBookmarkBackgroundColor.ToCOLORREF());
					}
					else //bookmark + breakpoint
					{
						Color color = mBookmarkBackgroundColor;
						if (!color.GetAlpha()) //we don't want transparent text
							color = mAddressColor;
						//painter->setPen(color);
						painter->SetTextColor(color.ToCOLORREF());
						if (bpxtype & bp_normal) //bookmark + normal breakpoint
						{
							//painter->fillRect(QRect(x, y, w, h), QBrush(mBreakpointBackgroundColor)); //fill red
							painter->FillSolidRect(&colRect, mBreakpointBackgroundColor.ToCOLORREF());
						}
						else if (bpxtype & bp_hardware) //bookmark + hardware breakpoint only
						{
							//painter->fillRect(QRect(x, y, w, h), QBrush(mHardwareBreakpointBackgroundColor)); //fill red
							painter->FillSolidRect(&colRect, mHardwareBreakpointBackgroundColor.ToCOLORREF());
						}
						else //other cases (bookmark + memory breakpoint in disassembly) -> do as normal
						{
							//painter->setPen(mBookmarkColor); //black address
							//painter->fillRect(QRect(x, y, w, h), QBrush(mBookmarkBackgroundColor)); //fill bookmark color
							painter->SetTextColor(mBookmarkColor.ToCOLORREF());
							painter->FillSolidRect(&colRect, mBookmarkBackgroundColor.ToCOLORREF());
						}
					}
				}
			}
		}
		CRect textRect = colRect;
		textRect.DeflateRect(4, 0);
		painter->DrawText(addrText, &textRect, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
	}
	return CString();

	case 2: //opcode
	{
		RichTextPainter::List richBytes;
		RichTextPainter::CustomRichText_t curByte;
		RichTextPainter::CustomRichText_t space;
		unsigned char opcodes[16];
		int opcodeSize = 0;
		mTraceFile->OpCode(index, opcodes, &opcodeSize);
		space.text = " ";
		space.flags = RichTextPainter::FlagNone;
		space.highlightWidth = 1;
		space.highlightConnectPrev = true;
		curByte.flags = RichTextPainter::FlagAll;
		curByte.highlightWidth = 1;
		space.highlight = false;
		curByte.highlight = false;

		for (int i = 0; i < opcodeSize; i++)
		{
			if (i)
				richBytes.push_back(space);

			curByte.text = ToByteString(opcodes[i]);
			curByte.textColor = mBytesColor;
			curByte.textBackground = mBytesBackgroundColor;
			richBytes.push_back(curByte);
		}

		RichTextPainter::paintRichText(painter, x, y, getColumnWidth(col), getRowHeight(), 4, richBytes, NULL);
		return CString();
	}

	case 3: //disassembly
	{
		RichTextPainter::List richText;
		unsigned char opcodes[16];
		int opcodeSize = 0;
		mTraceFile->OpCode(index, opcodes, &opcodeSize);

		Instruction_t inst = mDisasm->DisassembleAt(opcodes, opcodeSize, 0, mTraceFile->Registers(index).regcontext.cip, false);

		if (mHighlightToken.text.GetLength())
			CapstoneTokenizer::TokenToRichText(inst.tokens, richText, &mHighlightToken);
		else
			CapstoneTokenizer::TokenToRichText(inst.tokens, richText, 0);
		RichTextPainter::paintRichText(painter, x + 0, y, getColumnWidth(col) - 0, getRowHeight(), 4, richText, NULL);
		return CString();
	}

	case 4: //comments
	{
		if (DbgIsDebugging())
		{
			//TODO: draw arguments
			CString comment;
			bool autoComment = false;
			char label[MAX_LABEL_SIZE] = "";
			if (GetCommentFormat(cur_addr, comment, &autoComment))
			{
				Color backgroundColor;
				if (autoComment)
				{
					//painter->setPen(mAutoCommentColor);
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
				int width = painter->GetTextExtent(comment).cx;
				if (width > w)
					width = w;
				if (width)
					//painter->fillRect(QRect(x, y, width, h), QBrush(backgroundColor)); //fill comment color
					painter->FillSolidRect(&colRect, backgroundColor.ToCOLORREF());
				CRect textRect(x, y, x + width, y + h);
				//painter->drawText(QRect(x, y, width, h), Qt::AlignVCenter | Qt::AlignLeft, comment);
				painter->DrawText(comment, &textRect, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
			}
			else if (DbgGetLabelAt(cur_addr, SEG_DEFAULT, label)) // label but no comment
			{
				CString labelText(label);
				Color backgroundColor;
				//painter->setPen(mLabelColor);
				painter->SetTextColor(mLabelColor.ToCOLORREF());
				backgroundColor = mLabelBackgroundColor;

				//int width = mFontMetrics->width(labelText);
				int width = painter->GetTextExtent(labelText).cx;
				if (width > w)
					width = w;
				if (width)
					//painter->fillRect(QRect(x, y, width, h), QBrush(backgroundColor)); //fill comment color
					painter->FillSolidRect(&colRect, backgroundColor.ToCOLORREF());
				//painter->drawText(QRect(x, y, width, h), Qt::AlignVCenter | Qt::AlignLeft, labelText);
				CRect textRect(x, y, x + width, y + h);				
				painter->DrawText(labelText, &textRect, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);				
			}
		}
		return CString();
	}
	default:
		return CString();
	}
}

CString CInstructTrace::getIndexText(duint index)
{
	CString indexString;
	indexString.Format(_T("%X"), index);
	if (mTraceFile->Length() < 16)
		return indexString;
	int digits;
	digits = floor(log2(mTraceFile->Length() - 1) / 4) + 1;
	digits -= indexString.GetLength();
	while (digits > 0)
	{
		indexString = '0' + indexString;
		digits = digits - 1;
	}
	return indexString;
}

void CInstructTrace::onToggleRunTrace()
{
	if (!DbgIsDebugging())
		return;
	if (DbgValFromString("tr.runtraceenabled()") == 1)
		DbgCmdExec("StopRunTrace");
	else
	{
		CString defaultFileName;
		char moduleName[MAX_MODULE_SIZE];
		CTime t = CTime::GetCurrentTime();
		CString text = t.Format("%m-%d-%H-%M-%S");
		duint defaultModule = DbgValFromString("mod.main()");
		if (DbgFunctions()->ModNameFromAddr(defaultModule, moduleName, false))
		{
			defaultFileName = CA2W((const char*)moduleName, CP_UTF8);
		}
		defaultFileName += "-";
		defaultFileName += text;
		defaultFileName += ArchValue(".trace32", ".trace64");		

		TCHAR pFileName[MAX_PATH];
		GetModuleFileName(GetModuleHandle(NULL), pFileName, MAX_PATH);
		CString csFullPath(pFileName);
		int nPos = csFullPath.ReverseFind(_T('\\'));
		int size = csFullPath.GetLength();
		CString fileDir;
		if (nPos > 0)
			fileDir = csFullPath.Left(nPos);			
		size = csFullPath.GetLength();
		fileDir += _T('\\');
		fileDir += _T("db");
		fileDir += _T('\\');
		fileDir += defaultFileName;

		CInstructTraceBrowserDlg browserDlg(this, _T("选择存储文件"), fileDir);
		
		if (browserDlg.DoModal() == IDOK)
		{
			CString fileName = browserDlg.m_filePath;
			if (fileName.FindOneOf(_T("\"")) != -1 || fileName.FindOneOf(_T("\'")) != -1)
				MessageBox(_T("Error"), _T("File name contains invalid character."), MB_OK);
			else
			{
				CString dbgCMD;
				dbgCMD.Format(_T("StartRunTrace \"%s\""), fileName.GetBuffer());
				DbgCmdExec(CT2A(dbgCMD, CP_UTF8));
			}
		}
	}
}

void CInstructTrace::onOpenFile()
{
	CFileDialog fileDlg(TRUE);
	fileDlg.m_ofn.lpstrTitle = _T("Open run trace file");
#ifdef _WIN64
	fileDlg.m_ofn.lpstrFilter = _T("trace Files(*.trace64)\0*.trace64\0All Files(*.*)\0*.*\0\0");
#else
	fileDlg.m_ofn.lpstrFilter = _T("trace Files(*.trace32)\0*.trace32\0All Files(*.*)\0*.*\0\0");
#endif //_WIN64

	if (fileDlg.DoModal() == IDOK)
	{
		CString fileName = fileDlg.m_ofn.lpstrFile;
		open(fileName);
	}
}

void CInstructTrace::onCloseFile()
{
	if (DbgValFromString("tr.runtraceenabled()") == 1)
		DbgCmdExec("StopRunTrace");
	mTraceFile->Close();
	delete mTraceFile;
	mTraceFile = nullptr;
	reloadData();
}

void CInstructTrace::onCloseAndDelete()
{
	if (MessageBox(_T("Are you really going to delete this file?"), _T("Close and delete"), MB_YESNO) == IDYES)
	{
		if (DbgValFromString("tr.runtraceenabled()") == 1)
			DbgCmdExecDirect("StopRunTrace");
		mTraceFile->Delete();
		delete mTraceFile;
		mTraceFile = nullptr;
		reloadData();
	}
}

void CInstructTrace::onCopySelection(bool copyBytes)
{
	if (mTraceFile == nullptr || mTraceFile->Progress() < 100)
		return;

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
	
	for (unsigned long long i = getSelectionStart(); i <= getSelectionEnd(); i++)
	{
		if (i != getSelectionStart())
			selectionString += "\r\n";
		duint cur_addr = mTraceFile->Registers(i).regcontext.cip;
		unsigned char opcode[16];
		int opcodeSize;
		mTraceFile->OpCode(i, opcode, &opcodeSize);
		Instruction_t inst;
		inst = mDisasm->DisassembleAt(opcode, opcodeSize, cur_addr, 0);
		CString address = getAddrText(cur_addr, 0, addressLen > sizeof(duint) * 2 + 1);
		CString bytes;
		if (copyBytes)
		{
			for (int j = 0; j < opcodeSize; j++)
			{
				if (j)
					bytes += " ";
				bytes += ToByteString((unsigned char)(opcode[j]));
			}
		}
		CString disassembly;
		CString htmlDisassembly;
		if (FALSE)
		{
			/*
			RichTextPainter::List richText;
			if (mHighlightToken.text.length())
				CapstoneTokenizer::TokenToRichText(inst.tokens, richText, &mHighlightToken);
			else
				CapstoneTokenizer::TokenToRichText(inst.tokens, richText, 0);
			RichTextPainter::htmlRichText(richText, htmlDisassembly, disassembly);
			*/			
		}
		else
		{
			for (const auto & token : inst.tokens.tokens)
				disassembly += token.text;
		}
		CString fullComment;
		CString comment;
		bool autocomment;
		if (GetCommentFormat(cur_addr, comment, &autocomment))
		{
			fullComment = " ";
			fullComment += comment;
		}			
		//stream << address.leftJustified(addressLen, QChar(' '), true);
		selectionString += address;
		if (copyBytes)
		{
			//stream << " | " + bytes.leftJustified(bytesLen, QChar(' '), true);
			selectionString += " | ";
			selectionString += bytes;
		}		

		//stream << " | " + disassembly.leftJustified(disassemblyLen, QChar(' '), true) + " |" + fullComment;
		selectionString += " | ";
		selectionString += disassembly;
		selectionString += " |";
		selectionString += fullComment;

		/*
		if (htmlStream)
		{
			*htmlStream << QString("<tr><td>%1</td><td>%2</td><td>").arg(address.toHtmlEscaped(), htmlDisassembly);
			if (!comment.isEmpty())
			{
				if (autocomment)
				{
					*htmlStream << QString("<span style=\"color:%1").arg(mAutoCommentColor.name());
					if (mAutoCommentBackgroundColor != Qt::transparent)
						*htmlStream << QString(";background-color:%2").arg(mAutoCommentBackgroundColor.name());
				}
				else
				{
					*htmlStream << QString("<span style=\"color:%1").arg(mCommentColor.name());
					if (mCommentBackgroundColor != Qt::transparent)
						*htmlStream << QString(";background-color:%2").arg(mCommentBackgroundColor.name());
				}
				*htmlStream << "\">" << comment.toHtmlEscaped() << "</span></td></tr>";
			}
			else
			{
				char label[MAX_LABEL_SIZE];
				if (DbgGetLabelAt(cur_addr, SEG_DEFAULT, label))
				{
					comment = QString::fromUtf8(label);
					*htmlStream << QString("<span style=\"color:%1").arg(mLabelColor.name());
					if (mLabelBackgroundColor != Qt::transparent)
						*htmlStream << QString(";background-color:%2").arg(mLabelBackgroundColor.name());
					*htmlStream << "\">" << comment.toHtmlEscaped() << "</span></td></tr>";
				}
				else
					*htmlStream << "</td></tr>";
			}
		}
		*/
		
	}
	/*
	if (htmlStream)
		*htmlStream << "</table>";
	*/	
	Bridge::CopyToClipboard(selectionString);
	
}

void CInstructTrace::onFollowDisassembly()
{
	if (mTraceFile == nullptr || mTraceFile->Progress() < 100)
		return;

	duint cip = mTraceFile->Registers(getInitialSelection()).regcontext.cip;
	if (DbgMemIsValidReadPtr(cip))
	{
		CString cmd = _T("dis ");
		cmd.Append(ToPtrString(cip));
		DbgCmdExec(CT2A(cmd, CP_UTF8));
	}
	else
	{
		CString msg;
		msg.Format(_T("Cannot follow %s. Address is invalid.\n"), ToPtrString(cip));
		GuiAddStatusBarMessage(CT2A(msg, CP_UTF8));
	}
}

void CInstructTrace::OnHandle(UINT id)
{
	switch (id)
	{
	case ID_InstructTrace_Start:
		onToggleRunTrace();
		break;
	case ID_InstructTrace_Open:
		onOpenFile();
		break;
	case ID_InstructTrace_Close:
		onCloseFile();
		break;
	case ID_InstructTrace_CloseAndDelete:
		onCloseAndDelete();
		break;
	case ID_InstructTrace_Copy_Selection:
		onCopySelection(true);
	case ID_InstructTrace_FollowInDisassembler:
		onFollowDisassembly();
		break;
	default:
		break;
	}
}

void CInstructTrace::expandSelectionUpTo(duint to)
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

void CInstructTrace::setSingleSelection(duint index)
{
	mSelection.firstSelectedIndex = index;
	mSelection.fromIndex = index;
	mSelection.toIndex = index;
}

duint CInstructTrace::getInitialSelection()
{
	return mSelection.firstSelectedIndex;
}

duint CInstructTrace::getSelectionStart()
{
	return mSelection.fromIndex;
}

duint CInstructTrace::getSelectionEnd()
{
	return mSelection.toIndex;
}

BEGIN_MESSAGE_MAP(CInstructTrace, CAbstractTableView)
	ON_COMMAND_RANGE(ID_InstructTrace_Start, ID_InstructTrace_FollowInDisassembler, &CInstructTrace::OnHandle)
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


void CInstructTrace::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: 在此处添加消息处理程序代码

	CMenu PopupMenu;
	// 创建弹出菜单
	BOOL bRet = PopupMenu.CreatePopupMenu();
	if (DbgIsDebugging())
	{	
		CString text;
		text = DbgValFromString("tr.runtraceenabled()") == 1 ? _T("停止指令跟踪") : _T("启动指令跟踪");
		PopupMenu.AppendMenu(MF_ENABLED, ID_InstructTrace_Start, text);
	}

	if (!mTraceFile)
	{
		PopupMenu.AppendMenu(MF_ENABLED, ID_InstructTrace_Open, _T("打开"));
	}
	else
	{
		PopupMenu.AppendMenu(MF_ENABLED, ID_InstructTrace_Close, _T("关闭"));
		PopupMenu.AppendMenu(MF_ENABLED, ID_InstructTrace_CloseAndDelete, _T("关闭且删除"));
		PopupMenu.AppendMenu(MF_SEPARATOR);
	}
	

	/*
	mMenuBuilder->addMenu(makeMenu(DIcon("recentfiles.png"), tr("Recent Files")), [this](QMenu * menu)
	{
		if (mTraceFile == nullptr)
		{
			mMRUList->appendMenu(menu);
			return true;
		}
		else
			return false;
	});
	*/

	CMenu SecondLevelPopupMenu;
	// 创建二级弹出菜单
	bRet = SecondLevelPopupMenu.CreateMenu();
	SecondLevelPopupMenu.AppendMenu(MF_ENABLED, ID_InstructTrace_Copy_Selection, _T("Selection"));
	SecondLevelPopupMenu.AppendMenu(MF_ENABLED, ID_InstructTrace_Copy_Selection_toFile, _T("election to &File"));
	SecondLevelPopupMenu.AppendMenu(MF_ENABLED, ID_InstructTrace_Copy_Selection_noBytes, _T("Selection (&No Bytes)"));
	SecondLevelPopupMenu.AppendMenu(MF_ENABLED, ID_InstructTrace_Copy_Selection_toFile_noBytes, _T("Selection to File (No Bytes)"));
	SecondLevelPopupMenu.AppendMenu(MF_ENABLED, ID_InstructTrace_Copy_Address, _T("Address"));

	if (mTraceFile != nullptr && mTraceFile->Progress() == 100 && mTraceFile->Length() > 0 && DbgIsDebugging())
	{
		SecondLevelPopupMenu.AppendMenu(MF_ENABLED, ID_InstructTrace_Copy_RVA, _T("RVA"));
		SecondLevelPopupMenu.AppendMenu(MF_ENABLED, ID_InstructTrace_Copy_FileOffset, _T("File Offset"));
	}
	
	SecondLevelPopupMenu.AppendMenu(MF_ENABLED, ID_InstructTrace_Copy_Disassembly, _T("Disassembly"));
	SecondLevelPopupMenu.AppendMenu(MF_ENABLED, ID_InstructTrace_Copy_Index, _T("Index"));

	if (mTraceFile != nullptr && mTraceFile->Progress() == 100 && mTraceFile->Length() > 0)
	{
		PopupMenu.AppendMenu(MF_POPUP, (UINT)SecondLevelPopupMenu.m_hMenu, _T("复制"));
		PopupMenu.AppendMenu(MF_ENABLED, ID_InstructTrace_FollowInDisassembler, _T("反汇编窗口跟随"));
	}
	/*
	mBreakpointMenu = new BreakpointMenu(this, getActionHelperFuncs(), [this, isValid]()
	{
		if (isValid(nullptr))
			return mTraceFile->Registers(getInitialSelection()).regcontext.cip;
		else
			return (duint)0;
	});
	mBreakpointMenu->build(mMenuBuilder);
	mMenuBuilder->addAction(makeShortcutAction(DIcon("label.png"), tr("Label Current Address"), SLOT(setLabelSlot()), "ActionSetLabel"), isDebugging);
	mMenuBuilder->addAction(makeShortcutAction(DIcon("comment.png"), tr("&Comment"), SLOT(setCommentSlot()), "ActionSetComment"), isDebugging);
	mMenuBuilder->addAction(makeShortcutAction(DIcon("highlight.png"), tr("&Highlighting mode"), SLOT(enableHighlightingModeSlot()), "ActionHighlightingMode"), isValid);
	MenuBuilder* gotoMenu = new MenuBuilder(this, isValid);
	gotoMenu->addAction(makeShortcutAction(DIcon("goto.png"), tr("Expression"), SLOT(gotoSlot()), "ActionGotoExpression"), isValid);
	gotoMenu->addAction(makeShortcutAction(DIcon("previous.png"), tr("Previous"), SLOT(gotoPreviousSlot()), "ActionGotoPrevious"), [this](QMenu*)
	{
		return mHistory.historyHasPrev();
	});
	gotoMenu->addAction(makeShortcutAction(DIcon("next.png"), tr("Next"), SLOT(gotoNextSlot()), "ActionGotoNext"), [this](QMenu*)
	{
		return mHistory.historyHasNext();
	});
	mMenuBuilder->addMenu(makeMenu(DIcon("goto.png"), tr("Go to")), gotoMenu);

	MenuBuilder* searchMenu = new MenuBuilder(this, isValid);
	searchMenu->addAction(makeAction(DIcon("search_for_constant.png"), tr("Constant"), SLOT(searchConstantSlot())));
	searchMenu->addAction(makeAction(DIcon("memory-map.png"), tr("Memory Reference"), SLOT(searchMemRefSlot())));
	mMenuBuilder->addMenu(makeMenu(DIcon("search.png"), tr("&Search")), searchMenu);

	// The following code adds a menu to view the information about currently selected instruction. When info box is completed, remove me.
	MenuBuilder* infoMenu = new MenuBuilder(this, [this, isValid](QMenu * menu)
	{
		duint MemoryAddress[MAX_MEMORY_OPERANDS];
		duint MemoryOldContent[MAX_MEMORY_OPERANDS];
		duint MemoryNewContent[MAX_MEMORY_OPERANDS];
		bool MemoryIsValid[MAX_MEMORY_OPERANDS];
		int MemoryOperandsCount;
		unsigned long long index;

		if (!isValid(nullptr))
			return false;
		index = getInitialSelection();
		MemoryOperandsCount = mTraceFile->MemoryAccessCount(index);
		if (MemoryOperandsCount > 0)
		{
			mTraceFile->MemoryAccessInfo(index, MemoryAddress, MemoryOldContent, MemoryNewContent, MemoryIsValid);
			bool RvaDisplayEnabled = mRvaDisplayEnabled;
			char nolabel[MAX_LABEL_SIZE];
			mRvaDisplayEnabled = false;
			for (int i = 0; i < MemoryOperandsCount; i++)
			{
				menu->addAction(QString("%1: %2 -> %3").arg(getAddrText(MemoryAddress[i], nolabel, false)).arg(ToPtrString(MemoryOldContent[i])).arg(ToPtrString(MemoryNewContent[i])));
			}
			mRvaDisplayEnabled = RvaDisplayEnabled;
			menu->addSeparator();
		}
#define addReg(str, reg) if(index + 1 < mTraceFile->Length()){menu->addAction(QString(str ":%1 -> %2").arg(ToPtrString(mTraceFile->Registers(index).regcontext.##reg)) \
    .arg(ToPtrString(mTraceFile->Registers(index + 1).regcontext.##reg))); }else{ menu->addAction(QString(str ":%1").arg(ToPtrString(mTraceFile->Registers(index).regcontext.##reg))); }
		addReg(ArchValue("EAX", "RAX"), cax)
			addReg(ArchValue("EBX", "RBX"), cbx)
			addReg(ArchValue("ECX", "RCX"), ccx)
			addReg(ArchValue("EDX", "RDX"), cdx)
			addReg(ArchValue("ESP", "RSP"), csp)
			addReg(ArchValue("EBP", "RBP"), cbp)
			addReg(ArchValue("ESI", "RSI"), csi)
			addReg(ArchValue("EDI", "RDI"), cdi)
#ifdef _WIN64
			addReg("R8", r8)
			addReg("R9", r9)
			addReg("R10", r10)
			addReg("R11", r11)
			addReg("R12", r12)
			addReg("R13", r13)
			addReg("R14", r14)
			addReg("R15", r15)
#endif //_WIN64
			addReg(ArchValue("EIP", "RIP"), cip)
			addReg(ArchValue("EFLAGS", "RFLAGS"), eflags)
			menu->addSeparator();
		menu->addAction(QString("ThreadID: %1").arg(mTraceFile->ThreadId(index)));
		if (index + 1 < mTraceFile->Length())
		{
			menu->addAction(QString("LastError: %1 -> %2").arg(ToPtrString(mTraceFile->Registers(index).lastError.code)).arg(ToPtrString(mTraceFile->Registers(index + 1).lastError.code)));
		}
		else
		{
			menu->addAction(QString("LastError: %1").arg(ToPtrString(mTraceFile->Registers(index).lastError.code)));
		}
		return true;
	});
	mMenuBuilder->addMenu(makeMenu(tr("Information")), infoMenu);
	*/

	// 显示菜单
	int nCmd = PopupMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this); //TPM_RETURNCMD
}


BOOL CInstructTrace::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 在此添加专用代码和/或调用基类
	UINT traceMessage = message - WM_USER;
	switch (traceMessage)
	{
	case TraceFileParserThread_Exit:
		mTraceFile->parseFinished();
		return TRUE;
	case TraceFileParser_Finish:
		parseFinished();
		return TRUE;
	default:
		return CAbstractTableView::OnWndMsg(message, wParam, lParam, pResult);
	}	
}


void CInstructTrace::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	duint index = getIndexOffsetFromY(transY(point.y)) + getTableOffset();
	if (getGuiState() == CAbstractTableView::NoState && mTraceFile != nullptr && mTraceFile->Progress() == 100)
	{
		if (index < getRowCount())
		{
			SetCapture();
			if (mHighlightingMode || mPermanentHighlightingMode)
			{
				if (getColumnIndexFromX(point.x) == 3) //click in instruction column
				{
					Instruction_t inst;
					unsigned char opcode[16];
					int opcodeSize;
					mTraceFile->OpCode(index, opcode, &opcodeSize);
					inst = mDisasm->DisassembleAt(opcode, opcodeSize, mTraceFile->Registers(index).regcontext.cip, 0);
					CapstoneTokenizer::SingleToken token;
					if (CapstoneTokenizer::TokenFromX(inst.tokens, token, point.x - getColumnPosition(3), &lf))
					{
						if (CapstoneTokenizer::IsHighlightableToken(token))
						{
							if (!CapstoneTokenizer::TokenEquals(&token, &mHighlightToken))
								mHighlightToken = token;
							else
								mHighlightToken = CapstoneTokenizer::SingleToken();
						}
						else if (!mPermanentHighlightingMode)
						{
							mHighlightToken = CapstoneTokenizer::SingleToken();
						}
					}
					else if (!mPermanentHighlightingMode)
					{
						mHighlightToken = CapstoneTokenizer::SingleToken();
					}
				}
				else if (!mPermanentHighlightingMode)
				{
					mHighlightToken = CapstoneTokenizer::SingleToken();
				}
				if (mHighlightingMode) //disable highlighting mode after clicked
				{
					mHighlightingMode = false;
					reloadData();
				}
			}
			if (GetKeyState(VK_SHIFT) < 0)
				expandSelectionUpTo(index);
			else
				setSingleSelection(index);
			mHistory.addVaToHistory(index);
			Invalidate();
			return;
		}
		
	}

	CAbstractTableView::OnLButtonDown(nFlags, point);
}


void CInstructTrace::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	dsint index;
	CRect viewRect;
	GetClientRect(&viewRect);
	if (point.y < 0)
		index = 0;	
	else if (point.y > viewRect.Height())
		index = getRowCount() - 1;
	else
		index  = getIndexOffsetFromY(transY(point.y)) + getTableOffset();

	if ((GetAsyncKeyState(VK_LBUTTON) < 0) && getGuiState() == CAbstractTableView::NoState && mTraceFile != nullptr && mTraceFile->Progress() == 100)
	{
		if (index < getRowCount())
		{
			setSingleSelection(getInitialSelection());
			expandSelectionUpTo(index);
		}
		if (point.y < 0)
		{
			SendMessage(WM_VSCROLL, SB_LINEUP, 0);
		}
		else if (point.y > getTableHeight())
		{
			SendMessage(WM_VSCROLL, SB_LINEDOWN, 0);
		}
		Invalidate();
	}
	CAbstractTableView::OnMouseMove(nFlags, point);
}


void CInstructTrace::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	ReleaseCapture();

	CAbstractTableView::OnLButtonUp(nFlags, point);
}
