#include "stdafx.h"
#include "CPUInfoBox.h"
#include "DisasmPane.h"
#include "MiscUtil.h"
#include <algorithm>

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CCPUInfoBox::CCPUInfoBox()
{
	//setWindowTitle("InfoBox");
	enableMultiSelection(false);
	setShowHeader(false);
	addColumnAt(0, _T(""), true);
	setRowCount(4);
	setCellContent(0, 0, _T(""));
	setCellContent(1, 0, _T(""));
	setCellContent(2, 0, _T(""));
	setCellContent(3, 0, _T(""));
	

	int height = getHeight();
	

	
	curAddr = 0;

	// Deselect any row (visual reasons only)
	setSingleSelection(-1);

	
}

int CCPUInfoBox::getHeight()
{
	return ((getRowHeight() + 1) * 4);
}

void CCPUInfoBox::addInfoLine(const CString & infoLine)
{
	auto rowCount = getRowCount();
	setRowCount(rowCount + 1);
	setCellContent(rowCount, 0, infoLine);
	reloadData();
}

void CCPUInfoBox::setInfoLine(int line, CString text)
{
	if (line < 0 || line > 5)
		return;

	setCellContent(line, 0, text);
	reloadData();
}

CString CCPUInfoBox::getInfoLine(int line)
{
	if (line < 0 || line > 3)
		return CString();

	return getCellContent(line, 0);
}

void CCPUInfoBox::disasmSelectionChanged(dsint parVA)
{
	curAddr = parVA;
	curRva = -1;
	curOffset = -1;

	if (!DbgIsDebugging() || !DbgMemIsValidReadPtr(parVA))
		return;

	// Rather than using clear() or setInfoLine(), only reset the first three cells to reduce flicker
	setRowCount(6);
	setCellContent(0, 0, _T(""));
	setCellContent(1, 0, _T(""));
	setCellContent(2, 0, _T(""));
	setCellContent(3, 0, _T(""));
	setCellContent(4, 0, _T(""));
	setCellContent(5, 0, _T(""));

	DISASM_INSTR instr;
	memset(&instr, 0, sizeof(instr));
	DbgDisasmAt(parVA, &instr);
	BASIC_INSTRUCTION_INFO basicinfo;
	memset(&basicinfo, 0, sizeof(basicinfo));
	DbgDisasmFastAt(parVA, &basicinfo);

	CString instruction(instr.instruction);
	int curPos = 0;
	CString instruct = instruction.Tokenize(_T(" "), curPos);

	char brief[MAX_STRING_SIZE] = "";
	DbgFunctions()->GetMnemonicBrief(CT2A(instruct, CP_UTF8), MAX_STRING_SIZE, brief);
	setInfoLine(0, (CString)CA2T(brief, CP_UTF8));

	int start = 2;
	if (basicinfo.branch && !basicinfo.call && (!ConfigBool("Disassembler", "OnlyCipAutoComments") || parVA == DbgValFromString("cip"))) //jump
	{
		bool taken = DbgIsJumpGoingToExecute(parVA);
		if (taken)
			setInfoLine(start + 0, _T("Jump is taken"));
		else
			setInfoLine(start + 0, _T("Jump is not taken"));
		start += 1;
	}

	bool bUpper = ConfigBool("Disassembler", "Uppercase");

	for (int i = 0, j = start; i < instr.argcount && j < start + 2; i++)
	{
		DISASM_ARG arg = instr.arg[i];
		CString argMnemonic = CString(arg.mnemonic);
		if (bUpper)
			argMnemonic = argMnemonic.MakeUpper();
		if (arg.type == arg_memory)
		{
			

			duint value;

			//bool ok = DbgFunctions()->ValFromString(CT2A(argMnemonic), &value);
			bool ok = valueFromCString(argMnemonic, &value, 16);

			bool result = DbgMemIsValidReadPtr(arg.value);
			CString s1 = ToPtrString(arg.value);
			CString s2 = ToHexString(arg.value);;

			CString valText = DbgMemIsValidReadPtr(arg.value) ? ToPtrString(arg.value) : ToHexString(arg.value);
			auto valTextSym = getSymbolicNameStr(arg.value);

			if (!valText.IsEmpty())
			{
				if (valTextSym.Find(valText) == -1)
				{
					CString temp = valText;
					valText.Format(_T("%s (%s)"), temp, valTextSym);
				}
			}
			else
				valText = valTextSym;

			if (!ok)
			{
				argMnemonic.AppendFormat(_T("]=[%s"), valText);
			}
			else
			{
				argMnemonic = valText;
			}
			
			CString sizeName;
			bool knownsize = true;
			switch (basicinfo.memory.size)
			{
			case size_byte:
				sizeName = _T("byte ptr ");
				break;
			case size_word:
				sizeName = _T("word ptr ");
				break;
			case size_dword:
				sizeName = _T("dword ptr ");
				break;
			case size_qword:
				sizeName = _T("qword ptr ");
				break;
			default:
				knownsize = false;
				break;
			}

			sizeName += [](SEGMENTREG seg)
			{
				switch (seg)
				{
				case SEG_ES:
					return "es:";
				case SEG_DS:
					return "ds:";
				case SEG_FS:
					return "fs:";
				case SEG_GS:
					return "gs:";
				case SEG_CS:
					return "cs:";
				case SEG_SS:
					return "ss:";
				default:
					return "";
				}
			}(arg.segment);

			if (bUpper)
				sizeName = sizeName.MakeUpper();

			if (!DbgMemIsValidReadPtr(arg.value))
			{
				sizeName += "[";
				sizeName += argMnemonic;
				sizeName += "]=???";
				setInfoLine(j, sizeName);
			}
			else if (knownsize)
			{
				CString addrText = getSymbolicNameStr(arg.memvalue);
				sizeName += "[";
				sizeName += argMnemonic;
				sizeName += "]=";
				sizeName += addrText;
				setInfoLine(j, sizeName);
			}
			else
			{
				//TODO: properly support XMM constants
				vector<unsigned char> data;
				data.resize(basicinfo.memory.size);
				memset(data.data(), 0, data.size());
				if (DbgMemRead(arg.value, data.data(), data.size()))
				{
					CString hex;
					
					for (int k = 0; k < data.size(); k++)
					{
						if (k)
							hex.Append(_T(" "));
						hex.Append(ToByteString(data[k]));
					}
					setInfoLine(j, sizeName + "[" + argMnemonic + "]=" + hex);
				}
				else
				{
					//setInfoLine(j, sizeName + "[" + argMnemonic + "]=???");
					sizeName += "[";
					sizeName += argMnemonic;
					sizeName += "]=???";
					setInfoLine(j, sizeName);
				}
			}

			j++;
		}
		else
		{
			bool result = DbgMemIsValidReadPtr(arg.value);
			CString s1 = ToPtrString(arg.value);
			CString s2 = ToHexString(arg.value);;
			CString valText = DbgMemIsValidReadPtr(arg.value) ? ToPtrString(arg.value) : ToHexString(arg.value);
			auto symbolicName = getSymbolicNameStr(arg.value);
			if (!valText.IsEmpty())
			{
				if (symbolicName.Find(valText) == -1)
				{
					CString temp = valText;
					valText.Format(_T("%s (%s)"), symbolicName, temp);
				}				
			}
			else
				valText = symbolicName;

			CString mnemonic(CA2T(arg.mnemonic, CP_UTF8));
			duint value;
			bool ok = valueFromCString(argMnemonic, &value, 16);

			if (ok) //skip certain numbers
			{
				if (ToHexString(arg.value) == symbolicName)
					continue;
				setInfoLine(j, symbolicName);
			}
			else if (mnemonic.Find(_T("xmm")) != 0 && //TODO: properly handle display of these registers
				mnemonic.Find(_T("ymm")) != 0 &&
				mnemonic.Find(_T("st")) != 0)
			{
				setInfoLine(j, mnemonic + "=" + valText);
				j++;
			}
		}
	}
	if (getInfoLine(0) == getInfoLine(1)) //check for duplicate info line
		setInfoLine(start + 1, _T(""));

	// check references details
	// code extracted from ExtraInfo plugin by torusrxxx
	XREF_INFO xrefInfo;
	xrefInfo.refcount = 0;
	xrefInfo.references = nullptr;

	if (DbgXrefGet(parVA, &xrefInfo) && xrefInfo.refcount > 0)
	{
		CString output;
		std::vector<XREF_RECORD*> data;
		for (duint i = 0; i < xrefInfo.refcount; i++)
			data.push_back(&xrefInfo.references[i]);

		std::sort(data.begin(), data.end(), [](const XREF_RECORD * A, const XREF_RECORD * B)
		{
			return ((A->type < B->type) || (A->addr < B->addr));
		});

		int t = XREF_NONE;
		duint i;

		for (i = 0; i < xrefInfo.refcount && i < 10; i++)
		{
			if (t != data[i]->type)
			{
				switch (data[i]->type)
				{
				case XREF_JMP:
					output += _T("Jump from ");
					break;
				case XREF_CALL:
					output += _T("Call from ");
					break;
				default:
					output += _T("Reference from ");
					break;
				}

				t = data[i]->type;
			}

			char clabel[MAX_LABEL_SIZE] = "";

			DbgGetLabelAt(data[i]->addr, SEG_DEFAULT, clabel);
			if (*clabel)
				output += CA2T(clabel, CP_UTF8);
			else
			{
				duint start;
				if (DbgFunctionGet(data[i]->addr, &start, nullptr) && DbgGetLabelAt(start, SEG_DEFAULT, clabel) && start != data[i]->addr)
					output.AppendFormat(_T("%s+%s"), CA2T(clabel, CP_UTF8), ToHexString(data[i]->addr - start));
				else
					output += ToHexString(data[i]->addr);
			}

			if (i != xrefInfo.refcount - 1)
				output += ", ";
		}

		data.clear();
		if (xrefInfo.refcount > 10)
			output += "...";

		setInfoLine(start + 2, output);
	}

	// Set last line
	//
	// Format: SECTION:VA MODULE:$RVA :#FILE_OFFSET FUNCTION, Accessed %u times
	CString info;

	// Section
	char section[MAX_SECTION_SIZE * 5];
	if (DbgFunctions()->SectionFromAddr(parVA, section))
	{
		info += CA2T(section, CP_UTF8);
		info +=  ":";
	}
		

	// VA
	info += ToPtrString(parVA);

	// Module name, RVA, and file offset
	char mod[MAX_MODULE_SIZE];
	if (DbgFunctions()->ModNameFromAddr(parVA, mod, true))
	{
		dsint modbase = DbgFunctions()->ModBaseFromAddr(parVA);

		// Append modname
		info += " ";
		info += +CA2T(mod, CP_UTF8);

		// Module RVA
		curRva = parVA - modbase;
		if (modbase)
			info.AppendFormat(_T(":$%s"), ToHexString(curRva));

		// File offset
		curOffset = DbgFunctions()->VaToFileOffset(parVA);
		info.AppendFormat(_T(" #%s"), ToHexString(curOffset));
	}

	// Function/label name
	char label[MAX_LABEL_SIZE];
	if (DbgGetLabelAt(parVA, SEG_DEFAULT, label))
		info.AppendFormat(_T(" <%s>"), CA2T(label, CP_UTF8));
	else
	{
		duint start;
		if (DbgFunctionGet(parVA, &start, nullptr) && DbgGetLabelAt(start, SEG_DEFAULT, label) && start != parVA)
			info.AppendFormat(_T(" <%s+%s>"), CA2T(label, CP_UTF8), ToHexString(parVA - start));
	}

	auto tracedCount = DbgFunctions()->GetTraceRecordHitCount(parVA);
	if (tracedCount != 0)
	{
		//info += ", " + tr("Accessed %n time(s)", nullptr, tracedCount);
		info += ", ";
		info += _T("Accessed %n time(s)");
	}

	setInfoLine(start + 3, info);

	DbgSelChanged(GUI_DISASSEMBLY, parVA);
}


BEGIN_MESSAGE_MAP(CCPUInfoBox, CStdTable)
ON_WM_ACTIVATE()
ON_WM_ACTIVATEAPP()
ON_WM_ENABLE()
ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


void CCPUInfoBox::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CStdTable::OnActivate(nState, pWndOther, bMinimized);

	// TODO: 在此处添加消息处理程序代码
}


void CCPUInfoBox::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
	CStdTable::OnActivateApp(bActive, dwThreadID);

	// TODO: 在此处添加消息处理程序代码
}


void CCPUInfoBox::OnEnable(BOOL bEnable)
{
	CStdTable::OnEnable(bEnable);

	// TODO: 在此处添加消息处理程序代码
}


void CCPUInfoBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDisasmPane* tp = (CDisasmPane*) this->GetParent();
	tp->setFocusState(CDisasmPane::FocusState::O);

	CStdTable::OnLButtonDown(nFlags, point);
}
