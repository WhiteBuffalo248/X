#include "stdafx.h"
#include "HexDump.h"

#include "StringUtil.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

static int getStringMaxLength(CHexDump::DataDescriptor desc);
static int byteStringMaxLength(CHexDump::ByteViewMode mode);
static int wordStringMaxLength(CHexDump::WordViewMode mode);
static int dwordStringMaxLength(CHexDump::DwordViewMode mode);
static int qwordStringMaxLength(CHexDump::QwordViewMode mode);
static int twordStringMaxLength(CHexDump::TwordViewMode mode);


static int getStringMaxLength(CHexDump::DataDescriptor desc)
{
	int wLength = 0;

	switch (desc.itemSize)
	{
	case CHexDump::Byte:
	{
		wLength = byteStringMaxLength(desc.byteMode);
	}
	break;

	case CHexDump::Word:
	{
		wLength = wordStringMaxLength(desc.wordMode);
	}
	break;

	case CHexDump::Dword:
	{
		wLength = dwordStringMaxLength(desc.dwordMode);
	}
	break;

	case CHexDump::Qword:
	{
		wLength = qwordStringMaxLength(desc.qwordMode);
	}
	break;

	case CHexDump::Tword:
	{
		wLength = twordStringMaxLength(desc.twordMode);
	}
	break;

	default:
	{

	}
	break;
	}

	return wLength;
}

static int byteStringMaxLength(CHexDump::ByteViewMode mode)
{
	int wLength = 0;

	switch (mode)
	{
	case CHexDump::HexByte:
	{
		wLength = 2;
	}
	break;

	case CHexDump::AsciiByte:
	{
		wLength = 1;
	}
	break;

	case CHexDump::SignedDecByte:
	{
		wLength = 4;
	}
	break;

	case CHexDump::UnsignedDecByte:
	{
		wLength = 3;
	}
	break;

	default:
	{

	}
	break;
	}

	return wLength;
}

static int wordStringMaxLength(CHexDump::WordViewMode mode)
{
	int wLength = 0;

	switch (mode)
	{
	case CHexDump::HexWord:
	{
		wLength = 4;
	}
	break;

	case CHexDump::UnicodeWord:
	{
		wLength = 1;
	}
	break;

	case CHexDump::SignedDecWord:
	{
		wLength = 6;
	}
	break;

	case CHexDump::UnsignedDecWord:
	{
		wLength = 5;
	}
	break;

	default:
	{

	}
	break;
	}

	return wLength;
}

static int dwordStringMaxLength(CHexDump::DwordViewMode mode)
{
	int wLength = 0;

	switch (mode)
	{
	case CHexDump::HexDword:
	{
		wLength = 8;
	}
	break;

	case CHexDump::SignedDecDword:
	{
		wLength = 11;
	}
	break;

	case CHexDump::UnsignedDecDword:
	{
		wLength = 10;
	}
	break;

	case CHexDump::FloatDword:
	{
		wLength = 13;
	}
	break;

	default:
	{

	}
	break;
	}

	return wLength;
}

static int qwordStringMaxLength(CHexDump::QwordViewMode mode)
{
	int wLength = 0;

	switch (mode)
	{
	case CHexDump::HexQword:
	{
		wLength = 16;
	}
	break;

	case CHexDump::SignedDecQword:
	{
		wLength = 20;
	}
	break;

	case CHexDump::UnsignedDecQword:
	{
		wLength = 20;
	}
	break;

	case CHexDump::DoubleQword:
	{
		wLength = 23;
	}
	break;

	default:
	{

	}
	break;
	}

	return wLength;
}

static int twordStringMaxLength(CHexDump::TwordViewMode mode)
{
	int wLength = 0;

	switch (mode)
	{
	case CHexDump::FloatTword:
	{
		wLength = 29;
	}
	break;

	default:
	{

	}
	break;
	}

	return wLength;
}

void CHexDump::byteToString(duint rva, byte_t byte, ByteViewMode mode, RichTextPainter::CustomRichText_t & richText)
{
	CString wStr;

	switch (mode)
	{
	case HexByte:
	{
		wStr = ToByteString((unsigned char)byte);
	}
	break;

	case AsciiByte:
	{
		/*
		QChar wChar = QChar::fromLatin1((char)byte);

		if (wChar.isPrint())
			wStr = QString(wChar);
		else if (!wChar.unicode())
			wStr = mNullReplace;
		else
			wStr = mNonprintReplace;
			*/
		if (isprint(byte))
			wStr = (char)byte;
		else if (iswprint(byte))
		{
			wStr = (wchar_t)byte;
		}
		else
			wStr = '.';
	}
	break;

	case SignedDecByte:
	{
		//wStr = QString::number((int)((char)byte));
		wStr.Format(_T("%d"), (int)((char)byte));
	}
	break;

	case UnsignedDecByte:
	{
		//wStr = QString::number((unsigned int)byte);
		wStr.Format(_T("%d"), ((unsigned int)byte));
	}
	break;

	default:
	{

	}
	break;
	}

	richText.text = wStr;

	DBGPATCHINFO patchInfo;
	if (DbgFunctions()->PatchGetEx(rvaToVa(rva), &patchInfo))
	{
		if (byte == patchInfo.newbyte)
		{
			richText.textColor = mModifiedBytesColor;
			richText.textBackground = mModifiedBytesBackgroundColor;
		}
		else
		{
			richText.textColor = mRestoredBytesColor;
			richText.textBackground = mRestoredBytesBackgroundColor;
		}
	}
	else
	{
		switch (byte)
		{
		case 0x00:
			richText.textColor = mByte00Color;
			richText.textBackground = mByte00BackgroundColor;
			break;
		case 0x7F:
			richText.textColor = mByte7FColor;
			richText.textBackground = mByte7FBackgroundColor;
			break;
		case 0xFF:
			richText.textColor = mByteFFColor;
			richText.textBackground = mByteFFBackgroundColor;
			break;
		default:
			if (isprint(byte) || isspace(byte))
			{
				richText.textColor = mByteIsPrintColor;
				richText.textBackground = mByteIsPrintBackgroundColor;
			}
			break;
		}
	}
}

void CHexDump::wordToString(duint rva, uint16 word, WordViewMode mode, RichTextPainter::CustomRichText_t & richText)
{
	CString wStr;

	switch (mode)
	{
	case HexWord:
	{
		wStr = ToWordString((unsigned short)word);
	}
	break;

	case UnicodeWord:
	{
		/*
		QChar wChar = QChar::fromLatin1((char)word & 0xFF);
		if (wChar.isPrint() && (word >> 8) == 0)
			wStr = QString(wChar);
		else if (!wChar.unicode())
			wStr = mNullReplace;
		else
			wStr = mNonprintReplace;
			*/
		TCHAR wChar = (char)word & 0xFF;
		wStr = wChar;
		
		if (iswprint(word))
		wStr = (wchar_t)word;
		else
		wStr = '.';		
	}
	break;

	case SignedDecWord:
	{
		//wStr = QString::number((int)((short)word));
		wStr.Format(_T("%d"), (int)((short)word));
	}
	break;

	case UnsignedDecWord:
	{
		//wStr = QString::number((unsigned int)word);
		wStr.Format(_T("%d"), (unsigned int)word);
	}
	break;

	default:
	{

	}
	break;
	}

	richText.text = wStr;
}

void CHexDump::dwordToString(duint rva, uint32 dword, DwordViewMode mode, RichTextPainter::CustomRichText_t & richText)
{
	CString wStr;

	switch (mode)
	{
	case HexDword:
	{
		//wStr = QString("%1").arg((unsigned int)dword, 8, 16, QChar('0')).toUpper();
		wStr.Format(_T("%08X"), (unsigned int)dword);
	}
	break;

	case SignedDecDword:
	{
		//wStr = QString::number((int)dword);
		wStr.Format(_T("%d"), (int)dword);
	}
	break;

	case UnsignedDecDword:
	{
		//wStr = QString::number((unsigned int)dword);
		wStr.Format(_T("%d"), (unsigned int)dword);
	}
	break;

	case FloatDword:
	{
		wStr = ToFloatString(&dword);
	}
	break;

	default:
	{

	}
	break;
	}

	richText.text = wStr;
}

void CHexDump::qwordToString(duint rva, uint64 qword, QwordViewMode mode, RichTextPainter::CustomRichText_t & richText)
{
	CString wStr;

	switch (mode)
	{
	case HexQword:
	{
		//wStr = QString("%1").arg((unsigned long long)qword, 16, 16, QChar('0')).toUpper();
		wStr.Format(_T("%016X"), (int)qword);
	}
	break;

	case SignedDecQword:
	{
		//wStr = QString::number((long long)qword);
		wStr.Format(_T("%ld"), (long long)qword);
	}
	break;

	case UnsignedDecQword:
	{
		//wStr = QString::number((unsigned long long)qword);
		wStr.Format(_T("%ld"), (unsigned long long)qword);
	}
	break;

	case DoubleQword:
	{
		wStr = ToDoubleString(&qword);
	}
	break;

	default:
	{

	}
	break;
	}

	richText.text = wStr;
}

void CHexDump::twordToString(duint rva, void* tword, TwordViewMode mode, RichTextPainter::CustomRichText_t & richText)
{
	
	CString wStr;

	switch (mode)
	{
	case FloatTword:
	{
		wStr = ToLongDoubleString(tword);
	}
	break;

	default:
	{

	}
	break;
	}

	richText.text = wStr;
}

CHexDump::CHexDump()
{
	
	mMemPage = new MemoryPage(0, 0);
	

	clearDescriptors();

	mBackgroundColor = ConfigColor("HexDumpBackgroundColor");
	mTextColor = ConfigColor("HexDumpTextColor");
	mSelectionColor = ConfigColor("HexDumpSelectionColor");
}


CHexDump::~CHexDump()
{
	delete mMemPage;
}

void CHexDump::updateFonts()
{
	lf = ConfigFont("HexDump");
}

void CHexDump::updateColors()
{
	CAbstractTableView::updateColors();

	mBackgroundColor = ConfigColor("HexDumpBackgroundColor");
	mTextColor = ConfigColor("HexDumpTextColor");
	mSelectionColor = ConfigColor("HexDumpSelectionColor");

	mModifiedBytesColor = ConfigColor("HexDumpModifiedBytesColor");
	mModifiedBytesBackgroundColor = ConfigColor("HexDumpModifiedBytesBackgroundColor");
	mRestoredBytesColor = ConfigColor("HexDumpRestoredBytesColor");
	mRestoredBytesBackgroundColor = ConfigColor("HexDumpRestoredBytesBackgroundColor");
	mByte00Color = ConfigColor("HexDumpByte00Color");
	mByte00BackgroundColor = ConfigColor("HexDumpByte00BackgroundColor");
	mByte7FColor = ConfigColor("HexDumpByte7FColor");
	mByte7FBackgroundColor = ConfigColor("HexDumpByte7FBackgroundColor");
	mByteFFColor = ConfigColor("HexDumpByteFFColor");
	mByteFFBackgroundColor = ConfigColor("HexDumpByteFFBackgroundColor");
	mByteIsPrintColor = ConfigColor("HexDumpByteIsPrintColor");
	mByteIsPrintBackgroundColor = ConfigColor("HexDumpByteIsPrintBackgroundColor");

	mUserModuleCodePointerHighlightColor = ConfigColor("HexDumpUserModuleCodePointerHighlightColor");
	mUserModuleDataPointerHighlightColor = ConfigColor("HexDumpUserModuleDataPointerHighlightColor");
	mSystemModuleCodePointerHighlightColor = ConfigColor("HexDumpSystemModuleCodePointerHighlightColor");
	mSystemModuleDataPointerHighlightColor = ConfigColor("HexDumpSystemModuleDataPointerHighlightColor");
	mUnknownCodePointerHighlightColor = ConfigColor("HexDumpUnknownCodePointerHighlightColor");
	mUnknownDataPointerHighlightColor = ConfigColor("HexDumpUnknownDataPointerHighlightColor");

	reloadData();
}

int CHexDump::getBytePerRowCount() const
{
	return mDescriptor.GetHead().itemCount * (int)(mDescriptor.GetHead().data.itemSize);
}

int CHexDump::getItemPixelWidth(ColumnDescriptor desc)
{
	int wCharWidth = getCharWidth();
	int wItemPixWidth = getStringMaxLength(desc.data) * wCharWidth + wCharWidth;

	return wItemPixWidth;
}

void CHexDump::appendDescriptor(int width, CString title, bool clickable, ColumnDescriptor descriptor)
{
	addColumnAt(width, title, clickable);
	mDescriptor.AddTail(descriptor);
}

//Clears the descriptors, append a new descriptor and fix the tableOffset (use this instead of clearDescriptors()
void CHexDump::appendResetDescriptor(int width, CString title, bool clickable, ColumnDescriptor descriptor)
{
	setAllowPainting(false);
	if (mDescriptor.GetSize())
	{
		dsint wRVA = getTableOffset() * getBytePerRowCount() - mByteOffset;
		clearDescriptors();
		appendDescriptor(width, title, clickable, descriptor);
		printDumpAt(rvaToVa(wRVA), true, false);
	}
	else
		appendDescriptor(width, title, clickable, descriptor);
	setAllowPainting(true);
}

void CHexDump::copyColumnWidth()
{
	m_vColumnWidth.clear();
	for (int i = 0; i < getColumnCount(); i++)
		m_vColumnWidth.push_back(getColumnWidth(i));
}

int CHexDump::getLastColumnWidth(int index)
{
	int size = m_vColumnWidth.size();
	if (size && index < size)
		return m_vColumnWidth[index];
	else
		return 200;
}

void CHexDump::clearDescriptors()
{
	deleteAllColumns();
	mDescriptor.RemoveAll();
	//int charwidth = getCharWidth();
	//addColumnAt(8 + charwidth * 2 * sizeof(duint), _T("Address"), false); //address
	
	addColumnAt(getLastColumnWidth(0), _T("Address"), true); //address
	
}

/************************************************************************************
Selection Management
************************************************************************************/
void CHexDump::expandSelectionUpTo(dsint rva)
{
	if (rva < mSelection.firstSelectedIndex)
	{
		mSelection.fromIndex = rva;
		mSelection.toIndex = mSelection.firstSelectedIndex;
		//emit selectionUpdated();
	}
	else if (rva > mSelection.firstSelectedIndex)
	{
		mSelection.fromIndex = mSelection.firstSelectedIndex;
		mSelection.toIndex = rva;
		//emit selectionUpdated();
	}
	else if (rva == mSelection.firstSelectedIndex)
	{
		setSingleSelection(rva);
	}
}

void CHexDump::setSingleSelection(dsint rva)
{
	mSelection.firstSelectedIndex = rva;
	mSelection.fromIndex = rva;
	mSelection.toIndex = rva;
	//emit selectionUpdated();
}

duint CHexDump::rvaToVa(dsint rva) const
{
	return mMemPage->va(rva);
}

bool CHexDump::isSelected(dsint rva) const
{
	return rva >= mSelection.fromIndex && rva <= mSelection.toIndex;
}

void CHexDump::toString(DataDescriptor desc, duint rva, byte_t* data, RichTextPainter::CustomRichText_t & richText) //convert data to string
{
	switch (desc.itemSize)
	{
	case Byte:
	{
		byteToString(rva, *((byte_t*)data), desc.byteMode, richText);
	}
	break;

	case Word:
	{
		wordToString(rva, *((uint16*)data), desc.wordMode, richText);
	}
	break;

	case Dword:
	{
		dwordToString(rva, *((uint32*)data), desc.dwordMode, richText);
	}
	break;

	case Qword:
	{
		qwordToString(rva, *((uint64*)data), desc.qwordMode, richText);
	}
	break;

	case Tword:
	{
		twordToString(rva, data, desc.twordMode, richText);
	}
	break;

	default:
	{

	}
	break;
	}

	if (desc.itemSize == Byte) //byte patches are handled in byteToString
		return;

	dsint start = rvaToVa(rva);
	dsint end = start + (int)(desc.itemSize) - 1;
	if (DbgFunctions()->PatchInRange(start, end))
		richText.textColor = ConfigColor("HexDumpModifiedBytesColor");
}

int CHexDump::getSizeOf(DataSize size)
{
	return int(size);
}

dsint CHexDump::getInitialSelection() const
{
	return mSelection.firstSelectedIndex;
}

dsint CHexDump::getSelectionStart() const
{
	return mSelection.fromIndex;
}

dsint CHexDump::getSelectionEnd() const
{
	return mSelection.toIndex;
}

int CHexDump::getItemPixelWidth(ColumnDescriptor desc) const
{
	int wCharWidth = getCharWidth();
	int wItemPixWidth = getStringMaxLength(desc.data) * wCharWidth + wCharWidth;

	return wItemPixWidth;
}

int CHexDump::getItemIndexFromX(int x) const
{
	int wColIndex = getColumnIndexFromX(x);

	if (wColIndex > 0)
	{
		int wColStartingPos = getColumnPosition(wColIndex);
		int wRelativeX = x - wColStartingPos;

		int wItemPixWidth = getItemPixelWidth(mDescriptor.GetAt(mDescriptor.FindIndex(wColIndex - 1)));
		int wCharWidth = getCharWidth();
		if (wItemPixWidth == wCharWidth)
			wRelativeX -= 4;

		int wItemIndex = wRelativeX / wItemPixWidth;

		wItemIndex = wItemIndex < 0 ? 0 : wItemIndex;
		wItemIndex = wItemIndex > (mDescriptor.GetAt(mDescriptor.FindIndex(wColIndex - 1)).itemCount - 1) ? (mDescriptor.GetAt(mDescriptor.FindIndex(wColIndex - 1)).itemCount - 1) : wItemIndex;

		return wItemIndex;
	}
	else
	{
		return 0;
	}
}

dsint CHexDump::getItemStartingAddress(int x, int y)
{
	int wRowOffset = getIndexOffsetFromY(transY(y));
	int wItemIndex = getItemIndexFromX(x);
	int wColIndex = getColumnIndexFromX(x);
	dsint wStartingAddress = 0;

	if (wColIndex > 0)
	{
		wColIndex -= 1;
		wStartingAddress = (getTableOffset() + wRowOffset) * (mDescriptor.GetAt(mDescriptor.FindIndex(wColIndex)).itemCount * getSizeOf(mDescriptor.GetAt(mDescriptor.FindIndex(wColIndex)).data.itemSize))
			+ wItemIndex * getSizeOf(mDescriptor.GetAt(mDescriptor.FindIndex(wColIndex)).data.itemSize) - mByteOffset;
	}

	return wStartingAddress;
}

CString CHexDump::makeAddrText(duint va) const
{
	char label[MAX_LABEL_SIZE] = "";
	CString addrText;
	dsint cur_addr = va;
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
			addrText += _T("$+");
			addrText.AppendFormat(_T("%-16X"), rva);
#else
			//addrText = "$+" + CString("%1").arg(rva, -7, 16, QChar(' ')).toUpper();
			addrText += _T("$+");
			addrText.AppendFormat(_T("%-8X"), rva);
#endif //_WIN64
		}
		else if (rva < 0)
		{
#ifdef _WIN64
			//addrText = "$-" + QString("%1").arg(-rva, -15, 16, QChar(' ')).toUpper();
			addrText += _T("$-");
			addrText.AppendFormat(_T("%-16X"), -rva);
#else
			//addrText = "$-" + QString("%1").arg(-rva, -7, 16, QChar(' ')).toUpper();
			addrText += _T("$-");
			addrText.AppendFormat(_T("%-8X"), -rva);
#endif //_WIN64
		}
	}
	addrText += ToPtrString(cur_addr);
	if (DbgGetLabelAt(cur_addr, SEG_DEFAULT, label)) //has label
	{
		char module[MAX_MODULE_SIZE] = "";
		//if (DbgGetModuleAt(cur_addr, module) && !QString(label).startsWith("JMP.&"))
		CString tmpStr(label);
		if (DbgGetModuleAt(cur_addr, module) && (tmpStr.Find(_T("JMP.&")) != 0))
		{
			//addrText += " <" + QString(module) + "." + QString(label) + ">";
			addrText += " <";
			addrText += module;
			addrText += ".";
			addrText += label;
			addrText += ">";
		}
			
		else
		{
			//addrText += " <" + QString(label) + ">";
			addrText += " <";
			addrText += label;
			addrText += ">";
		}
			
	}
	else
		*label = 0;
	return std::move(addrText);
}

void CHexDump::getColumnRichText(int col, dsint rva, RichTextPainter::List & richText)
{
	RichTextPainter::CustomRichText_t curData;
	curData.highlight = false;
	curData.flags = RichTextPainter::FlagAll;
	curData.textColor = mTextColor;
	curData.textBackground = Color::Transparent;
	curData.highlightColor = Color::Transparent;
	if (col == 1)
		curData.highlightConnectPrev = true;

	RichTextPainter::CustomRichText_t spaceData;
	spaceData.highlight = false;
	spaceData.flags = RichTextPainter::FlagNone;
	spaceData.highlightColor = Color::Transparent;

	if (!col) //address
	{
		curData.text = makeAddrText(rvaToVa(rva));
		richText.push_back(curData);
	}
	else if (mDescriptor.GetAt(mDescriptor.FindIndex(col - 1)).isData == true)
	{
		const ColumnDescriptor & desc = mDescriptor.GetAt(mDescriptor.FindIndex(col - 1));
		int wI;

		int wByteCount = (int)(desc.data.itemSize);
		int wBufferByteCount = desc.itemCount * wByteCount;

		wBufferByteCount = wBufferByteCount > (dsint)(mMemPage->getSize() - rva) ? mMemPage->getSize() - rva : wBufferByteCount;

		byte_t* wData = new byte_t[wBufferByteCount];
		mMemPage->read(wData, rva, wBufferByteCount);

		if (desc.textCodec) //convert the row bytes to unicode
		{
			//This might produce invalid characters in variables-width encodings. This is currently ignored.
			/*
			curData.text = desc.textCodec->toUnicode(QByteArray((const char*)wData, wBufferByteCount));
			curData.text.replace('\t', "\\t");
			curData.text.replace('\f', "\\f");
			curData.text.replace('\v', "\\v");
			curData.text.replace('\n', "\\n");
			curData.text.replace('\r', "\\r");
			richText.push_back(curData);
			*/
		}
		else
		{
			for (wI = 0; wI < desc.itemCount && (rva + wI) < (dsint)mMemPage->getSize(); wI++)
			{
				curData.text = _T("");
				curData.textColor = mTextColor;
				curData.textBackground = Color::Transparent;
				curData.flags = RichTextPainter::FlagAll;

				int maxLen = getStringMaxLength(desc.data);
				if ((rva + wI + wByteCount - 1) < (dsint)mMemPage->getSize())
				{
					toString(desc.data, rva + wI * wByteCount, wData + wI * wByteCount, curData);
					if (curData.text.GetLength() < maxLen)
					{
						//spaceData.text = QString(' ').repeated(maxLen - curData.text.length());
						spaceData.text = _T(" ");
						for (int i = 0; i < maxLen - curData.text.GetLength(); i++)
						{
							spaceData.text += _T(" ");
						}
						richText.push_back(spaceData);
					}
					if (wI % sizeof(duint) == 0 && wByteCount == 1 && desc.data.byteMode == HexByte) //pointer underlining
					{
						auto ptr = *(duint*)(wData + wI * wByteCount);
						if (spaceData.highlight = curData.highlight = DbgMemIsValidReadPtr(ptr))
						{
							auto codePage = DbgFunctions()->MemIsCodePage(ptr, false);
							auto modbase = DbgFunctions()->ModBaseFromAddr(ptr);
							if (modbase)
							{
								if (DbgFunctions()->ModGetParty(modbase) == 1) //system
									spaceData.highlightColor = curData.highlightColor = codePage ? mSystemModuleCodePointerHighlightColor : mSystemModuleDataPointerHighlightColor;
								else //user
									spaceData.highlightColor = curData.highlightColor = codePage ? mUserModuleCodePointerHighlightColor : mUserModuleDataPointerHighlightColor;
							}
							else
								spaceData.highlightColor = curData.highlightColor = codePage ? mUnknownCodePointerHighlightColor : mUnknownDataPointerHighlightColor;
						}
					}
					richText.push_back(curData);
					if (maxLen)
					{
						spaceData.text = _T(' ');
						if (wI % sizeof(duint) == sizeof(duint) - 1)
							spaceData.highlight = false;
						richText.push_back(spaceData);
					}
				}
				else
				{
					//curData.text = QString("?").rightJustified(maxLen, ' ');
					curData.text.Replace(_T(" "), _T("?"));
					if (maxLen)
						//curData.text.append(' ');
						curData.text.AppendChar(' ');
					richText.push_back(curData);
				}
			}
		}

		delete[] wData;
	}
}

void CHexDump::printDumpAt(dsint parVA)
{
	printDumpAt(parVA, true);
}

void CHexDump::printDumpAt(dsint parVA, bool select, bool repaint, bool updateTableOffset)
{
	duint wSize;
	auto wBase = DbgMemFindBaseAddr(parVA, &wSize); //get memory base
	if (!wBase || !wSize)
		return;
	dsint wRVA = parVA - wBase; //calculate rva
	int wBytePerRowCount = getBytePerRowCount(); //get the number of bytes per row
	dsint wRowCount;

	// Byte offset used to be aligned on the given RVA
	mByteOffset = (int)((dsint)wRVA % (dsint)wBytePerRowCount);
	mByteOffset = mByteOffset > 0 ? wBytePerRowCount - mByteOffset : 0;

	// Compute row count
	wRowCount = wSize / wBytePerRowCount;
	wRowCount += mByteOffset > 0 ? 1 : 0;

	if (mRvaDisplayEnabled && mMemPage->getBase() != mRvaDisplayPageBase)
		mRvaDisplayEnabled = false;

	setRowCount(wRowCount); //set the number of rows

	mMemPage->setAttributes(wBase, wSize);  // Set base and size (Useful when memory page changed)

	if (updateTableOffset)
	{
		setTableOffset(-1); //make sure the requested address is always first
		setTableOffset((wRVA + mByteOffset) / wBytePerRowCount); //change the displayed offset
	}

	if (select)
	{
		setSingleSelection(wRVA);
		dsint wEndingAddress = wRVA + (int)(mDescriptor.GetHead().data.itemSize) - 1;
		expandSelectionUpTo(wEndingAddress);
	}

	if (repaint)
		reloadData();
}

CString CHexDump::paintContent(CDC* painter, Graphics& graphics, dsint rowBase, int rowOffset, int col, int x, int y, int w, int h)
{
	// Reset byte offset when base address is reached
	if (rowBase == 0 && mByteOffset != 0)
		printDumpAt(mMemPage->getBase(), false, false);

	// Compute RVA
	int wBytePerRowCount = getBytePerRowCount();
	dsint wRva = (rowBase + rowOffset) * wBytePerRowCount - mByteOffset;
	//if (col && mDescriptor.at(col - 1).isData)
	int xinc = 4;
	if(col && mDescriptor.GetAt(mDescriptor.FindIndex(col - 1)).isData)
		printSelected(painter, graphics, rowBase, rowOffset, col, x, y, w, h);

	RichTextPainter::List richText;
	getColumnRichText(col, wRva, richText);
	
	RichTextPainter::paintRichText(painter, x, y, w, h, xinc, richText, NULL, bAlign);

	return CString();
}

void CHexDump::printSelected(CDC* painter, Graphics& graphics, dsint rowBase, int rowOffset, int col, int x, int y, int w, int h)
{
	if ((col > 0) && ((col - 1) < mDescriptor.GetSize()))
	{
		ColumnDescriptor curDescriptor = mDescriptor.GetAt(mDescriptor.FindIndex(col - 1));
		int wBytePerRowCount = getBytePerRowCount();
		dsint wRva = (rowBase + rowOffset) * wBytePerRowCount - mByteOffset;
		
		int wItemPixWidth = getItemPixelWidth(curDescriptor);
		int wCharWidth = getCharWidth();
		//int wItemPixWidth = getStringMaxLength(curDescriptor.data) * wCharWidth + wCharWidth;
		
		if (wItemPixWidth == wCharWidth)
			x += 4;

		for (int i = 0; i < curDescriptor.itemCount; i++)
		{
			
			int wSelectionX = x + i * wItemPixWidth;
			if (isSelected(wRva + i * (int)(curDescriptor.data.itemSize)) == true)
			{
				int wSelectionWidth = wItemPixWidth > w - (wSelectionX - x) ? w - (wSelectionX - x) : wItemPixWidth;
				wSelectionWidth = wSelectionWidth < 0 ? 0 : wSelectionWidth;
				//painter->setPen(mTextColor);
				//painter->fillRect(QRect(wSelectionX, y, wSelectionWidth, h), QBrush(mSelectionColor));
				painter->SetTextColor(mTextColor.ToCOLORREF());
				painter->FillSolidRect(CRect(wSelectionX, y, wSelectionX + wSelectionWidth, y + h), mSelectionColor.ToCOLORREF());
			}
			int separator = curDescriptor.separator;
			if (i && separator && !(i % separator))
			{
				//painter->setPen(mSeparatorColor);
				//painter->drawLine(wSelectionX, y, wSelectionX, y + h);
				CPen separatorPen;
				separatorPen.CreatePen(PS_SOLID, 1, mSeparatorColor.ToCOLORREF());
				painter->SelectObject(&separatorPen);
				painter->MoveTo(wSelectionX + 2, y);
				painter->LineTo(wSelectionX + 2, y + h);
			}
		}
	}
}

void CHexDump::HexDumpClear()
{
	mMemPage->setAttributes(0, 0);
	setRowCount(0);
	reloadData();
}

BEGIN_MESSAGE_MAP(CHexDump, CAbstractTableView)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


BOOL CHexDump::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return TRUE;
	return CAbstractTableView::OnEraseBkgnd(pDC);
}


void CHexDump::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int x = point.x;
	int y = point.y;

	bool wAccept = false;
	CRect viewRect;
	GetClientRect(&viewRect);

	if (getGuiState() == CAbstractTableView::NoState)
	{
		if (y > getHeaderHeight() && y <= viewRect.Height())
		{
			int wColIndex = getColumnIndexFromX(x);

			if (mForceColumn != -1)
			{
				wColIndex = mForceColumn;
				x = getColumnPosition(mForceColumn) + 1;
			}

			if (wColIndex > 0 && mDescriptor.GetAt(mDescriptor.FindIndex(wColIndex - 1)).isData == true) // No selection for first column (addresses) and no data columns
			{
				dsint wStartingAddress = getItemStartingAddress(x, y);
				dsint dataSize = getSizeOf(mDescriptor.GetAt(mDescriptor.FindIndex(wColIndex)).data.itemSize) - 1;
				dsint wEndingAddress = wStartingAddress + dataSize;

				if (wEndingAddress < (dsint)mMemPage->getSize())
				{
					bool bUpdateTo = false;
					//if (!(event->modifiers() & Qt::ShiftModifier))
					if (GetKeyState(VK_SHIFT) >= 0)
						setSingleSelection(wStartingAddress);
					else if (getInitialSelection() > wEndingAddress)
					{
						wEndingAddress -= dataSize;
						bUpdateTo = true;
					}
					expandSelectionUpTo(wEndingAddress);
					if (bUpdateTo)
					{
						mSelection.toIndex += dataSize;
						//emit selectionUpdated();
						selectionUpdated();
					}

					mGuiState = CHexDump::MultiRowsSelectionState;

					Invalidate();
				}
			}
			else if (wColIndex == 0)
			{
				dsint wStartingAddress = getItemStartingAddress(getColumnPosition(1) + 1, y);
				dsint dataSize = getSizeOf(mDescriptor.GetHead().data.itemSize) * mDescriptor.GetHead().itemCount - 1;
				dsint wEndingAddress = wStartingAddress + dataSize;

				if (wEndingAddress < (dsint)mMemPage->getSize())
				{
					bool bUpdateTo = false;
					//if (!(event->modifiers() & Qt::ShiftModifier))
					if (GetKeyState(VK_SHIFT) >= 0)
						setSingleSelection(wStartingAddress);
					else if (getInitialSelection() > wEndingAddress)
					{
						wEndingAddress -= dataSize;
						bUpdateTo = true;
					}
					expandSelectionUpTo(wEndingAddress);
					if (bUpdateTo)
					{
						mSelection.toIndex += dataSize;
						//emit selectionUpdated();
						selectionUpdated();
					}

					mGuiState = CHexDump::MultiRowsSelectionState;

					Invalidate();
				}
			}			
		}
		else
		{
			int wColIndex = getColumnIndexFromX(point.x);
			onLButtonOnHeader(wColIndex);
		}			

		wAccept = true;
		
	}
	if (wAccept == false)
		CAbstractTableView::OnLButtonDown(nFlags, point);
}


void CHexDump::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CAbstractTableView::OnLButtonUp(nFlags, point);
}


void CHexDump::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CAbstractTableView::OnMouseMove(nFlags, point);
}
