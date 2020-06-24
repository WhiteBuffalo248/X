#pragma once
#include <stdint.h>
#include "Bridge.h"

#include<set>
#include<map>
using namespace std;



typedef struct
{
	const char* string;
	unsigned int value;
} STRING_VALUE_TABLE_t;

// CRegistersView

class CRegistersView : public CWnd
{
	DECLARE_DYNAMIC(CRegistersView)

public:
	CRegistersView();
	virtual ~CRegistersView();

public:
public:
	// all possible register ids
	enum REGISTER_NAME
	{
		CAX, CCX, CDX, CBX, CDI, CBP, CSI, CSP,
#ifdef _WIN64
		R8, R9, R10, R11, R12, R13, R14, R15,
#endif //_WIN64
		CIP,
		EFLAGS, CF, PF, AF, ZF, SF, TF, IF, DF, OF,
		GS, FS, ES, DS, CS, SS,
		LastError, LastStatus,
		DR0, DR1, DR2, DR3, DR6, DR7,
		// x87 stuff
		x87r0, x87r1, x87r2, x87r3, x87r4, x87r5, x87r6, x87r7,
		x87TagWord, x87ControlWord, x87StatusWord,
		// x87 Tag Word fields
		x87TW_0, x87TW_1, x87TW_2, x87TW_3, x87TW_4, x87TW_5,
		x87TW_6, x87TW_7,
		// x87 Status Word fields
		x87SW_B, x87SW_C3, x87SW_TOP, x87SW_C2, x87SW_C1, x87SW_O,
		x87SW_ES, x87SW_SF, x87SW_P, x87SW_U, x87SW_Z,
		x87SW_D, x87SW_I, x87SW_C0,
		// x87 Control Word fields
		x87CW_IC, x87CW_RC, x87CW_PC, x87CW_PM,
		x87CW_UM, x87CW_OM, x87CW_ZM, x87CW_DM, x87CW_IM,
		//MxCsr
		MxCsr, MxCsr_FZ, MxCsr_PM, MxCsr_UM, MxCsr_OM, MxCsr_ZM,
		MxCsr_IM, MxCsr_DM, MxCsr_DAZ, MxCsr_PE, MxCsr_UE, MxCsr_OE,
		MxCsr_ZE, MxCsr_DE, MxCsr_IE, MxCsr_RC,
		// MMX and XMM
		MM0, MM1, MM2, MM3, MM4, MM5, MM6, MM7,
		XMM0, XMM1, XMM2, XMM3, XMM4, XMM5, XMM6, XMM7,
#ifdef _WIN64
		XMM8, XMM9, XMM10, XMM11, XMM12, XMM13, XMM14, XMM15,
#endif //_WIN64
		// YMM
		YMM0, YMM1, YMM2, YMM3, YMM4, YMM5, YMM6, YMM7,
#ifdef _WIN64
		YMM8, YMM9, YMM10, YMM11, YMM12, YMM13, YMM14, YMM15,
#endif //_WIN64
		UNKNOWN
	};

	enum SIMD_REG_DISP_MODE
	{
		SIMD_REG_DISP_HEX,
		SIMD_REG_DISP_FLOAT,
		SIMD_REG_DISP_DOUBLE,
		SIMD_REG_DISP_WORD_SIGNED,
		SIMD_REG_DISP_DWORD_SIGNED,
		SIMD_REG_DISP_QWORD_SIGNED,
		SIMD_REG_DISP_WORD_UNSIGNED,
		SIMD_REG_DISP_DWORD_UNSIGNED,
		SIMD_REG_DISP_QWORD_UNSIGNED,
		SIMD_REG_DISP_WORD_HEX,
		SIMD_REG_DISP_DWORD_HEX,
		SIMD_REG_DISP_QWORD_HEX
	};
	

	// contains viewport position of register
	struct Register_Position
	{
		int line;
		int start;
		int valuesize;
		int labelwidth;

		Register_Position(int l, int s, int w, int v)
		{
			line = l;
			start = s;
			valuesize = v;
			labelwidth = w;
		}
		Register_Position()
		{
			line = 0;
			start = 0;
			valuesize = 0;
			labelwidth = 0;
		}
	};

public:
	void updateRegistersSlot();
	int getRowHeight() const;

protected:
	void initScorll();
	void registerRegisters();
	void initSIMDRegDispMode();
	void InitMappings();
	void fontsUpdated();
	bool identifyRegister(const int y, const int x, REGISTER_NAME* clickedReg);

	void OnButtonDown(UINT nFlags, CPoint point);

	//unsigned int GetMxCsrRCValueFromString(const char* string);
	void ModifyFields(const CString & title, STRING_VALUE_TABLE_t* table, SIZE_T size);
	void displayEditDialog();
	void ShowFPU(bool set_showfpu);
	void onChangeFPUViewAction();
	void appendRegister(CString & text, REGISTER_NAME reg, const char* name64, const char* name32);
	bool setTextToClipboard(CString& text);
	void onCopy();
	void onCopyAll();
	void onFollowInDisassembly();
	void onFollowInDump();
	void onFollowInStack();
	void onFollowInMemoryMap();
	void onCopySymbol();
	void onHighlight();
	void onUndo();
	void onZero();
	void onSetTo_1();
	void onToggleValue();
	void onIncrementx87Stack();
	void onDecrementx87Stack();
	void onIncrement();
	void onDecrement();
	void onPush();
	void onPop();


private:
	LOGFONT lf;

	bool mShowFpu;

	int mVScrollOffset;
	int mMaxLine;
	int mRowsNeeded;
	int mCharWidth;
	int mCharCount;
	int yTopSpacing;

	int hScrollValue;
	int vScrollValue;

	dsint mCip;

	// contains a dump of the current register values
	REGDUMP wRegDumpStruct;
	REGDUMP wCipRegDumpStruct;
	// holds current selected register
	REGISTER_NAME mSelected;

	// SIMD registers display mode
	SIMD_REG_DISP_MODE wSIMDRegDispMode;

	set<REGISTER_NAME> mUINTDISPLAY;
	set<REGISTER_NAME> mUSHORTDISPLAY;
	set<REGISTER_NAME> mFIELDVALUE;
	set<REGISTER_NAME> mDWORDDISPLAY;
	set<REGISTER_NAME> mBOOLDISPLAY;
	set<REGISTER_NAME> mTAGWORD;
	set<REGISTER_NAME> mLABELDISPLAY;
	set<REGISTER_NAME> mONLYMODULEANDLABELDISPLAY;
	set<REGISTER_NAME> mCANSTOREADDRESS;
	set<REGISTER_NAME> mUNDODISPLAY;
	set<REGISTER_NAME> mSEGMENTREGISTER;
	set<REGISTER_NAME> mSETONEZEROTOGGLE;
	set<REGISTER_NAME> mINCREMENTDECREMET;
	set<REGISTER_NAME> mFPUx87_80BITSDISPLAY;
	set<REGISTER_NAME> mFPU;

	// all flags
	set<REGISTER_NAME> mFlags;
	// FPU x87, XMM and MMX registers
	set<REGISTER_NAME> mFPUx87;
	set<REGISTER_NAME> mFPUMMX;
	set<REGISTER_NAME> mFPUXMM;
	set<REGISTER_NAME> mFPUYMM;
	// contains all id's of registers if there occurs a change
	set<REGISTER_NAME> mRegisterUpdates;
	set<REGISTER_NAME> mMODIFYDISPLAY;

	// general purposes register id s (cax, ..., r8, ....)
	set<REGISTER_NAME> mGPR;
	// registers that do not allow changes
	set<REGISTER_NAME> mNoChange;

	// maps from id to name
	map<REGISTER_NAME, string> mRegisterMapping;
	// contains viewport positions
	map<REGISTER_NAME, Register_Position> mRegisterPlaces;

	std::vector<std::pair<const char*, uint8_t>> mHighlightRegs;

	// use-in-class-only methods
	void drawRegister(CDC* painter, REGISTER_NAME reg, char* value);
	char* registerValue(const REGDUMP* regd, const REGISTER_NAME reg);
	void setRegister(REGISTER_NAME reg, duint value);
	void setRegisters(REGDUMP* reg);
	SIZE_T GetSizeRegister(const REGISTER_NAME reg_name);
	CString getRegisterLabel(REGISTER_NAME);
	CString helpRegister(REGISTER_NAME reg);

	CString GetTagWordStateString(unsigned short);
	CString GetMxCsrRCStateString(unsigned short);
	CString GetControlWordRCStateString(unsigned short);
	CString GetControlWordPCStateString(unsigned short);
	CString GetStatusWordTOPStateString(unsigned short state);

	CString GetRegStringValueFromValue(REGISTER_NAME reg, const char* value);
	int CompareRegisters(const REGISTER_NAME reg_name, REGDUMP* regdump1, REGDUMP* regdump2);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnUnknownHandle(UINT id);
	afx_msg void OnLocalHandle(UINT id);
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};


