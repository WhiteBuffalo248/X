#include "cmd-breakpoint-control.h"
#include "memory.h"
#include "value.h"
#include "debugger.h"

// breakpoint enumeration callbacks
static bool cbDeleteAllBreakpoints(const BREAKPOINT* bp)
{
	if (bp->type != BPNORMAL)
		return true;
	if (!BpDelete(bp->addr, BPNORMAL))
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "Delete breakpoint failed (BpDelete): %p\n"), bp->addr);
		return false;
	}
	if (bp->enabled && !DeleteBPX(bp->addr))
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "Delete breakpoint failed (DeleteBPX): %p\n"), bp->addr);
		return false;
	}
	return true;
}

static bool cbEnableAllBreakpoints(const BREAKPOINT* bp)
{
	if (bp->type != BPNORMAL || bp->enabled)
		return true;

	if (!SetBPX(bp->addr, bp->titantype, (void*)cbUserBreakpoint))
	{
		if (!MemIsValidReadPtr(bp->addr))
			return true;
		dprintf(QT_TRANSLATE_NOOP("DBG", "Could not enable breakpoint %p (SetBPX)\n"), bp->addr);
		return false;
	}
	if (!BpEnable(bp->addr, BPNORMAL, true))
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "Could not enable breakpoint %p (BpEnable)\n"), bp->addr);
		return false;
	}
	return true;
}

static bool cbDisableAllBreakpoints(const BREAKPOINT* bp)
{
	if (bp->type != BPNORMAL || !bp->enabled)
		return true;

	if (!BpEnable(bp->addr, BPNORMAL, false))
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "Could not disable breakpoint %p (BpEnable)\n"), bp->addr);
		return false;
	}
	if (!DeleteBPX(bp->addr))
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "Could not disable breakpoint %p (DeleteBPX)\n"), bp->addr);
		return false;
	}
	return true;
}

bool cbDebugSetBPX(int argc, char* argv[]) //bp addr [,name [,type]]
{
	if (IsArgumentsLessThan(argc, 2))
		return false;
	char argaddr[deflen] = "";
	strcpy_s(argaddr, argv[1]);
	char argname[deflen] = "";
	if (argc > 2)
		strcpy_s(argname, argv[2]);
	char argtype[deflen] = "";
	bool has_arg2 = argc > 3;
	if (has_arg2)
		strcpy_s(argtype, argv[3]);
	if (!has_arg2 && (scmp(argname, "ss") || scmp(argname, "long") || scmp(argname, "ud2")))
	{
		strcpy_s(argtype, argname);
		*argname = 0;
	}
	_strlwr_s(argtype);
	duint addr = 0;
	if (!valfromstring(argaddr, &addr))
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "Invalid addr: \"%s\"\n"), argaddr);
		return false;
	}
	int type = 0;
	bool singleshoot = false;
	if (strstr(argtype, "ss"))
	{
		type |= UE_SINGLESHOOT;
		singleshoot = true;
	}
	else
		type |= UE_BREAKPOINT;
	if (strstr(argtype, "long"))
		type |= UE_BREAKPOINT_TYPE_LONG_INT3;
	else if (strstr(argtype, "ud2"))
		type |= UE_BREAKPOINT_TYPE_UD2;
	else if (strstr(argtype, "short"))
		type |= UE_BREAKPOINT_TYPE_INT3;
	short oldbytes;
	const char* bpname = 0;
	if (*argname)
		bpname = argname;
	BREAKPOINT bp;
	if (BpGet(addr, BPNORMAL, bpname, &bp))
	{
		if (!bp.enabled)
			return DbgCmdExecDirect(StringUtils::sprintf("bpe %p", bp.addr).c_str());
		dputs(QT_TRANSLATE_NOOP("DBG", "Breakpoint already set!"));
		return true;
	}
	if (IsBPXEnabled(addr))
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "Error setting breakpoint at %p! (IsBPXEnabled)\n"), addr);
		return false;
	}
	if (!MemRead(addr, &oldbytes, sizeof(short)))
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "Error setting breakpoint at %p! (memread)\n"), addr);
		return false;
	}
	if (!BpNew(addr, true, singleshoot, oldbytes, BPNORMAL, type, bpname))
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "Error setting breakpoint at %p! (bpnew)\n"), addr);
		return false;
	}
	if (!SetBPX(addr, type, (void*)cbUserBreakpoint))
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "Error setting breakpoint at %p! (SetBPX)\n"), addr);
		if (!BpDelete(addr, BPNORMAL))
			dprintf(QT_TRANSLATE_NOOP("DBG", "Error handling invalid breakpoint at %p! (bpdel)\n"), addr);
		return false;
	}
	GuiUpdateAllViews();
	if (bpname)
		dprintf(QT_TRANSLATE_NOOP("DBG", "Breakpoint at %p (%s) set!\n"), addr, bpname);
	else
		dprintf(QT_TRANSLATE_NOOP("DBG", "Breakpoint at %p set!\n"), addr);
	return true;
}

bool cbDebugDeleteBPX(int argc, char* argv[])
{
	if (argc < 2) //delete all breakpoints
	{
		if (!BpGetCount(BPNORMAL))
		{
			dputs(QT_TRANSLATE_NOOP("DBG", "No breakpoints to delete!"));
			return true;
		}
		if (!BpEnumAll(cbDeleteAllBreakpoints)) //at least one deletion failed
		{
			GuiUpdateAllViews();
			return false;
		}
		dputs(QT_TRANSLATE_NOOP("DBG", "All breakpoints deleted!"));
		GuiUpdateAllViews();
		return true;
	}
	BREAKPOINT found;
	if (BpGet(0, BPNORMAL, argv[1], &found)) //found a breakpoint with name
	{
		if (!BpDelete(found.addr, BPNORMAL))
		{
			dprintf(QT_TRANSLATE_NOOP("DBG", "Delete breakpoint failed (bpdel): %p\n"), found.addr);
			return false;
		}
		if (found.enabled && !DeleteBPX(found.addr))
		{
			GuiUpdateAllViews();
			if (!MemIsValidReadPtr(found.addr))
				return true;
			dprintf(QT_TRANSLATE_NOOP("DBG", "Delete breakpoint failed (DeleteBPX): %p\n"), found.addr);
			return false;
		}
		return true;
	}
	duint addr = 0;
	if (!valfromstring(argv[1], &addr) || !BpGet(addr, BPNORMAL, 0, &found)) //invalid breakpoint
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "No such breakpoint \"%s\"\n"), argv[1]);
		return false;
	}
	if (!BpDelete(found.addr, BPNORMAL))
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "Delete breakpoint failed (bpdel): %p\n"), found.addr);
		return false;
	}
	if (found.enabled && !DeleteBPX(found.addr))
	{
		GuiUpdateAllViews();
		if (!MemIsValidReadPtr(found.addr))
			return true;
		dprintf(QT_TRANSLATE_NOOP("DBG", "Delete breakpoint failed (DeleteBPX): %p\n"), found.addr);
		return false;
	}
	dputs(QT_TRANSLATE_NOOP("DBG", "Breakpoint deleted!"));
	GuiUpdateAllViews();
	return true;
}

bool cbDebugEnableBPX(int argc, char* argv[])
{
	if (argc < 2) //enable all breakpoints
	{
		if (!BpGetCount(BPNORMAL))
		{
			dputs(QT_TRANSLATE_NOOP("DBG", "No breakpoints to enable!"));
			return true;
		}
		if (!BpEnumAll(cbEnableAllBreakpoints)) //at least one enable failed
			return false;
		dputs(QT_TRANSLATE_NOOP("DBG", "All breakpoints enabled!"));
		GuiUpdateAllViews();
		return true;
	}
	BREAKPOINT found;
	if (BpGet(0, BPNORMAL, argv[1], &found)) //found a breakpoint with name
	{
		if (!SetBPX(found.addr, found.titantype, (void*)cbUserBreakpoint))
		{
			dprintf(QT_TRANSLATE_NOOP("DBG", "Could not enable breakpoint %p (SetBPX)\n"), found.addr);
			return false;
		}
		if (!BpEnable(found.addr, BPNORMAL, true))
		{
			dprintf(QT_TRANSLATE_NOOP("DBG", "Could not enable breakpoint %p (BpEnable)\n"), found.addr);
			return false;
		}
		GuiUpdateAllViews();
		return true;
	}
	duint addr = 0;
	if (!valfromstring(argv[1], &addr) || !BpGet(addr, BPNORMAL, 0, &found)) //invalid breakpoint
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "No such breakpoint \"%s\"\n"), argv[1]);
		return false;
	}
	if (found.enabled)
	{
		dputs(QT_TRANSLATE_NOOP("DBG", "Breakpoint already enabled!"));
		GuiUpdateAllViews();
		return true;
	}
	if (!SetBPX(found.addr, found.titantype, (void*)cbUserBreakpoint))
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "Could not enable breakpoint %p (SetBPX)\n"), found.addr);
		return false;
	}
	if (!BpEnable(found.addr, BPNORMAL, true))
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "Could not enable breakpoint %p (BpEnable)\n"), found.addr);
		return false;
	}
	GuiUpdateAllViews();
	dputs(QT_TRANSLATE_NOOP("DBG", "Breakpoint enabled!"));
	return true;
}

bool cbDebugDisableBPX(int argc, char* argv[])
{
	if (argc < 2) //delete all breakpoints
	{
		if (!BpGetCount(BPNORMAL))
		{
			dputs(QT_TRANSLATE_NOOP("DBG", "No breakpoints to disable!"));
			return true;
		}
		if (!BpEnumAll(cbDisableAllBreakpoints)) //at least one deletion failed
			return false;
		dputs(QT_TRANSLATE_NOOP("DBG", "All breakpoints disabled!"));
		GuiUpdateAllViews();
		return true;
	}
	BREAKPOINT found;
	if (BpGet(0, BPNORMAL, argv[1], &found)) //found a breakpoint with name
	{
		if (!BpEnable(found.addr, BPNORMAL, false))
		{
			dprintf(QT_TRANSLATE_NOOP("DBG", "Could not disable breakpoint %p (BpEnable)\n"), found.addr);
			return false;
		}
		if (!DeleteBPX(found.addr))
		{
			GuiUpdateAllViews();
			if (!MemIsValidReadPtr(found.addr))
				return true;
			dprintf(QT_TRANSLATE_NOOP("DBG", "Could not disable breakpoint %p (DeleteBPX)\n"), found.addr);
			return false;
		}
		GuiUpdateAllViews();
		return true;
	}
	duint addr = 0;
	if (!valfromstring(argv[1], &addr) || !BpGet(addr, BPNORMAL, 0, &found)) //invalid breakpoint
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "No such breakpoint \"%s\"\n"), argv[1]);
		return false;
	}
	if (!found.enabled)
	{
		dputs(QT_TRANSLATE_NOOP("DBG", "Breakpoint already disabled!"));
		return true;
	}
	if (!BpEnable(found.addr, BPNORMAL, false))
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "Could not disable breakpoint %p (BpEnable)\n"), found.addr);
		return false;
	}
	if (!DeleteBPX(found.addr))
	{
		GuiUpdateAllViews();
		if (!MemIsValidReadPtr(found.addr))
			return true;
		dprintf(QT_TRANSLATE_NOOP("DBG", "Could not disable breakpoint %p (DeleteBPX)\n"), found.addr);
		return false;
	}
	dputs(QT_TRANSLATE_NOOP("DBG", "Breakpoint disabled!"));
	GuiUpdateAllViews();
	return true;
}

static bool cbDeleteAllHardwareBreakpoints(const BREAKPOINT* bp)
{
	if (bp->type != BPHARDWARE)
		return true;
	if (!BpDelete(bp->addr, BPHARDWARE))
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "Delete hardware breakpoint failed (BpDelete): %p\n"), bp->addr);
		return false;
	}
	if (bp->enabled && !DeleteHardwareBreakPoint(TITANGETDRX(bp->titantype)))
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "Delete hardware breakpoint failed (DeleteHardwareBreakPoint): %p\n"), bp->addr);
		return false;
	}
	return true;
}

static bool cbEnableAllHardwareBreakpoints(const BREAKPOINT* bp)
{
	if (bp->type != BPHARDWARE || bp->enabled)
		return true;
	DWORD drx = 0;
	if (!GetUnusedHardwareBreakPointRegister(&drx))
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "Did not enable hardware breakpoint %p (all slots full)\n"), bp->addr);
		return true;
	}
	int titantype = bp->titantype;
	TITANSETDRX(titantype, drx);
	BpSetTitanType(bp->addr, BPHARDWARE, titantype);
	if (!BpEnable(bp->addr, BPHARDWARE, true))
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "Could not enable hardware breakpoint %p (BpEnable)\n"), bp->addr);
		return false;
	}
	if (!SetHardwareBreakPoint(bp->addr, drx, TITANGETTYPE(bp->titantype), TITANGETSIZE(bp->titantype), (void*)cbHardwareBreakpoint))
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "Could not enable hardware breakpoint %p (SetHardwareBreakPoint)\n"), bp->addr);
		return false;
	}
	return true;
}

static bool cbDisableAllHardwareBreakpoints(const BREAKPOINT* bp)
{
	if (bp->type != BPHARDWARE)
		return true;
	if (!BpEnable(bp->addr, BPHARDWARE, false))
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "Could not disable hardware breakpoint %p (BpEnable)\n"), bp->addr);
		return false;
	}
	if (bp->enabled && !DeleteHardwareBreakPoint(TITANGETDRX(bp->titantype)))
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "Could not disable hardware breakpoint %p (DeleteHardwareBreakPoint)\n"), bp->addr);
		return false;
	}
	return true;
}

bool cbDebugSetHardwareBreakpoint(int argc, char* argv[])
{
	if (IsArgumentsLessThan(argc, 2))
		return false;
	duint addr;
	if (!valfromstring(argv[1], &addr))
		return false;
	DWORD type = UE_HARDWARE_EXECUTE;
	if (argc > 2)
	{
		switch (*argv[2])
		{
		case 'r':
			type = UE_HARDWARE_READWRITE;
			break;
		case 'w':
			type = UE_HARDWARE_WRITE;
			break;
		case 'x':
			break;
		default:
			dputs(QT_TRANSLATE_NOOP("DBG", "Invalid type, assuming 'x'"));
			break;
		}
	}
	DWORD titsize = UE_HARDWARE_SIZE_1;
	if (argc > 3)
	{
		duint size;
		if (!valfromstring(argv[3], &size))
			return false;
		switch (size)
		{
		case 1:
			titsize = UE_HARDWARE_SIZE_1;
			break;
		case 2:
			titsize = UE_HARDWARE_SIZE_2;
			break;
		case 4:
			titsize = UE_HARDWARE_SIZE_4;
			break;
#ifdef _WIN64
		case 8:
			titsize = UE_HARDWARE_SIZE_8;
			break;
#endif // _WIN64
		default:
			titsize = UE_HARDWARE_SIZE_1;
			dputs(QT_TRANSLATE_NOOP("DBG", "Invalid size, using 1"));
			break;
		}
		if ((addr % size) != 0)
		{
			dprintf(QT_TRANSLATE_NOOP("DBG", "Address not aligned to %d\n"), int(size));
			return false;
		}
	}
	DWORD drx = 0;
	if (!GetUnusedHardwareBreakPointRegister(&drx))
	{
		dputs(QT_TRANSLATE_NOOP("DBG", "You can only set 4 hardware breakpoints"));
		return false;
	}
	int titantype = 0;
	TITANSETDRX(titantype, drx);
	TITANSETTYPE(titantype, type);
	TITANSETSIZE(titantype, titsize);
	//TODO: hwbp in multiple threads TEST
	BREAKPOINT bp;
	if (BpGet(addr, BPHARDWARE, 0, &bp))
	{
		if (!bp.enabled)
			return DbgCmdExecDirect(StringUtils::sprintf("bphwe %p", bp.addr).c_str());
		dputs(QT_TRANSLATE_NOOP("DBG", "Hardware breakpoint already set!"));
		return true;
	}
	if (!BpNew(addr, true, false, 0, BPHARDWARE, titantype, 0))
	{
		dputs(QT_TRANSLATE_NOOP("DBG", "Error setting hardware breakpoint (bpnew)!"));
		return false;
	}
	if (!SetHardwareBreakPoint(addr, drx, type, titsize, (void*)cbHardwareBreakpoint))
	{
		dputs(QT_TRANSLATE_NOOP("DBG", "Error setting hardware breakpoint (TitanEngine)!"));
		return false;
	}
	dprintf(QT_TRANSLATE_NOOP("DBG", "Hardware breakpoint at %p set!\n"), addr);
	GuiUpdateAllViews();
	return true;
}

bool cbDebugDeleteHardwareBreakpoint(int argc, char* argv[])
{
	if (argc < 2) //delete all breakpoints
	{
		if (!BpGetCount(BPHARDWARE))
		{
			dputs(QT_TRANSLATE_NOOP("DBG", "No hardware breakpoints to delete!"));
			return true;
		}
		if (!BpEnumAll(cbDeleteAllHardwareBreakpoints)) //at least one deletion failed
			return false;
		dputs(QT_TRANSLATE_NOOP("DBG", "All hardware breakpoints deleted!"));
		GuiUpdateAllViews();
		return true;
	}
	BREAKPOINT found;
	if (BpGet(0, BPHARDWARE, argv[1], &found)) //found a breakpoint with name
	{
		if (!BpDelete(found.addr, BPHARDWARE))
		{
			dprintf(QT_TRANSLATE_NOOP("DBG", "Delete hardware breakpoint failed: %p (BpDelete)\n"), found.addr);
			return false;
		}
		if (!DeleteHardwareBreakPoint(TITANGETDRX(found.titantype)))
		{
			dprintf(QT_TRANSLATE_NOOP("DBG", "Delete hardware breakpoint failed: %p (DeleteHardwareBreakPoint)\n"), found.addr);
			return false;
		}
		return true;
	}
	duint addr = 0;
	if (!valfromstring(argv[1], &addr) || !BpGet(addr, BPHARDWARE, 0, &found)) //invalid breakpoint
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "No such hardware breakpoint \"%s\"\n"), argv[1]);
		return false;
	}
	if (!BpDelete(found.addr, BPHARDWARE))
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "Delete hardware breakpoint failed: %p (BpDelete)\n"), found.addr);
		return false;
	}
	if (!DeleteHardwareBreakPoint(TITANGETDRX(found.titantype)))
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "Delete hardware breakpoint failed: %p (DeleteHardwareBreakPoint)\n"), found.addr);
		return false;
	}
	dputs(QT_TRANSLATE_NOOP("DBG", "Hardware breakpoint deleted!"));
	GuiUpdateAllViews();
	return true;
}

bool cbDebugEnableHardwareBreakpoint(int argc, char* argv[])
{
	DWORD drx = 0;
	if (!GetUnusedHardwareBreakPointRegister(&drx))
	{
		dputs(QT_TRANSLATE_NOOP("DBG", "You can only set 4 hardware breakpoints"));
		return false;
	}
	if (argc < 2) //enable all hardware breakpoints
	{
		if (!BpGetCount(BPHARDWARE))
		{
			dputs(QT_TRANSLATE_NOOP("DBG", "No hardware breakpoints to enable!"));
			return true;
		}
		if (!BpEnumAll(cbEnableAllHardwareBreakpoints)) //at least one enable failed
			return false;
		dputs(QT_TRANSLATE_NOOP("DBG", "All hardware breakpoints enabled!"));
		GuiUpdateAllViews();
		return true;
	}
	BREAKPOINT found;
	duint addr = 0;
	if (!valfromstring(argv[1], &addr) || !BpGet(addr, BPHARDWARE, 0, &found)) //invalid hardware breakpoint
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "No such hardware breakpoint \"%s\"\n"), argv[1]);
		return false;
	}
	if (found.enabled)
	{
		dputs(QT_TRANSLATE_NOOP("DBG", "Hardware breakpoint already enabled!"));
		GuiUpdateAllViews();
		return true;
	}
	TITANSETDRX(found.titantype, drx);
	BpSetTitanType(found.addr, BPHARDWARE, found.titantype);
	if (!SetHardwareBreakPoint(found.addr, drx, TITANGETTYPE(found.titantype), TITANGETSIZE(found.titantype), (void*)cbHardwareBreakpoint))
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "Could not enable hardware breakpoint %p (SetHardwareBreakpoint)\n"), found.addr);
		return false;
	}
	if (!BpEnable(found.addr, BPHARDWARE, true))
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "Could not enable hardware breakpoint %p (BpEnable)\n"), found.addr);
		return false;
	}
	dputs(QT_TRANSLATE_NOOP("DBG", "Hardware breakpoint enabled!"));
	GuiUpdateAllViews();
	return true;
}

bool cbDebugDisableHardwareBreakpoint(int argc, char* argv[])
{
	if (argc < 2) //delete all hardware breakpoints
	{
		if (!BpGetCount(BPHARDWARE))
		{
			dputs(QT_TRANSLATE_NOOP("DBG", "No hardware breakpoints to disable!"));
			return true;
		}
		if (!BpEnumAll(cbDisableAllHardwareBreakpoints)) //at least one deletion failed
			return false;
		dputs(QT_TRANSLATE_NOOP("DBG", "All hardware breakpoints disabled!"));
		GuiUpdateAllViews();
		return true;
	}
	BREAKPOINT found;
	duint addr = 0;
	if (!valfromstring(argv[1], &addr) || !BpGet(addr, BPHARDWARE, 0, &found)) //invalid hardware breakpoint
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "No such hardware breakpoint \"%s\"\n"), argv[1]);
		return false;
	}
	if (!found.enabled)
	{
		dputs(QT_TRANSLATE_NOOP("DBG", "Hardware breakpoint already disabled!"));
		return true;
	}
	if (!BpEnable(found.addr, BPHARDWARE, false))
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "Could not disable hardware breakpoint %p (BpEnable)\n"), found.addr);
		return false;
	}
	if (!DeleteHardwareBreakPoint(TITANGETDRX(found.titantype)))
	{
		dprintf(QT_TRANSLATE_NOOP("DBG", "Could not disable hardware breakpoint %p (DeleteHardwareBreakpoint)\n"), found.addr);
		return false;
	}
	dputs(QT_TRANSLATE_NOOP("DBG", "Hardware breakpoint disabled!"));
	GuiUpdateAllViews();
	return true;
}