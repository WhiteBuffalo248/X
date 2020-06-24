

#include "stdafx.h"

#include "Configuration.h"
#include "AbstractTableView.h"
using namespace Gdiplus;

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

Configuration* Configuration::mPtr = nullptr;

inline void insertMenuBuilderBools(map<string, bool>* config, const char* id, size_t count)
{
	char buffer[50];
	string tmpstr;
	for (size_t i = 0; i < count; i++)
	{		
		sprintf_s(buffer, 50, "Menu%sHidden%d",id, i);
		tmpstr = buffer;
		config->insert(pair<string, bool>(tmpstr, false));
	}		
}

inline static void addWindowPosConfig(map<string, duint> & guiUint, const char* windowName)
{
	string n(windowName);
	guiUint.insert(pair<string, duint>(n + "X", 0));
	guiUint.insert(pair<string, duint>(n + "Y", 0));
}

Configuration::Configuration()
{
	mPtr = this;
	//setup default color map
	defaultColors.clear();
	//defaultColors.insert(map<CString, Color>::value_type(_T("AbstractTableViewSeparatorColor"), Color(0xFF808080)));
	
	defaultColors.insert(pair<string, Color>("AbstractTableViewSeparatorColor", Color(0xFF808080)));
	defaultColors.insert(pair<string, Color>("AbstractTableViewBackgroundColor", Color(0xFFFFF8F0)));
	defaultColors.insert(pair<string, Color>("AbstractTableViewTextColor", Color(0xFF000000)));
	defaultColors.insert(pair<string, Color>("AbstractTableViewHeaderTextColor", 0XFF000000));
	defaultColors.insert(pair<string, Color>("AbstractTableViewSelectionColor", 0XFFC0C0C0));
							  
	defaultColors.insert(pair<string, Color>("DisassemblyCipColor", Color(0xFFFFFFFF)));
	//CipBackground
	defaultColors.insert(pair<string, Color>("DisassemblyCipBackgroundColor", Color(0xFF00B4FF)));
	defaultColors.insert(pair<string, Color>("DisassemblyBreakpointColor", Color(0xFF000000)));
	defaultColors.insert(pair<string, Color>("DisassemblyBreakpointBackgroundColor", Color(0xFFFF0000)));
	defaultColors.insert(pair<string, Color>("DisassemblyHardwareBreakpointColor", Color(0xFF000000)));
	defaultColors.insert(pair<string, Color>("DisassemblyHardwareBreakpointBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("DisassemblyBookmarkColor", Color(0xFF000000)));
	defaultColors.insert(pair<string, Color>("DisassemblyBookmarkBackgroundColor", Color(0xFFFEE970)));
	defaultColors.insert(pair<string, Color>("DisassemblyLabelColor", Color(0xFFFF0000)));
							 
	defaultColors.insert(pair<string, Color>("DisassemblyLabelBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("DisassemblyBackgroundColor", Color(0xFFFFF8F0)));
	defaultColors.insert(pair<string, Color>("DisassemblySelectionColor", Color(0xFFC0C0C0)));   // Color(0xFFC0C0C0))
	defaultColors.insert(pair<string, Color>("DisassemblyTracedBackgroundColor", Color(0xFFC0FFC0)));
	defaultColors.insert(pair<string, Color>("DisassemblyAddressColor", Color(0xFF808080)));
							  
	defaultColors.insert(pair<string, Color>("DisassemblyAddressBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("DisassemblySelectedAddressColor", Color(0xFF000000)));
	defaultColors.insert(pair<string, Color>("DisassemblySelectedAddressBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("DisassemblyConditionalJumpLineTrueColor", Color(0xFFFF0000)));
	defaultColors.insert(pair<string, Color>("DisassemblyHardwareBreakpointColor", Color(0xFFFF0000)));
	defaultColors.insert(pair<string, Color>("DisassemblyConditionalJumpLineFalseColor", Color(0xFF808080)));
	defaultColors.insert(pair<string, Color>("DisassemblyUnconditionalJumpLineColor", Color(0xFFFF0000)));
	defaultColors.insert(pair<string, Color>("DisassemblyBytesColor", Color(0xFF000000)));
	defaultColors.insert(pair<string, Color>("DisassemblyBytesBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("DisassemblyModifiedBytesColor", Color(0xFFFF0000)));
	defaultColors.insert(pair<string, Color>("DisassemblyModifiedBytesBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("DisassemblyRestoredBytesColor", Color(0xFF808080)));
	defaultColors.insert(pair<string, Color>("DisassemblyRestoredBytesBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("DisassemblyByte00Color", Color(0xFF008000)));
	defaultColors.insert(pair<string, Color>("DisassemblyModifiedBytesBackgroundColor", Color(0xFF008000)));
	defaultColors.insert(pair<string, Color>("DisassemblyByte00BackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("DisassemblyByte7FColor", Color(0xFF808000)));
	defaultColors.insert(pair<string, Color>("DisassemblyByte7FBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("DisassemblyByteFFColor", Color(0xFF800000)));
	defaultColors.insert(pair<string, Color>("DisassemblyByteFFBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("DisassemblyByteIsPrintColor", Color(0xFF800080)));
	defaultColors.insert(pair<string, Color>("DisassemblyByteIsPrintBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("DisassemblyRelocationUnderlineColor", Color(0xFF000000)));
	defaultColors.insert(pair<string, Color>("DisassemblyCommentColor", Color(0xFF000000)));
	defaultColors.insert(pair<string, Color>("DisassemblyCommentBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("DisassemblyAutoCommentColor", Color(0xFFAA5500)));
	defaultColors.insert(pair<string, Color>("DisassemblyAutoCommentBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("DisassemblyCommentBackgroundColor", Color(0xFF717171)));
	defaultColors.insert(pair<string, Color>("DisassemblyMnemonicBriefBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("DisassemblyFunctionColor", Color(0xFF000000)));
	defaultColors.insert(pair<string, Color>("DisassemblyLoopColor", Color(0xFF000000)));	
	
	defaultColors.insert(pair<string, Color>("SideBarCipLabelColor", Color(0xFFFFFFFF)));
	defaultColors.insert(pair<string, Color>("SideBarCipLabelBackgroundColor", Color(0xFF4040FF)));
	defaultColors.insert(pair<string, Color>("SideBarBackgroundColor", Color(0xFFFFF8F0)));
	defaultColors.insert(pair<string, Color>("SideBarConditionalJumpLineTrueColor", Color(0xFFFF0000)));
	defaultColors.insert(pair<string, Color>("SideBarConditionalJumpLineFalseColor", Color(0xFF808080)));
	defaultColors.insert(pair<string, Color>("SideBarUnconditionalJumpLineTrueColor", Color(0xFFFF0000)));
	defaultColors.insert(pair<string, Color>("SideBarUnconditionalJumpLineFalseColor", Color(0xFF808080)));
	defaultColors.insert(pair<string, Color>("SideBarBulletColor", Color(0xFF808080)));
	defaultColors.insert(pair<string, Color>("SideBarBulletBreakpointColor", Color(0xFFFF0000)));
	defaultColors.insert(pair<string, Color>("SideBarBulletDisabledBreakpointColor", Color(0xFF00AA00)));
	defaultColors.insert(pair<string, Color>("SideBarBulletBookmarkColor", Color(0xFFFEE970)));
	defaultColors.insert(pair<string, Color>("SideBarCheckBoxForeColor", Color(0xFF000000)));
	defaultColors.insert(pair<string, Color>("SideBarCheckBoxBackColor", Color(0xFFFFFFFF)));
	
	defaultColors.insert(pair<string, Color>("RegistersBackgroundColor", Color(0xFFFFF8F0)));
	defaultColors.insert(pair<string, Color>("RegistersColor", Color(0xFF000000)));
	defaultColors.insert(pair<string, Color>("RegistersModifiedColor", Color(0xFFFF0000)));
	defaultColors.insert(pair<string, Color>("RegistersSelectionColor", Color(0xFFC0C0C0))); //0xFFEEEEEE
	defaultColors.insert(pair<string, Color>("RegistersLabelColor", Color(0xFF000000)));
	defaultColors.insert(pair<string, Color>("RegistersArgumentLabelColor", Color(0xFF00CC00)));
	defaultColors.insert(pair<string, Color>("RegistersExtraInfoColor", Color(0xFF000000)));
	defaultColors.insert(pair<string, Color>("RegistersHighlightReadColor", Color(0xFF00A000)));
	defaultColors.insert(pair<string, Color>("RegistersHighlightWriteColor", Color(0xFFB00000)));
	defaultColors.insert(pair<string, Color>("RegistersHighlightReadWriteColor", Color(0xFF808000)));
	
	defaultColors.insert(pair<string, Color>("InstructionHighlightColor", Color(0xFFFF0000)));
	defaultColors.insert(pair<string, Color>("InstructionCommaColor", Color(0xFF000000)));
	defaultColors.insert(pair<string, Color>("InstructionCommaBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("InstructionPrefixColor", Color(0xFF000000)));
	defaultColors.insert(pair<string, Color>("InstructionPrefixBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("InstructionUncategorizedColor", Color(0xFF000000)));
	defaultColors.insert(pair<string, Color>("InstructionUncategorizedBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("InstructionAddressColor", Color(0xFF000000)));
	defaultColors.insert(pair<string, Color>("InstructionAddressBackgroundColor", Color(0xFFFFFF00)));
	defaultColors.insert(pair<string, Color>("InstructionValueColor", Color(0xFF828200)));
	defaultColors.insert(pair<string, Color>("InstructionValueBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("InstructionMnemonicColor", Color(0xFF000000)));
	defaultColors.insert(pair<string, Color>("InstructionMnemonicBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("InstructionPushPopColor", Color(0xFF0000FF)));
	defaultColors.insert(pair<string, Color>("InstructionPushPopBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("InstructionCallColor", Color(0xFF000000)));
	defaultColors.insert(pair<string, Color>("InstructionCallBackgroundColor", Color(0xFF00FFFF)));
	defaultColors.insert(pair<string, Color>("InstructionRetColor", Color(0xFF000000)));
	defaultColors.insert(pair<string, Color>("InstructionRetBackgroundColor", Color(0xFF00FFFF)));
	defaultColors.insert(pair<string, Color>("InstructionConditionalJumpColor", Color(0xFFFF0000)));
	defaultColors.insert(pair<string, Color>("InstructionConditionalJumpBackgroundColor", Color(0xFFFFFF00)));
	defaultColors.insert(pair<string, Color>("InstructionUnconditionalJumpColor", Color(0xFF000000)));
	defaultColors.insert(pair<string, Color>("InstructionUnconditionalJumpBackgroundColor", Color(0xFFFFFF00)));
	defaultColors.insert(pair<string, Color>("InstructionUnusualColor", Color(0xFF000000)));
	defaultColors.insert(pair<string, Color>("InstructionUnusualBackgroundColor", Color(0xFFC00000)));
	defaultColors.insert(pair<string, Color>("InstructionNopColor", Color(0xFF808080)));
	defaultColors.insert(pair<string, Color>("InstructionNopBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("InstructionFarColor", Color(0xFF000000)));
	defaultColors.insert(pair<string, Color>("InstructionFarBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("InstructionInt3Color", Color(0xFF000000)));
	defaultColors.insert(pair<string, Color>("InstructionInt3BackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("InstructionMemorySizeColor", Color(0xFF000080)));
	defaultColors.insert(pair<string, Color>("InstructionMemorySizeBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("InstructionMemorySegmentColor", Color(0xFFFF00FF)));
	defaultColors.insert(pair<string, Color>("InstructionMemorySegmentBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("InstructionMemoryBracketsColor", Color(0xFF000000)));
	defaultColors.insert(pair<string, Color>("InstructionMemoryBracketsBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("InstructionMemoryStackBracketsColor", Color(0xFF000000)));
	defaultColors.insert(pair<string, Color>("InstructionMemoryStackBracketsBackgroundColor", Color(0xFF00FFFF)));
	defaultColors.insert(pair<string, Color>("InstructionMemoryBaseRegisterColor", Color(0xFFB03434)));
	defaultColors.insert(pair<string, Color>("InstructionMemoryBaseRegisterBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("InstructionMemoryIndexRegisterColor", Color(0xFF3838BC)));
	defaultColors.insert(pair<string, Color>("InstructionMemoryIndexRegisterBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("InstructionMemoryScaleColor", Color(0xFFB30059)));
	defaultColors.insert(pair<string, Color>("InstructionMemoryScaleBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("InstructionMemoryOperatorColor", Color(0xFFF27711)));
	defaultColors.insert(pair<string, Color>("InstructionMemoryOperatorBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("InstructionGeneralRegisterColor", Color(0xFF008300)));
	defaultColors.insert(pair<string, Color>("InstructionGeneralRegisterBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("InstructionFpuRegisterColor", Color(0xFF000080)));
	defaultColors.insert(pair<string, Color>("InstructionFpuRegisterBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("InstructionMmxRegisterColor", Color(0xFF000080)));
	defaultColors.insert(pair<string, Color>("InstructionMmxRegisterBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("InstructionXmmRegisterColor", Color(0xFF000080)));
	defaultColors.insert(pair<string, Color>("InstructionXmmRegisterBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("InstructionYmmRegisterColor", Color(0xFF000080)));
	defaultColors.insert(pair<string, Color>("InstructionYmmRegisterBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("InstructionZmmRegisterColor", Color(0xFF000080)));
	defaultColors.insert(pair<string, Color>("InstructionZmmRegisterBackgroundColor", Color(Color::Transparent)));
	

	defaultColors.insert(pair<string, Color>("HexDumpTextColor", Color(0xFF000000)));
	defaultColors.insert(pair<string, Color>("HexDumpModifiedBytesColor", Color(0xFFFF0000)));
	defaultColors.insert(pair<string, Color>("HexDumpModifiedBytesBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("HexDumpRestoredBytesColor", Color(0xFF808080)));
	defaultColors.insert(pair<string, Color>("HexDumpRestoredBytesBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("HexDumpByte00Color", Color(0xFF008000)));
	defaultColors.insert(pair<string, Color>("HexDumpByte00BackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("HexDumpByte7FColor", Color(0xFF808000)));
	defaultColors.insert(pair<string, Color>("HexDumpByte7FBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("HexDumpByteFFColor", Color(0xFF800000)));
	defaultColors.insert(pair<string, Color>("HexDumpByteFFBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("HexDumpByteIsPrintColor", Color(0xFF800080)));
	defaultColors.insert(pair<string, Color>("HexDumpByteIsPrintBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("HexDumpBackgroundColor", Color(0xFFFFF8F0)));
	defaultColors.insert(pair<string, Color>("HexDumpSelectionColor", Color(0xFFC0C0C0)));
	defaultColors.insert(pair<string, Color>("HexDumpAddressColor", Color(0xFF000000)));
	defaultColors.insert(pair<string, Color>("HexDumpAddressBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("HexDumpLabelColor", Color(0xFFFF0000)));
	defaultColors.insert(pair<string, Color>("HexDumpLabelBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("HexDumpUserModuleCodePointerHighlightColor", Color(0xFF00FF00)));
	defaultColors.insert(pair<string, Color>("HexDumpUserModuleDataPointerHighlightColor", Color(0xFF008000)));
	defaultColors.insert(pair<string, Color>("HexDumpSystemModuleCodePointerHighlightColor", Color(0xFFFF0000)));
	defaultColors.insert(pair<string, Color>("HexDumpSystemModuleDataPointerHighlightColor", Color(0xFF800000)));
	defaultColors.insert(pair<string, Color>("HexDumpUnknownCodePointerHighlightColor", Color(0xFF0000FF)));
	defaultColors.insert(pair<string, Color>("HexDumpUnknownDataPointerHighlightColor", Color(0xFF000080)));


	defaultColors.insert(pair<string, Color>("StackTextColor", Color(0xFF000000)));
	defaultColors.insert(pair<string, Color>("StackInactiveTextColor", Color(0xFF808080)));
	defaultColors.insert(pair<string, Color>("StackBackgroundColor", Color(0xFFFFF8F0)));
	defaultColors.insert(pair<string, Color>("StackSelectionColor", Color(0xFFC0C0C0)));
	defaultColors.insert(pair<string, Color>("StackCspColor", Color(0xFFFFFFFF)));
	//CspBackground
	defaultColors.insert(pair<string, Color>("StackCspBackgroundColor", Color(0xFF00B4FF)));
	defaultColors.insert(pair<string, Color>("StackAddressColor", Color(0xFF808080)));
	defaultColors.insert(pair<string, Color>("StackAddressBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("StackSelectedAddressColor", Color(0xFF000000)));
	defaultColors.insert(pair<string, Color>("StackSelectedAddressBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("StackLabelColor", Color(0xFFFF0000)));
	defaultColors.insert(pair<string, Color>("StackLabelBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("StackReturnToColor", Color(0xFFFF0000)));
	defaultColors.insert(pair<string, Color>("StackSEHChainColor", Color(0xFFAE81FF)));
	defaultColors.insert(pair<string, Color>("StackFrameColor", Color(0xFF000000)));
	defaultColors.insert(pair<string, Color>("StackFrameSystemColor", Color(0xFF0000FF)));

	defaultColors.insert(pair<string, Color>("HexEditTextColor", Color(0xFF000000)));
	defaultColors.insert(pair<string, Color>("HexEditWildcardColor", Color(0xFFFF0000)));
	defaultColors.insert(pair<string, Color>("HexEditBackgroundColor", Color(0xFFFFF8F0)));
	defaultColors.insert(pair<string, Color>("HexEditSelectionColor", Color(0xFFC0C0C0)));

	defaultColors.insert(pair<string, Color>("GraphJmpColor", Color(0xFF0148FB)));
	defaultColors.insert(pair<string, Color>("GraphBrtrueColor", Color(0xFF387804)));
	defaultColors.insert(pair<string, Color>("GraphBrfalseColor", Color(0xFFED4630)));
	defaultColors.insert(pair<string, Color>("GraphRetShadowColor", Color(0xFF900000)));
	defaultColors.insert(pair<string, Color>("GraphIndirectcallShadowColor", Color(0xFF008080)));
	defaultColors.insert(pair<string, Color>("GraphBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("GraphNodeColor", Color(0xFF000000)));
	defaultColors.insert(pair<string, Color>("GraphNodeBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("GraphCipColor", Color(0xFF00FF00)));
	defaultColors.insert(pair<string, Color>("GraphBreakpointColor", Color(0xFFFF0000)));
	defaultColors.insert(pair<string, Color>("GraphDisabledBreakpointColor", Color(0xFF00AA00)));


	defaultColors.insert(pair<string, Color>("ThreadCurrentColor", Color(0xFFFFFFFF)));
	defaultColors.insert(pair<string, Color>("ThreadCurrentBackgroundColor", Color(0xFF000000)));
	defaultColors.insert(pair<string, Color>("WatchTriggeredColor", Color(0xFFFF0000)));
	defaultColors.insert(pair<string, Color>("WatchTriggeredBackgroundColor", Color(0xFFFFF8F0)));
	defaultColors.insert(pair<string, Color>("MemoryMapBreakpointColor", Color(0xFF000000)));

	defaultColors.insert(pair<string, Color>("MemoryMapBreakpointBackgroundColor", Color(0xFFFF0000)));
	defaultColors.insert(pair<string, Color>("MemoryMapCipColor", Color(0xFF000000)));
	defaultColors.insert(pair<string, Color>("MemoryMapCipBackgroundColor", Color(0xFF00FF00)));
	defaultColors.insert(pair<string, Color>("MemoryMapSectionTextColor", Color(0xFF8B671F)));
	defaultColors.insert(pair<string, Color>("SearchListViewHighlightColor", Color(0xFFFF0000)));
	defaultColors.insert(pair<string, Color>("StructBackgroundColor", Color(0xFFFFF8F0)));
	defaultColors.insert(pair<string, Color>("StructAlternateBackgroundColor", Color(0xFFDCD9CF)));
	defaultColors.insert(pair<string, Color>("LogLinkColor", Color(0xFF00CC00)));
	defaultColors.insert(pair<string, Color>("LogLinkBackgroundColor", Color(Color::Transparent)));
	defaultColors.insert(pair<string, Color>("BreakpointSummaryParenColor", Color(0xFFFF0000)));
	defaultColors.insert(pair<string, Color>("BreakpointSummaryKeywordColor", Color(0xFF8B671F)));
	defaultColors.insert(pair<string, Color>("BreakpointSummaryStringColor", Color(0xFF008000)));

	defaultColors.insert(make_pair("PatchRelocatedByteHighlightColor", Color(0xFF0000DD)));

	//bool settings

	map<string, bool> disassemblyBool;
	disassemblyBool.insert(pair<string, bool>("ArgumentSpaces", false));
	disassemblyBool.insert(pair<string, bool>("HidePointerSizes", false));
	disassemblyBool.insert(pair<string, bool>("HideNormalSegments", false));
	disassemblyBool.insert(pair<string, bool>("MemorySpaces", false));
	disassemblyBool.insert(pair<string, bool>("KeepSize", false));
	disassemblyBool.insert(pair<string, bool>("FillNOPs", false));
	disassemblyBool.insert(pair<string, bool>("Uppercase", false));
	disassemblyBool.insert(pair<string, bool>("FindCommandEntireBlock", false));
	disassemblyBool.insert(pair<string, bool>("OnlyCipAutoComments", false));
	disassemblyBool.insert(pair<string, bool>("TabbedMnemonic", false));
	disassemblyBool.insert(pair<string, bool>("LongDataInstruction", false));
	disassemblyBool.insert(pair<string, bool>("NoHighlightOperands", false));
	disassemblyBool.insert(pair<string, bool>("PermanentHighlightingMode", false));
	disassemblyBool.insert(pair<string, bool>("0xPrefixValues", false));
	disassemblyBool.insert(pair<string, bool>("NoCurrentModuleText", false));

	defaultBools.insert(pair<string, map<string, bool>>("Disassembler", disassemblyBool));

	map<string, bool> engineBool;
	engineBool.insert(pair<string, bool>("ListAllPages", false));
	engineBool.insert(pair<string, bool>("ShowSuspectedCallStack", false));

	defaultBools.insert(pair<string, map<string, bool>>("Engine", engineBool));
	

	map<string, bool> guiBool;
	guiBool.insert(pair<string, bool>("FpuRegistersLittleEndian", false));
	guiBool.insert(pair<string, bool>("SaveColumnOrder", false));
	guiBool.insert(pair<string, bool>("NoCloseDialog", false));
	guiBool.insert(pair<string, bool>("PidInHex", false));
	guiBool.insert(pair<string, bool>("SidebarWatchLabels", false));
	guiBool.insert(pair<string, bool>("LoadSaveTabOrder", false));
	guiBool.insert(pair<string, bool>("ShowGraphRva", false));
	guiBool.insert(pair<string, bool>("ShowExitConfirmation", false));
	guiBool.insert(pair<string, bool>("DisableAutoComplete", false));

	//Named menu settings
	insertMenuBuilderBools(&guiBool, "CPUDisassembly", 50); //CPUDisassembly
	insertMenuBuilderBools(&guiBool, "CPUDump", 50); //CPUDump
	insertMenuBuilderBools(&guiBool, "WatchView", 50); //Watch
	insertMenuBuilderBools(&guiBool, "CallStackView", 50); //CallStackView
	insertMenuBuilderBools(&guiBool, "ThreadView", 50); //Thread
	insertMenuBuilderBools(&guiBool, "CPUStack", 50); //Stack
	insertMenuBuilderBools(&guiBool, "SourceView", 10); //Source
	insertMenuBuilderBools(&guiBool, "DisassemblerGraphView", 50); //Graph
	insertMenuBuilderBools(&guiBool, "XrefBrowseDialog", 10); //XrefBrowseDialog
	insertMenuBuilderBools(&guiBool, "StructWidget", 8); //StructWidget
	insertMenuBuilderBools(&guiBool, "File", 50); //Main Menu : File
	insertMenuBuilderBools(&guiBool, "Debug", 50); //Main Menu : Debug
	insertMenuBuilderBools(&guiBool, "Option", 50); //Main Menu : Option
	//"Favourites" menu cannot be customized for item hiding.
	insertMenuBuilderBools(&guiBool, "Help", 50); //Main Menu : Help
	insertMenuBuilderBools(&guiBool, "View", 50); //Main Menu : View
	defaultBools.insert(pair<string, map<string, bool>>("Gui", guiBool));

	map<string, duint> guiUint;
	
	CAbstractTableView::setupColumnConfigDefaultValue(guiUint, "CPUDisassembly", 4);
	CAbstractTableView::setupColumnConfigDefaultValue(guiUint, "CPUStack", 3);
	CAbstractTableView::setupColumnConfigDefaultValue(guiUint, "CPUDump", 4);
	/*
	char buffer[50];
	string tmpstr;
	for (int i = 1; i <= 5; i++)
	{
	sprintf_s(buffer, 50, "CPUDump%d", i);
	tmpstr = buffer;
	CAbstractTableView::setupColumnConfigDefaultValue(guiUint, tmpstr, 4);
	}
	*/
	
		
	CAbstractTableView::setupColumnConfigDefaultValue(guiUint, "Watch1", 6);
	CAbstractTableView::setupColumnConfigDefaultValue(guiUint, "BreakpointsView", 7);
	CAbstractTableView::setupColumnConfigDefaultValue(guiUint, "MemoryMap", 8);
	CAbstractTableView::setupColumnConfigDefaultValue(guiUint, "CallStack", 6);
	CAbstractTableView::setupColumnConfigDefaultValue(guiUint, "SEH", 4);
	CAbstractTableView::setupColumnConfigDefaultValue(guiUint, "Script", 3);
	CAbstractTableView::setupColumnConfigDefaultValue(guiUint, "Thread", 14);
	CAbstractTableView::setupColumnConfigDefaultValue(guiUint, "Handle", 5);
	CAbstractTableView::setupColumnConfigDefaultValue(guiUint, "Window", 10);
	CAbstractTableView::setupColumnConfigDefaultValue(guiUint, "TcpConnection", 3);
	CAbstractTableView::setupColumnConfigDefaultValue(guiUint, "Privilege", 2);
	CAbstractTableView::setupColumnConfigDefaultValue(guiUint, "LocalVarsView", 3);
	CAbstractTableView::setupColumnConfigDefaultValue(guiUint, "Module", 4);
	
	CAbstractTableView::setupColumnConfigDefaultValue(guiUint, "Symbol", 4);
	CAbstractTableView::setupColumnConfigDefaultValue(guiUint, "InstructTrace", 5);
	guiUint.insert(pair<string, duint>("SIMDRegistersDisplayMode", 0));
	addWindowPosConfig(guiUint, "AssembleDialog");
	addWindowPosConfig(guiUint, "AttachDialog");
	addWindowPosConfig(guiUint, "GotoDialog");
	addWindowPosConfig(guiUint, "EditBreakpointDialog");
	addWindowPosConfig(guiUint, "BrowseDialog");
	addWindowPosConfig(guiUint, "FavouriteTools");
	addWindowPosConfig(guiUint, "EntropyDialog");
	addWindowPosConfig(guiUint, "HexEditDialog");
	addWindowPosConfig(guiUint, "WordEditDialog");
	defaultUints.insert(pair<string, map<string, duint>>("Gui", guiUint));
	guiUint.clear();

	//frameUint
	map<string, duint> frameUint;
	frameUint.insert(pair<string, duint>("SideBarWidth", 20));
	frameUint.insert(pair<string, duint>("OutputWndHight", 20));
	frameUint.insert(pair<string, duint>("HandleHight1", 40));
	frameUint.insert(pair<string, duint>("HandleHight2", 40));
	defaultUints.insert(pair<string, map<string, duint>>("DefaultFrameData", frameUint));

	//uint settings
	map<string, duint> hexdumpUint;
	hexdumpUint.insert(pair<string, duint>("DefaultView", 0));
	defaultUints.insert(pair<string, map<string, duint>>("HexDump", hexdumpUint));

	map<string, duint> disasmUint;
	disasmUint.insert(pair<string, duint>("MaxModuleSize", -1));
	defaultUints.insert(pair<string, map<string, duint>>("Disassembler", disasmUint));

	map<string, duint> tabOrderUint;
	int curTab = 0;
	tabOrderUint.insert(pair<string, duint>("CPUTab", curTab++));
	tabOrderUint.insert(pair<string, duint>("GraphTab", curTab++));
	tabOrderUint.insert(pair<string, duint>("LogTab", curTab++));
	tabOrderUint.insert(pair<string, duint>("NotesTab", curTab++));
	tabOrderUint.insert(pair<string, duint>("BreakpointsTab", curTab++));
	tabOrderUint.insert(pair<string, duint>("MemoryMapTab", curTab++));
	tabOrderUint.insert(pair<string, duint>("CallStackTab", curTab++));
	tabOrderUint.insert(pair<string, duint>("SEHTab", curTab++));
	tabOrderUint.insert(pair<string, duint>("ScriptTab", curTab++));
	tabOrderUint.insert(pair<string, duint>("SymbolsTab", curTab++));
	tabOrderUint.insert(pair<string, duint>("SourceTab", curTab++));
	tabOrderUint.insert(pair<string, duint>("ReferencesTab", curTab++));
	tabOrderUint.insert(pair<string, duint>("ThreadsTab", curTab++));
	tabOrderUint.insert(pair<string, duint>("SnowmanTab", curTab++));
	tabOrderUint.insert(pair<string, duint>("HandlesTab", curTab++));
	tabOrderUint.insert(pair<string, duint>("TraceTab", curTab++));

	defaultUints.insert(pair<string, map<string, duint>>("TabOrder", tabOrderUint));	
	//font settings	
	LOGFONT defaultfont;
	memset(&defaultfont, 0, sizeof(defaultfont));
	defaultfont.lfWeight = FW_NORMAL;
	_tcscpy_s(defaultfont.lfFaceName, LF_FACESIZE, _T("Rockwell"));
	defaultfont.lfHeight = MulDiv(12, 96, 72);;

	LOGFONT LucidaFont = defaultfont;
	_tcscpy_s(LucidaFont.lfFaceName, LF_FACESIZE, _T("Lucida Console")); //Lucida Console
	LucidaFont.lfHeight = MulDiv(10, 96, 72);;

	LOGFONT YHFont = defaultfont;
	_tcscpy_s(YHFont.lfFaceName, LF_FACESIZE, _T("Î¢ÈíÑÅºÚ")); //Î¢ÈíÑÅºÚ
	YHFont.lfHeight = MulDiv(13, 96, 72);;

	LOGFONT STFont = defaultfont;
	_tcscpy_s(STFont.lfFaceName, LF_FACESIZE, _T("ËÎÌå")); //ËÎÌå
	STFont.lfWeight = FW_REGULAR;
	STFont.lfHeight = MulDiv(12, 96, 72);;

	defaultFonts.insert(pair<string, LOGFONT>("AbstractTableView", LucidaFont));
	defaultFonts.insert(pair<string, LOGFONT>("Disassembly", defaultfont));
	defaultFonts.insert(pair<string, LOGFONT>("HexDump", LucidaFont));
	defaultFonts.insert(pair<string, LOGFONT>("Stack", defaultfont));	
	defaultFonts.insert(pair<string, LOGFONT>("Registers", LucidaFont));
	defaultFonts.insert(pair<string, LOGFONT>("HexEdit", defaultfont));
	defaultFonts.insert(pair<string, LOGFONT>("Application", defaultfont));
	defaultFonts.insert(pair<string, LOGFONT>("Log", YHFont));
	
	// hotkeys settings
	/*
	defaultShortcuts.insert("FileOpen", Shortcut({_T("File"), _T("Open") }, _T("F3"), true)); 
	defaultShortcuts.insert("FileOpen", Shortcut({ tr("File"), tr("Open") }, "F3", true));
	defaultShortcuts.insert("FileAttach", Shortcut({ tr("File"), tr("Attach") }, "Alt+A", true));
	defaultShortcuts.insert("FileDetach", Shortcut({ tr("File"), tr("Detach") }, "Ctrl+Alt+F2", true));
	defaultShortcuts.insert("FileImportDatabase", Shortcut({ tr("File"), tr("Import database") }, "", true));
	defaultShortcuts.insert("FileExportDatabase", Shortcut({ tr("File"), tr("Export database") }, "", true));
	defaultShortcuts.insert("FileRestartAdmin", Shortcut({ tr("File"), tr("Restart as Admin") }, "", true));
	defaultShortcuts.insert("FileExit", Shortcut({ tr("File"), tr("Exit") }, "Alt+X", true));

	defaultShortcuts.insert("ViewCpu", Shortcut({ tr("View"), tr("CPU") }, "Alt+C", true));
	defaultShortcuts.insert("ViewLog", Shortcut({ tr("View"), tr("Log") }, "Alt+L", true));
	defaultShortcuts.insert("ViewBreakpoints", Shortcut({ tr("View"), tr("Breakpoints") }, "Alt+B", true));
	defaultShortcuts.insert("ViewMemoryMap", Shortcut({ tr("View"), tr("Memory Map") }, "Alt+M", true));
	defaultShortcuts.insert("ViewCallStack", Shortcut({ tr("View"), tr("Call Stack") }, "Alt+K", true));
	defaultShortcuts.insert("ViewNotes", Shortcut({ tr("View"), tr("Notes") }, "Alt+N", true));
	defaultShortcuts.insert("ViewSEHChain", Shortcut({ tr("View"), tr("SEH") }, "", true));
	defaultShortcuts.insert("ViewScript", Shortcut({ tr("View"), tr("Script") }, "Alt+S", true));
	defaultShortcuts.insert("ViewSymbolInfo", Shortcut({ tr("View"), tr("Symbol Info") }, "Ctrl+Alt+S", true));
	defaultShortcuts.insert("ViewModules", Shortcut({ tr("View"), tr("Modules") }, "Alt+E", true));
	defaultShortcuts.insert("ViewSource", Shortcut({ tr("View"), tr("Source") }, "Ctrl+Shift+S", true));
	defaultShortcuts.insert("ViewReferences", Shortcut({ tr("View"), tr("References") }, "Alt+R", true));
	defaultShortcuts.insert("ViewThreads", Shortcut({ tr("View"), tr("Threads") }, "Alt+T", true));
	defaultShortcuts.insert("ViewPatches", Shortcut({ tr("View"), tr("Patches") }, "Ctrl+P", true));
	defaultShortcuts.insert("ViewComments", Shortcut({ tr("View"), tr("Comments") }, "Ctrl+Alt+C", true));
	defaultShortcuts.insert("ViewLabels", Shortcut({ tr("View"), tr("Labels") }, "Ctrl+Alt+L", true));
	defaultShortcuts.insert("ViewBookmarks", Shortcut({ tr("View"), tr("Bookmarks") }, "Ctrl+Alt+B", true));
	defaultShortcuts.insert("ViewFunctions", Shortcut({ tr("View"), tr("Functions") }, "Ctrl+Alt+F", true));
	defaultShortcuts.insert("ViewVariables", Shortcut({ tr("View"), tr("Variables") }, "", true));
	defaultShortcuts.insert("ViewSnowman", Shortcut({ tr("View"), tr("Snowman") }, "", true));
	defaultShortcuts.insert("ViewHandles", Shortcut({ tr("View"), tr("Handles") }, "", true));
	defaultShortcuts.insert("ViewGraph", Shortcut({ tr("View"), tr("Graph") }, "Alt+G", true));
	defaultShortcuts.insert("ViewPreviousTab", Shortcut({ tr("View"), tr("Previous Tab") }, "Alt+Left"));
	defaultShortcuts.insert("ViewNextTab", Shortcut({ tr("View"), tr("Next Tab") }, "Alt+Right"));
	defaultShortcuts.insert("ViewPreviousHistory", Shortcut({ tr("View"), tr("Previous View") }, "Ctrl+Shift+Tab"));
	defaultShortcuts.insert("ViewNextHistory", Shortcut({ tr("View"), tr("Next View") }, "Ctrl+Tab"));
	defaultShortcuts.insert("ViewHideTab", Shortcut({ tr("View"), tr("Hide Tab") }, "Ctrl+W"));

	defaultShortcuts.insert("DebugRun", Shortcut({ tr("Debug"), tr("Run") }, "F9", true));
	defaultShortcuts.insert("DebugeRun", Shortcut({ tr("Debug"), tr("Run (pass exception)") }, "Shift+F9", true));
	defaultShortcuts.insert("DebugseRun", Shortcut({ tr("Debug"), tr("Run (swallow exception)") }, "Ctrl+Alt+Shift+F9", true));
	defaultShortcuts.insert("DebugRunSelection", Shortcut({ tr("Debug"), tr("Run until selection") }, "F4", true));
	defaultShortcuts.insert("DebugRunExpression", Shortcut({ tr("Debug"), tr("Run until expression") }, "Shift+F4", true));
	defaultShortcuts.insert("DebugPause", Shortcut({ tr("Debug"), tr("Pause") }, "F12", true));
	defaultShortcuts.insert("DebugRestart", Shortcut({ tr("Debug"), tr("Restart") }, "Ctrl+F2", true));
	defaultShortcuts.insert("DebugClose", Shortcut({ tr("Debug"), tr("Close") }, "Alt+F2", true));
	defaultShortcuts.insert("DebugStepInto", Shortcut({ tr("Debug"), tr("Step into") }, "F7", true));
	defaultShortcuts.insert("DebugeStepInto", Shortcut({ tr("Debug"), tr("Step into (pass exception)") }, "Shift+F7", true));
	defaultShortcuts.insert("DebugseStepInto", Shortcut({ tr("Debug"), tr("Step into (swallow exception)") }, "Ctrl+Alt+Shift+F7", true));
	defaultShortcuts.insert("DebugStepIntoSource", Shortcut({ tr("Debug"), tr("Step into (source)") }, "F11", true));
	defaultShortcuts.insert("DebugStepOver", Shortcut({ tr("Debug"), tr("Step over") }, "F8", true));
	defaultShortcuts.insert("DebugeStepOver", Shortcut({ tr("Debug"), tr("Step over (pass exception)") }, "Shift+F8", true));
	defaultShortcuts.insert("DebugseStepOver", Shortcut({ tr("Debug"), tr("Step over (swallow exception)") }, "Ctrl+Alt+Shift+F8", true));
	defaultShortcuts.insert("DebugStepOverSource", Shortcut({ tr("Debug"), tr("Step over (source)") }, "F10", true));
	defaultShortcuts.insert("DebugRtr", Shortcut({ tr("Debug"), tr("Execute till return") }, "Ctrl+F9", true));
	defaultShortcuts.insert("DebugeRtr", Shortcut({ tr("Debug"), tr("Execute till return (pass exception)") }, "Ctrl+Shift+F9", true));
	defaultShortcuts.insert("DebugRtu", Shortcut({ tr("Debug"), tr("Run to user code") }, "Alt+F9", true));
	defaultShortcuts.insert("DebugSkipNextInstruction", Shortcut({ tr("Debug"), tr("Skip next instruction") }, "", true));
	defaultShortcuts.insert("DebugCommand", Shortcut({ tr("Debug"), tr("Command") }, "Ctrl+Return", true));
	defaultShortcuts.insert("DebugTraceIntoConditional", Shortcut({ tr("Debug"), tr("Trace into...") }, "Ctrl+Alt+F7", true));
	defaultShortcuts.insert("DebugTraceOverConditional", Shortcut({ tr("Debug"), tr("Trace over...") }, "Ctrl+Alt+F8", true));
	defaultShortcuts.insert("DebugEnableTraceRecordBit", Shortcut({ tr("Debug"), tr("Trace Record"), tr("Bit") }, "", true));
	defaultShortcuts.insert("DebugTraceRecordNone", Shortcut({ tr("Debug"), tr("Trace Record"), tr("None") }, "", true));
	defaultShortcuts.insert("DebugInstrUndo", Shortcut({ tr("Debug"), tr("Undo instruction") }, "Alt+U", true));
	defaultShortcuts.insert("DebugAnimateInto", Shortcut({ tr("Debug"), tr("Animate into") }, "Ctrl+F7", true));
	defaultShortcuts.insert("DebugAnimateOver", Shortcut({ tr("Debug"), tr("Animate over") }, "Ctrl+F8", true));
	defaultShortcuts.insert("DebugAnimateCommand", Shortcut({ tr("Debug"), tr("Animate command") }, "", true));
	defaultShortcuts.insert("DebugTraceIntoIntoTracerecord", Shortcut({ tr("Debug"), tr("Trace into into trace record") }, "", true));
	defaultShortcuts.insert("DebugTraceOverIntoTracerecord", Shortcut({ tr("Debug"), tr("Trace over into trace record") }, "", true));
	defaultShortcuts.insert("DebugTraceIntoBeyondTracerecord", Shortcut({ tr("Debug"), tr("Trace into beyond trace record") }, "", true));
	defaultShortcuts.insert("DebugTraceOverBeyondTracerecord", Shortcut({ tr("Debug"), tr("Trace over beyond trace record") }, "", true));

	defaultShortcuts.insert("PluginsScylla", Shortcut({ tr("Plugins"), tr("Scylla") }, "Ctrl+I", true));

	defaultShortcuts.insert("FavouritesManage", Shortcut({ tr("Favourites"), tr("Manage Favourite Tools") }, "", true));

	defaultShortcuts.insert("OptionsPreferences", Shortcut({ tr("Options"), tr("Preferences") }, "", true));
	defaultShortcuts.insert("OptionsAppearance", Shortcut({ tr("Options"), tr("Appearance") }, "", true));
	defaultShortcuts.insert("OptionsShortcuts", Shortcut({ tr("Options"), tr("Shortcuts") }, "", true));
	defaultShortcuts.insert("OptionsTopmost", Shortcut({ tr("Options"), tr("Topmost") }, "Ctrl+F5", true));
	defaultShortcuts.insert("OptionsReloadStylesheet", Shortcut({ tr("Options"), tr("Reload style.css") }, "", true));

	defaultShortcuts.insert("HelpAbout", Shortcut({ tr("Help"), tr("About") }, "", true));
	defaultShortcuts.insert("HelpBlog", Shortcut({ tr("Help"), tr("Blog") }, "", true));
	defaultShortcuts.insert("HelpDonate", Shortcut({ tr("Help"), tr("Donate") }, "", true));
	defaultShortcuts.insert("HelpCalculator", Shortcut({ tr("Help"), tr("Calculator") }, "?"));
	defaultShortcuts.insert("HelpReportBug", Shortcut({ tr("Help"), tr("Report Bug") }, "", true));
	defaultShortcuts.insert("HelpManual", Shortcut({ tr("Help"), tr("Manual") }, "F1", true));
	defaultShortcuts.insert("HelpCrashDump", Shortcut({ tr("Help"), tr("Generate Crash Dump") }, "", true));

	defaultShortcuts.insert("ActionFindStrings", Shortcut({ tr("Actions"), tr("Find Strings") }, "", true));
	defaultShortcuts.insert("ActionFindIntermodularCalls", Shortcut({ tr("Actions"), tr("Find Intermodular Calls") }, "", true));
	defaultShortcuts.insert("ActionToggleBreakpoint", Shortcut({ tr("Actions"), tr("Toggle Breakpoint") }, "F2"));
	defaultShortcuts.insert("ActionToggleBookmark", Shortcut({ tr("Actions"), tr("Toggle Bookmark") }, "Ctrl+D"));
	defaultShortcuts.insert("ActionDeleteBreakpoint", Shortcut({ tr("Actions"), tr("Delete Breakpoint") }, "Delete"));
	defaultShortcuts.insert("ActionEnableDisableBreakpoint", Shortcut({ tr("Actions"), tr("Enable/Disable Breakpoint") }, "Space"));
	defaultShortcuts.insert("ActionResetHitCountBreakpoint", Shortcut({ tr("Actions"), tr("Reset breakpoint hit count") }));
	defaultShortcuts.insert("ActionEnableAllBreakpoints", Shortcut({ tr("Actions"), tr("Enable all breakpoints") }));
	defaultShortcuts.insert("ActionDisableAllBreakpoints", Shortcut({ tr("Actions"), tr("Disable all breakpoints") }));
	defaultShortcuts.insert("ActionRemoveAllBreakpoints", Shortcut({ tr("Actions"), tr("Remove all breakpoints") }));

	defaultShortcuts.insert("ActionBinaryEdit", Shortcut({ tr("Actions"), tr("Binary Edit") }, "Ctrl+E"));
	defaultShortcuts.insert("ActionBinaryFill", Shortcut({ tr("Actions"), tr("Binary Fill") }, "F"));
	defaultShortcuts.insert("ActionBinaryFillNops", Shortcut({ tr("Actions"), tr("Binary Fill NOPs") }, "Ctrl+9"));
	defaultShortcuts.insert("ActionBinaryCopy", Shortcut({ tr("Actions"), tr("Binary Copy") }, "Shift+C"));
	defaultShortcuts.insert("ActionBinaryPaste", Shortcut({ tr("Actions"), tr("Binary Paste") }, "Shift+V"));
	defaultShortcuts.insert("ActionBinaryPasteIgnoreSize", Shortcut({ tr("Actions"), tr("Binary Paste (Ignore Size)") }, "Ctrl+Shift+V"));
	defaultShortcuts.insert("ActionBinarySave", Shortcut({ tr("Actions"), tr("Binary Save") }));
	defaultShortcuts.insert("ActionUndoSelection", Shortcut({ tr("Actions"), tr("Undo Selection") }, "Ctrl+Backspace"));
	defaultShortcuts.insert("ActionSetLabel", Shortcut({ tr("Actions"), tr("Set Label") }, ":"));
	defaultShortcuts.insert("ActionSetLabelOperand", Shortcut({ tr("Actions"), tr("Set Label for the Operand") }, "Alt+;"));
	defaultShortcuts.insert("ActionSetComment", Shortcut({ tr("Actions"), tr("Set Comment") }, ";"));
	defaultShortcuts.insert("ActionToggleFunction", Shortcut({ tr("Actions"), tr("Toggle Function") }, "Shift+F"));
	defaultShortcuts.insert("ActionToggleArgument", Shortcut({ tr("Actions"), tr("Toggle Argument") }, "Shift+A"));
	defaultShortcuts.insert("ActionAssemble", Shortcut({ tr("Actions"), tr("Assemble") }, "Space"));
	defaultShortcuts.insert("ActionYara", Shortcut({ tr("Actions"), tr("Yara") }, "Ctrl+Y"));
	defaultShortcuts.insert("ActionSetNewOriginHere", Shortcut({ tr("Actions"), tr("Set New Origin Here") }, "Ctrl+*"));
	defaultShortcuts.insert("ActionGotoOrigin", Shortcut({ tr("Actions"), tr("Goto Origin") }, "*"));
	defaultShortcuts.insert("ActionGotoPrevious", Shortcut({ tr("Actions"), tr("Goto Previous") }, "-"));
	defaultShortcuts.insert("ActionGotoNext", Shortcut({ tr("Actions"), tr("Goto Next") }, "+"));
	defaultShortcuts.insert("ActionGotoExpression", Shortcut({ tr("Actions"), tr("Goto Expression") }, "Ctrl+G"));
	defaultShortcuts.insert("ActionGotoStart", Shortcut({ tr("Actions"), tr("Goto Start of Page") }, "Home"));
	defaultShortcuts.insert("ActionGotoEnd", Shortcut({ tr("Actions"), tr("Goto End of Page") }, "End"));
	defaultShortcuts.insert("ActionGotoFunctionStart", Shortcut({ tr("Actions"), tr("Goto Start of Function") }, "Ctrl+Home"));
	defaultShortcuts.insert("ActionGotoFunctionEnd", Shortcut({ tr("Actions"), tr("Goto End of Function") }, "Ctrl+End"));
	defaultShortcuts.insert("ActionGotoFileOffset", Shortcut({ tr("Actions"), tr("Goto File Offset") }, "Ctrl+Shift+G"));
	defaultShortcuts.insert("ActionFindReferencesToSelectedAddress", Shortcut({ tr("Actions"), tr("Find References to Selected Address") }, "Ctrl+R"));
	defaultShortcuts.insert("ActionFindPattern", Shortcut({ tr("Actions"), tr("Find Pattern") }, "Ctrl+B"));
	defaultShortcuts.insert("ActionFindPatternInModule", Shortcut({ tr("Actions"), tr("Find Pattern in Current Module") }, "Ctrl+Shift+B"));
	defaultShortcuts.insert("ActionFindNamesInModule", Shortcut({ tr("Actions"), tr("Find Names in Current Module") }, "Ctrl+N"));
	defaultShortcuts.insert("ActionFindReferences", Shortcut({ tr("Actions"), tr("Find References") }, "Ctrl+R"));
	defaultShortcuts.insert("ActionXrefs", Shortcut({ tr("Actions"), tr("xrefs...") }, "X"));
	defaultShortcuts.insert("ActionAnalyzeSingleFunction", Shortcut({ tr("Actions"), tr("Analyze Single Function") }, "A"));
	defaultShortcuts.insert("ActionAnalyzeModule", Shortcut({ tr("Actions"), tr("Analyze Module") }, "Ctrl+A"));
	defaultShortcuts.insert("ActionHelpOnMnemonic", Shortcut({ tr("Actions"), tr("Help on Mnemonic") }, "Ctrl+F1"));
	defaultShortcuts.insert("ActionToggleMnemonicBrief", Shortcut({ tr("Actions"), tr("Toggle Mnemonic Brief") }, "Ctrl+Shift+F1"));
	defaultShortcuts.insert("ActionHighlightingMode", Shortcut({ tr("Actions"), tr("Highlighting Mode") }, "H"));
	defaultShortcuts.insert("ActionToggleDestinationPreview", Shortcut({ tr("Actions"), tr("Enable/Disable Branch Destination Preview") }, "P"));
	defaultShortcuts.insert("ActionFind", Shortcut({ tr("Actions"), tr("Find") }, "Ctrl+F"));
	defaultShortcuts.insert("ActionFindInModule", Shortcut({ tr("Actions"), tr("Find in Current Module") }, "Ctrl+Shift+F"));
	defaultShortcuts.insert("ActionDecompileFunction", Shortcut({ tr("Actions"), tr("Decompile Function") }, "F5"));
	defaultShortcuts.insert("ActionDecompileSelection", Shortcut({ tr("Actions"), tr("Decompile Selection") }, "Shift+F5"));
	defaultShortcuts.insert("ActionEditBreakpoint", Shortcut({ tr("Actions"), tr("Edit breakpoint") }, ""));
	defaultShortcuts.insert("ActionToggleLogging", Shortcut({ tr("Actions"), tr("Enable/Disable Logging") }, ""));
	defaultShortcuts.insert("ActionAllocateMemory", Shortcut({ tr("Actions"), tr("Allocate Memory") }, ""));
	defaultShortcuts.insert("ActionFreeMemory", Shortcut({ tr("Actions"), tr("Free Memory") }, ""));
	defaultShortcuts.insert("ActionSyncWithExpression", Shortcut({ tr("Actions"), tr("Sync With Expression") }, ""));
	defaultShortcuts.insert("ActionEntropy", Shortcut({ tr("Actions"), tr("Entropy") }, ""));
	defaultShortcuts.insert("ActionCopyAllRegisters", Shortcut({ tr("Actions"), tr("Copy All Registers") }, ""));
	defaultShortcuts.insert("ActionMarkAsUser", Shortcut({ tr("Actions"), tr("Mark As User Module") }, ""));
	defaultShortcuts.insert("ActionMarkAsSystem", Shortcut({ tr("Actions"), tr("Mark As System Module") }, ""));
	defaultShortcuts.insert("ActionMarkAsParty", Shortcut({ tr("Actions"), tr("Mark As Party") }, ""));
	defaultShortcuts.insert("ActionSetHwBpE", Shortcut({ tr("Actions"), tr("Set Hardware Breakpoint (Execute)") }, ""));
	defaultShortcuts.insert("ActionRemoveHwBp", Shortcut({ tr("Actions"), tr("Remove Hardware Breakpoint") }, ""));
	defaultShortcuts.insert("ActionRemoveTypeAnalysisFromModule", Shortcut({ tr("Actions"), tr("Remove Type Analysis From Module") }, "Ctrl+Shift+U"));
	defaultShortcuts.insert("ActionRemoveTypeAnalysisFromSelection", Shortcut({ tr("Actions"), tr("Remove Type Analysis From Selection") }, "U"));
	defaultShortcuts.insert("ActionTreatSelectionAsCode", Shortcut({ tr("Actions"), tr("Treat Selection As"), tr("Code") }, "C"));
	defaultShortcuts.insert("ActionTreatSelectionAsByte", Shortcut({ tr("Actions"), tr("Treat Selection As"), tr("Byte") }, "B"));
	defaultShortcuts.insert("ActionTreatSelectionAsWord", Shortcut({ tr("Actions"), tr("Treat Selection As"), tr("Word") }, "W"));
	defaultShortcuts.insert("ActionTreatSelectionAsDword", Shortcut({ tr("Actions"), tr("Treat Selection As"), tr("Dword") }, "D"));
	defaultShortcuts.insert("ActionTreatSelectionAsFword", Shortcut({ tr("Actions"), tr("Treat Selection As"), tr("Fword") }, ""));
	defaultShortcuts.insert("ActionTreatSelectionAsQword", Shortcut({ tr("Actions"), tr("Treat Selection As"), tr("Qword") }, "Q"));
	defaultShortcuts.insert("ActionTreatSelectionAsTbyte", Shortcut({ tr("Actions"), tr("Treat Selection As"), tr("Tbyte") }, ""));
	defaultShortcuts.insert("ActionTreatSelectionAsOword", Shortcut({ tr("Actions"), tr("Treat Selection As"), tr("Oword") }, ""));
	defaultShortcuts.insert("ActionTreatSelectionAsFloat", Shortcut({ tr("Actions"), tr("Treat Selection As"), tr("Float") }, ""));
	defaultShortcuts.insert("ActionTreatSelectionAsDouble", Shortcut({ tr("Actions"), tr("Treat Selection As"), tr("Double") }, ""));
	defaultShortcuts.insert("ActionTreatSelectionAsLongDouble", Shortcut({ tr("Actions"), tr("Treat Selection As"), tr("LongDouble") }, ""));
	defaultShortcuts.insert("ActionTreatSelectionAsASCII", Shortcut({ tr("Actions"), tr("Treat Selection As"), tr("ASCII") }, ""));
	defaultShortcuts.insert("ActionTreatSelectionAsUNICODE", Shortcut({ tr("Actions"), tr("Treat Selection As"), tr("UNICODE") }, ""));
	defaultShortcuts.insert("ActionTreatSelectionAsMMWord", Shortcut({ tr("Actions"), tr("Treat Selection As"), tr("MMWord") }, ""));
	defaultShortcuts.insert("ActionTreatSelectionAsXMMWord", Shortcut({ tr("Actions"), tr("Treat Selection As"), tr("XMMWord") }, ""));
	defaultShortcuts.insert("ActionTreatSelectionAsYMMWord", Shortcut({ tr("Actions"), tr("Treat Selection As"), tr("YMMWord") }, ""));
	defaultShortcuts.insert("ActionTreatSelectionHeadAsCode", Shortcut({ tr("Actions"), tr("Treat Selection Head As"), tr("Code") }, ""));
	defaultShortcuts.insert("ActionTreatSelectionHeadAsByte", Shortcut({ tr("Actions"), tr("Treat Selection Head As"), tr("Byte") }, ""));
	defaultShortcuts.insert("ActionTreatSelectionHeadAsWord", Shortcut({ tr("Actions"), tr("Treat Selection Head As"), tr("Word") }, ""));
	defaultShortcuts.insert("ActionTreatSelectionHeadAsDword", Shortcut({ tr("Actions"), tr("Treat Selection Head As"), tr("Dword") }, ""));
	defaultShortcuts.insert("ActionTreatSelectionHeadAsFword", Shortcut({ tr("Actions"), tr("Treat Selection Head As"), tr("Fword") }, ""));
	defaultShortcuts.insert("ActionTreatSelectionHeadAsQword", Shortcut({ tr("Actions"), tr("Treat Selection Head As"), tr("Qword") }, ""));
	defaultShortcuts.insert("ActionTreatSelectionHeadAsTbyte", Shortcut({ tr("Actions"), tr("Treat Selection Head As"), tr("Tbyte") }, ""));
	defaultShortcuts.insert("ActionTreatSelectionHeadAsOword", Shortcut({ tr("Actions"), tr("Treat Selection Head As"), tr("Oword") }, ""));
	defaultShortcuts.insert("ActionTreatSelectionHeadAsFloat", Shortcut({ tr("Actions"), tr("Treat Selection Head As"), tr("Float") }, ""));
	defaultShortcuts.insert("ActionTreatSelectionHeadAsDouble", Shortcut({ tr("Actions"), tr("Treat Selection Head As"), tr("Double") }, ""));
	defaultShortcuts.insert("ActionTreatSelectionHeadAsLongDouble", Shortcut({ tr("Actions"), tr("Treat Selection Head As"), tr("LongDouble") }, ""));
	defaultShortcuts.insert("ActionTreatSelectionHeadAsASCII", Shortcut({ tr("Actions"), tr("Treat Selection Head As"), tr("ASCII") }, ""));
	defaultShortcuts.insert("ActionTreatSelectionHeadAsUNICODE", Shortcut({ tr("Actions"), tr("Treat Selection Head As"), tr("UNICODE") }, ""));
	defaultShortcuts.insert("ActionTreatSelectionHeadAsMMWord", Shortcut({ tr("Actions"), tr("Treat Selection Head As"), tr("MMWord") }, ""));
	defaultShortcuts.insert("ActionTreatSelectionHeadAsXMMWord", Shortcut({ tr("Actions"), tr("Treat Selection Head As"), tr("XMMWord") }, ""));
	defaultShortcuts.insert("ActionTreatSelectionHeadAsYMMWord", Shortcut({ tr("Actions"), tr("Treat Selection Head As"), tr("YMMWord") }, ""));
	defaultShortcuts.insert("ActionIncreaseRegister", Shortcut({ tr("Actions"), tr("Increase Register") }, "+"));
	defaultShortcuts.insert("ActionDecreaseRegister", Shortcut({ tr("Actions"), tr("Decrease Register") }, "-"));
	defaultShortcuts.insert("ActionIncreaseRegisterPtrSize", Shortcut({ tr("Actions"), tr("Increase Register by") + ArchValue(QString(" 4"), QString(" 8")) }));
	defaultShortcuts.insert("ActionDecreaseRegisterPtrSize", Shortcut({ tr("Actions"), tr("Decrease Register by") + ArchValue(QString(" 4"), QString(" 8")) }));
	defaultShortcuts.insert("ActionZeroRegister", Shortcut({ tr("Actions"), tr("Zero Register") }, "0"));
	defaultShortcuts.insert("ActionSetOneRegister", Shortcut({ tr("Actions"), tr("Set Register to One") }, "1"));
	defaultShortcuts.insert("ActionToggleRegisterValue", Shortcut({ tr("Actions"), tr("Toggle Register Value") }, "Space"));
	defaultShortcuts.insert("ActionCopy", Shortcut({ tr("Actions"), tr("Copy") }, "Ctrl+C"));
	defaultShortcuts.insert("ActionCopyAddress", Shortcut({ tr("Actions"), tr("Copy Address") }, "Alt+INS"));
	defaultShortcuts.insert("ActionCopyRva", Shortcut({ tr("Actions"), tr("Copy RVA") }, ""));
	defaultShortcuts.insert("ActionCopySymbol", Shortcut({ tr("Actions"), tr("Copy Symbol") }, "Ctrl+S"));
	defaultShortcuts.insert("ActionLoadScript", Shortcut({ tr("Actions"), tr("Load Script") }, "Ctrl+O"));
	defaultShortcuts.insert("ActionReloadScript", Shortcut({ tr("Actions"), tr("Reload Script") }, "Ctrl+R"));
	defaultShortcuts.insert("ActionUnloadScript", Shortcut({ tr("Actions"), tr("Unload Script") }, "Ctrl+U"));
	defaultShortcuts.insert("ActionEditScript", Shortcut({ tr("Actions"), tr("Edit Script") }, ""));
	defaultShortcuts.insert("ActionRunScript", Shortcut({ tr("Actions"), tr("Run Script") }, "Space"));
	defaultShortcuts.insert("ActionToggleBreakpointScript", Shortcut({ tr("Actions"), tr("Toggle Script Breakpoint") }, "F2"));
	defaultShortcuts.insert("ActionRunToCursorScript", Shortcut({ tr("Actions"), tr("Run Script to Cursor") }, "Shift+F4"));
	defaultShortcuts.insert("ActionStepScript", Shortcut({ tr("Actions"), tr("Step Script") }, "Tab"));
	defaultShortcuts.insert("ActionAbortScript", Shortcut({ tr("Actions"), tr("Abort Script") }, "Esc"));
	defaultShortcuts.insert("ActionExecuteCommandScript", Shortcut({ tr("Actions"), tr("Execute Script Command") }, "X"));
	defaultShortcuts.insert("ActionRefresh", Shortcut({ tr("Actions"), tr("Refresh") }, "F5"));
	defaultShortcuts.insert("ActionGraph", Shortcut({ tr("Actions"), tr("Graph") }, "G"));
	defaultShortcuts.insert("ActionGraphFollowDisassembler", Shortcut({ tr("Actions"), tr("Graph"), tr("Follow in disassembler") }, "Shift+Return"));
	defaultShortcuts.insert("ActionGraphSaveImage", Shortcut({ tr("Actions"), tr("Graph"), tr("Save as image") }, "I"));
	defaultShortcuts.insert("ActionGraphToggleOverview", Shortcut({ tr("Actions"), tr("Graph"), tr("Toggle overview") }, "O"));
	defaultShortcuts.insert("ActionGraphToggleSummary", Shortcut({ tr("Actions"), tr("Graph"), tr("Toggle summary") }, "U"));
	defaultShortcuts.insert("ActionGraphSyncOrigin", Shortcut({ tr("Actions"), tr("Graph"), tr("Toggle sync with origin") }, "S"));
	defaultShortcuts.insert("ActionGraphDecompile", Shortcut({ tr("Actions"), tr("Graph"), tr("Decompile") }, "Tab"));
	defaultShortcuts.insert("ActionIncrementx87Stack", Shortcut({ tr("Actions"), tr("Increment x87 Stack") }));
	defaultShortcuts.insert("ActionDecrementx87Stack", Shortcut({ tr("Actions"), tr("Decrement x87 Stack") }));
	defaultShortcuts.insert("ActionPush", Shortcut({ tr("Actions"), tr("Push") }));
	defaultShortcuts.insert("ActionPop", Shortcut({ tr("Actions"), tr("Pop") }));
	defaultShortcuts.insert("ActionRedirectLog", Shortcut({ tr("Actions"), tr("Redirect Log") }));
	defaultShortcuts.insert("ActionBrowseInExplorer", Shortcut({ tr("Actions"), tr("Browse in Explorer") }));
	defaultShortcuts.insert("ActionDownloadSymbol", Shortcut({ tr("Actions"), tr("Download Symbols for This Module") }));
	defaultShortcuts.insert("ActionDownloadAllSymbol", Shortcut({ tr("Actions"), tr("Download Symbols for All Modules") }));
	defaultShortcuts.insert("ActionCreateNewThreadHere", Shortcut({ tr("Actions"), tr("Create New Thread Here") }));
	defaultShortcuts.insert("ActionOpenSourceFile", Shortcut({ tr("Actions"), tr("Open Source File") }));
	defaultShortcuts.insert("ActionFollowMemMap", Shortcut({ tr("Actions"), tr("Follow in Memory Map") }));
	defaultShortcuts.insert("ActionFollowStack", Shortcut({ tr("Actions"), tr("Follow in Stack") }));
	defaultShortcuts.insert("ActionFollowDisasm", Shortcut({ tr("Actions"), tr("Follow in Disassembler") }));
	defaultShortcuts.insert("ActionFollowDwordQwordDisasm", Shortcut({ tr("Actions"), tr("Follow DWORD/QWORD in Disassembler") }));
	defaultShortcuts.insert("ActionFollowDwordQwordDump", Shortcut({ tr("Actions"), tr("Follow DWORD/QWORD in Dump") }));
	defaultShortcuts.insert("ActionFreezeStack", Shortcut({ tr("Actions"), tr("Freeze the stack") }));
	defaultShortcuts.insert("ActionGotoBaseOfStackFrame", Shortcut({ tr("Actions"), tr("Go to Base of Stack Frame") }));
	defaultShortcuts.insert("ActionGotoPrevStackFrame", Shortcut({ tr("Actions"), tr("Go to Previous Stack Frame") }));
	defaultShortcuts.insert("ActionGotoNextStackFrame", Shortcut({ tr("Actions"), tr("Go to Next Stack Frame") }));
	defaultShortcuts.insert("ActionGotoPreviousReference", Shortcut({ tr("Actions"), tr("Go to Previous Reference") }, "Ctrl+K"));
	defaultShortcuts.insert("ActionGotoNextReference", Shortcut({ tr("Actions"), tr("Go to Next Reference") }, "Ctrl+L"));
	defaultShortcuts.insert("ActionModifyValue", Shortcut({ tr("Actions"), tr("Modify value") }, "Space"));
	defaultShortcuts.insert("ActionWatchDwordQword", Shortcut({ tr("Actions"), tr("Watch DWORD/QWORD") }));
	defaultShortcuts.insert("ActionCopyFileOffset", Shortcut({ tr("Actions"), tr("Copy File Offset") }));
	defaultShortcuts.insert("ActionToggleRunTrace", Shortcut({ tr("Actions"), tr("Start or Stop Run Trace") }));

	Shortcuts = defaultShortcuts;

	load();

	//because we changed the default this needs special handling for old configurations
	if (Shortcuts["ViewPreviousTab"].Hotkey.toString() == Shortcuts["ViewPreviousHistory"].Hotkey.toString())
	{
		Shortcuts["ViewPreviousTab"].Hotkey = defaultShortcuts["ViewPreviousTab"].Hotkey;
		save();
	}
	if (Shortcuts["ViewNextTab"].Hotkey.toString() == Shortcuts["ViewNextHistory"].Hotkey.toString())
	{
		Shortcuts["ViewNextTab"].Hotkey = defaultShortcuts["ViewNextTab"].Hotkey;
		save();
	}
	*/
load();
save();

}

Configuration::~Configuration()
{	
	mPtr = nullptr;
}

Configuration* Configuration::instance()
{
	return mPtr;
}

void Configuration::load()
{

	readUints();
	readColors();
	readFonts();
}

void Configuration::save()
{

	writeUints();
	writeFonts();

}

void Configuration::readColors()
{
	Colors = defaultColors;
	//read config
	/*
	for (int i = 0; i < Colors.size(); i++)
	{
		CString id = Colors.keys().at(i);
		Colors[id] = colorFromConfig(id);
	}
	*/
	map <string, Color>::iterator iter;
	for (iter = Colors.begin(); iter != Colors.end(); iter++)
	{
		string id = iter->first;
		Colors[id] = colorFromConfig(id);
	}
}

void Configuration::readUints()
{
	Uints = defaultUints;
	//read config
	/*
	for (int i = 0; i < Uints.size(); i++)
	{
		QString category = Uints.keys().at(i);
		QMap<QString, duint> & currentUint = Uints[category];
		for (int j = 0; j < currentUint.size(); j++)
		{
			QString id = currentUint.keys().at(j);
			currentUint[id] = uintFromConfig(category, id);
		}
	}
	*/
	map<string, map<string, duint>>::iterator iter;
	for (iter = Uints.begin(); iter != Uints.end(); iter++)
	{
		string category = iter->first;
		map<string, duint> & currentUint = Uints[category];
		map<string, duint>::iterator inIter;
		for (inIter = currentUint.begin(); inIter != currentUint.end(); inIter++)
		{
			string id = inIter->first;
			currentUint[id] = uintFromConfig(category, id);
		}

	}
}

void Configuration::writeUints()
{
	duint setting;
	bool bSaveLoadTabOrder = ConfigBool("Gui", "LoadSaveTabOrder");

	//write config
	/*
	for (int i = 0; i < Uints.size(); i++)
	{
		//QString category = Uints.keys().at(i);
		for (int j = 0; j < currentUint->size(); j++)
		{
			QString id = (*currentUint).keys().at(j);

			// Do not save settings to file if saveLoadTabOrder checkbox is Unchecked
			if (!bSaveLoadTabOrder && category == "TabOrder" && BridgeSettingGetUint(category.toUtf8().constData(), id.toUtf8().constData(), &setting))
				continue;

			uintToConfig(category, id, (*currentUint)[id]);
		}
	}
	*/
	map<string, map<string, duint>>::iterator iter;
	for (iter = Uints.begin(); iter != Uints.end(); iter++)
	{
		string category = iter->first;
		map<string, duint>* currentUint = &Uints[category];
		map<string, duint>::iterator inIter;
		for (inIter = currentUint->begin(); inIter != currentUint->end(); inIter++)
		{
			string id = inIter->first;
			// Do not save settings to file if saveLoadTabOrder checkbox is Unchecked
			bool s = BridgeSettingGetUint(category.data(), id.data(), &setting);
			
			if (!bSaveLoadTabOrder && category == "TabOrder" && s)
				continue;
			
			uintToConfig(category, id, (*currentUint)[id]);
		}
	}
}



LOGFONT Configuration::fontFromConfig(const string id)
{
	char setting[MAX_SETTING_SIZE] = "";
	if (!BridgeSettingGet("Fonts", id.data(), setting))
	{
		if (defaultFonts.count(id))
		{
			LOGFONT ret = defaultFonts.find(id)->second;
			fontToConfig(id, ret);
			return ret;
		}
		/*
		if (id == "Application")
		{
			LOGFONT logfont;
			CFont f;
			f.GetLogFont(&logfont);
		}		
		*/		
		LOGFONT retLogFont;
		memset(&retLogFont, 0, sizeof(retLogFont));
		retLogFont.lfWeight = FW_NORMAL;
		_tcscpy_s(retLogFont.lfFaceName, LF_FACESIZE, _T("Lucida Console"));
		retLogFont.lfHeight = -20;
		return retLogFont;
	}

	LOGFONT font;
	memset(&font, 0, sizeof(font));
	CString settingFont;
	CString extractMsg;
	CString remainingMsg;
	settingFont = setting;
	CString findStr = _T(",");
	

	int len = findStr.GetLength();
	
	for (int i = 0; i < 4; i++)
	{
		int index = settingFont.FindOneOf(findStr);
		if (index == -1)
			break;
		extractMsg = settingFont.Left(index);
		remainingMsg = settingFont.Mid(index + len);
		switch (i)
		{
		case 0:
			_tcscpy_s(font.lfFaceName, LF_FACESIZE, extractMsg);
			break;
		case 1:
			font.lfWidth = _ttol(extractMsg);
			break;		
		case 2:
			font.lfHeight = _ttol(extractMsg);
			break;
		case 3:
			font.lfWeight = _ttol(extractMsg);
			break;
		default:
			break;
		}
		settingFont = remainingMsg;
	}
	/*
	if (!font.fromString(setting))
	{
		if (defaultFonts.contains(id))
		{
			QFont ret = defaultFonts.find(id).value();
			fontToConfig(id, ret);
			return ret;
		}
		if (id == "Application")
			return QApplication::font();
		QFont ret("Lucida Console", 8, QFont::Normal, false);
		ret.setFixedPitch(true);
		ret.setStyleHint(QFont::Monospace);
		return ret;
	}
	*/
	return font;
}

bool Configuration::fontToConfig(const string id, const LOGFONT& font)
{
	CString fontString;
	fontString.Format(_T("%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d"), font.lfFaceName, font.lfWidth, font.lfHeight, font.lfWeight,
		font.lfEscapement,
		font.lfOrientation,
		font.lfItalic,
		font.lfUnderline,
		font.lfStrikeOut,
		font.lfCharSet,
		font.lfOutPrecision,
		font.lfClipPrecision,
		font.lfQuality,
		font.lfPitchAndFamily);
	string s;
	s = CW2A(fontString);
	return BridgeSettingSet("Fonts", id.data(), s.data());
}

void Configuration::writeFonts()
{
	//write config
	/*
	for (int i = 0; i < Fonts.size(); i++)
	{
		QString id = Fonts.keys().at(i);
		fontToConfig(id, Fonts[id]);
	}
	*/
	map <string, LOGFONT>::iterator iter;
	for (iter = Fonts.begin(); iter != Fonts.end(); iter++)
	{
		string id = iter->first;
		fontToConfig(id, Fonts[id]);
	}
	//emit fontsUpdated();
}

void Configuration::readFonts()
{
	Fonts = defaultFonts;
	//read config
	/*
	for (int i = 0; i < Fonts.size(); i++)
	{
		QString id = Fonts.keys().at(i);
		QFont font = fontFromConfig(id);
		QFontInfo fontInfo(font);
		if (id == "Application" || fontInfo.fixedPitch())
			Fonts[id] = font;
	}
	*/
	map<string, LOGFONT>::iterator iter;
	for (iter = Fonts.begin(); iter != Fonts.end(); iter++)
	{
		string id = iter->first;
		LOGFONT font = fontFromConfig(id);
	}

}

const Color Configuration::getColor(const string id) const
{
	if (Colors.count(id))
		return Colors.find(id)->second;		
	else
	{
		return Color::Black;
	}
}

const bool Configuration::getBool(const string category, const string id) const
{
	if (Bools.count(category))
	{
		//if (Bools[category].contains(id))
		auto iter = Bools.find(category);
		if (iter->second.count(id))
		{
			//return Bools[category][id];
			auto inIter = iter->second.find(id);
			return inIter->second;
		}			
		if (noMoreMsgbox)
			return false;
		if (MessageBox(NULL, _T("NOT FOUND IN CONFIG!"), NULL, MB_OKCANCEL) == IDCANCEL)
			noMoreMsgbox = true;
		return false;
	}
	if (noMoreMsgbox)
		return false;
	
	if (MessageBox(NULL, _T("NOT FOUND IN CONFIG!"), NULL, MB_OKCANCEL) == IDCANCEL)
		noMoreMsgbox = true;
	return false; 
	
}

const duint Configuration::getUint(const string category, const string id) const
{
	if (Uints.count(category))
	{
		//if (Uints[category].contains(id))
		auto iter = Uints.find(category);
		if (iter->second.count(id))
		{
			//return Uints[category][id];
			auto inIter = iter->second.find(id);
			return inIter->second;
		}			
		if (noMoreMsgbox)
			return 0;
		/*
		QMessageBox msg(QMessageBox::Warning, tr("NOT FOUND IN CONFIG!"), category + ":" + id, QMessageBox::Retry | QMessageBox::Cancel);
		msg.setWindowIcon(DIcon("compile-warning.png"));
		msg.setWindowFlags(msg.windowFlags() & (~Qt::WindowContextHelpButtonHint));
		if (msg.exec() == QMessageBox::Cancel)
			noMoreMsgbox = true;
			*/
		MessageBox(NULL, _T("NOT FOUND IN CONFIG!"), NULL, MB_OKCANCEL);
		if (MessageBox(NULL, _T("NOT FOUND IN CONFIG!"), NULL, MB_OKCANCEL) == IDCANCEL)
			noMoreMsgbox = true;
		return 0;
	}
	if (noMoreMsgbox)
		return 0;
	/*
	QMessageBox msg(QMessageBox::Warning, tr("NOT FOUND IN CONFIG!"), category, QMessageBox::Retry | QMessageBox::Cancel);
	msg.setWindowIcon(DIcon("compile-warning.png"));
	msg.setWindowFlags(msg.windowFlags() & (~Qt::WindowContextHelpButtonHint));
	if (msg.exec() == QMessageBox::Cancel)
		noMoreMsgbox = true;
		*/
	MessageBox(NULL, _T("NOT FOUND IN CONFIG!"), NULL, MB_OKCANCEL);
	if (MessageBox(NULL, _T("NOT FOUND IN CONFIG!"), NULL, MB_OKCANCEL) == IDCANCEL)
		noMoreMsgbox = true;
	return 0;
}

void Configuration::setUint(const string category, const string id, const duint i)
{
	auto iter = Uints.find(category);
	if (iter != Uints.end())
	{
		if (iter->second.count(id))
		{
			Uints[category][id] = i;
			return;
		}
		if (noMoreMsgbox)
			return;
		MessageBox(NULL, _T("NOT FOUND IN CONFIG!"), NULL, MB_OKCANCEL);
		if (MessageBox(NULL, _T("NOT FOUND IN CONFIG!"), NULL, MB_OKCANCEL) == IDCANCEL)
			noMoreMsgbox = true;
		return;
	}
	if (noMoreMsgbox)
		return;
	MessageBox(NULL, _T("NOT FOUND IN CONFIG!"), NULL, MB_OKCANCEL);
	if (MessageBox(NULL, _T("NOT FOUND IN CONFIG!"), NULL, MB_OKCANCEL) == IDCANCEL)
		noMoreMsgbox = true;
}

const LOGFONT Configuration::getFont(const string id) const
{
	if (Fonts.count(id))
		return Fonts.find(id)->second;	
	/*
	LOGFONT ret("Lucida Console", 8, QFont::Normal, false);
	ret.setFixedPitch(true);
	ret.setStyleHint(QFont::Monospace);
	*/
	LOGFONT retLogFont;
	memset(&retLogFont, 0, sizeof(retLogFont));
	retLogFont.lfWeight = FW_NORMAL;
	_tcscpy_s(retLogFont.lfFaceName, LF_FACESIZE, _T("Lucida Console"));
	retLogFont.lfHeight = -20;

	if (noMoreMsgbox)
		return retLogFont;
	/*
	QMessageBox msg(QMessageBox::Warning, tr("NOT FOUND IN CONFIG!"), id, QMessageBox::Retry | QMessageBox::Cancel);
	msg.setWindowIcon(DIcon("compile-warning.png"));
	msg.setWindowFlags(msg.windowFlags() & (~Qt::WindowContextHelpButtonHint));
	if (msg.exec() == QMessageBox::Cancel)
		noMoreMsgbox = true;
		*/
	if (MessageBox(NULL, _T("NOT FOUND IN CONFIG!"), NULL, MB_OKCANCEL) == IDCANCEL)
		noMoreMsgbox = true;
	return retLogFont;
}

bool Configuration::uintToConfig(const string category, const string id, duint i)
{
	return BridgeSettingSetUint(category.data(), id.data(), i);
}

duint Configuration::uintFromConfig(const string category, const string id)
{
	duint setting;

	if (!BridgeSettingGetUint(category.data(), id.data(), &setting))
	{
		if (defaultUints.count(category) && defaultUints[category].count(id))
		{
			setting = defaultUints[category][id];
			uintToConfig(category, id, setting);
			return setting;
		}
		return 0; //DAFUG
	}
	return setting;
}

bool Configuration::colorToConfig(const string id, const Color color)
{
	char buffer[50];
	sprintf_s(buffer, 50, "0x%.2X%.2X%.2X%.2X", color.GetAlpha(), color.GetR(), color.GetG(), color.GetB());
	const char * p = id.c_str();
	bool s = BridgeSettingSet("Colors", p, buffer);
	
	return s;
}

Color Configuration::colorFromConfig(const string id)
{
	Color tmpColor;
	char setting[MAX_SETTING_SIZE] = "";

	const char* p = id.c_str();
	bool s = BridgeSettingGet("Colors", p, setting);
	
	if (!s)
	{
		if (defaultColors.count(id))
		{
			Color ret = defaultColors.find(id)->second;
			colorToConfig(id, ret);
			return ret;
		}
		return tmpColor.Black; //black is default
	}
	//tmpStr.Format(_T("%s"), setting);
	CString tmpStr(setting);
	DWORD dwValue = _tcstoul(tmpStr, NULL, 16);
	Color color(dwValue);
	return color;
}