#pragma once

#define User_Defined_Start									2048
//全局复制命令ID(50-60)
#define	ID_GLOBAL_COPY_START								50
//信号，供主框架调用 必须大于 GUI_INVALIDATE_SYMBOL_SOURCE 定义
#define Signal_SelectionChange_Msg							111
#define Signal_DisplayReferencesPane_Msg					112
#define Signal_SetSideBarRow_Msg							113

//中间值用来动态添加，勿暂用，暂使用10
#define	ID_GLOBAL_COPY_END									60
#define	ID_GLOBAL_COPY_LINE									71
#define	ID_GLOBAL_COPY_CROPPED_TABLE						72
#define	ID_GLOBAL_COPY_FULL_TABLE							73
#define	ID_GLOBAL_COPY_LINE_TO_LOG							74
#define	ID_GLOBAL_COPY_CROPPED_TABLE_TO_LOG					75
#define	ID_GLOBAL_COPY_FULL_TABLE_TO_LOG					76

//RegistersView
//Global Unknown
#define	ID_REGISTERS_ChangeFPUView							101
#define	ID_REGISTERS_CopyAll								102
//二级弹出菜单
#define	ID_REGISTERS_SwitchSIMDDispMode_Hexadecimal			103
#define	ID_REGISTERS_SwitchSIMDDispMode_Float				104
#define	ID_REGISTERS_SwitchSIMDDispMode_Double				105
#define	ID_REGISTERS_SwitchSIMDDispMode_SignedWord			106
#define	ID_REGISTERS_SwitchSIMDDispMode_SignedDWord			107
#define	ID_REGISTERS_SwitchSIMDDispMode_SignedQWord			108
#define	ID_REGISTERS_SwitchSIMDDispMode_UnsignedWord		109
#define	ID_REGISTERS_SwitchSIMDDispMode_UnsignedDWord		110
#define	ID_REGISTERS_SwitchSIMDDispMode_UnsignedQWord		111
#define	ID_REGISTERS_SwitchSIMDDispMode_HexadecimalWord		112
#define	ID_REGISTERS_SwitchSIMDDispMode_HexadecimalDWord	113
#define	ID_REGISTERS_SwitchSIMDDispMode_HexadecimalQWord	114
//
#define	ID_REGISTERS_SetHBP									115
//局部已知区域
#define	ID_REGISTERS_ModifyValue							120
#define	ID_REGISTERS_Copy									121
#define	ID_REGISTERS_HighLight								122
#define	ID_REGISTERS_SetValue								123
#define	ID_REGISTERS_Increment								124
#define	ID_REGISTERS_Decrement								125
#define	ID_REGISTERS_Push									126
#define	ID_REGISTERS_Pop									127
#define	ID_REGISTERS_FollowInDump							128
#define	ID_REGISTERS_FollowInDisassembly					129
#define	ID_REGISTERS_FollowInMemoryMap						130
#define	ID_REGISTERS_FollowInStack							131
#define	ID_REGISTERS_CopySymbol								132
#define	ID_REGISTERS_Undo									133
#define	ID_REGISTERS_Zero									134
#define	ID_REGISTERS_SetTo_1								135
#define	ID_REGISTERS_Toggle									136
#define	ID_REGISTERS_Incrementx87Stack						137
#define	ID_REGISTERS_Decrementx87Stack						138

//BreakPointsManage
#define	ID_BREAKPOINTS_REMOVE								201
#define	ID_BREAKPOINTS_DISABLE								202
#define	ID_BREAKPOINTS_EDIT									203
#define	ID_BREAKPOINTS_RESET_HIT_COUNT						204
#define	ID_BREAKPOINTS_ENABLE_ALL							205
#define	ID_BREAKPOINTS_DISABLE_ALL							206
#define	ID_BREAKPOINTS_REMOVE_ALL							207
#define	ID_BREAKPOINTS_ADD_DLL_BREAKPOINT					208
#define	ID_BREAKPOINTS_ADD_EXCEPTION_BREAKPOINT				209

//CPUStack
#define ID_CPUSTACK_PUSH_DWORD								301
#define ID_CPUSTACK_POP_DWORD								302
#define ID_CPUSTACK_MODIFY									303

#define ID_CPUSTACK_BINARY_EDIT								305
#define ID_CPUSTACK_BINARY_FILL								306
#define ID_CPUSTACK_BINARY_COPY								307
#define ID_CPUSTACK_BINARY_PASTE							308
#define ID_CPUSTACK_BINARY_PASTEIGNORESIZE					309

#define ID_CPUSTACK_COPY_SELECTION							311
#define ID_CPUSTACK_COPY_ADDRESS							312
#define ID_CPUSTACK_COPY_RVA								313

#define ID_CPUSTACK_BREAKPOINT_REMOVE_HARDWARE				314
#define ID_CPUSTACK_BREAKPOINT_HARDWARE_ACCESS_BYTE			315
#define ID_CPUSTACK_BREAKPOINT_HARDWARE_ACCESS_WORD			316
#define ID_CPUSTACK_BREAKPOINT_HARDWARE_ACCESS_DWORD		317
#define ID_CPUSTACK_BREAKPOINT_HARDWARE_ACCESS_QWORD		318

#define ID_CPUSTACK_BREAKPOINT_HARDWARE_WRITE_BYTE			321
#define ID_CPUSTACK_BREAKPOINT_HARDWARE_WRITE_WORD			322
#define ID_CPUSTACK_BREAKPOINT_HARDWARE_WRITE_DWORD			323
#define ID_CPUSTACK_BREAKPOINT_HARDWARE_WRITE_QWORD			324

#define ID_CPUSTACK_BREAKPOINT_REMOVE_MEMORY				330
#define ID_CPUSTACK_BREAKPOINT_MEMORY_ACCESS_SINGLESHOOT	331
#define ID_CPUSTACK_BREAKPOINT_MEMORY_ACCESS_RESTOREONHIT	332
#define ID_CPUSTACK_BREAKPOINT_MEMORY_WRITE_SINGLESHOOT		333
#define ID_CPUSTACK_BREAKPOINT_MEMORY_WRITE_RESTOREONHIT	334

#define ID_CPUSTACK_RESTORE_SELECTION						340
#define ID_CPUSTACK_FIND_PATTERN							341
#define ID_CPUSTACK_FOLLOW_ESP								342
#define ID_CPUSTACK_FOLLOW_EBP								343

#define ID_CPUSTACK_GOTO_EXPRESSION							345
#define ID_CPUSTACK_GOTO_BASEOFSTACKFRAME					346
#define ID_CPUSTACK_GOTO_PREVIOUSSTACKFRAME					347
#define ID_CPUSTACK_GOTO_NEXTSTACKFRAME						348
#define ID_CPUSTACK_GOTO_PREVIOUS							349
#define ID_CPUSTACK_GOTO_NEXT								350

#define ID_CPUSTACK_FREEZETHESTACK							351
#define ID_CPUSTACK_FOLLOW_IN_MEMORYMAP						352
#define ID_CPUSTACK_FOLLOW_IN_STACK							353
#define ID_CPUSTACK_FOLLOW_IN_DISASSEMBLE					354
#define ID_CPUSTACK_FOLLOW_IN_DUMP							355
#define ID_CPUSTACK_FOLLOW_DWORD_IN_DUMP					356

//InstructTrace
#define ID_InstructTrace_Start								401
#define ID_InstructTrace_Open								402
#define ID_InstructTrace_Close								403
#define ID_InstructTrace_CloseAndDelete						404

#define ID_InstructTrace_Copy_Selection						411
#define ID_InstructTrace_Copy_Selection_toFile				412
#define ID_InstructTrace_Copy_Selection_noBytes				413
#define ID_InstructTrace_Copy_Selection_toFile_noBytes		414

#define ID_InstructTrace_Copy_Address						415
#define ID_InstructTrace_Copy_RVA							416
#define ID_InstructTrace_Copy_FileOffset					417
#define ID_InstructTrace_Copy_Disassembly					418
#define ID_InstructTrace_Copy_Index							419

#define ID_InstructTrace_FollowInDisassembler				421

//Disassembly
#define ID_Disassembly_Copy_Selection						431

#define ID_Disassembly_Search_CurrentRegion_Command				471
#define ID_Disassembly_Search_CurrentRegion_Constant			472
#define ID_Disassembly_Search_CurrentRegion_String				473
#define ID_Disassembly_Search_CurrentRegion_Intermodular_Calls	474
#define ID_Disassembly_Search_CurrentRegion_Pattern				475
#define ID_Disassembly_Search_CurrentRegion_GUID				476

#define ID_Disassembly_Search_CurrentModule_Command			481
#define ID_Disassembly_Search_CurrentModule_Constant		482
#define ID_Disassembly_Search_CurrentModule_String			483
#define ID_Disassembly_Search_CurrentModule_Intermodular_Calls			484
#define ID_Disassembly_Search_CurrentModule_Pattern			485
#define ID_Disassembly_Search_CurrentModule_GUID			486
#define ID_Disassembly_Search_CurrentModule_Names			487

#define ID_Disassembly_Search_AllModule_Command				491
#define ID_Disassembly_Search_AllModule_Constant			492
#define ID_Disassembly_Search_AllModule_String				493
#define ID_Disassembly_Search_AllModule_Intermodular_Calls	494
#define ID_Disassembly_Search_AllModule_Pattern				495
#define ID_Disassembly_Search_AllModule_GUID				496


//Handles
#define ID_Hnadles_Refresh									501
//WinProg
#define ID_WinProg_Refresh									521
#define ID_WinProg_Enable									522
#define ID_WinProg_Follow_Pro								523
#define ID_WinProg_Toggle_Break								524
#define ID_WinProg_Message_Break							525
#define ID_WinProg_Search									526
//TcpConnections
#define ID_TcpConnections_Refresh							541

//MyTabCtrl
#define ID_MyTabCtrl_AllClose								601

//ReferenceTable
#define ID_Reference_Search									602




