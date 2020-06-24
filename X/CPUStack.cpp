#include "stdafx.h"
#include "CPUStack.h"
#include "WordEditDlg.h"
#include "GlobalID.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CCPUStack::CCPUStack()
{
	ColumnDescriptor wColDesc;
	DataDescriptor dDesc;
	/*
	bStackFrozen = false;
	mMultiDump = multiDump;
	*/
	mForceColumn = 1;
	
	wColDesc.isData = true; //void*
	wColDesc.itemCount = 1;
	wColDesc.separator = 0;
#ifdef _WIN64
	wColDesc.data.itemSize = Qword;
	wColDesc.data.qwordMode = HexQword;
#else
	wColDesc.data.itemSize = Dword;
	wColDesc.data.dwordMode = HexDword;
#endif
	appendDescriptor(200, _T("void*"), false, wColDesc);

	wColDesc.isData = false; //comments
	wColDesc.itemCount = 0;
	wColDesc.separator = 0;
	dDesc.itemSize = Byte;
	dDesc.byteMode = AsciiByte;
	wColDesc.data = dDesc;
	appendDescriptor(2000, _T("Comments"), false, wColDesc);

	Initialize();
}


CCPUStack::~CCPUStack()
{
}

void CCPUStack::updateFonts()
{
	lf = ConfigFont("Stack");
}

void CCPUStack::updateColors()
{
	CHexDump::updateColors();

	mBackgroundColor = ConfigColor("StackBackgroundColor");
	mTextColor = ConfigColor("StackTextColor");
	mSelectionColor = ConfigColor("StackSelectionColor");
	mStackReturnToColor = ConfigColor("StackReturnToColor");
	mStackSEHChainColor = ConfigColor("StackSEHChainColor");
	mUserStackFrameColor = ConfigColor("StackFrameColor");
	mSystemStackFrameColor = ConfigColor("StackFrameSystemColor");
}

void CCPUStack::selectionUpdated()
{
	duint selectedData;
	if (mMemPage->read((byte_t*)&selectedData, getInitialSelection(), sizeof(duint)))
		if (DbgMemIsValidReadPtr(selectedData)) //data is a pointer
		{
			duint stackBegin = mMemPage->getBase();
			duint stackEnd = stackBegin + mMemPage->getSize();
			if (selectedData >= stackBegin && selectedData < stackEnd) //data is a pointer to stack address
			{
				/*
				disconnect(SIGNAL(enterPressedSignal()));
				connect(this, SIGNAL(enterPressedSignal()), this, SLOT(followStackSlot()));
				mFollowDisasm->setShortcut(QKeySequence(""));
				mFollowStack->setShortcut(QKeySequence("enter"));
				*/				
			}
			else
			{
				/*
				disconnect(SIGNAL(enterPressedSignal()));
				connect(this, SIGNAL(enterPressedSignal()), this, SLOT(followDisasmSlot()));
				mFollowStack->setShortcut(QKeySequence(""));
				mFollowDisasm->setShortcut(QKeySequence("enter"));
				*/				
			}
		}
}

void CCPUStack::stackDumpAt(duint addr, duint csp)
{
	if (DbgMemIsValidReadPtr(addr))
		mHistory.addVaToHistory(addr);
	mCsp = csp;

	// Get the callstack
	DBGCALLSTACK callstack;
	memset(&callstack, 0, sizeof(DBGCALLSTACK));
	DbgFunctions()->GetCallStack(&callstack);
	mCallstack.resize(callstack.total);
	if (mCallstack.size())
	{
		// callstack data highest >> lowest
		std::qsort(callstack.entries, callstack.total, sizeof(DBGCALLSTACKENTRY), [](const void* a, const void* b)
		{
			auto p = (const DBGCALLSTACKENTRY*)a;
			auto q = (const DBGCALLSTACKENTRY*)b;
			if (p->addr < q->addr)
				return -1;
			else
				return 1;
		});
		for (size_t i = 0; i < mCallstack.size(); i++)
		{
			mCallstack[i].addr = callstack.entries[i].addr;
			mCallstack[i].party = DbgFunctions()->ModGetParty(callstack.entries[i].to);
		}
		BridgeFree(callstack.entries);
	}

	printDumpAt(addr);
}

void CCPUStack::getColumnRichText(int col, dsint rva, RichTextPainter::List & richText)
{
	// Compute VA
	duint wVa = rvaToVa(rva);

	bool wActiveStack = (wVa >= mCsp); //inactive stack

	STACK_COMMENT comment;
	RichTextPainter::CustomRichText_t curData;
	curData.highlight = false;
	curData.flags = RichTextPainter::FlagColor;
	curData.textColor = mTextColor;

	//if (col && mDescriptor.at(col - 1).isData == true) //paint stack data
	if (col && mDescriptor.GetAt(mDescriptor.FindIndex(col - 1)).isData == true) //paint stack data
	{
		CHexDump::getColumnRichText(col, rva, richText);
		if (!wActiveStack)
		{
			Color inactiveColor = ConfigColor("StackInactiveTextColor");
			for (int i = 0; i < int(richText.size()); i++)
			{
				richText[i].flags = RichTextPainter::FlagColor;
				richText[i].textColor = inactiveColor;
			}
		}
	}
	else if (col && DbgStackCommentGet(wVa, &comment)) //paint stack comments
	{
		if (wActiveStack)
		{
			if (*comment.color)
			{
				if (comment.color[0] == '!')
				{
					if (strcmp(comment.color, "!sehclr") == 0)
						curData.textColor = mStackSEHChainColor;
					else if (strcmp(comment.color, "!rtnclr") == 0)
						curData.textColor = mStackReturnToColor;
					else
						curData.textColor = mTextColor;
				}
				else
					curData.textColor = atoi(comment.color);
			}
			else
				curData.textColor = mTextColor;
		}
		else
			curData.textColor = ConfigColor("StackInactiveTextColor");
		curData.text = CA2T(comment.comment, CP_UTF8);
		richText.push_back(curData);
	}
	else
		CHexDump::getColumnRichText(col, rva, richText);
}

CString CCPUStack::paintContent(CDC* painter, Graphics& graphics, dsint rowBase, int rowOffset, int col, int x, int y, int w, int h)
{
	// Compute RVA
	int wBytePerRowCount = getBytePerRowCount();
	dsint wRva = (rowBase + rowOffset) * wBytePerRowCount - mByteOffset;
	duint wVa = rvaToVa(wRva);
	CRect colRect(x, y, x + w, y + h);

	painter->FillSolidRect(&colRect, mBackgroundColor.ToCOLORREF());

	bool wIsSelected = isSelected(wRva);
	if (wIsSelected) //highlight if selected
		//painter->fillRect(QRect(x, y, w, h), QBrush(mSelectionColor));
		painter->FillSolidRect(&colRect, mSelectionColor.ToCOLORREF());

	if (col == 0) // paint stack address
	{
		Color background;
		if (DbgGetLabelAt(wVa, SEG_DEFAULT, nullptr)) //label
		{
			if (wVa == mCsp) //CSP
			{
				background = ConfigColor("StackCspBackgroundColor");
				//painter->setPen(QPen(ConfigColor("StackCspColor")));
				painter->SetTextColor(ConfigColor("StackCspColor").ToCOLORREF());
			}
			else //no CSP
			{
				background = ConfigColor("StackLabelBackgroundColor");
				//painter->setPen(ConfigColor("StackLabelColor"));
				painter->SetTextColor(ConfigColor("StackLabelColor").ToCOLORREF());
			}
		}
		else //no label
		{
			if (wVa == mCsp) //CSP
			{
				background = ConfigColor("StackCspBackgroundColor");
				//painter->setPen(QPen(ConfigColor("StackCspColor")));
				painter->SetTextColor(ConfigColor("StackCspColor").ToCOLORREF());
			}
			else if (wIsSelected) //selected normal address
			{
				background = ConfigColor("StackSelectedAddressBackgroundColor");
				//painter->setPen(QPen(ConfigColor("StackSelectedAddressColor"))); //black address (DisassemblySelectedAddressColor)
				painter->SetTextColor(ConfigColor("StackSelectedAddressColor").ToCOLORREF());
			}
			else //normal address
			{
				background = ConfigColor("StackAddressBackgroundColor");
				//painter->setPen(QPen(ConfigColor("StackAddressColor")));
				painter->SetTextColor(ConfigColor("StackAddressColor").ToCOLORREF());
			}
		}
		if (background.GetAlpha())
			//painter->fillRect(QRect(x, y, w, h), QBrush(background)); //fill background when defined
			painter->FillSolidRect(&colRect, background.ToCOLORREF());
		//painter->drawText(QRect(x + 4, y, w - 4, h), Qt::AlignVCenter | Qt::AlignLeft, makeAddrText(wVa));
		CRect textRect(colRect);
		textRect.DeflateRect(4, 0);
		painter->DrawText(makeAddrText(wVa), &textRect, DT_SINGLELINE | DT_VCENTER);
		return CString();
	}
	else if (col == 1) // paint stack data
	{
		if (mCallstack.size())
		{
			int stackFrameBitfield = 0; // 0:none, 1:top of stack frame, 2:bottom of stack frame, 4:middle of stack frame
			int party = 0;
			if (wVa >= mCallstack[0].addr)
			{
				for (size_t i = 0; i < mCallstack.size() - 1; i++)
				{
					if (wVa >= mCallstack[i].addr && wVa < mCallstack[i + 1].addr)
					{
						stackFrameBitfield |= (mCallstack[i].addr == wVa) ? 1 : 0;
						stackFrameBitfield |= (mCallstack[i + 1].addr == wVa + sizeof(duint)) ? 2 : 0;
						if (stackFrameBitfield == 0)
							stackFrameBitfield = 4;
						party = mCallstack[i].party;
						break;
					}
				}
				// draw stack frame
				if (stackFrameBitfield == 0)
					return CHexDump::paintContent(painter, graphics, rowBase, rowOffset, 1, x, y, w, h);
				else
				{
					int height = getRowHeight();
					int halfHeight = height / 2;
					int width = 5;
					int offset = 2;
					auto result = CHexDump::paintContent(painter, graphics, rowBase, rowOffset, 1, x + (width - 2), y, w - (width - 2), h);
					CPen linePen;
					Color lineColor;
					if (party == 0)
						//painter->setPen(QPen(mUserStackFrameColor, 2));
						lineColor = mUserStackFrameColor;
					else
						//painter->setPen(QPen(mSystemStackFrameColor, 2));
						lineColor = mSystemStackFrameColor;
					linePen.CreatePen(PS_SOLID, 1, lineColor.ToCOLORREF());
					painter->SelectObject(&linePen);
					if ((stackFrameBitfield & 1) != 0)
					{
						//painter->drawLine(x + width, y + halfHeight / 2, x + offset, y + halfHeight / 2);
						//painter->drawLine(x + offset, y + halfHeight / 2, x + offset, y + halfHeight);
						painter->MoveTo(x + width, y + halfHeight / 2);
						painter->LineTo(x + offset, y + halfHeight / 2);
						painter->MoveTo(x + offset, y + halfHeight / 2);
						painter->LineTo(x + offset, y + halfHeight + 1);
					}
					else
					{
						//painter->drawLine(x + offset, y, x + offset, y + halfHeight);
						painter->MoveTo(x + offset, y);
						painter->LineTo(x + offset, y + halfHeight + 1);
					}
						
					if ((stackFrameBitfield & 2) != 0)
					{
						//painter->drawLine(x + width, y + height / 4 * 3, x + offset, y + height / 4 * 3);
						//painter->drawLine(x + offset, y + height / 4 * 3, x + offset, y + halfHeight);
						painter->MoveTo(x + width, y + height / 4 * 3);
						painter->LineTo(x + offset, y + height / 4 * 3);
						painter->MoveTo(x + offset, y + height / 4 * 3);
						painter->LineTo(x + offset, y + halfHeight);
					}
					else
					{
						//painter->drawLine(x + offset, y + height, x + offset, y + halfHeight);
						painter->MoveTo(x + offset, y + height);
						painter->LineTo(x + offset, y + halfHeight);
					}
						
					return result;
				}
			}
			else
				return CHexDump::paintContent(painter, graphics, rowBase, rowOffset, 1, x, y, w, h);
		}
		else
			return CHexDump::paintContent(painter, graphics, rowBase, rowOffset, 1, x, y, w, h);
	}
	else
		return CHexDump::paintContent(painter, graphics, rowBase, rowOffset, col, x, y, w, h);
}

BEGIN_MESSAGE_MAP(CCPUStack, CHexDump)
	ON_WM_CREATE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


int CCPUStack::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CHexDump::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������
	loadColumnFromConfig(_T("CPUStack"));

	return 0;
}


void CCPUStack::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (!DbgIsDebugging())
		return;
	switch (getColumnIndexFromX(point.x))
	{
	case 0: //address
	{
		//very ugly way to calculate the base of the current row (no clue why it works)
		dsint deltaRowBase = getInitialSelection() % getBytePerRowCount() + mByteOffset;
		if (deltaRowBase >= getBytePerRowCount())
			deltaRowBase -= getBytePerRowCount();
		dsint mSelectedVa = rvaToVa(getInitialSelection() - deltaRowBase);
		if (mRvaDisplayEnabled && mSelectedVa == mRvaDisplayBase)
			mRvaDisplayEnabled = false;
		else
		{
			mRvaDisplayEnabled = true;
			mRvaDisplayBase = mSelectedVa;
			mRvaDisplayPageBase = mMemPage->getBase();
		}
		reloadData();
	}
	break;

	default:	
		dsint addr = getInitialSelection();	
		dsint value = 0;
		mMemPage->read(&value, addr, sizeof(dsint));
		CWordEditDlg modifyDlg(_T("Modify"), value, sizeof(dsint), this);
		if (modifyDlg.DoModal() == IDOK)
		{
			value = modifyDlg.getValue();
			mMemPage->write(&value, addr, sizeof(dsint));
			GuiUpdateAllViews();
		}	
	break;
	}	
}


void CCPUStack::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: �ڴ˴������Ϣ����������
	
	
	if (!DbgIsDebugging())
		return;

	CMenu PopupMenu;
	// ���������˵�
	BOOL bRet = PopupMenu.CreatePopupMenu();

	//Push
	PopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_PUSH_DWORD, ArchValue(_T("Push DWORD..."), _T("Push QWORD...")));
	//Pop
	PopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_POP_DWORD, ArchValue(_T("Pop DWORD"), _T("Pop QWORD")));
	//Realign

	// Modify
	PopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_MODIFY, _T("�޸�..."));

	CMenu SecondLevelBinaryPopupMenu;
	// �������������˵�
	bRet = SecondLevelBinaryPopupMenu.CreateMenu();
	//Binary->Edit
	SecondLevelBinaryPopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_BINARY_EDIT, _T("�༭"));
	SecondLevelBinaryPopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_BINARY_FILL, _T("���"));
	SecondLevelBinaryPopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_BINARY_COPY, _T("����"));
	SecondLevelBinaryPopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_BINARY_PASTE, _T("ճ��"));
	

	//Binary->Paste (Ignore Size)
	SecondLevelBinaryPopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_BINARY_PASTEIGNORESIZE, _T("ճ�� (���Դ�С)"));
	
	PopupMenu.AppendMenu(MF_POPUP, (UINT)SecondLevelBinaryPopupMenu.m_hMenu, _T("������"));

	CMenu SecondLevelCopyPopupMenu;
	// �������������˵�
	bRet = SecondLevelCopyPopupMenu.CreateMenu();

	SecondLevelCopyPopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_COPY_SELECTION, _T("ѡ����"));
	SecondLevelCopyPopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_COPY_ADDRESS, _T("��ַ"));
	if (DbgFunctions()->ModBaseFromAddr(rvaToVa(getInitialSelection())) != 0)
		SecondLevelCopyPopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_COPY_RVA, _T("RVA"));

	PopupMenu.AppendMenu(MF_POPUP, (UINT)SecondLevelCopyPopupMenu.m_hMenu, _T("����"));

	// �������������˵�
	CMenu SecondLevelBreakPointPopupMenu;	
	bRet = SecondLevelBreakPointPopupMenu.CreateMenu();

	// �������������˵�
	CMenu ThirdLevelBreakPointHardwareAccessPopupMenu;	
	bRet = ThirdLevelBreakPointHardwareAccessPopupMenu.CreateMenu();
	

	// �������������˵�
	CMenu ThirdLevelBreakPointHardwareWritePopupMenu;	
	bRet = ThirdLevelBreakPointHardwareWritePopupMenu.CreateMenu();
	

	// �������������˵�
	CMenu ThirdLevelBreakPointMemoryAccessPopupMenu;	
	bRet = ThirdLevelBreakPointMemoryAccessPopupMenu.CreateMenu();
	

	// �������������˵�
	CMenu ThirdLevelBreakPointMemoryWritePopupMenu;	
	bRet = ThirdLevelBreakPointMemoryWritePopupMenu.CreateMenu();
	
	

	duint selectedAddr = rvaToVa(getInitialSelection());
	if (DbgGetBpxTypeAt(selectedAddr) & bp_hardware) //hardware breakpoint set
	{
		SecondLevelBreakPointPopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_BREAKPOINT_REMOVE_HARDWARE, _T("�Ƴ�Ӳ���ϵ�"));
	}
	else //memory breakpoint not set
	{
		ThirdLevelBreakPointHardwareAccessPopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_BREAKPOINT_HARDWARE_ACCESS_BYTE, _T("�ֽ�"));
		ThirdLevelBreakPointHardwareAccessPopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_BREAKPOINT_HARDWARE_ACCESS_WORD, _T("��"));
		ThirdLevelBreakPointHardwareAccessPopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_BREAKPOINT_HARDWARE_ACCESS_DWORD, _T("˫��"));
#ifdef _WIN64
		ThirdLevelBreakPointHardwareAccessPopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_BREAKPOINT_HARDWARE_ACCESS_QWORD, _T("Qword"));
#endif //_WIN64
		SecondLevelBreakPointPopupMenu.AppendMenu(MF_POPUP, (UINT)ThirdLevelBreakPointHardwareAccessPopupMenu.m_hMenu, _T("Ӳ������"));
		
		ThirdLevelBreakPointHardwareWritePopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_BREAKPOINT_HARDWARE_WRITE_BYTE, _T("�ֽ�"));
		ThirdLevelBreakPointHardwareWritePopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_BREAKPOINT_HARDWARE_WRITE_WORD, _T("��"));
		ThirdLevelBreakPointHardwareWritePopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_BREAKPOINT_HARDWARE_WRITE_DWORD, _T("˫��"));
#ifdef _WIN64
		ThirdLevelBreakPointHardwareWritePopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_BREAKPOINT_HARDWARE_WRITE_QWORD, _T("Qword"));
#endif //_WIN64
		SecondLevelBreakPointPopupMenu.AppendMenu(MF_POPUP, (UINT)ThirdLevelBreakPointHardwareWritePopupMenu.m_hMenu, _T("Ӳ��д��"));		
	}

	SecondLevelBreakPointPopupMenu.AppendMenu(MF_SEPARATOR);

	if (DbgGetBpxTypeAt(selectedAddr) & bp_memory) //memory breakpoint set
	{
		SecondLevelBreakPointPopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_BREAKPOINT_REMOVE_MEMORY, _T("�Ƴ��ڴ�ϵ�"));
	}
	else //memory breakpoint not set
	{
		ThirdLevelBreakPointMemoryAccessPopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_BREAKPOINT_MEMORY_ACCESS_SINGLESHOOT, _T("Singleshoot"));
		ThirdLevelBreakPointMemoryAccessPopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_BREAKPOINT_MEMORY_ACCESS_RESTOREONHIT, _T("Restore on hit"));
		SecondLevelBreakPointPopupMenu.AppendMenu(MF_POPUP, (UINT)ThirdLevelBreakPointMemoryAccessPopupMenu.m_hMenu, _T("�ڴ����"));
		

		ThirdLevelBreakPointMemoryWritePopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_BREAKPOINT_MEMORY_WRITE_SINGLESHOOT, _T("&Singleshoot"));
		ThirdLevelBreakPointMemoryWritePopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_BREAKPOINT_MEMORY_WRITE_RESTOREONHIT, _T("&Restore on hit"));
		SecondLevelBreakPointPopupMenu.AppendMenu(MF_POPUP, (UINT)ThirdLevelBreakPointMemoryWritePopupMenu.m_hMenu, _T("�ڴ�д��"));
	}

	PopupMenu.AppendMenu(MF_POPUP, (UINT)SecondLevelBreakPointPopupMenu.m_hMenu, _T("�ϵ����"));	

	// Restore Selection
	dsint start = rvaToVa(getSelectionStart());
	dsint end = rvaToVa(getSelectionEnd());
	if (DbgFunctions()->PatchInRange(start, end))
		PopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_RESTORE_SELECTION, _T("Restore selection"));

	//Find Pattern
	PopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_FIND_PATTERN, _T("Find Pattern..."));
	//Follow CSP
	PopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_FOLLOW_ESP, ArchValue(_T("Follow ESP"), _T("Follow RSP")));
	if (DbgMemIsValidReadPtr(DbgValFromString("cbp")))
		PopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_FOLLOW_EBP, ArchValue(_T("Follow EBP"), _T("Follow RBP")));

	// �������������˵�
	CMenu SecondLevelGotoPopupMenu;
	bRet = SecondLevelGotoPopupMenu.CreateMenu();

	SecondLevelGotoPopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_GOTO_EXPRESSION, _T("Go to &Expression"));
	SecondLevelGotoPopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_GOTO_BASEOFSTACKFRAME, _T("Go to Base of Stack Frame"));
	SecondLevelGotoPopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_GOTO_PREVIOUSSTACKFRAME, _T("Go to Previous Stack Frame"));
	SecondLevelGotoPopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_GOTO_NEXTSTACKFRAME, _T("Go to Next Stack Frame"));
	if (mHistory.historyHasPrev())
		SecondLevelGotoPopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_GOTO_PREVIOUS, _T("Go to Previous"));
	if (mHistory.historyHasNext())
		SecondLevelGotoPopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_GOTO_NEXT, _T("Go to Next"));
	
	PopupMenu.AppendMenu(MF_POPUP, (UINT)SecondLevelGotoPopupMenu.m_hMenu, _T("Go to"));

	//Freeze the stack
	PopupMenu.AppendMenu(MF_ENABLED | MF_CHECKED, ID_CPUSTACK_FREEZETHESTACK, _T("Freeze the stack"));
	//Follow in Memory Map
	duint ptr;
	if (DbgMemRead(rvaToVa(getInitialSelection()), (unsigned char*)&ptr, sizeof(ptr)) && DbgMemIsValidReadPtr(ptr))
		PopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_FOLLOW_IN_MEMORYMAP, ArchValue(_T("Follow DWORD in Memory Map"), _T("Follow QWORD in Memory Map")));

	if (DbgMemRead(rvaToVa(getInitialSelection()), (unsigned char*)&ptr, sizeof(ptr)))
	{
		duint stackBegin = mMemPage->getBase();
		duint stackEnd = stackBegin + mMemPage->getSize();
		if (ptr >= stackBegin && ptr < stackEnd)
			PopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_FOLLOW_IN_STACK, ArchValue(_T("Follow DWORD in &Stack"), _T("Follow QWORD in &Stack")));
	}
		

	//Follow in Disassembler
	if (DbgMemRead(rvaToVa(getInitialSelection()), (unsigned char*)&ptr, sizeof(ptr)) && DbgMemIsValidReadPtr(ptr))
		PopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_FOLLOW_IN_DISASSEMBLE, ArchValue(_T("&Follow DWORD in Disassembler"), _T("&Follow QWORD in Disassembler")));

	//Follow in Dump
	PopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_FOLLOW_IN_DUMP, _T("Follow in Dump"));
	//Follow PTR in Dump
	if (DbgMemRead(rvaToVa(getInitialSelection()), (unsigned char*)&ptr, sizeof(ptr)) && DbgMemIsValidReadPtr(ptr))
		PopupMenu.AppendMenu(MF_ENABLED, ID_CPUSTACK_FOLLOW_DWORD_IN_DUMP, ArchValue(_T("Follow DWORD in &Dump"), _T("Follow QWORD in &Dump")));

	// ��ʾ�˵�
	int nCmd = PopupMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this); //TPM_RETURNCMD
	
}
