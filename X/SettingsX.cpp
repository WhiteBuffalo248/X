#include "stdafx.h"
#include "SettingsX.h"
#include "Bridge.h"
#include "Configuration.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

SettingsX::SettingsX()
{
	settings.exceptionRanges = &realExceptionRanges;
	LoadSettings(); //load settings from file
}


SettingsX::~SettingsX()
{
}

void SettingsX::LoadSettings()
{
	//Flush pending config changes
	Config()->save();

	//Defaults
	memset(&settings, 0, sizeof(SettingsStruct));
	settings.eventSystemBreakpoint = true;
	settings.eventTlsCallbacks = true;
	settings.eventEntryBreakpoint = true;
	settings.eventAttachBreakpoint = true;
	settings.engineCalcType = calc_unsigned;
	settings.engineBreakpointType = break_int3short;
	settings.engineUndecorateSymbolNames = true;
	settings.engineEnableSourceDebugging = false;
	settings.engineEnableTraceRecordDuringTrace = true;
	settings.engineNoScriptTimeout = false;
	settings.engineIgnoreInconsistentBreakpoints = false;
	settings.engineNoWow64SingleStepWorkaround = false;
	settings.engineMaxTraceCount = 50000;
	settings.engineHardcoreThreadSwitchWarning = false;
	settings.engineVerboseExceptionLogging = true;
	settings.exceptionRanges = &realExceptionRanges;
	settings.disasmArgumentSpaces = false;
	settings.disasmHidePointerSizes = false;
	settings.disasmHideNormalSegments = false;
	settings.disasmMemorySpaces = false;
	settings.disasmUppercase = false;
	settings.disasmOnlyCipAutoComments = false;
	settings.disasmTabBetweenMnemonicAndArguments = false;
	settings.disasmNoCurrentModuleText = false;
	settings.disasm0xPrefixValues = false;
	settings.disasmNoSourceLineAutoComments = false;
	settings.disasmMaxModuleSize = -1;
	settings.guiNoForegroundWindow = true;
	settings.guiDisableAutoComplete = false;
	settings.guiAsciiAddressDumpMode = false;
	/*
	//Events tab
	GetSettingBool("Events", "SystemBreakpoint", &settings.eventSystemBreakpoint);
	GetSettingBool("Events", "TlsCallbacks", &settings.eventTlsCallbacks);
	GetSettingBool("Events", "EntryBreakpoint", &settings.eventEntryBreakpoint);
	GetSettingBool("Events", "DllEntry", &settings.eventDllEntry);
	GetSettingBool("Events", "ThreadEntry", &settings.eventThreadEntry);
	GetSettingBool("Events", "AttachBreakpoint", &settings.eventAttachBreakpoint);
	GetSettingBool("Events", "DllLoad", &settings.eventDllLoad);
	GetSettingBool("Events", "DllUnload", &settings.eventDllUnload);
	GetSettingBool("Events", "ThreadStart", &settings.eventThreadStart);
	GetSettingBool("Events", "ThreadEnd", &settings.eventThreadEnd);
	GetSettingBool("Events", "DebugStrings", &settings.eventDebugStrings);
	ui->chkSystemBreakpoint->setCheckState(bool2check(settings.eventSystemBreakpoint));
	ui->chkTlsCallbacks->setCheckState(bool2check(settings.eventTlsCallbacks));
	ui->chkEntryBreakpoint->setCheckState(bool2check(settings.eventEntryBreakpoint));
	ui->chkDllEntry->setCheckState(bool2check(settings.eventDllEntry));
	ui->chkThreadEntry->setCheckState(bool2check(settings.eventThreadEntry));
	ui->chkAttachBreakpoint->setCheckState(bool2check(settings.eventAttachBreakpoint));
	ui->chkDllLoad->setCheckState(bool2check(settings.eventDllLoad));
	ui->chkDllUnload->setCheckState(bool2check(settings.eventDllUnload));
	ui->chkThreadStart->setCheckState(bool2check(settings.eventThreadStart));
	ui->chkThreadEnd->setCheckState(bool2check(settings.eventThreadEnd));
	ui->chkDebugStrings->setCheckState(bool2check(settings.eventDebugStrings));

	//Engine tab
	duint cur;
	if (BridgeSettingGetUint("Engine", "CalculationType", &cur))
	{
		switch (cur)
		{
		case calc_signed:
		case calc_unsigned:
			settings.engineCalcType = (CalcType)cur;
			break;
		}
	}
	if (BridgeSettingGetUint("Engine", "BreakpointType", &cur))
	{
		switch (cur)
		{
		case break_int3short:
		case break_int3long:
		case break_ud2:
			settings.engineBreakpointType = (BreakpointType)cur;
			break;
		}
	}
	GetSettingBool("Engine", "UndecorateSymbolNames", &settings.engineUndecorateSymbolNames);
	GetSettingBool("Engine", "EnableDebugPrivilege", &settings.engineEnableDebugPrivilege);
	GetSettingBool("Engine", "EnableSourceDebugging", &settings.engineEnableSourceDebugging);
	GetSettingBool("Engine", "SaveDatabaseInProgramDirectory", &settings.engineSaveDatabaseInProgramDirectory);
	GetSettingBool("Engine", "DisableDatabaseCompression", &settings.engineDisableDatabaseCompression);
	GetSettingBool("Engine", "TraceRecordEnabledDuringTrace", &settings.engineEnableTraceRecordDuringTrace);
	GetSettingBool("Engine", "SkipInt3Stepping", &settings.engineSkipInt3Stepping);
	GetSettingBool("Engine", "NoScriptTimeout", &settings.engineNoScriptTimeout);
	GetSettingBool("Engine", "IgnoreInconsistentBreakpoints", &settings.engineIgnoreInconsistentBreakpoints);
	GetSettingBool("Engine", "HardcoreThreadSwitchWarning", &settings.engineHardcoreThreadSwitchWarning);
	GetSettingBool("Engine", "VerboseExceptionLogging", &settings.engineVerboseExceptionLogging);
	GetSettingBool("Engine", "NoWow64SingleStepWorkaround", &settings.engineNoWow64SingleStepWorkaround);
	if (BridgeSettingGetUint("Engine", "MaxTraceCount", &cur))
		settings.engineMaxTraceCount = int(cur);
	switch (settings.engineCalcType)
	{
	case calc_signed:
		ui->radioSigned->setChecked(true);
		break;
	case calc_unsigned:
		ui->radioUnsigned->setChecked(true);
		break;
	}
	switch (settings.engineBreakpointType)
	{
	case break_int3short:
		ui->radioInt3Short->setChecked(true);
		break;
	case break_int3long:
		ui->radioInt3Long->setChecked(true);
		break;
	case break_ud2:
		ui->radioUd2->setChecked(true);
		break;
	}
	ui->chkUndecorateSymbolNames->setChecked(settings.engineUndecorateSymbolNames);
	ui->chkEnableDebugPrivilege->setChecked(settings.engineEnableDebugPrivilege);
	ui->chkEnableSourceDebugging->setChecked(settings.engineEnableSourceDebugging);
	ui->chkSaveDatabaseInProgramDirectory->setChecked(settings.engineSaveDatabaseInProgramDirectory);
	ui->chkDisableDatabaseCompression->setChecked(settings.engineDisableDatabaseCompression);
	ui->chkTraceRecordEnabledDuringTrace->setChecked(settings.engineEnableTraceRecordDuringTrace);
	ui->chkSkipInt3Stepping->setChecked(settings.engineSkipInt3Stepping);
	ui->chkNoScriptTimeout->setChecked(settings.engineNoScriptTimeout);
	ui->chkIgnoreInconsistentBreakpoints->setChecked(settings.engineIgnoreInconsistentBreakpoints);
	ui->chkHardcoreThreadSwitchWarning->setChecked(settings.engineHardcoreThreadSwitchWarning);
	ui->chkVerboseExceptionLogging->setChecked(settings.engineVerboseExceptionLogging);
	ui->chkNoWow64SingleStepWorkaround->setChecked(settings.engineNoWow64SingleStepWorkaround);
	ui->spinMaxTraceCount->setValue(settings.engineMaxTraceCount);

	//Exceptions tab
	char exceptionRange[MAX_SETTING_SIZE] = "";
	if (BridgeSettingGet("Exceptions", "IgnoreRange", exceptionRange))
	{
		QStringList ranges = QString(exceptionRange).split(QString(","), QString::SkipEmptyParts);
		for (int i = 0; i < ranges.size(); i++)
		{
			unsigned long start;
			unsigned long end;
			if (sscanf_s(ranges.at(i).toUtf8().constData(), "%08X-%08X", &start, &end) == 2 && start <= end)
			{
				RangeStruct newRange;
				newRange.start = start;
				newRange.end = end;
				AddRangeToList(newRange);
			}
		}
	}

	//Disasm tab
	GetSettingBool("Disassembler", "ArgumentSpaces", &settings.disasmArgumentSpaces);
	GetSettingBool("Disassembler", "HidePointerSizes", &settings.disasmHidePointerSizes);
	GetSettingBool("Disassembler", "HideNormalSegments", &settings.disasmHideNormalSegments);
	GetSettingBool("Disassembler", "MemorySpaces", &settings.disasmMemorySpaces);
	GetSettingBool("Disassembler", "Uppercase", &settings.disasmUppercase);
	GetSettingBool("Disassembler", "OnlyCipAutoComments", &settings.disasmOnlyCipAutoComments);
	GetSettingBool("Disassembler", "TabbedMnemonic", &settings.disasmTabBetweenMnemonicAndArguments);
	GetSettingBool("Disassembler", "NoHighlightOperands", &settings.disasmNoHighlightOperands);
	GetSettingBool("Disassembler", "PermanentHighlightingMode", &settings.disasmPermanentHighlightingMode);
	GetSettingBool("Disassembler", "NoCurrentModuleText", &settings.disasmNoCurrentModuleText);
	GetSettingBool("Disassembler", "0xPrefixValues", &settings.disasm0xPrefixValues);
	GetSettingBool("Disassembler", "NoSourceLineAutoComments", &settings.disasmNoSourceLineAutoComments);
	if (BridgeSettingGetUint("Disassembler", "MaxModuleSize", &cur))
		settings.disasmMaxModuleSize = int(cur);
	ui->chkArgumentSpaces->setChecked(settings.disasmArgumentSpaces);
	ui->chkHidePointerSizes->setChecked(settings.disasmHidePointerSizes);
	ui->chkHideNormalSegments->setChecked(settings.disasmHideNormalSegments);
	ui->chkMemorySpaces->setChecked(settings.disasmMemorySpaces);
	ui->chkUppercase->setChecked(settings.disasmUppercase);
	ui->chkOnlyCipAutoComments->setChecked(settings.disasmOnlyCipAutoComments);
	ui->chkTabBetweenMnemonicAndArguments->setChecked(settings.disasmTabBetweenMnemonicAndArguments);
	ui->chkNoHighlightOperands->setChecked(settings.disasmNoHighlightOperands);
	ui->chkPermanentHighlightingMode->setChecked(settings.disasmPermanentHighlightingMode);
	ui->chkNoCurrentModuleText->setChecked(settings.disasmNoCurrentModuleText);
	ui->chk0xPrefixValues->setChecked(settings.disasm0xPrefixValues);
	ui->chkNoSourceLinesAutoComments->setChecked(settings.disasmNoSourceLineAutoComments);
	ui->spinMaximumModuleNameSize->setValue(settings.disasmMaxModuleSize);

	//Gui tab
	GetSettingBool("Gui", "FpuRegistersLittleEndian", &settings.guiFpuRegistersLittleEndian);
	GetSettingBool("Gui", "SaveColumnOrder", &settings.guiSaveColumnOrder);
	GetSettingBool("Gui", "NoCloseDialog", &settings.guiNoCloseDialog);
	GetSettingBool("Gui", "PidInHex", &settings.guiPidInHex);
	GetSettingBool("Gui", "SidebarWatchLabels", &settings.guiSidebarWatchLabels);
	GetSettingBool("Gui", "NoForegroundWindow", &settings.guiNoForegroundWindow);
	GetSettingBool("Gui", "LoadSaveTabOrder", &settings.guiLoadSaveTabOrder);
	GetSettingBool("Gui", "ShowGraphRva", &settings.guiShowGraphRva);
	GetSettingBool("Gui", "ShowExitConfirmation", &settings.guiShowExitConfirmation);
	GetSettingBool("Gui", "DisableAutoComplete", &settings.guiDisableAutoComplete);
	GetSettingBool("Gui", "AsciiAddressDumpMode", &settings.guiAsciiAddressDumpMode);
	ui->chkFpuRegistersLittleEndian->setChecked(settings.guiFpuRegistersLittleEndian);
	ui->chkSaveColumnOrder->setChecked(settings.guiSaveColumnOrder);
	ui->chkNoCloseDialog->setChecked(settings.guiNoCloseDialog);
	ui->chkPidInHex->setChecked(settings.guiPidInHex);
	ui->chkSidebarWatchLabels->setChecked(settings.guiSidebarWatchLabels);
	ui->chkNoForegroundWindow->setChecked(settings.guiNoForegroundWindow);
	ui->chkSaveLoadTabOrder->setChecked(settings.guiLoadSaveTabOrder);
	ui->chkShowGraphRva->setChecked(settings.guiShowGraphRva);
	ui->chkShowExitConfirmation->setChecked(settings.guiShowExitConfirmation);
	ui->chkDisableAutoComplete->setChecked(settings.guiDisableAutoComplete);
	ui->chkAsciiAddressDumpMode->setChecked(settings.guiAsciiAddressDumpMode);

	//Misc tab
	if (DbgFunctions()->GetJit)
	{
		char jit_entry[MAX_SETTING_SIZE] = "";
		char jit_def_entry[MAX_SETTING_SIZE] = "";
		bool isx64 = true;
#ifndef _WIN64
		isx64 = false;
#endif
		bool jit_auto_on;
		bool get_jit_works;
		get_jit_works = DbgFunctions()->GetJit(jit_entry, isx64);
		DbgFunctions()->GetDefJit(jit_def_entry);

		if (get_jit_works)
		{
			if (_strcmpi(jit_entry, jit_def_entry) == 0)
				settings.miscSetJIT = true;
		}
		else
			settings.miscSetJIT = false;
		ui->editJIT->setText(jit_entry);
		ui->editJIT->setCursorPosition(0);

		ui->chkSetJIT->setCheckState(bool2check(settings.miscSetJIT));

		bool get_jit_auto_works = DbgFunctions()->GetJitAuto(&jit_auto_on);
		if (!get_jit_auto_works || !jit_auto_on)
			settings.miscSetJITAuto = true;
		else
			settings.miscSetJITAuto = false;

		ui->chkConfirmBeforeAtt->setCheckState(bool2check(settings.miscSetJITAuto));

		if (!DbgFunctions()->IsProcessElevated())
		{
			ui->chkSetJIT->setDisabled(true);
			ui->chkConfirmBeforeAtt->setDisabled(true);
			ui->lblAdminWarning->setText(QString(tr("<font color=\"red\"><b>Warning</b></font>: Run the debugger as Admin to enable JIT.")));
		}
		else
			ui->lblAdminWarning->setText("");
	}
	char setting[MAX_SETTING_SIZE] = "";
	if (BridgeSettingGet("Symbols", "DefaultStore", setting))
		ui->editSymbolStore->setText(QString(setting));
	else
	{
		QString defaultStore("https://msdl.microsoft.com/download/symbols");
		ui->editSymbolStore->setText(defaultStore);
		BridgeSettingSet("Symbols", "DefaultStore", defaultStore.toUtf8().constData());
	}
	if (BridgeSettingGet("Symbols", "CachePath", setting))
		ui->editSymbolCache->setText(QString(setting));
	if (BridgeSettingGet("Misc", "HelpOnSymbolicNameUrl", setting))
		ui->editHelpOnSymbolicNameUrl->setText(QString(setting));

	bJitOld = settings.miscSetJIT;
	bJitAutoOld = settings.miscSetJITAuto;

	GetSettingBool("Misc", "Utf16LogRedirect", &settings.miscUtf16LogRedirect);
	GetSettingBool("Misc", "UseLocalHelpFile", &settings.miscUseLocalHelpFile);
	GetSettingBool("Misc", "QueryProcessCookie", &settings.miscQueryProcessCookie);
	GetSettingBool("Misc", "QueryWorkingSet", &settings.miscQueryWorkingSet);
	ui->chkUtf16LogRedirect->setChecked(settings.miscUtf16LogRedirect);
	ui->chkUseLocalHelpFile->setChecked(settings.miscUseLocalHelpFile);
	ui->chkQueryProcessCookie->setChecked(settings.miscQueryProcessCookie);
	ui->chkQueryWorkingSet->setChecked(settings.miscQueryWorkingSet);
	*/

}

void SettingsX::SaveSettings()
{
	//Events tab
	BridgeSettingSetUint("Events", "SystemBreakpoint", settings.eventSystemBreakpoint);
	BridgeSettingSetUint("Events", "TlsCallbacks", settings.eventTlsCallbacks);
	BridgeSettingSetUint("Events", "EntryBreakpoint", settings.eventEntryBreakpoint);
	BridgeSettingSetUint("Events", "DllEntry", settings.eventDllEntry);
	BridgeSettingSetUint("Events", "ThreadEntry", settings.eventThreadEntry);
	BridgeSettingSetUint("Events", "AttachBreakpoint", settings.eventAttachBreakpoint);
	BridgeSettingSetUint("Events", "DllLoad", settings.eventDllLoad);
	BridgeSettingSetUint("Events", "DllUnload", settings.eventDllUnload);
	BridgeSettingSetUint("Events", "ThreadStart", settings.eventThreadStart);
	BridgeSettingSetUint("Events", "ThreadEnd", settings.eventThreadEnd);
	BridgeSettingSetUint("Events", "DebugStrings", settings.eventDebugStrings);

	//Engine tab
	BridgeSettingSetUint("Engine", "CalculationType", settings.engineCalcType);
	BridgeSettingSetUint("Engine", "BreakpointType", settings.engineBreakpointType);
	BridgeSettingSetUint("Engine", "UndecorateSymbolNames", settings.engineUndecorateSymbolNames);
	BridgeSettingSetUint("Engine", "EnableDebugPrivilege", settings.engineEnableDebugPrivilege);
	BridgeSettingSetUint("Engine", "EnableSourceDebugging", settings.engineEnableSourceDebugging);
	BridgeSettingSetUint("Engine", "SaveDatabaseInProgramDirectory", settings.engineSaveDatabaseInProgramDirectory);
	BridgeSettingSetUint("Engine", "DisableDatabaseCompression", settings.engineDisableDatabaseCompression);
	BridgeSettingSetUint("Engine", "TraceRecordEnabledDuringTrace", settings.engineEnableTraceRecordDuringTrace);
	BridgeSettingSetUint("Engine", "SkipInt3Stepping", settings.engineSkipInt3Stepping);
	BridgeSettingSetUint("Engine", "NoScriptTimeout", settings.engineNoScriptTimeout);
	BridgeSettingSetUint("Engine", "IgnoreInconsistentBreakpoints", settings.engineIgnoreInconsistentBreakpoints);
	BridgeSettingSetUint("Engine", "MaxTraceCount", settings.engineMaxTraceCount);
	BridgeSettingSetUint("Engine", "VerboseExceptionLogging", settings.engineVerboseExceptionLogging);
	BridgeSettingSetUint("Engine", "HardcoreThreadSwitchWarning", settings.engineHardcoreThreadSwitchWarning);
	BridgeSettingSetUint("Engine", "NoWow64SingleStepWorkaround", settings.engineNoWow64SingleStepWorkaround);

	//Exceptions tab
	CString exceptionRange = _T("");
	int count = settings.exceptionRanges->GetCount();
	for (int i = 0; i < count; i++)
	{
		POSITION pos = settings.exceptionRanges->FindIndex(i);
		exceptionRange.AppendFormat(_T("%.8X-%.8X"), settings.exceptionRanges->GetAt(pos).start, settings.exceptionRanges->GetAt(pos).end );
		if (i + 1 < count)
		exceptionRange.Append(_T(","));
	}
	
	if (exceptionRange.GetLength())
	{
		int iTextLen = WideCharToMultiByte(CP_ACP, 0, exceptionRange, -1, NULL, 0, NULL, NULL);
		char* tmpBuffer = new char[iTextLen + 1];
		memset((void*)tmpBuffer, 0, sizeof(char) * (iTextLen + 1));
		WideCharToMultiByte(CP_ACP, 0, exceptionRange, -1, tmpBuffer, iTextLen + 1, NULL, NULL);
		BridgeSettingSet("Exceptions", "IgnoreRange", tmpBuffer);
		delete[] tmpBuffer;
	}		
	else
		BridgeSettingSet("Exceptions", "IgnoreRange", "");

	//Disasm tab
	BridgeSettingSetUint("Disassembler", "ArgumentSpaces", settings.disasmArgumentSpaces);
	BridgeSettingSetUint("Disassembler", "HidePointerSizes", settings.disasmHidePointerSizes);
	BridgeSettingSetUint("Disassembler", "HideNormalSegments", settings.disasmHideNormalSegments);
	BridgeSettingSetUint("Disassembler", "MemorySpaces", settings.disasmMemorySpaces);
	BridgeSettingSetUint("Disassembler", "Uppercase", settings.disasmUppercase);
	BridgeSettingSetUint("Disassembler", "OnlyCipAutoComments", settings.disasmOnlyCipAutoComments);
	BridgeSettingSetUint("Disassembler", "TabbedMnemonic", settings.disasmTabBetweenMnemonicAndArguments);
	BridgeSettingSetUint("Disassembler", "NoHighlightOperands", settings.disasmNoHighlightOperands);
	BridgeSettingSetUint("Disassembler", "PermanentHighlightingMode", settings.disasmPermanentHighlightingMode);
	BridgeSettingSetUint("Disassembler", "NoCurrentModuleText", settings.disasmNoCurrentModuleText);
	BridgeSettingSetUint("Disassembler", "0xPrefixValues", settings.disasm0xPrefixValues);  //0xPrefixValues
	BridgeSettingSetUint("Disassembler", "NoSourceLineAutoComments", settings.disasmNoSourceLineAutoComments);
	BridgeSettingSetUint("Disassembler", "MaxModuleSize", settings.disasmMaxModuleSize);

	//Gui tab
	BridgeSettingSetUint("Gui", "FpuRegistersLittleEndian", settings.guiFpuRegistersLittleEndian);
	BridgeSettingSetUint("Gui", "SaveColumnOrder", settings.guiSaveColumnOrder);
	BridgeSettingSetUint("Gui", "NoCloseDialog", settings.guiNoCloseDialog);
	BridgeSettingSetUint("Gui", "PidInHex", settings.guiPidInHex);
	BridgeSettingSetUint("Gui", "SidebarWatchLabels", settings.guiSidebarWatchLabels);
	BridgeSettingSetUint("Gui", "NoForegroundWindow", settings.guiNoForegroundWindow);
	BridgeSettingSetUint("Gui", "LoadSaveTabOrder", settings.guiLoadSaveTabOrder);
	BridgeSettingSetUint("Gui", "ShowGraphRva", settings.guiShowGraphRva);
	BridgeSettingSetUint("Gui", "ShowExitConfirmation", settings.guiShowExitConfirmation);
	BridgeSettingSetUint("Gui", "DisableAutoComplete", settings.guiDisableAutoComplete);
	BridgeSettingSetUint("Gui", "AsciiAddressDumpMode", settings.guiAsciiAddressDumpMode);

	//Misc tab
	if (DbgFunctions()->GetJit)
	{
		if (bJitOld != settings.miscSetJIT)
		{
			if (settings.miscSetJIT)
				DbgCmdExec("setjit oldsave");
			else
				DbgCmdExec("setjit restore");
		}

		if (bJitAutoOld != settings.miscSetJITAuto)
		{
			if (!settings.miscSetJITAuto)
				DbgCmdExec("setjitauto on");
			else
				DbgCmdExec("setjitauto off");
		}
	}
	/*
	if (settings.miscSymbolStore)
		BridgeSettingSet("Symbols", "DefaultStore", ui->editSymbolStore->text().toUtf8().constData());
	if (settings.miscSymbolCache)
		BridgeSettingSet("Symbols", "CachePath", ui->editSymbolCache->text().toUtf8().constData());
	BridgeSettingSet("Misc", "HelpOnSymbolicNameUrl", ui->editHelpOnSymbolicNameUrl->text().toUtf8().constData());
	*/
	BridgeSettingSetUint("Misc", "Utf16LogRedirect", settings.miscUtf16LogRedirect);
	BridgeSettingSetUint("Misc", "UseLocalHelpFile", settings.miscUseLocalHelpFile);
	BridgeSettingSetUint("Misc", "QueryProcessCookie", settings.miscQueryProcessCookie);
	BridgeSettingSetUint("Misc", "QueryWorkingSet", settings.miscQueryWorkingSet);

	BridgeSettingFlush();
	
	Config()->load();
	/*
	if (bTokenizerConfigUpdated)
	{
		emit Config()->tokenizerConfigUpdated();
		bTokenizerConfigUpdated = false;
	}
	if (bDisableAutoCompleteUpdated)
	{
		emit Config()->disableAutoCompleteUpdated();
		bDisableAutoCompleteUpdated = false;
	}
	if (bAsciiAddressDumpModeUpdated)
	{
		emit Config()->asciiAddressDumpModeUpdated();
		bAsciiAddressDumpModeUpdated = false;
	}
	*/
	DbgSettingsUpdated();
	GuiUpdateAllViews();
}