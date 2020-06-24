#pragma once
class SettingsX
{
public:
	SettingsX();
	~SettingsX();
	void SaveSettings();
	void LoadSettings();

private:
	//enums
	enum CalcType
	{
		calc_signed = 0,
		calc_unsigned = 1
	};
	enum BreakpointType
	{
		break_int3short = 0,
		break_int3long = 1,
		break_ud2 = 2
	};
	//structures
	struct RangeStruct
	{
		unsigned long start;
		unsigned long end;
	};
	struct SettingsStruct
	{
		//Event Tab
		bool eventSystemBreakpoint;
		bool eventTlsCallbacks;
		bool eventEntryBreakpoint;
		bool eventDllEntry;
		bool eventThreadEntry;
		bool eventAttachBreakpoint;
		bool eventDllLoad;
		bool eventDllUnload;
		bool eventThreadStart;
		bool eventThreadEnd;
		bool eventDebugStrings;
		//Engine Tab
		CalcType engineCalcType;
		BreakpointType engineBreakpointType;
		bool engineUndecorateSymbolNames;
		bool engineEnableDebugPrivilege;
		bool engineEnableSourceDebugging;
		bool engineSaveDatabaseInProgramDirectory;
		bool engineDisableDatabaseCompression;
		bool engineEnableTraceRecordDuringTrace;
		bool engineSkipInt3Stepping;
		bool engineNoScriptTimeout;
		bool engineIgnoreInconsistentBreakpoints;
		bool engineHardcoreThreadSwitchWarning;
		bool engineVerboseExceptionLogging;
		bool engineNoWow64SingleStepWorkaround;
		int engineMaxTraceCount;
		//Exception Tab
		CList<RangeStruct>* exceptionRanges;
		//Disasm Tab
		bool disasmArgumentSpaces;
		bool disasmMemorySpaces;
		bool disasmHidePointerSizes;
		bool disasmHideNormalSegments;
		bool disasmUppercase;
		bool disasmOnlyCipAutoComments;
		bool disasmTabBetweenMnemonicAndArguments;
		bool disasmNoHighlightOperands;
		bool disasmNoCurrentModuleText;
		bool disasmPermanentHighlightingMode;
		bool disasm0xPrefixValues;
		bool disasmNoSourceLineAutoComments;
		int disasmMaxModuleSize;
		//Gui Tab
		bool guiFpuRegistersLittleEndian;
		bool guiSaveColumnOrder;
		bool guiNoCloseDialog;
		bool guiPidInHex;
		bool guiSidebarWatchLabels;
		bool guiNoForegroundWindow;
		bool guiLoadSaveTabOrder;
		bool guiShowGraphRva;
		bool guiShowExitConfirmation;
		bool guiDisableAutoComplete;
		bool guiAsciiAddressDumpMode;
		//Misc Tab
		bool miscSetJIT;
		bool miscSetJITAuto;
		bool miscSymbolStore;
		bool miscSymbolCache;
		bool miscUtf16LogRedirect;
		bool miscUseLocalHelpFile;
		bool miscQueryProcessCookie;
		bool miscQueryWorkingSet;
	};
	//variables
	bool bJitOld;
	bool bJitAutoOld;
	SettingsStruct settings;
	CList<RangeStruct> realExceptionRanges;
};

