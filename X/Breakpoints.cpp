#include "stdafx.h"
#include "Breakpoints.h"
//#include "EditBreakpointDialog.h"
#include "StringUtil.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

using namespace std;

Breakpoints::Breakpoints()
{
}

/**
* @brief       Set a new breakpoint according to the given type at the given address.
*
* @param[in]   type    Type of the breakpoint
* @param[in]   va      Virtual Address
*
* @return      Nothing.
*/
void Breakpoints::setBP(BPXTYPE type, duint va)
{
	CString wCmd;

	switch (type)
	{
	case bp_normal:
	{
		wCmd = _T("bp ") + ToPtrString(va);
	}
	break;

	case bp_hardware:
	{
		wCmd = _T("bph ") + ToPtrString(va);
	}
	break;

	case bp_memory:
	{
		wCmd = _T("bpm ") + ToPtrString(va);
	}
	break;

	default:
	{

	}
	break;
	}
	DbgCmdExec(CT2A(wCmd, CP_UTF8));
}

/**
* @brief       Enable breakpoint according to the given breakpoint descriptor.
*
* @param[in]   bp  Breakpoint descriptor
*
* @return      Nothing.
*/
void Breakpoints::enableBP(const BRIDGEBP & bp)
{
	CString wCmd;

	if (bp.type == bp_hardware)
	{
		wCmd.Format(_T("bphwe \"%s\""), ToPtrString(bp.addr).GetBuffer());
	}
	else if (bp.type == bp_normal)
	{
		wCmd.Format(_T("be \"%s\""), ToPtrString(bp.addr).GetBuffer());
	}
	else if (bp.type == bp_memory)
	{
		wCmd.Format(_T("bpme \"%s\""), ToPtrString(bp.addr).GetBuffer());
	}
	else if (bp.type == bp_dll)
	{
		CString str(bp.mod);
		wCmd.Format(_T("LibrarianEnableBreakPoint \"%s\""), str.GetBuffer());
	}
	else if (bp.type == bp_exception)
	{
		wCmd.Format(_T("EnableExceptionBPX \"%s\""), ToPtrString(bp.addr).GetBuffer());
	}
	DbgCmdExec(CT2A(wCmd, CP_UTF8));
}

/**
* @brief       Enable breakpoint that has been previously disabled according to its type and virtual address.
*              If breakpoint was removed, this method has no effect.@n
*              Breakpoint type is usefull when there are several types of breakpoints on the same address.
*              bp_none enables all breakpoints at the given address.
*
* @param[in]   type    Type of the breakpoint.
* @param[in]   va      Virtual Address
*
* @return      Nothing.
*/
void Breakpoints::enableBP(BPXTYPE type, duint va)
{
	BPMAP wBPList;

	// Get breakpoints list
	DbgGetBpList(type, &wBPList);

	// Find breakpoint at address VA
	for (int wI = 0; wI < wBPList.count; wI++)
	{
		if (wBPList.bp[wI].addr == va)
		{
			enableBP(wBPList.bp[wI]);
		}
	}
	if (wBPList.count)
		BridgeFree(wBPList.bp);
}

/**
* @brief       Disable breakpoint according to the given breakpoint descriptor.
*
* @param[in]   bp  Breakpoint descriptor
*
* @return      Nothing.
*/
void Breakpoints::disableBP(const BRIDGEBP & bp)
{
	CString wCmd;

	if (bp.type == bp_hardware)
	{
		wCmd.Format(_T("bphwd \"%s\""), ToPtrString(bp.addr).GetBuffer());
	}
	else if (bp.type == bp_normal)
	{
		wCmd.Format(_T("bd \"%s\""), ToPtrString(bp.addr).GetBuffer());
	}
	else if (bp.type == bp_memory)
	{
		wCmd.Format(_T("bpmd \"%s\""), ToPtrString(bp.addr).GetBuffer());
	}
	else if (bp.type == bp_dll)
	{
		CString str(bp.mod);
		wCmd.Format(_T("LibrarianDisableBreakPoint \"%s\""), str.GetBuffer());
	}
	else if (bp.type == bp_exception)
	{
		wCmd.Format(_T("DisableExceptionBPX \"%s\""), ToPtrString(bp.addr).GetBuffer());
	}
	DbgCmdExec(CT2A(wCmd, CP_UTF8));
}

/**
* @brief       Disable breakpoint that has been previously enabled according to its type and virtual address.
*              If breakpoint was removed, this method has no effect.@n
*              Breakpoint type is usefull when there are several types of breakpoints on the same address.
*              bp_none disbales all breakpoints at the given address.
*
* @param[in]   type    Type of the breakpoint.
* @param[in]   va      Virtual Address
*
* @return      Nothing.
*/
void Breakpoints::disableBP(BPXTYPE type, duint va)
{
	BPMAP wBPList;

	// Get breakpoints list
	DbgGetBpList(type, &wBPList);

	// Find breakpoint at address VA
	for (int wI = 0; wI < wBPList.count; wI++)
	{
		if (wBPList.bp[wI].addr == va)
		{
			disableBP(wBPList.bp[wI]);
		}
	}
	if (wBPList.count)
		BridgeFree(wBPList.bp);
}

/**
* @brief       Remove breakpoint according to the given breakpoint descriptor.
*
* @param[in]   bp  Breakpoint descriptor
*
* @return      Nothing.
*/
void Breakpoints::removeBP(const BRIDGEBP & bp)
{
	CString wCmd;

	switch (bp.type)
	{
	case bp_normal:
		wCmd.Format(_T("bc \"%s\""), ToPtrString(bp.addr).GetBuffer());
		break;

	case bp_hardware:
		wCmd.Format(_T("bphc \"%s\""), ToPtrString(bp.addr).GetBuffer());
		break;

	case bp_memory:
		wCmd.Format(_T("bpmc \"%s\"") ,ToPtrString(bp.addr).GetBuffer());
		break;

	case bp_dll:
	{
		CString str(bp.mod);
		wCmd.Format(_T("bcdll \"%s\""), str.GetBuffer());
	}		
		break;

	case bp_exception:
		wCmd.Format(_T("DeleteExceptionBPX \"%s\""), ToPtrString(bp.addr).GetBuffer());
		break;

	default:
		break;
	}
	DbgCmdExec(CT2A(wCmd, CP_UTF8));
}

/**
* @brief       Remove breakpoint at the given given address and type
*              If breakpoint doesn't exists, this method has no effect.@n
*              Breakpoint type is usefull when there are several types of breakpoints on the same address.
*              bp_none disbales all breakpoints at the given address.
*
* @param[in]   type    Type of the breakpoint.
* @param[in]   va      Virtual Address
*
* @return      Nothing.
*/
void Breakpoints::removeBP(BPXTYPE type, duint va)
{
	BPMAP wBPList;

	// Get breakpoints list
	DbgGetBpList(type, &wBPList);

	// Find breakpoint at address VA
	for (int wI = 0; wI < wBPList.count; wI++)
	{
		if (wBPList.bp[wI].addr == va)
		{
			removeBP(wBPList.bp[wI]);
		}
	}
	if (wBPList.count)
		BridgeFree(wBPList.bp);
}

void Breakpoints::removeBP(const CString & DLLName)
{
	BPMAP wBPList;

	// Get breakpoints list
	DbgGetBpList(bp_dll, &wBPList);

	// Find breakpoint at DLLName
	for (int wI = 0; wI < wBPList.count; wI++)
	{
		if (CString(wBPList.bp[wI].mod) == DLLName)
		{
			removeBP(wBPList.bp[wI]);
		}
	}
	if (wBPList.count)
		BridgeFree(wBPList.bp);
}

/**
* @brief       Toggle the given breakpoint by disabling it when enabled.@n
*              If breakpoint is initially active and enabled, it will be disabled.@n
*              If breakpoint is initially active and disabled, it will stay disabled.@n
*
* @param[in]   bp  Breakpoint descriptor
*
* @return      Nothing.
*/
void Breakpoints::toggleBPByDisabling(const BRIDGEBP & bp)
{
	if (bp.enabled)
		disableBP(bp);
	else
		enableBP(bp);
}

/**
* @brief       Toggle the given breakpoint by disabling it when enabled.@n
*              If breakpoint is initially active and enabled, it will be disabled.@n
*              If breakpoint is initially active and disabled, it will stay disabled.@n
*              If breakpoint was previously removed, this method has no effect.@n
*
* @param[in]   type    Type of the breakpoint.
* @param[in]   va      Virtual Address
*
* @return      Nothing.
*/
void Breakpoints::toggleBPByDisabling(BPXTYPE type, duint va)
{
	BPMAP wBPList;

	// Get breakpoints list
	DbgGetBpList(type, &wBPList);

	// Find breakpoint at address VA
	for (int wI = 0; wI < wBPList.count; wI++)
	{
		if (wBPList.bp[wI].addr == va)
		{
			toggleBPByDisabling(wBPList.bp[wI]);
		}
	}
	if (wBPList.count)
		BridgeFree(wBPList.bp);
}

void Breakpoints::toggleBPByDisabling(const CString & DLLName)
{
	BPMAP wBPList;

	// Get breakpoints list
	DbgGetBpList(bp_dll, &wBPList);

	// Find breakpoint at module name
	for (int wI = 0; wI < wBPList.count; wI++)
	{
		if (CString(wBPList.bp[wI].mod) == DLLName)
		{
			toggleBPByDisabling(wBPList.bp[wI]);
		}
	}
	if (wBPList.count)
		BridgeFree(wBPList.bp);
}

void Breakpoints::toggleAllBP(BPXTYPE type, bool bEnable)
{
	BPMAP wBPList;

	// Get breakpoints list
	DbgGetBpList(type, &wBPList);

	if (bEnable)
	{
		// Find breakpoint at address VA
		for (int wI = 0; wI < wBPList.count; wI++)
		{
			enableBP(wBPList.bp[wI]);
		}
	}
	else
	{
		// Find breakpoint at address VA
		for (int wI = 0; wI < wBPList.count; wI++)
		{
			disableBP(wBPList.bp[wI]);
		}
	}

	if (wBPList.count)
		BridgeFree(wBPList.bp);
}

/**
* @brief       returns if a breakpoint is disabled or not
*
* @param[in]   type    Type of the breakpoint.
* @param[in]   va      Virtual Address
*
* @return      enabled/disabled.
*/
BPXSTATE Breakpoints::BPState(BPXTYPE type, duint va)
{
	BPMAP wBPList;
	BPXSTATE result = bp_non_existent;

	// Get breakpoints list
	DbgGetBpList(type, &wBPList);

	// Find breakpoint at address VA
	for (int wI = 0; wI < wBPList.count; wI++)
	{
		if (wBPList.bp[wI].addr == va)
		{
			if (wBPList.bp[wI].enabled)
			{
				result = bp_enabled;
				break;
			}
			else
			{
				result = bp_disabled;
				break;
			}
		}
	}
	if (wBPList.count)
		BridgeFree(wBPList.bp);

	return result;
}

bool Breakpoints::BPTrival(BPXTYPE type, duint va)
{
	BPMAP wBPList;
	bool trival = true;

	// Get breakpoints list
	DbgGetBpList(type, &wBPList);

	// Find breakpoint at address VA
	for (int wI = 0; wI < wBPList.count; wI++)
	{
		BRIDGEBP & bp = wBPList.bp[wI];
		if (bp.addr == va)
		{
			trival = !(bp.breakCondition[0] || bp.logCondition[0] || bp.commandCondition[0] || bp.commandText[0] || bp.logText[0] || bp.name[0] || bp.fastResume || bp.silent);
			break;
		}
	}
	if (wBPList.count)
		BridgeFree(wBPList.bp);

	return trival;
}


/**
* @brief       Toggle the given breakpoint by disabling it when enabled.@n
*              If breakpoint is initially active and enabled, it will be disabled.@n
*              If breakpoint is initially active and disabled, it will stay disabled.@n
*              If breakpoint was previously removed, this method has no effect.@n
*
* @param[in]   type    Type of the breakpoint.
* @param[in]   va      Virtual Address
*
* @return      Nothing.
*/
void Breakpoints::toggleBPByRemoving(BPXTYPE type, duint va)
{
	BPMAP wBPList;
	bool wNormalWasRemoved = false;
	bool wMemoryWasRemoved = false;
	bool wHardwareWasRemoved = false;

	// Get breakpoints list
	DbgGetBpList(type, &wBPList);

	// Find breakpoints at address VA and remove them
	for (int wI = 0; wI < wBPList.count; wI++)
	{
		if (wBPList.bp[wI].addr == va)
		{
			removeBP(wBPList.bp[wI]);

			switch (wBPList.bp[wI].type)
			{
			case bp_normal:
				wNormalWasRemoved = true;
				break;
			case bp_memory:
				wMemoryWasRemoved = true;
				break;
			case bp_hardware:
				wHardwareWasRemoved = true;
				break;
			default:
				break;
			}
		}
	}
	if (wBPList.count)
		BridgeFree(wBPList.bp);

	if ((type == bp_none || type == bp_normal) && (wNormalWasRemoved == false))
	{
		setBP(bp_normal, va);
	}
	else if ((type == bp_none || type == bp_memory) && (wMemoryWasRemoved == false))
	{
		setBP(bp_memory, va);
	}
	else if ((type == bp_none || type == bp_hardware) && (wHardwareWasRemoved == false))
	{
		setBP(bp_hardware, va);
	}
}

bool Breakpoints::editBP(BPXTYPE type, const CString & addrText, CWnd* widget)
{
	BRIDGEBP bridgebp;
	if (type != bp_dll)
	{
		ULONGLONG a;
		//swscanf(addrText, L"%I64x", &a);
		swscanf_s(addrText, _T("%I64x"), &a);
		duint addr = (duint)a;
		//duint addr = addrText.toULongLong(nullptr, 16);
		if (!DbgFunctions()->GetBridgeBp(type, addr, &bridgebp))
			return false;
	}
	CString tmpStr = addrText;
	string str = CT2A(tmpStr.GetBuffer());

	/*
	else if (!DbgFunctions()->GetBridgeBp(type, reinterpret_cast<duint>(addrText.toUtf8().constData()), &bridgebp))
		return false;
	
	EditBreakpointDialog dialog(widget, bridgebp);
	if (dialog.exec() != QDialog::Accepted)
		return false;
	auto bp = dialog.getBp();
	auto exec = [](const CString & command)
	{
		DbgCmdExecDirect(command.toUtf8().constData());
	};
	switch (type)
	{
	case bp_normal:
		exec(CString("SetBreakpointName %1, \"%2\"").arg(addrText).arg(bp.name));
		exec(CString("SetBreakpointCondition %1, \"%2\"").arg(addrText).arg(bp.breakCondition));
		exec(CString("SetBreakpointLog %1, \"%2\"").arg(addrText).arg(bp.logText));
		exec(CString("SetBreakpointLogCondition %1, \"%2\"").arg(addrText).arg(bp.logCondition));
		exec(CString("SetBreakpointCommand %1, \"%2\"").arg(addrText).arg(bp.commandText));
		exec(CString("SetBreakpointCommandCondition %1, \"%2\"").arg(addrText).arg(bp.commandCondition));
		exec(CString("ResetBreakpointHitCount %1, %2").arg(addrText).arg(ToPtrString(bp.hitCount)));
		exec(CString("SetBreakpointFastResume %1, %2").arg(addrText).arg(bp.fastResume));
		exec(CString("SetBreakpointSilent %1, %2").arg(addrText).arg(bp.silent));
		exec(CString("SetBreakpointSingleshoot %1, %2").arg(addrText).arg(bp.singleshoot));
		break;
	case bp_hardware:
		exec(CString("SetHardwareBreakpointName %1, \"%2\"").arg(addrText).arg(bp.name));
		exec(CString("SetHardwareBreakpointCondition %1, \"%2\"").arg(addrText).arg(bp.breakCondition));
		exec(CString("SetHardwareBreakpointLog %1, \"%2\"").arg(addrText).arg(bp.logText));
		exec(CString("SetHardwareBreakpointLogCondition %1, \"%2\"").arg(addrText).arg(bp.logCondition));
		exec(CString("SetHardwareBreakpointCommand %1, \"%2\"").arg(addrText).arg(bp.commandText));
		exec(CString("SetHardwareBreakpointCommandCondition %1, \"%2\"").arg(addrText).arg(bp.commandCondition));
		exec(CString("ResetHardwareBreakpointHitCount %1, %2").arg(addrText).arg(ToPtrString(bp.hitCount)));
		exec(CString("SetHardwareBreakpointFastResume %1, %2").arg(addrText).arg(bp.fastResume));
		exec(CString("SetHardwareBreakpointSilent %1, %2").arg(addrText).arg(bp.silent));
		exec(CString("SetHardwareBreakpointSingleshoot %1, %2").arg(addrText).arg(bp.singleshoot));
		break;
	case bp_memory:
		exec(CString("SetMemoryBreakpointName %1, \"\"%2\"\"").arg(addrText).arg(bp.name));
		exec(CString("SetMemoryBreakpointCondition %1, \"%2\"").arg(addrText).arg(bp.breakCondition));
		exec(CString("SetMemoryBreakpointLog %1, \"%2\"").arg(addrText).arg(bp.logText));
		exec(CString("SetMemoryBreakpointLogCondition %1, \"%2\"").arg(addrText).arg(bp.logCondition));
		exec(CString("SetMemoryBreakpointCommand %1, \"%2\"").arg(addrText).arg(bp.commandText));
		exec(CString("SetMemoryBreakpointCommandCondition %1, \"%2\"").arg(addrText).arg(bp.commandCondition));
		exec(CString("ResetMemoryBreakpointHitCount %1, %2").arg(addrText).arg(ToPtrString(bp.hitCount)));
		exec(CString("SetMemoryBreakpointFastResume %1, %2").arg(addrText).arg(bp.fastResume));
		exec(CString("SetMemoryBreakpointSilent %1, %2").arg(addrText).arg(bp.silent));
		exec(CString("SetMemoryBreakpointSingleshoot %1, %2").arg(addrText).arg(bp.singleshoot));
		break;
	case bp_dll:
		exec(CString("SetLibrarianBreakpointName \"%1\", \"\"%2\"\"").arg(addrText).arg(bp.name));
		exec(CString("SetLibrarianBreakpointCondition \"%1\", \"%2\"").arg(addrText).arg(bp.breakCondition));
		exec(CString("SetLibrarianBreakpointLog \"%1\", \"%2\"").arg(addrText).arg(bp.logText));
		exec(CString("SetLibrarianBreakpointLogCondition \"%1\", \"%2\"").arg(addrText).arg(bp.logCondition));
		exec(CString("SetLibrarianBreakpointCommand \"%1\", \"%2\"").arg(addrText).arg(bp.commandText));
		exec(CString("SetLibrarianBreakpointCommandCondition \"%1\", \"%2\"").arg(addrText).arg(bp.commandCondition));
		exec(CString("ResetLibrarianBreakpointHitCount \"%1\", %2").arg(addrText).arg(ToPtrString(bp.hitCount)));
		exec(CString("SetLibrarianBreakpointFastResume \"%1\", %2").arg(addrText).arg(bp.fastResume));
		exec(CString("SetLibrarianBreakpointSilent \"%1\", %2").arg(addrText).arg(bp.silent));
		exec(CString("SetLibrarianBreakpointSingleshoot \"%1\", %2").arg(addrText).arg(bp.singleshoot));
		break;
	case bp_exception:
		exec(CString("SetExceptionBreakpointName %1, \"%2\"").arg(addrText).arg(bp.name));
		exec(CString("SetExceptionBreakpointCondition %1, \"%2\"").arg(addrText).arg(bp.breakCondition));
		exec(CString("SetExceptionBreakpointLog %1, \"%2\"").arg(addrText).arg(bp.logText));
		exec(CString("SetExceptionBreakpointLogCondition %1, \"%2\"").arg(addrText).arg(bp.logCondition));
		exec(CString("SetExceptionBreakpointCommand %1, \"%2\"").arg(addrText).arg(bp.commandText));
		exec(CString("SetExceptionBreakpointCommandCondition %1, \"%2\"").arg(addrText).arg(bp.commandCondition));
		exec(CString("ResetExceptionBreakpointHitCount %1, %2").arg(addrText).arg(ToPtrString(bp.hitCount)));
		exec(CString("SetExceptionBreakpointFastResume %1, %2").arg(addrText).arg(bp.fastResume));
		exec(CString("SetExceptionBreakpointSilent %1, %2").arg(addrText).arg(bp.silent));
		exec(CString("SetExceptionBreakpointSingleshoot %1, %2").arg(addrText).arg(bp.singleshoot));
		break;
	default:
		return false;
	}
	GuiUpdateBreakpointsView();
	*/
	return true;
}
