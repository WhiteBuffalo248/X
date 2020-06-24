#include "stdafx.h"
#include "CPUDump.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CCPUDump::CCPUDump()
{
	bAlign = true;
	setView((ViewEnum_t)ConfigUint("HexDump", "DefaultView"));

	//addColumnAt(200, L"地址", true); //bytes
	//addColumnAt(200, L"进制", true); //disassembly
	//addColumnAt(400, L"编码", true); //comments
	setShowHeader(true); //hide header
	Initialize();
}


CCPUDump::~CCPUDump()
{
}

void CCPUDump::selectionUpdated()
{
	
}

void CCPUDump::setView(ViewEnum_t view)
{
	switch (view)
	{
	case ViewHexAscii:
		hexAscii();
		break;
		
	case ViewHexUnicode:
		hexUnicode();
		break;
		/*
	case ViewTextAscii:
		textAsciiSlot();
		break;
	case ViewTextUnicode:
		textUnicodeSlot();
		break;
	case ViewIntegerSignedByte:
		integerSignedByteSlot();
		break;
	case ViewIntegerSignedShort:
		integerSignedShortSlot();
		break;
	case ViewIntegerSignedLong:
		integerSignedLongSlot();
		break;
	case ViewIntegerSignedLongLong:
		integerSignedLongLongSlot();
		break;
	case ViewIntegerUnsignedByte:
		integerUnsignedByteSlot();
		break;
	case ViewIntegerUnsignedShort:
		integerUnsignedShortSlot();
		break;
	case ViewIntegerUnsignedLong:
		integerUnsignedLongSlot();
		break;
	case ViewIntegerUnsignedLongLong:
		integerUnsignedLongLongSlot();
		break;
	case ViewIntegerHexShort:
		integerHexShortSlot();
		break;
	case ViewIntegerHexLong:
		integerHexLongSlot();
		break;
	case ViewIntegerHexLongLong:
		integerHexLongLongSlot();
		break;
	case ViewFloatFloat:
		floatFloatSlot();
		break;
	case ViewFloatDouble:
		floatDoubleSlot();
		break;
	case ViewFloatLongDouble:
		floatLongDoubleSlot();
		break;
	case ViewAddress:
		addressSlot();
		break;
	case ViewAddressAscii:
		addressAsciiSlot();
		break;
	case ViewAddressUnicode:
		addressUnicodeSlot();
		break;
	case ViewHexCodepage:
		hexLastCodepageSlot();
		break;
	case ViewTextCodepage:
		textLastCodepageSlot();
		break;
		*/
	default:
		hexAscii();
		break;
	}
}

void CCPUDump::hexAscii()
{
	m_viewMode = ViewHexAscii;
	Config()->setUint("HexDump", "DefaultView", (duint)ViewHexAscii);
	ColumnDescriptor wColDesc;
	DataDescriptor dDesc;

	copyColumnWidth();

	wColDesc.isData = true; //hex byte
	wColDesc.itemCount = 16;
	wColDesc.separator = mAsciiSeparator ? mAsciiSeparator : 4;
	dDesc.itemSize = Byte;
	dDesc.byteMode = HexByte;
	wColDesc.data = dDesc;
	//appendResetDescriptor(8 + charwidth * 47, _T("Hex"), false, wColDesc);
	appendResetDescriptor(getLastColumnWidth(1), _T("Hex"), true, wColDesc);

	wColDesc.isData = true; //ascii byte
	wColDesc.itemCount = 16;
	wColDesc.separator = 0;
	dDesc.itemSize = Byte;
	dDesc.byteMode = AsciiByte;
	wColDesc.data = dDesc;
	//appendDescriptor(8 + charwidth * 16, _T("ASCII"), false, wColDesc);
	appendDescriptor(getLastColumnWidth(2), _T("ASCII"), true, wColDesc);
	
	wColDesc.isData = false; //empty column
	wColDesc.itemCount = 0;
	wColDesc.separator = 0;
	dDesc.itemSize = Byte;
	dDesc.byteMode = AsciiByte;
	wColDesc.data = dDesc;
	appendDescriptor(0, _T(""), true, wColDesc);	

	reloadData();
}

void CCPUDump::hexUnicode()
{
	m_viewMode = ViewHexUnicode;
	Config()->setUint("HexDump", "DefaultView", (duint)ViewHexUnicode);
	int charwidth = getCharWidth();
	ColumnDescriptor wColDesc;
	DataDescriptor dDesc;

	copyColumnWidth();

	wColDesc.isData = true; //hex byte
	wColDesc.itemCount = 16;
	wColDesc.separator = mAsciiSeparator ? mAsciiSeparator : 4;
	dDesc.itemSize = Byte;
	dDesc.byteMode = HexByte;
	wColDesc.data = dDesc;

	appendResetDescriptor(getLastColumnWidth(1), _T("Hex"), false, wColDesc);

	wColDesc.isData = true; //unicode short
	wColDesc.itemCount = 8;
	wColDesc.separator = 0;
	dDesc.itemSize = Word;
	dDesc.wordMode = UnicodeWord;
	wColDesc.data = dDesc;
	appendDescriptor(getLastColumnWidth(2), _T("UNICODE"), false, wColDesc);
	
	wColDesc.isData = false; //empty column
	wColDesc.itemCount = 0;
	wColDesc.separator = 0;
	dDesc.itemSize = Byte;
	dDesc.byteMode = AsciiByte;
	wColDesc.data = dDesc;
	appendDescriptor(0, _T(""), false, wColDesc);	

	reloadData();
}

CString CCPUDump::paintContent(CDC* painter, Graphics& graphics, dsint rowBase, int rowOffset, int col, int x, int y, int w, int h)
{
	// Reset byte offset when base address is reached
	if (rowBase == 0 && mByteOffset != 0)
		printDumpAt(mMemPage->getBase(), false, false);

	CRect colRect(x, y, x + w, y + h);
	painter->FillSolidRect(&colRect, mBackgroundColor.ToCOLORREF());

	if (!col) //address
	{
		char label[MAX_LABEL_SIZE] = "";
		dsint cur_addr = rvaToVa((rowBase + rowOffset) * getBytePerRowCount() - mByteOffset);
		Color background;		
		if (DbgGetLabelAt(cur_addr, SEG_DEFAULT, label)) //label
		{
			background = ConfigColor("HexDumpLabelBackgroundColor");
			//painter->setPen(ConfigColor("HexDumpLabelColor")); //TODO: config
			painter->SetTextColor(ConfigColor("HexDumpLabelColor").ToCOLORREF());
		}
		else
		{
			background = ConfigColor("HexDumpAddressBackgroundColor");
			//painter->setPen(ConfigColor("HexDumpAddressColor")); //TODO: config
			painter->SetTextColor(ConfigColor("HexDumpAddressColor").ToCOLORREF());
		}
		if (background.GetAlpha())
			painter->FillSolidRect(&colRect, background.ToCOLORREF()); //fill background color
		colRect.DeflateRect(4, 0);
		painter->DrawText(makeAddrText(cur_addr), &colRect, DT_VCENTER | DT_LEFT);
		return CString();
	}
	return CHexDump::paintContent(painter, graphics, rowBase, rowOffset, col, x, y, w, h);
}

void CCPUDump::onLButtonOnHeader(int wColIndex)
{
	if (wColIndex == 2)
	{
		switch (m_viewMode)
		{
		case CCPUDump::ViewHexAscii:
			setView(ViewHexUnicode);
			break;
		case CCPUDump::ViewHexUnicode:
			setView(ViewHexAscii);
			break;
		case CCPUDump::ViewTextAscii:
			break;
		case CCPUDump::ViewTextUnicode:
			break;
		case CCPUDump::ViewIntegerSignedShort:
			break;
		case CCPUDump::ViewIntegerSignedLong:
			break;
		case CCPUDump::ViewIntegerSignedLongLong:
			break;
		case CCPUDump::ViewIntegerUnsignedShort:
			break;
		case CCPUDump::ViewIntegerUnsignedLong:
			break;
		case CCPUDump::ViewIntegerUnsignedLongLong:
			break;
		case CCPUDump::ViewIntegerHexShort:
			break;
		case CCPUDump::ViewIntegerHexLong:
			break;
		case CCPUDump::ViewIntegerHexLongLong:
			break;
		case CCPUDump::ViewFloatFloat:
			break;
		case CCPUDump::ViewFloatDouble:
			break;
		case CCPUDump::ViewFloatLongDouble:
			break;
		case CCPUDump::ViewAddress:
			break;
		case CCPUDump::ViewIntegerSignedByte:
			break;
		case CCPUDump::ViewIntegerUnsignedByte:
			break;
		case CCPUDump::ViewAddressAscii:
			break;
		case CCPUDump::ViewAddressUnicode:
			break;
		case CCPUDump::ViewHexCodepage:
			break;
		case CCPUDump::ViewTextCodepage:
			break;
		default:
			break;
		}
	}
}

BEGIN_MESSAGE_MAP(CCPUDump, CHexDump)
	ON_WM_CREATE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


int CCPUDump::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CHexDump::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	loadColumnFromConfig(_T("CPUDump"));

	return 0;
}


void CCPUDump::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	CHexDump::OnLButtonUp(nFlags, point);
}


void CCPUDump::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CHexDump::OnLButtonDown(nFlags, point);
}
