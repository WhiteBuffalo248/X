#include "stdafx.h"
#include "GlobalID.h"
#include "BreakPointsManage.h"
#include "StringUtil.h"

#include "Breakpoints.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


CBreakPointsManage::CBreakPointsManage()
{
	addColumnAt(200, _T("Type"), false);
	addColumnAt(200, _T("Address"), true);
	addColumnAt(222, _T("Module/Label/Exception"), true);
	addColumnAt(200, _T("State"), true);
	addColumnAt(200, _T("Disassembly"), true);
	addColumnAt(200, _T("Hits"), true);
	addColumnAt(200, _T("Summary"), true);
	setShowHeader(true); //hide header
	loadColumnFromConfig(_T("BreakpointsView"));

	enableMultiSelection(true);
	
	mDisasm = new QBeaEngine(ConfigUint("Disassembler", "MaxModuleSize"));
	mDisasm->UpdateConfig();

	Initialize();
}


CBreakPointsManage::~CBreakPointsManage()
{
	delete mDisasm;
}

void CBreakPointsManage::updateColors()
{
	CAbstractStdTable::updateColors();
	mDisasmBackgroundColor = ConfigColor("DisassemblyBackgroundColor");
	mDisasmSelectionColor = ConfigColor("DisassemblySelectionColor");
	mCipBackgroundColor = ConfigColor("ThreadCurrentBackgroundColor");
	mCipColor = ConfigColor("ThreadCurrentColor");
	mSummaryParenColor = ConfigColor("BreakpointSummaryParenColor");
	mSummaryKeywordColor = ConfigColor("BreakpointSummaryKeywordColor");
	mSummaryStringColor = ConfigColor("BreakpointSummaryStringColor");
	mDisasm->UpdateConfig();
	updateBreakpoints(); 
}

void CBreakPointsManage::updateBreakpoints()
{
	if (mExceptionMap.empty() && DbgFunctions()->EnumExceptions)
	{
		BridgeList<CONSTANTINFO> exceptions;
		DbgFunctions()->EnumExceptions(&exceptions);
		for (int i = 0; i < exceptions.Count(); i++)
		{
			mExceptionMap.insert({ exceptions[i].value, exceptions[i].name });
			mExceptionList.push_back(CString(exceptions[i].name));
			mExceptionMaxLength = max(mExceptionMaxLength, int(strlen(exceptions[i].name)));
		}
		mExceptionList.sort();
	}
	BPMAP bpmap;
	DbgGetBpList(bp_none, &bpmap);
	setRowCount(bpmap.count);
	mBps.clear();
	mBps.reserve(bpmap.count + 5);
	mRich.clear();
	mRich.reserve(bpmap.count + 5);
	BPXTYPE lasttype = bp_none;
	for (int i = 0, row = 0; i < bpmap.count; i++, row++)
	{
		BRIDGEBP & bp = bpmap.bp[i];

		if (lasttype != bp.type)
		{
			lasttype = bp.type;
			setRowCount(getRowCount() + 1);
			setCellContent(row, ColType, bpTypeName(bp.type));
			setCellUserdata(row, ColType, bp.type);
			setCellContent(row, ColHits, CString());
			setCellContent(row, ColAddr, CString());
			setCellUserdata(row, ColAddr, row);
			setCellContent(row, ColModLabel, CString());
			setCellContent(row, ColState, CString());
			setCellContent(row, ColDisasm, CString());
			setCellContent(row, ColSummary, CString());
			row++;

			BRIDGEBP fakebp;
			memset(&fakebp, 0, sizeof(fakebp));
			fakebp.type = lasttype;
			mBps.push_back(fakebp);
			mRich.push_back(std::make_pair(RichTextPainter::List(), RichTextPainter::List()));
		}

		mBps.push_back(bp);

		RichTextPainter::List richSummary, richDisasm;

		auto addrText = [&]()
		{
			if (bp.type == bp_dll)
			{
				auto base = DbgModBaseFromName(bp.mod);
				if (!base)
					base = -1;
				return ToPtrString(base);
			}
			else
				return ToPtrString(bp.addr);
		};
		auto modLabelText = [&]() -> CString
		{
			char label[MAX_LABEL_SIZE] = "";
			if (bp.type == bp_exception)
			{
				auto found = mExceptionMap.find(bp.addr);
				return found == mExceptionMap.end() ? _T("") : (CString)found->second;
			}
			else if (bp.type != bp_dll && DbgGetLabelAt(bp.addr, SEG_DEFAULT, label))
			{
				//return QString("<%1.%2>").arg(bp.mod, label);
				CString retText;
				retText = CA2W(bp.mod, CP_UTF8);
				retText += ".";
				retText += CA2W(label, CP_UTF8);
				return retText;
			}
			else
				return (CString)CA2W(bp.mod, CP_UTF8);
		};
		auto stateName = [&]()
		{
			if (!bp.active)
				return _T("Inactive");
			if (bp.enabled)
				return bp.singleshoot ? _T("One-time") : _T("Enabled");
			else
				return _T("Disabled");
		};
		auto disasmText = [&]() -> CString
		{
			CString result;
			if (!bp.active || bp.type == bp_dll || bp.type == bp_exception)
				return result;
			byte_t data[MAX_DISASM_BUFFER];
			if (DbgMemRead(bp.addr, data, sizeof(data)))
			{
				auto instr = mDisasm->DisassembleAt(data, sizeof(data), 0, bp.addr);
				CapstoneTokenizer::TokenToRichText(instr.tokens, richDisasm, 0);
				for (auto & token : richDisasm)
					result += token.text;
			}
			return result;
		};
		//memory/hardware/dll/exception type, name, address comment, condition, log(text+condition), command(text+condition)
		auto summaryText = [&]() -> CString
		{
			auto colored = [&richSummary](CString text, Color color)
			{
				RichTextPainter::CustomRichText_t token;
				token.highlight = false;
				token.flags = RichTextPainter::FlagColor;
				token.textColor = color;
				token.text = text;
				richSummary.push_back(token);
			};
			auto text = [this, &richSummary](CString text)
			{
				RichTextPainter::CustomRichText_t token;
				token.highlight = false;
				token.flags = RichTextPainter::FlagColor;
				token.textColor = this->mTextColor;
				token.text = text;
				richSummary.push_back(token);
			};
			auto next = [&richSummary, &text]()
			{
				if (!richSummary.empty())
					text(_T(", "));
			};

			char comment[MAX_COMMENT_SIZE];
			if (bp.type != bp_dll && bp.type != bp_exception && DbgGetCommentAt(bp.addr, comment) && *comment != '\1')
			{
				next();
				colored((CString)comment, mSummaryStringColor);
			}
			else if (*bp.name)
			{
				next();
				colored((CString)bp.name, mSummaryStringColor);
			}

			switch (bp.type)
			{
			case bp_normal:
				break;

			case bp_hardware:
			{
				auto size = [](BPHWSIZE size)
				{
					switch (size)
					{
					case hw_byte:
						return _T("byte");
					case hw_word:
						return _T("word");
					case hw_dword:
						return _T("dword");
					case hw_qword:
						return _T("qword");
					default:
						return _T("");
					}
				}(BPHWSIZE(bp.hwSize));

				switch (bp.typeEx)
				{
				case hw_access:
					next();
					colored(_T("access"), mSummaryKeywordColor);
					colored(_T("("), mSummaryParenColor);
					text(size);
					colored(_T(")"), mSummaryParenColor);
					break;
				case hw_write:
					next();
					colored(_T("write"), mSummaryKeywordColor);
					colored(_T("("), mSummaryParenColor);
					text(size);
					colored(_T(")"), mSummaryParenColor);
					break;
				case hw_execute:
					next();
					colored(_T("execute"), mSummaryKeywordColor);
					colored(_T("()"), mSummaryParenColor);
					break;
				}
			}
			break;

			case bp_memory:
			{
				auto op = [](BPMEMTYPE type)
				{
					switch (type)
					{
					case mem_access:
						return _T("access");
					case mem_read:
						return _T("read");
					case mem_write:
						return _T("write");
					case mem_execute:
						return _T("execute");
					default:
						return _T("");
					}
				}(BPMEMTYPE(bp.typeEx));
				next();
				colored(op, mSummaryKeywordColor);
				colored(_T("("), mSummaryParenColor);
				text(ToHexString(DbgFunctions()->MemBpSize(bp.addr)));
				colored(_T(")"), mSummaryParenColor);
			}
			break;

			case bp_dll:
				switch (bp.typeEx)
				{
				case dll_load:
					next();
					colored(_T("load"), mSummaryKeywordColor);
					colored(_T("()"), mSummaryParenColor);
					break;
				case dll_unload:
					next();
					colored(_T("unload"), mSummaryKeywordColor);
					colored(_T("()"), mSummaryParenColor);
					break;
				case dll_all:
					next();
					colored(_T("all"), mSummaryKeywordColor);
					colored(_T("()"), mSummaryParenColor);
					break;
				}
				break;

			case bp_exception:
				switch (bp.typeEx)
				{
				case ex_firstchance:
					next();
					colored(_T("firstchance"), mSummaryKeywordColor);
					colored(_T("()"), mSummaryParenColor);
					break;
				case ex_secondchance:
					next();
					colored(_T("secondchance"), mSummaryKeywordColor);
					colored(_T("()"), mSummaryParenColor);
					break;
				case ex_all:
					next();
					colored(_T("anychance"), mSummaryKeywordColor);
					colored(_T("()"), mSummaryParenColor);
					break;
				}
				break;

			default:
				return _T("");
			}

			if (*bp.breakCondition)
			{
				next();
				colored(_T("breakif"), mSummaryKeywordColor);
				colored(_T("("), mSummaryParenColor);
				text((CString)bp.breakCondition);
				colored(_T(")"), mSummaryParenColor);
			}

			if (bp.fastResume)
			{
				next();
				colored(_T("fastresume"), mSummaryKeywordColor);
				colored(_T("()"), mSummaryParenColor);
			}
			else //fast resume skips all other steps
			{
				if (*bp.logText)
				{
					next();
					colored(_T("log"), mSummaryKeywordColor);
					if (*bp.logCondition)
					{
						colored(_T("if"), mSummaryKeywordColor);
						colored(_T("("), mSummaryParenColor);
						text((CString)bp.logCondition);
						colored(_T(","), mSummaryParenColor);
						text(_T(" "));
					}
					else
						colored(_T("("), mSummaryParenColor);
					//colored(QString("\"%1\"").arg(bp.logText), mSummaryStringColor);
					CString logText;
					logText.Format(_T("\"%s\""), bp.logText, mSummaryStringColor);
					colored(logText, mSummaryStringColor);
					colored(_T(")"), mSummaryParenColor);
				}

				if (*bp.commandText)
				{
					next();
					colored(_T("cmd"), mSummaryKeywordColor);
					if (*bp.commandCondition)
					{
						colored(_T("if"), mSummaryKeywordColor);
						colored(_T("("), mSummaryParenColor);
						text((CString)bp.commandCondition);
						colored(_T(","), mSummaryParenColor);
						text(_T(" "));
					}
					else
						colored(_T("("), mSummaryParenColor);
					//colored(QString("\"%1\"").arg(bp.commandText), mSummaryStringColor);
					CString commandText;
					commandText.Format(_T("\"%s\""), bp.commandText);
					colored(commandText, mSummaryStringColor);
					colored(_T(")"), mSummaryParenColor);
				}
			}
			CString result;
			for (auto & token : richSummary)
				result += token.text;
			//return (const wchar_t*)result;
			return result;
		};

		setCellContent(row, ColType, CString());
		setCellUserdata(row, ColType, bp.type);
		setCellContent(row, ColAddr, addrText());
		setCellUserdata(row, ColAddr, row);
		setCellContent(row, ColModLabel, modLabelText());
		setCellContent(row, ColState, stateName());
		setCellContent(row, ColDisasm, disasmText());
		CString hitCountText;
		hitCountText.Format(_T("%u"), bp.hitCount);
		setCellContent(row, ColHits, hitCountText);		
		setCellContent(row, ColSummary, summaryText());

		mRich.push_back(std::make_pair(std::move(richDisasm), std::move(richSummary)));
	}
	if (bpmap.count)
	{
		BridgeFree(bpmap.bp);

		auto sel = getInitialSelection();
		auto rows = getRowCount();
		if (sel >= rows)
			setSingleSelection(rows - 1);
	}
	reloadData();
}

CString CBreakPointsManage::paintContent(CDC* painter, Graphics& graphics, dsint rowBase, int rowOffset, int col, int x, int y, int w, int h)
{
	CRect colRect(x, y, x + w, y + h);
	painter->FillSolidRect(&colRect, mDisasmBackgroundColor.ToCOLORREF());
	if (isSelected(rowBase, rowOffset))
		painter->FillSolidRect(&colRect, (col == ColDisasm ? mDisasmSelectionColor : mSelectionColor).ToCOLORREF());
	else if (col == ColDisasm)
		painter->FillSolidRect(colRect, mDisasmBackgroundColor.ToCOLORREF());
	auto index = bpIndex(rowBase + rowOffset);
	auto & bp = mBps.at(index);
	auto cellContent = getCellContent(rowBase + rowOffset, col);
	if (col > ColType && !bp.addr && !bp.active)
	{
		auto mid = h / 2;
		//painter->drawLine(QPointF(x, y + mid), QPointF(x + w, y + mid));
		painter->MoveTo(x, y + mid);
		painter->LineTo(x + w, y + mid);
	}
	else if (col == ColAddr)
	{
		if (bp.type == bp_dll || bp.type == bp_exception)
			return cellContent;
		else if (bp.addr && bp.addr == mCip)
		{
			painter->FillSolidRect(colRect, mCipBackgroundColor.ToCOLORREF());
			CPen pen;
			pen.CreatePen(PS_SOLID, 1, mCipColor.ToCOLORREF());
			painter->SelectObject(&pen);
			CRect textRect = colRect;
			textRect.InflateRect(4, 0);
			painter->DrawText(cellContent, &textRect, DT_VCENTER | DT_LEFT);
			return CString();
		}
	}
	else if (col == ColDisasm)
	{
		RichTextPainter::paintRichText(painter, x + 4, y, w - 4, h, 0, mRich.at(index).first, NULL);
		return CString();
	}
	else if (col == ColSummary)
	{
		RichTextPainter::paintRichText(painter, x + 4, y, w - 4, h, 0, mRich.at(index).second, NULL);
		return CString();
	}
	return cellContent;
}

BEGIN_MESSAGE_MAP(CBreakPointsManage, CAbstractStdTable)
	ON_COMMAND_RANGE(ID_BREAKPOINTS_REMOVE, ID_BREAKPOINTS_ADD_EXCEPTION_BREAKPOINT, &CBreakPointsManage::OnBreakPointsManageHandle)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


void CBreakPointsManage::followEvent()
{
	if (!isValidBp())
		return;
	auto & bp = selectedBp();
	if (bp.type == bp_exception || !bp.active)
		return;
	duint addr = bp.type == bp_dll ? DbgModBaseFromName(bp.mod) : bp.addr;
	if (!DbgMemIsValidReadPtr(addr))
		return;
	CString CMD;

	if (DbgFunctions()->MemIsCodePage(addr, false))
	{
		CMD = _T("disasm ");
		CMD += ToPtrString(addr);
		DbgCmdExecDirect(CT2A(CMD, CP_UTF8));
	}
		
	else
	{
		CMD = _T("dump ");
		CMD += ToPtrString(addr);
		DbgCmdExecDirect(CT2A(CMD, CP_UTF8));
		//emit Bridge::getBridge()->getDumpAttention();
	}
}

void CBreakPointsManage::OnBreakPointsManageHandle(UINT nID)
{
	switch (nID)
	{
	case ID_BREAKPOINTS_REMOVE:
		removeBreakpoint();
		break;
	case ID_BREAKPOINTS_DISABLE:
		toggleBreakpoint();
		break;
	case ID_BREAKPOINTS_RESET_HIT_COUNT:
		resetHitCountBreakpoint();
		break;
	case ID_BREAKPOINTS_ENABLE_ALL:
		enableAllBreakpoints();
		break;
	case ID_BREAKPOINTS_DISABLE_ALL:
		disableAllBreakpoints();
		break;
	case ID_BREAKPOINTS_REMOVE_ALL:
		removeAllBreakpoints();
		break;
	case ID_BREAKPOINTS_ADD_DLL_BREAKPOINT:
		addDllBreakpoint();
		break;
	case ID_BREAKPOINTS_ADD_EXCEPTION_BREAKPOINT:
		addExceptionBreakpoint();
		break;
	default:
		break;
	}
}

void CBreakPointsManage::removeBreakpoint()
{
	for (int i : getSelection())
	{
		if (isValidBp(i))
		{
			const BRIDGEBP & bp = selectedBp(i);
			if (bp.active)
				Breakpoints::removeBP(bp);
			else
			{
				//DbgCmdExec(QString().sprintf("bc \"%s\":$%X", bp.mod, bp.addr));
				char buffer[50];
				sprintf_s(buffer, 50, "bc \"%s\":$%X", bp.mod, bp.addr);
				DbgCmdExec(buffer);
			}			
		}
	}
}

void CBreakPointsManage::toggleBreakpoint()
{
	for (int i : getSelection())
		if (isValidBp(i))
			Breakpoints::toggleBPByDisabling(selectedBp(i));
}




void CBreakPointsManage::resetHitCountBreakpoint()
{
	for (int i : getSelection())
	{
		if (!isValidBp(i))
			continue;
		auto & bp = selectedBp(i);
		CString text;
		switch (bp.type)
		{
		case bp_normal:
			text = _T("ResetBreakpointHitCount ");
			text += ToPtrString(bp.addr);
			break;
		case bp_hardware:
			text = _T("ResetHardwareBreakpointHitCount ");
			text += ToPtrString(bp.addr);
			break;
		case bp_memory:
			text = _T("ResetMemoryBreakpointHitCount ");
			text += ToPtrString(bp.addr);
			break;
		case bp_dll:
			text.Format(_T("ResetMemoryBreakpointHitCount \"%s\""), bp.mod);
			break;
		case bp_exception:
			text = _T("ResetExceptionBreakpointHitCount ");
			text += ToPtrString(bp.addr);
			break;
		default:
			text = _T("invalid");
			break;
		}
		DbgCmdExec(CT2A(text, CP_UTF8));
	}
}

void CBreakPointsManage::enableAllBreakpoints()
{
	if (mBps.empty())
		return;
	DbgCmdExec([this]()
	{
		switch (selectedBp().type)
		{
		case bp_normal:
			return "bpe";
		case bp_hardware:
			return "bphwe";
		case bp_memory:
			return "bpme";
		case bp_dll:
			return "bpdll";
		case bp_exception:
			return "EnableExceptionBPX";
		default:
			return "invalid";
		}
	}());
}

void CBreakPointsManage::disableAllBreakpoints()
{
	if (mBps.empty())
		return;
	DbgCmdExec([this]()
	{
		switch (selectedBp().type)
		{
		case bp_normal:
			return "bpd";
		case bp_hardware:
			return "bphwd";
		case bp_memory:
			return "bpmd";
		case bp_dll:
			return "bpddll";
		case bp_exception:
			return "DisableExceptionBPX";
		default:
			return "invalid";
		}
	}());
}

void CBreakPointsManage::removeAllBreakpoints()
{
	if (mBps.empty())
		return;
	DbgCmdExec([this]()
	{
		switch (selectedBp().type)
		{
		case bp_normal:
			return "bc";
		case bp_hardware:
			return "bphwc";
		case bp_memory:
			return "bpmc";
		case bp_dll:
			return "bcdll";
		case bp_exception:
			return "DeleteExceptionBPX";
		default:
			return "invalid";
		}
	}());
}

void CBreakPointsManage::addDllBreakpoint()
{
	CString fileName;
	/*
	if (SimpleInputBox(this, tr("Enter the module name"), "", fileName, tr("Example: mydll.dll"), &DIcon("breakpoint.png")) && !fileName.isEmpty())
	DbgCmdExec(QString("bpdll \"%1\"").arg(fileName));
	*/
	
}

void CBreakPointsManage::addExceptionBreakpoint()
{
	CString exception;
	/*
	if (SimpleChoiceBox(this, tr("Enter the exception code"), "", mExceptionList, exception, true, tr("Example: EXCEPTION_ACCESS_VIOLATION"), &DIcon("breakpoint.png"), mExceptionMaxLength) && !exception.isEmpty())
		DbgCmdExec((QString("SetExceptionBPX ") + exception));
	*/
	
}

int CBreakPointsManage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CAbstractStdTable::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	
	return 0;
}


void CBreakPointsManage::OnSize(UINT nType, int cx, int cy)
{
	CAbstractStdTable::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	
}


void CBreakPointsManage::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: 在此处添加消息处理程序代码
	if (!getRButtonInHeader() && DbgIsDebugging())
	{
		CMenu PopupMenu;
		// 创建弹出菜单
		BOOL bRet = PopupMenu.CreatePopupMenu();

		if (isValidBp())
		{
			PopupMenu.AppendMenu(MF_ENABLED, ID_BREAKPOINTS_REMOVE, _T("移除"));	
			PopupMenu.AppendMenu(MF_ENABLED, ID_BREAKPOINTS_DISABLE, selectedBp().enabled ? _T("禁用") : _T("激活"));
			PopupMenu.AppendMenu(MF_ENABLED, ID_BREAKPOINTS_EDIT, _T("编辑"));
			PopupMenu.AppendMenu(MF_SEPARATOR);
			if (selectedBp().hitCount > 0)
			{
				PopupMenu.AppendMenu(MF_ENABLED, ID_BREAKPOINTS_RESET_HIT_COUNT, _T("重置Hit次数"));
				PopupMenu.AppendMenu(MF_SEPARATOR);
			}
			
			PopupMenu.AppendMenu(MF_ENABLED, ID_BREAKPOINTS_ENABLE_ALL, CString("Enable all ") + bpTypeName(selectedBp().type));			
			PopupMenu.AppendMenu(MF_ENABLED, ID_BREAKPOINTS_DISABLE_ALL, CString("Disable all ") + bpTypeName(selectedBp().type));			
			PopupMenu.AppendMenu(MF_ENABLED, ID_BREAKPOINTS_REMOVE_ALL, CString("Remove all ") + bpTypeName(selectedBp().type));

			PopupMenu.AppendMenu(MF_SEPARATOR);

		}
		PopupMenu.AppendMenu(MF_ENABLED, ID_BREAKPOINTS_ADD_DLL_BREAKPOINT, _T("添加DLL断点"));
		PopupMenu.AppendMenu(MF_ENABLED, ID_BREAKPOINTS_ADD_EXCEPTION_BREAKPOINT, _T("添加异常断点"));
		PopupMenu.AppendMenu(MF_SEPARATOR);

		CMenu SecondLevelPopupMenu;
		// 创建二级弹出菜单
		bRet = SecondLevelPopupMenu.CreateMenu();
		if (bRet)
		{
			setupCopyMenu(&SecondLevelPopupMenu);
			PopupMenu.AppendMenu(MF_POPUP, (UINT)SecondLevelPopupMenu.m_hMenu, _T("复制"));
		}
		// 显示菜单
		int nCmd = PopupMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this); //TPM_RETURNCMD
	}
}


void CBreakPointsManage::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (point.y > getHeaderHeight() && DbgIsDebugging())
		followEvent();
	else
		CStdTable::OnLButtonDblClk(nFlags, point);	
}
