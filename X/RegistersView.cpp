// RegistersView.cpp : 实现文件
//
#include "stdafx.h"
#include "X.h"
#include "GlobalID.h"
#include "RegistersView.h"
#include "Configuration.h"
#include "capstone_gui.h"
#include "StringUtil.h"
#include "MainFrm.h"
#include "WordEditDlg.h"
#include "LineEditDlg.h"
#include "ldconvert\ldconvert.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

static CString fillValue(const char* value, int valsize = 2, bool bFpuRegistersLittleEndian = false)
{
	CString charText;
	CByteArray byteArray;

	byteArray.SetSize(valsize);
	for (int i = 0; i < valsize; i++)
	{
		byteArray.SetAt(i, value[i]);
		auto ch = (BYTE)byteArray.GetAt(i);
		charText.AppendFormat(_T("%02X"), byteArray.GetAt(i));
	}	

	if (bFpuRegistersLittleEndian)
	{
		//return QString(QByteArray(value, valsize).toHex()).toUpper();
		return CString(charText.MakeUpper());
	}		
	else // Big Endian
	{
		//return QString(ByteReverse(QByteArray(value, valsize)).toHex()).toUpper();
		return CString(charText.MakeReverse());
	}
		
}

static CString composeRegTextXMM(const char* value, CRegistersView::SIMD_REG_DISP_MODE wSIMDRegDispMode, bool bFpuRegistersLittleEndian)
{
	CString valueText;
	switch (wSIMDRegDispMode)
	{
	default:
	case CRegistersView::SIMD_REG_DISP_HEX:
	{
		valueText = fillValue(value, 16, bFpuRegistersLittleEndian);
	}
	break;
	case CRegistersView::SIMD_REG_DISP_DOUBLE:
	{
		const double* dbl_values = reinterpret_cast<const double*>(value);
		if (bFpuRegistersLittleEndian)
			//valueText = QString::number(dbl_values[0]) + ' ' + QString::number(dbl_values[1]);
			valueText.Format(_T("%lf %lf"), dbl_values[0], dbl_values[1]);
		else // Big Endian
			//valueText = QString::number(dbl_values[1]) + ' ' + QString::number(dbl_values[0]);
			valueText.Format(_T("%lf %lf"), dbl_values[1], dbl_values[0]);
	}
	break;
	case CRegistersView::SIMD_REG_DISP_FLOAT:
	{
		const float* flt_values = reinterpret_cast<const float*>(value);
		if (bFpuRegistersLittleEndian)
			//valueText = QString::number(flt_values[0]) + ' ' + QString::number(flt_values[1]) + ' ' + QString::number(flt_values[2]) + ' ' + QString::number(flt_values[3]);
			valueText.Format(_T("%f %f %f %f"), flt_values[0], flt_values[1], flt_values[2], flt_values[3]);
		else // Big Endian
			//valueText = QString::number(flt_values[3]) + ' ' + QString::number(flt_values[2]) + ' ' + QString::number(flt_values[1]) + ' ' + QString::number(flt_values[0]);
			valueText.Format(_T("%f %f %f %f"), flt_values[3], flt_values[2], flt_values[1], flt_values[0]);
	}
	break;
	case CRegistersView::SIMD_REG_DISP_WORD_HEX:
	{
		if (bFpuRegistersLittleEndian)
			valueText = fillValue(value) + ' ' + fillValue(value + 1 * 2) + ' ' + fillValue(value + 2 * 2) + ' ' + fillValue(value + 3 * 2)
			+ ' ' + fillValue(value + 4 * 2) + ' ' + fillValue(value + 5 * 2) + ' ' + fillValue(value + 6 * 2) + ' ' + fillValue(value + 7 * 2);
		else // Big Endian
			valueText = fillValue(value + 7 * 2) + ' ' + fillValue(value + 6 * 2) + ' ' + fillValue(value + 5 * 2) + ' ' + fillValue(value + 4 * 2)
			+ ' ' + fillValue(value + 3 * 2) + ' ' + fillValue(value + 2 * 2) + ' ' + fillValue(value + 1 * 2) + ' ' + fillValue(value);
	}
	break;
	case CRegistersView::SIMD_REG_DISP_WORD_SIGNED:
	{
		const short* sword_values = reinterpret_cast<const short*>(value);
		if (bFpuRegistersLittleEndian)
			//valueText = QString::number(sword_values[0]) + ' ' + QString::number(sword_values[1]) + ' ' + QString::number(sword_values[2]) + ' ' + QString::number(sword_values[3])
			//+ ' ' + QString::number(sword_values[4]) + ' ' + QString::number(sword_values[5]) + ' ' + QString::number(sword_values[6]) + ' ' + QString::number(sword_values[7]);			
			valueText.Format(_T("%d %d %d %d %d %d %d %d"), sword_values[0], sword_values[1], sword_values[2], sword_values[3], sword_values[4], sword_values[5], sword_values[6], sword_values[7]);
		else // Big Endian
			//valueText = QString::number(sword_values[7]) + ' ' + QString::number(sword_values[6]) + ' ' + QString::number(sword_values[5]) + ' ' + QString::number(sword_values[4])
			//+ ' ' + QString::number(sword_values[3]) + ' ' + QString::number(sword_values[2]) + ' ' + QString::number(sword_values[1]) + ' ' + QString::number(sword_values[0]);
			valueText.Format(_T("%d %d %d %d %d %d %d %d"), sword_values[7], sword_values[6], sword_values[5], sword_values[4], sword_values[3], sword_values[2], sword_values[1], sword_values[0]);
	}
	break;
	case CRegistersView::SIMD_REG_DISP_WORD_UNSIGNED:
	{
		const unsigned short* uword_values = reinterpret_cast<const unsigned short*>(value);
		if (bFpuRegistersLittleEndian)
			//valueText = QString::number(uword_values[0]) + ' ' + QString::number(uword_values[1]) + ' ' + QString::number(uword_values[2]) + ' ' + QString::number(uword_values[3])
			//+ ' ' + QString::number(uword_values[4]) + ' ' + QString::number(uword_values[5]) + ' ' + QString::number(uword_values[6]) + ' ' + QString::number(uword_values[7]);
			valueText.Format(_T("%d %d %d %d %d %d %d %d"), uword_values[0], uword_values[1], uword_values[2], uword_values[3], uword_values[4], uword_values[5], uword_values[6], uword_values[7]);
		else // Big Endian
			//valueText = QString::number(uword_values[7]) + ' ' + QString::number(uword_values[6]) + ' ' + QString::number(uword_values[5]) + ' ' + QString::number(uword_values[4])
			//+ ' ' + QString::number(uword_values[3]) + ' ' + QString::number(uword_values[2]) + ' ' + QString::number(uword_values[1]) + ' ' + QString::number(uword_values[0]);
			valueText.Format(_T("%d %d %d %d %d %d %d %d"), uword_values[7], uword_values[6], uword_values[5], uword_values[4], uword_values[3], uword_values[2], uword_values[1], uword_values[0]);
	}
	break;
	case CRegistersView::SIMD_REG_DISP_DWORD_HEX:
	{
		if (bFpuRegistersLittleEndian)
			valueText = fillValue(value, 4) + ' ' + fillValue(value + 1 * 4, 4) + ' ' + fillValue(value + 2 * 4, 4) + ' ' + fillValue(value + 3 * 4, 4);
		else // Big Endian
			valueText = fillValue(value + 3 * 4, 4) + ' ' + fillValue(value + 2 * 4, 4) + ' ' + fillValue(value + 1 * 4, 4) + ' ' + fillValue(value, 4);
	}
	break;
	case CRegistersView::SIMD_REG_DISP_DWORD_SIGNED:
	{
		const int* sdword_values = reinterpret_cast<const int*>(value);
		if (bFpuRegistersLittleEndian)
			//valueText = QString::number(sdword_values[0]) + ' ' + QString::number(sdword_values[1]) + ' ' + QString::number(sdword_values[2]) + ' ' + QString::number(sdword_values[3]);
			valueText.Format(_T("%d %d %d %d"), sdword_values[0], sdword_values[1], sdword_values[2], sdword_values[3]);
		else // Big Endian
			//valueText = QString::number(sdword_values[3]) + ' ' + QString::number(sdword_values[2]) + ' ' + QString::number(sdword_values[1]) + ' ' + QString::number(sdword_values[0]);
			valueText.Format(_T("%d %d %d %d"), sdword_values[3], sdword_values[2], sdword_values[1], sdword_values[0]);
	}
	break;
	case CRegistersView::SIMD_REG_DISP_DWORD_UNSIGNED:
	{
		const unsigned int* udword_values = reinterpret_cast<const unsigned int*>(value);
		if (bFpuRegistersLittleEndian)
			//valueText = QString::number(udword_values[0]) + ' ' + QString::number(udword_values[1]) + ' ' + QString::number(udword_values[2]) + ' ' + QString::number(udword_values[3]);
			valueText.Format(_T("%d %d %d %d"), udword_values[0], udword_values[1], udword_values[2], udword_values[3]);
		else // Big Endian
			//valueText = QString::number(udword_values[3]) + ' ' + QString::number(udword_values[2]) + ' ' + QString::number(udword_values[1]) + ' ' + QString::number(udword_values[0]);
			valueText.Format(_T("%d %d %d %d"), udword_values[3], udword_values[2], udword_values[1], udword_values[0]);
	}
	break;
	case CRegistersView::SIMD_REG_DISP_QWORD_HEX:
	{
		if (bFpuRegistersLittleEndian)
			valueText = fillValue(value, 8) + ' ' + fillValue(value + 8, 8);
		else // Big Endian
			valueText = fillValue(value + 8, 8) + ' ' + fillValue(value, 8);
	}
	break;
	case CRegistersView::SIMD_REG_DISP_QWORD_SIGNED:
	{
		const long long* sqword_values = reinterpret_cast<const long long*>(value);
		if (bFpuRegistersLittleEndian)
			//valueText = QString::number(sqword_values[0]) + ' ' + QString::number(sqword_values[1]);
			valueText.Format(_T("%ld %ld"), sqword_values[0], sqword_values[1]);
		else // Big Endian
			//valueText = QString::number(sqword_values[1]) + ' ' + QString::number(sqword_values[0]);
			valueText.Format(_T("%ld %ld"), sqword_values[1], sqword_values[0]);
	}
	break;
	case CRegistersView::SIMD_REG_DISP_QWORD_UNSIGNED:
	{
		const unsigned long long* uqword_values = reinterpret_cast<const unsigned long long*>(value);
		if (bFpuRegistersLittleEndian)
			//valueText = QString::number(uqword_values[0]) + ' ' + QString::number(uqword_values[1]);
			valueText.Format(_T("%lu %lu"), uqword_values[0], uqword_values[1]);
		else // Big Endian
			//valueText = QString::number(uqword_values[1]) + ' ' + QString::number(uqword_values[0]);
			valueText.Format(_T("%lu %lu"), uqword_values[1], uqword_values[0]);
	}
	break;
	}
	return std::move(valueText);
}

// CRegistersView

IMPLEMENT_DYNAMIC(CRegistersView, CWnd)

CRegistersView::CRegistersView()
{
	mShowFpu = false;
	mMaxLine = 0;
	mVScrollOffset = 0;
	yTopSpacing = 4;

	hScrollValue = 0;
	vScrollValue = 0;

	mCip = 0;

	memset(&wRegDumpStruct, 0, sizeof(REGDUMP));
	memset(&wCipRegDumpStruct, 0, sizeof(REGDUMP));
	initSIMDRegDispMode();
	registerRegisters();
	fontsUpdated();

}

CRegistersView::~CRegistersView()
{
}

int CRegistersView::getRowHeight() const
{
	return lf.lfHeight | 1;
}

void CRegistersView::fontsUpdated()
{
	lf = ConfigFont("Registers");
	/*
	setFont(font);
	if (mChangeViewButton)
		mChangeViewButton->setFont(font);
	//update metrics information
	int wRowsHeight = QFontMetrics(this->font()).height();
	wRowsHeight = (wRowsHeight * 105) / 100;
	wRowsHeight = (wRowsHeight % 2) == 0 ? wRowsHeight : wRowsHeight + 1;
	mRowHeight = wRowsHeight;
	mCharWidth = QFontMetrics(this->font()).averageCharWidth();
	//adjust the height of the area.
	setFixedHeight(getEstimateHeight());
	*/

	//reload layout because the layout is dependent on the font.
	InitMappings();
	if (this->GetSafeHwnd() != NULL)
		Invalidate();
}

void CRegistersView::ShowFPU(bool set_showfpu)
{
	mShowFpu = set_showfpu;
	InitMappings();
	initScorll();
	if (this->GetSafeHwnd() != NULL)
		Invalidate();
}

void CRegistersView::initSIMDRegDispMode()
{
	switch (ConfigUint("Gui", "SIMDRegistersDisplayMode"))
	{
	case 0:
		wSIMDRegDispMode = SIMD_REG_DISP_HEX;
		break;
	case 1:
		wSIMDRegDispMode = SIMD_REG_DISP_FLOAT;
		break;
	case 2:
		wSIMDRegDispMode = SIMD_REG_DISP_DOUBLE;
		break;
	case 3:
		wSIMDRegDispMode = SIMD_REG_DISP_WORD_SIGNED;
		break;
	case 4:
		wSIMDRegDispMode = SIMD_REG_DISP_DWORD_SIGNED;
		break;
	case 5:
		wSIMDRegDispMode = SIMD_REG_DISP_QWORD_SIGNED;
		break;
	case 6:
		wSIMDRegDispMode = SIMD_REG_DISP_WORD_UNSIGNED;
		break;
	case 7:
		wSIMDRegDispMode = SIMD_REG_DISP_DWORD_UNSIGNED;
		break;
	case 8:
		wSIMDRegDispMode = SIMD_REG_DISP_QWORD_UNSIGNED;
		break;
	case 9:
		wSIMDRegDispMode = SIMD_REG_DISP_WORD_HEX;
		break;
	case 10:
		wSIMDRegDispMode = SIMD_REG_DISP_DWORD_HEX;
		break;
	case 11:
		wSIMDRegDispMode = SIMD_REG_DISP_QWORD_HEX;
		break;
	}
}

void CRegistersView::registerRegisters()
{
	// general purposes register (we allow the user to modify the value)
	mGPR.insert(CAX);
	mCANSTOREADDRESS.insert(CAX);
	mUINTDISPLAY.insert(CAX);
	mLABELDISPLAY.insert(CAX);
	mMODIFYDISPLAY.insert(CAX);
	mUNDODISPLAY.insert(CAX);
	mINCREMENTDECREMET.insert(CAX);
	mSETONEZEROTOGGLE.insert(CAX);

	mSETONEZEROTOGGLE.insert(CBX);
	mINCREMENTDECREMET.insert(CBX);
	mGPR.insert(CBX);
	mUINTDISPLAY.insert(CBX);
	mLABELDISPLAY.insert(CBX);
	mMODIFYDISPLAY.insert(CBX);
	mUNDODISPLAY.insert(CBX);
	mCANSTOREADDRESS.insert(CBX);

	mSETONEZEROTOGGLE.insert(CCX);
	mINCREMENTDECREMET.insert(CCX);
	mGPR.insert(CCX);
	mUINTDISPLAY.insert(CCX);
	mLABELDISPLAY.insert(CCX);
	mMODIFYDISPLAY.insert(CCX);
	mUNDODISPLAY.insert(CCX);
	mCANSTOREADDRESS.insert(CCX);

	mSETONEZEROTOGGLE.insert(CDX);
	mINCREMENTDECREMET.insert(CDX);
	mGPR.insert(CDX);
	mUINTDISPLAY.insert(CDX);
	mLABELDISPLAY.insert(CDX);
	mMODIFYDISPLAY.insert(CDX);
	mUNDODISPLAY.insert(CDX);
	mCANSTOREADDRESS.insert(CDX);

	mSETONEZEROTOGGLE.insert(CBP);
	mINCREMENTDECREMET.insert(CBP);
	mCANSTOREADDRESS.insert(CBP);
	mGPR.insert(CBP);
	mUINTDISPLAY.insert(CBP);
	mLABELDISPLAY.insert(CBP);
	mMODIFYDISPLAY.insert(CBP);
	mUNDODISPLAY.insert(CBP);

	mSETONEZEROTOGGLE.insert(CSP);
	mINCREMENTDECREMET.insert(CSP);
	mCANSTOREADDRESS.insert(CSP);
	mGPR.insert(CSP);
	mUINTDISPLAY.insert(CSP);
	mLABELDISPLAY.insert(CSP);
	mMODIFYDISPLAY.insert(CSP);
	mUNDODISPLAY.insert(CSP);

	mSETONEZEROTOGGLE.insert(CSI);
	mINCREMENTDECREMET.insert(CSI);
	mCANSTOREADDRESS.insert(CSI);
	mGPR.insert(CSI);
	mUINTDISPLAY.insert(CSI);
	mLABELDISPLAY.insert(CSI);
	mMODIFYDISPLAY.insert(CSI);
	mUNDODISPLAY.insert(CSI);

	mSETONEZEROTOGGLE.insert(CDI);
	mINCREMENTDECREMET.insert(CDI);
	mCANSTOREADDRESS.insert(CDI);
	mGPR.insert(CDI);
	mUINTDISPLAY.insert(CDI);
	mLABELDISPLAY.insert(CDI);
	mMODIFYDISPLAY.insert(CDI);
	mUNDODISPLAY.insert(CDI);
#ifdef _WIN64
	mSETONEZEROTOGGLE.insert(R8);
	mINCREMENTDECREMET.insert(R8);
	mCANSTOREADDRESS.insert(R8);
	mGPR.insert(R8);
	mLABELDISPLAY.insert(R8);
	mUINTDISPLAY.insert(R8);
	mMODIFYDISPLAY.insert(R8);
	mUNDODISPLAY.insert(R8);

	mSETONEZEROTOGGLE.insert(R9);
	mINCREMENTDECREMET.insert(R9);
	mCANSTOREADDRESS.insert(R9);
	mGPR.insert(R9);
	mLABELDISPLAY.insert(R9);
	mUINTDISPLAY.insert(R9);
	mMODIFYDISPLAY.insert(R9);
	mUNDODISPLAY.insert(R9);

	mSETONEZEROTOGGLE.insert(R10);
	mINCREMENTDECREMET.insert(R10);
	mCANSTOREADDRESS.insert(R10);
	mGPR.insert(R10);
	mMODIFYDISPLAY.insert(R10);
	mUNDODISPLAY.insert(R10);
	mUINTDISPLAY.insert(R10);
	mLABELDISPLAY.insert(R10);

	mSETONEZEROTOGGLE.insert(R11);
	mINCREMENTDECREMET.insert(R11);
	mCANSTOREADDRESS.insert(R11);
	mGPR.insert(R11);
	mMODIFYDISPLAY.insert(R11);
	mUNDODISPLAY.insert(R11);
	mUINTDISPLAY.insert(R11);
	mLABELDISPLAY.insert(R11);

	mSETONEZEROTOGGLE.insert(R12);
	mINCREMENTDECREMET.insert(R12);
	mCANSTOREADDRESS.insert(R12);
	mGPR.insert(R12);
	mMODIFYDISPLAY.insert(R12);
	mUNDODISPLAY.insert(R12);
	mUINTDISPLAY.insert(R12);
	mLABELDISPLAY.insert(R12);

	mSETONEZEROTOGGLE.insert(R13);
	mINCREMENTDECREMET.insert(R13);
	mCANSTOREADDRESS.insert(R13);
	mGPR.insert(R13);
	mMODIFYDISPLAY.insert(R13);
	mUNDODISPLAY.insert(R13);
	mUINTDISPLAY.insert(R13);
	mLABELDISPLAY.insert(R13);

	mSETONEZEROTOGGLE.insert(R14);
	mINCREMENTDECREMET.insert(R14);
	mCANSTOREADDRESS.insert(R14);
	mGPR.insert(R14);
	mMODIFYDISPLAY.insert(R14);
	mUNDODISPLAY.insert(R14);
	mUINTDISPLAY.insert(R14);
	mLABELDISPLAY.insert(R14);

	mSETONEZEROTOGGLE.insert(R15);
	mINCREMENTDECREMET.insert(R15);
	mCANSTOREADDRESS.insert(R15);
	mGPR.insert(R15);
	mMODIFYDISPLAY.insert(R15);
	mUNDODISPLAY.insert(R15);
	mUINTDISPLAY.insert(R15);
	mLABELDISPLAY.insert(R15);
#endif //_WIN64

	mSETONEZEROTOGGLE.insert(EFLAGS);
	mGPR.insert(EFLAGS);
	mMODIFYDISPLAY.insert(EFLAGS);
	mUNDODISPLAY.insert(EFLAGS);
	mUINTDISPLAY.insert(EFLAGS);

	// flags (we allow the user to toggle them)
	mFlags.insert(CF);
	mBOOLDISPLAY.insert(CF);
	mSETONEZEROTOGGLE.insert(CF);

	mSETONEZEROTOGGLE.insert(PF);
	mFlags.insert(PF);
	mBOOLDISPLAY.insert(PF);

	mSETONEZEROTOGGLE.insert(AF);
	mFlags.insert(AF);
	mBOOLDISPLAY.insert(AF);

	mSETONEZEROTOGGLE.insert(ZF);
	mFlags.insert(ZF);
	mBOOLDISPLAY.insert(ZF);

	mSETONEZEROTOGGLE.insert(SF);
	mFlags.insert(SF);
	mBOOLDISPLAY.insert(SF);

	mSETONEZEROTOGGLE.insert(TF);
	mFlags.insert(TF);
	mBOOLDISPLAY.insert(TF);

	mFlags.insert(IF);
	mBOOLDISPLAY.insert(IF);

	mSETONEZEROTOGGLE.insert(DF);
	mFlags.insert(DF);
	mBOOLDISPLAY.insert(DF);

	mSETONEZEROTOGGLE.insert(OF);
	mFlags.insert(OF);
	mBOOLDISPLAY.insert(OF);

	// FPU: XMM, x87 and MMX registers
	mSETONEZEROTOGGLE.insert(MxCsr);
	mDWORDDISPLAY.insert(MxCsr);
	mMODIFYDISPLAY.insert(MxCsr);
	mUNDODISPLAY.insert(MxCsr);
	mFPU.insert(MxCsr);

	mMODIFYDISPLAY.insert(x87r0);
	mUNDODISPLAY.insert(x87r0);
	mFPUx87.insert(x87r0);
	mFPUx87_80BITSDISPLAY.insert(x87r0);
	mFPU.insert(x87r0);

	mMODIFYDISPLAY.insert(x87r1);
	mUNDODISPLAY.insert(x87r1);
	mFPUx87.insert(x87r1);
	mFPUx87_80BITSDISPLAY.insert(x87r1);
	mFPU.insert(x87r1);

	mMODIFYDISPLAY.insert(x87r2);
	mUNDODISPLAY.insert(x87r2);
	mFPUx87.insert(x87r2);
	mFPUx87_80BITSDISPLAY.insert(x87r2);
	mFPU.insert(x87r2);

	mMODIFYDISPLAY.insert(x87r3);
	mUNDODISPLAY.insert(x87r3);
	mFPUx87.insert(x87r3);
	mFPUx87_80BITSDISPLAY.insert(x87r3);
	mFPU.insert(x87r3);

	mMODIFYDISPLAY.insert(x87r4);
	mUNDODISPLAY.insert(x87r4);
	mFPUx87.insert(x87r4);
	mFPUx87_80BITSDISPLAY.insert(x87r4);
	mFPU.insert(x87r4);

	mMODIFYDISPLAY.insert(x87r5);
	mUNDODISPLAY.insert(x87r5);
	mFPUx87.insert(x87r5);
	mFPU.insert(x87r5);
	mFPUx87_80BITSDISPLAY.insert(x87r5);

	mMODIFYDISPLAY.insert(x87r6);
	mUNDODISPLAY.insert(x87r6);
	mFPUx87.insert(x87r6);
	mFPU.insert(x87r6);
	mFPUx87_80BITSDISPLAY.insert(x87r6);

	mMODIFYDISPLAY.insert(x87r7);
	mUNDODISPLAY.insert(x87r7);
	mFPUx87.insert(x87r7);
	mFPU.insert(x87r7);
	mFPUx87_80BITSDISPLAY.insert(x87r7);

	mSETONEZEROTOGGLE.insert(x87TagWord);
	mFPUx87.insert(x87TagWord);
	mMODIFYDISPLAY.insert(x87TagWord);
	mUNDODISPLAY.insert(x87TagWord);
	mUSHORTDISPLAY.insert(x87TagWord);
	mFPU.insert(x87TagWord);

	mSETONEZEROTOGGLE.insert(x87StatusWord);
	mUSHORTDISPLAY.insert(x87StatusWord);
	mMODIFYDISPLAY.insert(x87StatusWord);
	mUNDODISPLAY.insert(x87StatusWord);
	mFPUx87.insert(x87StatusWord);
	mFPU.insert(x87StatusWord);

	mSETONEZEROTOGGLE.insert(x87ControlWord);
	mFPUx87.insert(x87ControlWord);
	mMODIFYDISPLAY.insert(x87ControlWord);
	mUNDODISPLAY.insert(x87ControlWord);
	mUSHORTDISPLAY.insert(x87ControlWord);
	mFPU.insert(x87ControlWord);

	mSETONEZEROTOGGLE.insert(x87SW_B);
	mFPUx87.insert(x87SW_B);
	mBOOLDISPLAY.insert(x87SW_B);
	mFPU.insert(x87SW_B);

	mSETONEZEROTOGGLE.insert(x87SW_C3);
	mFPUx87.insert(x87SW_C3);
	mBOOLDISPLAY.insert(x87SW_C3);
	mFPU.insert(x87SW_C3);

	mFPUx87.insert(x87SW_TOP);
	mFIELDVALUE.insert(x87SW_TOP);
	mFPU.insert(x87SW_TOP);
	mMODIFYDISPLAY.insert(x87SW_TOP);
	mUNDODISPLAY.insert(x87SW_TOP);

	mFPUx87.insert(x87SW_C2);
	mBOOLDISPLAY.insert(x87SW_C2);
	mSETONEZEROTOGGLE.insert(x87SW_C2);
	mFPU.insert(x87SW_C2);

	mSETONEZEROTOGGLE.insert(x87SW_C1);
	mFPUx87.insert(x87SW_C1);
	mBOOLDISPLAY.insert(x87SW_C1);
	mFPU.insert(x87SW_C1);

	mSETONEZEROTOGGLE.insert(x87SW_C0);
	mFPUx87.insert(x87SW_C0);
	mBOOLDISPLAY.insert(x87SW_C0);
	mFPU.insert(x87SW_C0);

	mSETONEZEROTOGGLE.insert(x87SW_ES);
	mFPUx87.insert(x87SW_ES);
	mBOOLDISPLAY.insert(x87SW_ES);
	mFPU.insert(x87SW_ES);

	mSETONEZEROTOGGLE.insert(x87SW_SF);
	mFPUx87.insert(x87SW_SF);
	mBOOLDISPLAY.insert(x87SW_SF);
	mFPU.insert(x87SW_SF);

	mSETONEZEROTOGGLE.insert(x87SW_P);
	mFPUx87.insert(x87SW_P);
	mBOOLDISPLAY.insert(x87SW_P);
	mFPU.insert(x87SW_P);

	mSETONEZEROTOGGLE.insert(x87SW_U);
	mFPUx87.insert(x87SW_U);
	mBOOLDISPLAY.insert(x87SW_U);
	mFPU.insert(x87SW_U);

	mSETONEZEROTOGGLE.insert(x87SW_O);
	mFPUx87.insert(x87SW_O);
	mBOOLDISPLAY.insert(x87SW_O);
	mFPU.insert(x87SW_O);

	mSETONEZEROTOGGLE.insert(x87SW_Z);
	mFPUx87.insert(x87SW_Z);
	mBOOLDISPLAY.insert(x87SW_Z);
	mFPU.insert(x87SW_Z);

	mSETONEZEROTOGGLE.insert(x87SW_D);
	mFPUx87.insert(x87SW_D);
	mBOOLDISPLAY.insert(x87SW_D);
	mFPU.insert(x87SW_D);

	mSETONEZEROTOGGLE.insert(x87SW_I);
	mFPUx87.insert(x87SW_I);
	mBOOLDISPLAY.insert(x87SW_I);
	mFPU.insert(x87SW_I);

	mSETONEZEROTOGGLE.insert(x87CW_IC);
	mFPUx87.insert(x87CW_IC);
	mBOOLDISPLAY.insert(x87CW_IC);
	mFPU.insert(x87CW_IC);

	mFPUx87.insert(x87CW_RC);
	mFIELDVALUE.insert(x87CW_RC);
	mFPU.insert(x87CW_RC);
	mMODIFYDISPLAY.insert(x87CW_RC);
	mUNDODISPLAY.insert(x87CW_RC);

	mFPUx87.insert(x87TW_0);
	mFIELDVALUE.insert(x87TW_0);
	mTAGWORD.insert(x87TW_0);
	mFPU.insert(x87TW_0);
	mMODIFYDISPLAY.insert(x87TW_0);
	mUNDODISPLAY.insert(x87TW_0);

	mFPUx87.insert(x87TW_1);
	mFIELDVALUE.insert(x87TW_1);
	mTAGWORD.insert(x87TW_1);
	mFPU.insert(x87TW_1);
	mMODIFYDISPLAY.insert(x87TW_1);
	mUNDODISPLAY.insert(x87TW_1);

	mFPUx87.insert(x87TW_2);
	mFIELDVALUE.insert(x87TW_2);
	mTAGWORD.insert(x87TW_2);
	mFPU.insert(x87TW_2);
	mMODIFYDISPLAY.insert(x87TW_2);
	mUNDODISPLAY.insert(x87TW_2);

	mFPUx87.insert(x87TW_3);
	mFIELDVALUE.insert(x87TW_3);
	mTAGWORD.insert(x87TW_3);
	mFPU.insert(x87TW_3);
	mMODIFYDISPLAY.insert(x87TW_3);
	mUNDODISPLAY.insert(x87TW_3);

	mFPUx87.insert(x87TW_4);
	mFIELDVALUE.insert(x87TW_4);
	mTAGWORD.insert(x87TW_4);
	mFPU.insert(x87TW_4);
	mMODIFYDISPLAY.insert(x87TW_4);
	mUNDODISPLAY.insert(x87TW_4);

	mFPUx87.insert(x87TW_5);
	mFIELDVALUE.insert(x87TW_5);
	mTAGWORD.insert(x87TW_5);
	mFPU.insert(x87TW_5);
	mMODIFYDISPLAY.insert(x87TW_5);
	mUNDODISPLAY.insert(x87TW_5);

	mFPUx87.insert(x87TW_6);
	mFIELDVALUE.insert(x87TW_6);
	mTAGWORD.insert(x87TW_6);
	mFPU.insert(x87TW_6);
	mMODIFYDISPLAY.insert(x87TW_6);
	mUNDODISPLAY.insert(x87TW_6);

	mFPUx87.insert(x87TW_7);
	mFIELDVALUE.insert(x87TW_7);
	mTAGWORD.insert(x87TW_7);
	mFPU.insert(x87TW_7);
	mMODIFYDISPLAY.insert(x87TW_7);
	mUNDODISPLAY.insert(x87TW_7);

	mFPUx87.insert(x87CW_PC);
	mFIELDVALUE.insert(x87CW_PC);
	mFPU.insert(x87CW_PC);
	mMODIFYDISPLAY.insert(x87CW_PC);
	mUNDODISPLAY.insert(x87CW_PC);

	mSETONEZEROTOGGLE.insert(x87CW_PM);
	mFPUx87.insert(x87CW_PM);
	mBOOLDISPLAY.insert(x87CW_PM);
	mFPU.insert(x87CW_PM);

	mSETONEZEROTOGGLE.insert(x87CW_UM);
	mFPUx87.insert(x87CW_UM);
	mBOOLDISPLAY.insert(x87CW_UM);
	mFPU.insert(x87CW_UM);

	mSETONEZEROTOGGLE.insert(x87CW_OM);
	mFPUx87.insert(x87CW_OM);
	mBOOLDISPLAY.insert(x87CW_OM);
	mFPU.insert(x87CW_OM);

	mSETONEZEROTOGGLE.insert(x87CW_ZM);
	mFPUx87.insert(x87CW_ZM);
	mBOOLDISPLAY.insert(x87CW_ZM);
	mFPU.insert(x87CW_ZM);

	mSETONEZEROTOGGLE.insert(x87CW_DM);
	mFPUx87.insert(x87CW_DM);
	mBOOLDISPLAY.insert(x87CW_DM);
	mFPU.insert(x87CW_DM);

	mSETONEZEROTOGGLE.insert(x87CW_IM);
	mFPUx87.insert(x87CW_IM);
	mBOOLDISPLAY.insert(x87CW_IM);
	mFPU.insert(x87CW_IM);

	mSETONEZEROTOGGLE.insert(MxCsr_FZ);
	mBOOLDISPLAY.insert(MxCsr_FZ);
	mFPU.insert(MxCsr_FZ);

	mSETONEZEROTOGGLE.insert(MxCsr_PM);
	mBOOLDISPLAY.insert(MxCsr_PM);
	mFPU.insert(MxCsr_PM);

	mSETONEZEROTOGGLE.insert(MxCsr_UM);
	mBOOLDISPLAY.insert(MxCsr_UM);
	mFPU.insert(MxCsr_UM);

	mSETONEZEROTOGGLE.insert(MxCsr_OM);
	mBOOLDISPLAY.insert(MxCsr_OM);
	mFPU.insert(MxCsr_OM);

	mSETONEZEROTOGGLE.insert(MxCsr_ZM);
	mBOOLDISPLAY.insert(MxCsr_ZM);
	mFPU.insert(MxCsr_ZM);

	mSETONEZEROTOGGLE.insert(MxCsr_IM);
	mBOOLDISPLAY.insert(MxCsr_IM);
	mFPU.insert(MxCsr_IM);

	mSETONEZEROTOGGLE.insert(MxCsr_DM);
	mBOOLDISPLAY.insert(MxCsr_DM);
	mFPU.insert(MxCsr_DM);

	mSETONEZEROTOGGLE.insert(MxCsr_DAZ);
	mBOOLDISPLAY.insert(MxCsr_DAZ);
	mFPU.insert(MxCsr_DAZ);

	mSETONEZEROTOGGLE.insert(MxCsr_PE);
	mBOOLDISPLAY.insert(MxCsr_PE);
	mFPU.insert(MxCsr_PE);

	mSETONEZEROTOGGLE.insert(MxCsr_UE);
	mBOOLDISPLAY.insert(MxCsr_UE);
	mFPU.insert(MxCsr_UE);

	mSETONEZEROTOGGLE.insert(MxCsr_OE);
	mBOOLDISPLAY.insert(MxCsr_OE);
	mFPU.insert(MxCsr_OE);

	mSETONEZEROTOGGLE.insert(MxCsr_ZE);
	mBOOLDISPLAY.insert(MxCsr_ZE);
	mFPU.insert(MxCsr_ZE);

	mSETONEZEROTOGGLE.insert(MxCsr_DE);
	mBOOLDISPLAY.insert(MxCsr_DE);
	mFPU.insert(MxCsr_DE);

	mSETONEZEROTOGGLE.insert(MxCsr_IE);
	mBOOLDISPLAY.insert(MxCsr_IE);
	mFPU.insert(MxCsr_IE);

	mFIELDVALUE.insert(MxCsr_RC);
	mFPU.insert(MxCsr_RC);
	mMODIFYDISPLAY.insert(MxCsr_RC);
	mUNDODISPLAY.insert(MxCsr_RC);

	mMODIFYDISPLAY.insert(MM0);
	mUNDODISPLAY.insert(MM0);
	mFPUMMX.insert(MM0);
	mFPU.insert(MM0);
	mMODIFYDISPLAY.insert(MM1);
	mUNDODISPLAY.insert(MM1);
	mFPUMMX.insert(MM1);
	mFPU.insert(MM1);
	mFPUMMX.insert(MM2);
	mMODIFYDISPLAY.insert(MM2);
	mUNDODISPLAY.insert(MM2);
	mFPU.insert(MM2);
	mFPUMMX.insert(MM3);
	mMODIFYDISPLAY.insert(MM3);
	mUNDODISPLAY.insert(MM3);
	mFPU.insert(MM3);
	mFPUMMX.insert(MM4);
	mMODIFYDISPLAY.insert(MM4);
	mUNDODISPLAY.insert(MM4);
	mFPU.insert(MM4);
	mFPUMMX.insert(MM5);
	mMODIFYDISPLAY.insert(MM5);
	mUNDODISPLAY.insert(MM5);
	mFPU.insert(MM5);
	mFPUMMX.insert(MM6);
	mMODIFYDISPLAY.insert(MM6);
	mUNDODISPLAY.insert(MM6);
	mFPU.insert(MM6);
	mFPUMMX.insert(MM7);
	mMODIFYDISPLAY.insert(MM7);
	mUNDODISPLAY.insert(MM7);
	mFPU.insert(MM7);

	mFPUXMM.insert(XMM0);
	mMODIFYDISPLAY.insert(XMM0);
	mUNDODISPLAY.insert(XMM0);
	mFPU.insert(XMM0);
	mFPUXMM.insert(XMM1);
	mMODIFYDISPLAY.insert(XMM1);
	mUNDODISPLAY.insert(XMM1);
	mFPU.insert(XMM1);
	mFPUXMM.insert(XMM2);
	mMODIFYDISPLAY.insert(XMM2);
	mUNDODISPLAY.insert(XMM2);
	mFPU.insert(XMM2);
	mFPUXMM.insert(XMM3);
	mMODIFYDISPLAY.insert(XMM3);
	mUNDODISPLAY.insert(XMM3);
	mFPU.insert(XMM3);
	mFPUXMM.insert(XMM4);
	mMODIFYDISPLAY.insert(XMM4);
	mUNDODISPLAY.insert(XMM4);
	mFPU.insert(XMM4);
	mFPUXMM.insert(XMM5);
	mMODIFYDISPLAY.insert(XMM5);
	mUNDODISPLAY.insert(XMM5);
	mFPU.insert(XMM5);
	mFPUXMM.insert(XMM6);
	mMODIFYDISPLAY.insert(XMM6);
	mUNDODISPLAY.insert(XMM6);
	mFPU.insert(XMM6);
	mFPUXMM.insert(XMM7);
	mMODIFYDISPLAY.insert(XMM7);
	mUNDODISPLAY.insert(XMM7);
	mFPU.insert(XMM7);
#ifdef _WIN64
	mFPUXMM.insert(XMM8);
	mMODIFYDISPLAY.insert(XMM8);
	mUNDODISPLAY.insert(XMM8);
	mFPU.insert(XMM8);
	mFPUXMM.insert(XMM9);
	mMODIFYDISPLAY.insert(XMM9);
	mUNDODISPLAY.insert(XMM9);
	mFPU.insert(XMM9);
	mFPUXMM.insert(XMM10);
	mMODIFYDISPLAY.insert(XMM10);
	mUNDODISPLAY.insert(XMM10);
	mFPU.insert(XMM10);
	mFPUXMM.insert(XMM11);
	mMODIFYDISPLAY.insert(XMM11);
	mUNDODISPLAY.insert(XMM11);
	mFPU.insert(XMM11);
	mFPUXMM.insert(XMM12);
	mMODIFYDISPLAY.insert(XMM12);
	mUNDODISPLAY.insert(XMM12);
	mFPU.insert(XMM12);
	mFPUXMM.insert(XMM13);
	mMODIFYDISPLAY.insert(XMM13);
	mUNDODISPLAY.insert(XMM13);
	mFPU.insert(XMM13);
	mFPUXMM.insert(XMM14);
	mMODIFYDISPLAY.insert(XMM14);
	mUNDODISPLAY.insert(XMM14);
	mFPU.insert(XMM14);
	mFPUXMM.insert(XMM15);
	mMODIFYDISPLAY.insert(XMM15);
	mUNDODISPLAY.insert(XMM15);
	mFPU.insert(XMM15);
#endif

	mFPUYMM.insert(YMM0);
	mMODIFYDISPLAY.insert(YMM0);
	mUNDODISPLAY.insert(YMM0);
	mFPU.insert(YMM0);
	mFPUYMM.insert(YMM1);
	mMODIFYDISPLAY.insert(YMM1);
	mUNDODISPLAY.insert(YMM1);
	mFPU.insert(YMM1);
	mFPUYMM.insert(YMM2);
	mMODIFYDISPLAY.insert(YMM2);
	mUNDODISPLAY.insert(YMM2);
	mFPU.insert(YMM2);
	mFPUYMM.insert(YMM3);
	mMODIFYDISPLAY.insert(YMM3);
	mUNDODISPLAY.insert(YMM3);
	mFPU.insert(YMM3);
	mFPUYMM.insert(YMM4);
	mMODIFYDISPLAY.insert(YMM4);
	mUNDODISPLAY.insert(YMM4);
	mFPU.insert(YMM4);
	mFPUYMM.insert(YMM5);
	mMODIFYDISPLAY.insert(YMM5);
	mUNDODISPLAY.insert(YMM5);
	mFPU.insert(YMM5);
	mFPUYMM.insert(YMM6);
	mMODIFYDISPLAY.insert(YMM6);
	mUNDODISPLAY.insert(YMM6);
	mFPU.insert(YMM6);
	mFPUYMM.insert(YMM7);
	mMODIFYDISPLAY.insert(YMM7);
	mUNDODISPLAY.insert(YMM7);
	mFPU.insert(YMM7);

#ifdef _WIN64
	mFPUYMM.insert(YMM8);
	mMODIFYDISPLAY.insert(YMM8);
	mUNDODISPLAY.insert(YMM8);
	mFPU.insert(YMM8);
	mFPUYMM.insert(YMM9);
	mMODIFYDISPLAY.insert(YMM9);
	mUNDODISPLAY.insert(YMM9);
	mFPU.insert(YMM9);
	mFPUYMM.insert(YMM10);
	mMODIFYDISPLAY.insert(YMM10);
	mUNDODISPLAY.insert(YMM10);
	mFPU.insert(YMM10);
	mFPUYMM.insert(YMM11);
	mMODIFYDISPLAY.insert(YMM11);
	mUNDODISPLAY.insert(YMM11);
	mFPU.insert(YMM11);
	mFPUYMM.insert(YMM12);
	mMODIFYDISPLAY.insert(YMM12);
	mUNDODISPLAY.insert(YMM12);
	mFPU.insert(YMM12);
	mFPUYMM.insert(YMM13);
	mMODIFYDISPLAY.insert(YMM13);
	mUNDODISPLAY.insert(YMM13);
	mFPU.insert(YMM13);
	mFPUYMM.insert(YMM14);
	mMODIFYDISPLAY.insert(YMM14);
	mUNDODISPLAY.insert(YMM14);
	mFPU.insert(YMM14);
	mFPUYMM.insert(YMM15);
	mMODIFYDISPLAY.insert(YMM15);
	mUNDODISPLAY.insert(YMM15);
	mFPU.insert(YMM15);
#endif
	//registers that should not be changed
	mNoChange.insert(LastError);
	mNoChange.insert(LastStatus);

	mNoChange.insert(GS);
	mUSHORTDISPLAY.insert(GS);
	mSEGMENTREGISTER.insert(GS);

	mNoChange.insert(FS);
	mUSHORTDISPLAY.insert(FS);
	mSEGMENTREGISTER.insert(FS);

	mNoChange.insert(ES);
	mUSHORTDISPLAY.insert(ES);
	mSEGMENTREGISTER.insert(ES);

	mNoChange.insert(DS);
	mUSHORTDISPLAY.insert(DS);
	mSEGMENTREGISTER.insert(DS);

	mNoChange.insert(CS);
	mUSHORTDISPLAY.insert(CS);
	mSEGMENTREGISTER.insert(CS);

	mNoChange.insert(SS);
	mUSHORTDISPLAY.insert(SS);
	mSEGMENTREGISTER.insert(SS);

	mNoChange.insert(DR0);
	mUINTDISPLAY.insert(DR0);
	mLABELDISPLAY.insert(DR0);
	mONLYMODULEANDLABELDISPLAY.insert(DR0);
	mCANSTOREADDRESS.insert(DR0);

	mNoChange.insert(DR1);
	mONLYMODULEANDLABELDISPLAY.insert(DR1);
	mUINTDISPLAY.insert(DR1);
	mCANSTOREADDRESS.insert(DR1);

	mLABELDISPLAY.insert(DR2);
	mONLYMODULEANDLABELDISPLAY.insert(DR2);
	mNoChange.insert(DR2);
	mUINTDISPLAY.insert(DR2);
	mCANSTOREADDRESS.insert(DR2);

	mNoChange.insert(DR3);
	mONLYMODULEANDLABELDISPLAY.insert(DR3);
	mLABELDISPLAY.insert(DR3);
	mUINTDISPLAY.insert(DR3);
	mCANSTOREADDRESS.insert(DR3);

	mNoChange.insert(DR6);
	mUINTDISPLAY.insert(DR6);

	mNoChange.insert(DR7);
	mUINTDISPLAY.insert(DR7);

	mNoChange.insert(CIP);
	mUINTDISPLAY.insert(CIP);
	mLABELDISPLAY.insert(CIP);
	mONLYMODULEANDLABELDISPLAY.insert(CIP);
	mCANSTOREADDRESS.insert(CIP);
	mMODIFYDISPLAY.insert(CIP);
	mUNDODISPLAY.insert(CIP);
}

void CRegistersView::InitMappings()
{
	// create mapping from internal id to name
	mRegisterMapping.clear();
	mRegisterPlaces.clear();
	int offset = 0;

	/* Register_Position is a struct definition the position
	*
	* (line , start, labelwidth, valuesize )
	*/
#ifdef _WIN64
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(CAX, "RAX"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(CAX, Register_Position(offset++, 0, 6, sizeof(duint) * 2)));
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(CBX, "RBX"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(CBX, Register_Position(offset++, 0, 6, sizeof(duint) * 2)));
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(CCX, "RCX"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(CCX, Register_Position(offset++, 0, 6, sizeof(duint) * 2)));
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(CDX, "RDX"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(CDX, Register_Position(offset++, 0, 6, sizeof(duint) * 2)));
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(CBP, "RBP"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(CBP, Register_Position(offset++, 0, 6, sizeof(duint) * 2)));
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(CSP, "RSP"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(CSP, Register_Position(offset++, 0, 6, sizeof(duint) * 2)));
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(CSI, "RSI"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(CSI, Register_Position(offset++, 0, 6, sizeof(duint) * 2)));
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(CDI, "RDI"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(CDI, Register_Position(offset++, 0, 6, sizeof(duint) * 2)));

	offset++;

	mRegisterMapping.insert(pair<REGISTER_NAME, string>(R8, "R8"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(R8, Register_Position(offset++, 0, 6, sizeof(duint) * 2)));
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(R9, "R9"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(R9, Register_Position(offset++, 0, 6, sizeof(duint) * 2)));
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(R10, "R10"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(R10, Register_Position(offset++, 0, 6, sizeof(duint) * 2)));
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(R11, "R11"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(R11, Register_Position(offset++, 0, 6, sizeof(duint) * 2)));
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(R12, "R12"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(R12, Register_Position(offset++, 0, 6, sizeof(duint) * 2)));
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(R13, "R13"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(R13, Register_Position(offset++, 0, 6, sizeof(duint) * 2)));
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(R14, "R14"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(R14, Register_Position(offset++, 0, 6, sizeof(duint) * 2)));
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(R15, "R15"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(R15, Register_Position(offset++, 0, 6, sizeof(duint) * 2)));

	offset++;

	mRegisterMapping.insert(pair<REGISTER_NAME, string>(CIP, "RIP"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(CIP, Register_Position(offset++, 0, 6, sizeof(duint) * 2)));

	offset++;

	mRegisterMapping.insert(pair<REGISTER_NAME, string>(EFLAGS, "RFLAGS"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(EFLAGS, Register_Position(offset++, 0, 9, sizeof(duint) * 2)));
#else //x32
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(CAX, "EAX"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(CAX, Register_Position(offset++, 0, 6, sizeof(duint) * 2)));
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(CBX, "EBX"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(CBX, Register_Position(offset++, 0, 6, sizeof(duint) * 2)));
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(CCX, "ECX"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(CCX, Register_Position(offset++, 0, 6, sizeof(duint) * 2)));
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(CDX, "EDX"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(CDX, Register_Position(offset++, 0, 6, sizeof(duint) * 2)));
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(CBP, "EBP"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(CBP, Register_Position(offset++, 0, 6, sizeof(duint) * 2)));
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(CSP, "ESP"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(CSP, Register_Position(offset++, 0, 6, sizeof(duint) * 2)));
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(CSI, "ESI"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(CSI, Register_Position(offset++, 0, 6, sizeof(duint) * 2)));
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(CDI, "EDI"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(CDI, Register_Position(offset++, 0, 6, sizeof(duint) * 2)));

	offset++;

	mRegisterMapping.insert(pair<REGISTER_NAME, string>(CIP, "EIP"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(CIP, Register_Position(offset++, 0, 6, sizeof(duint) * 2)));

	offset++;

	mRegisterMapping.insert(pair<REGISTER_NAME, string>(EFLAGS, "EFLAGS"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(EFLAGS, Register_Position(offset++, 0, 9, sizeof(duint) * 2)));
#endif

	mRegisterMapping.insert(pair<REGISTER_NAME, string>(ZF, "ZF"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(ZF, Register_Position(offset, 0, 3, 1)));
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(PF, "PF"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(PF, Register_Position(offset, 6, 3, 1)));
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(AF, "AF"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(AF, Register_Position(offset++, 12, 3, 1)));

	mRegisterMapping.insert(pair<REGISTER_NAME, string>(OF, "OF"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(OF, Register_Position(offset, 0, 3, 1)));
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(SF, "SF"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(SF, Register_Position(offset, 6, 3, 1)));
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(DF, "DF"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(DF, Register_Position(offset++, 12, 3, 1)));

	mRegisterMapping.insert(pair<REGISTER_NAME, string>(CF, "CF"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(CF, Register_Position(offset, 0, 3, 1)));
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(TF, "TF"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(TF, Register_Position(offset, 6, 3, 1)));
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(IF, "IF"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(IF, Register_Position(offset++, 12, 3, 1)));

	offset++;

	mRegisterMapping.insert(pair<REGISTER_NAME, string>(LastError, "LastError"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(LastError, Register_Position(offset++, 0, 11, 20)));
	mMODIFYDISPLAY.insert(LastError);
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(LastStatus, "LastStatus"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(LastStatus, Register_Position(offset++, 0, 11, 20)));
	mMODIFYDISPLAY.insert(LastStatus);

	offset++;

	mRegisterMapping.insert(pair<REGISTER_NAME, string>(GS, "GS"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(GS, Register_Position(offset, 0, 3, 4)));
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(FS, "FS"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(FS, Register_Position(offset++, 9, 3, 4)));
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(ES, "ES"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(ES, Register_Position(offset, 0, 3, 4)));
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(DS, "DS"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(DS, Register_Position(offset++, 9, 3, 4)));
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(CS, "CS"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(CS, Register_Position(offset, 0, 3, 4)));
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(SS, "SS"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(SS, Register_Position(offset++, 9, 3, 4)));

	if (mShowFpu)
	{
		offset++;

		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87r0, "x87r0"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87r0, Register_Position(offset++, 0, 6, 10 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87r1, "x87r1"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87r1, Register_Position(offset++, 0, 6, 10 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87r2, "x87r2"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87r2, Register_Position(offset++, 0, 6, 10 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87r3, "x87r3"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87r3, Register_Position(offset++, 0, 6, 10 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87r4, "x87r4"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87r4, Register_Position(offset++, 0, 6, 10 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87r5, "x87r5"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87r5, Register_Position(offset++, 0, 6, 10 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87r6, "x87r6"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87r6, Register_Position(offset++, 0, 6, 10 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87r7, "x87r7"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87r7, Register_Position(offset++, 0, 6, 10 * 2)));

		offset++;

		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87TagWord, "x87TagWord"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87TagWord, Register_Position(offset++, 0, 11, sizeof(WORD) * 2)));
		//Special treatment of long internationalized string
		int NextColumnPosition = 20;
		
		
		//13 = 20 - strlen("Nonzero")
		/*
		int temp;
		temp = metrics.width(QApplication::translate("RegistersView_ConstantsOfRegisters", "Nonzero")) / mCharWidth + 13;
		NextColumnPosition = max(NextColumnPosition, temp);
		temp = metrics.width(QApplication::translate("RegistersView_ConstantsOfRegisters", "Zero")) / mCharWidth + 13;
		NextColumnPosition = max(NextColumnPosition, temp);
		temp = metrics.width(QApplication::translate("RegistersView_ConstantsOfRegisters", "Special")) / mCharWidth + 13;
		NextColumnPosition = max(NextColumnPosition, temp);
		temp = metrics.width(QApplication::translate("RegistersView_ConstantsOfRegisters", "Empty")) / mCharWidth + 13;
		NextColumnPosition = std::max(NextColumnPosition, temp));
		*/
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87TW_0, "x87TW_0"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87TW_0, Register_Position(offset, 0, 8, 10)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87TW_1, "x87TW_1"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87TW_1, Register_Position(offset++, NextColumnPosition, 8, 10)));

		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87TW_2, "x87TW_2"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87TW_2, Register_Position(offset, 0, 8, 10)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87TW_3, "x87TW_3"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87TW_3, Register_Position(offset++, NextColumnPosition, 8, 10)));

		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87TW_4, "x87TW_4"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87TW_4, Register_Position(offset, 0, 8, 10)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87TW_5, "x87TW_5"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87TW_5, Register_Position(offset++, NextColumnPosition, 8, 10)));

		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87TW_6, "x87TW_6"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87TW_6, Register_Position(offset, 0, 8, 10)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87TW_7, "x87TW_7"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87TW_7, Register_Position(offset++, NextColumnPosition, 8, 10)));

		offset++;

		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87StatusWord, "x87StatusWord"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87StatusWord, Register_Position(offset++, 0, 14, sizeof(WORD) * 2)));

		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87SW_B, "x87SW_B"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87SW_B, Register_Position(offset, 0, 9, 1)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87SW_C3, "x87SW_C3"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87SW_C3, Register_Position(offset, 12, 10, 1)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87SW_C2, "x87SW_C2"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87SW_C2, Register_Position(offset++, 25, 10, 1)));

		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87SW_C1, "x87SW_C1"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87SW_C1, Register_Position(offset, 0, 9, 1)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87SW_C0, "x87SW_C0"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87SW_C0, Register_Position(offset, 12, 10, 1)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87SW_ES, "x87SW_ES"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87SW_ES, Register_Position(offset++, 25, 10, 1)));

		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87SW_SF, "x87SW_SF"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87SW_SF, Register_Position(offset, 0, 9, 1)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87SW_P, "x87SW_P"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87SW_P, Register_Position(offset, 12, 10, 1)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87SW_U, "x87SW_U"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87SW_U, Register_Position(offset++, 25, 10, 1)));

		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87SW_O, "x87SW_O"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87SW_O, Register_Position(offset, 0, 9, 1)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87SW_Z, "x87SW_Z"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87SW_Z, Register_Position(offset, 12, 10, 1)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87SW_D, "x87SW_D"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87SW_D, Register_Position(offset++, 25, 10, 1)));

		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87SW_I, "x87SW_I"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87SW_I, Register_Position(offset, 0, 9, 1)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87SW_TOP, "x87SW_TOP"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87SW_TOP, Register_Position(offset++, 12, 10, 13)));

		offset++;

		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87ControlWord, "x87ControlWord"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87ControlWord, Register_Position(offset++, 0, 15, sizeof(WORD) * 2)));

		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87CW_IC, "x87CW_IC"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87CW_IC, Register_Position(offset, 0, 9, 1)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87CW_ZM, "x87CW_ZM"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87CW_ZM, Register_Position(offset, 12, 10, 1)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87CW_PM, "x87CW_PM"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87CW_PM, Register_Position(offset++, 25, 10, 1)));

		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87CW_UM, "x87CW_UM"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87CW_UM, Register_Position(offset, 0, 9, 1)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87CW_OM, "x87CW_OM"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87CW_OM, Register_Position(offset, 12, 10, 1)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87CW_PC, "x87CW_PC"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87CW_PC, Register_Position(offset++, 25, 10, 14)));

		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87CW_DM, "x87CW_DM"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87CW_DM, Register_Position(offset, 0, 9, 1)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87CW_IM, "x87CW_IM"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87CW_IM, Register_Position(offset, 12, 10, 1)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(x87CW_RC, "x87CW_RC"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(x87CW_RC, Register_Position(offset++, 25, 10, 14)));

		offset++;

		mRegisterMapping.insert(pair<REGISTER_NAME, string>(MxCsr, "MxCsr"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(MxCsr, Register_Position(offset++, 0, 6, sizeof(DWORD) * 2)));

		mRegisterMapping.insert(pair<REGISTER_NAME, string>(MxCsr_FZ, "MxCsr_FZ"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(MxCsr_FZ, Register_Position(offset, 0, 9, 1)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(MxCsr_PM, "MxCsr_PM"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(MxCsr_PM, Register_Position(offset, 12, 10, 1)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(MxCsr_UM, "MxCsr_UM"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(MxCsr_UM, Register_Position(offset++, 25, 10, 1)));

		mRegisterMapping.insert(pair<REGISTER_NAME, string>(MxCsr_OM, "MxCsr_OM"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(MxCsr_OM, Register_Position(offset, 0, 9, 1)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(MxCsr_ZM, "MxCsr_ZM"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(MxCsr_ZM, Register_Position(offset, 12, 10, 1)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(MxCsr_IM, "MxCsr_IM"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(MxCsr_IM, Register_Position(offset++, 25, 10, 1)));

		mRegisterMapping.insert(pair<REGISTER_NAME, string>(MxCsr_UE, "MxCsr_UE"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(MxCsr_UE, Register_Position(offset, 0, 9, 1)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(MxCsr_PE, "MxCsr_PE"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(MxCsr_PE, Register_Position(offset, 12, 10, 1)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(MxCsr_DAZ, "MxCsr_DAZ"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(MxCsr_DAZ, Register_Position(offset++, 25, 10, 1)));

		mRegisterMapping.insert(pair<REGISTER_NAME, string>(MxCsr_OE, "MxCsr_OE"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(MxCsr_OE, Register_Position(offset, 0, 9, 1)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(MxCsr_ZE, "MxCsr_ZE"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(MxCsr_ZE, Register_Position(offset, 12, 10, 1)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(MxCsr_DE, "MxCsr_DE"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(MxCsr_DE, Register_Position(offset++, 25, 10, 1)));

		mRegisterMapping.insert(pair<REGISTER_NAME, string>(MxCsr_IE, "MxCsr_IE"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(MxCsr_IE, Register_Position(offset, 0, 9, 1)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(MxCsr_DM, "MxCsr_DM"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(MxCsr_DM, Register_Position(offset, 12, 10, 1)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(MxCsr_RC, "MxCsr_RC"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(MxCsr_RC, Register_Position(offset++, 25, 10, 19)));

		offset++;

		mRegisterMapping.insert(pair<REGISTER_NAME, string>(MM0, "MM0"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(MM0, Register_Position(offset++, 0, 4, 8 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(MM1, "MM1"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(MM1, Register_Position(offset++, 0, 4, 8 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(MM2, "MM2"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(MM2, Register_Position(offset++, 0, 4, 8 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(MM3, "MM3"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(MM3, Register_Position(offset++, 0, 4, 8 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(MM4, "MM4"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(MM4, Register_Position(offset++, 0, 4, 8 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(MM5, "MM5"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(MM5, Register_Position(offset++, 0, 4, 8 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(MM6, "MM6"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(MM6, Register_Position(offset++, 0, 4, 8 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(MM7, "MM7"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(MM7, Register_Position(offset++, 0, 4, 8 * 2)));

		offset++;

		mRegisterMapping.insert(pair<REGISTER_NAME, string>(XMM0, "XMM0"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(XMM0, Register_Position(offset++, 0, 6, 16 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(XMM1, "XMM1"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(XMM1, Register_Position(offset++, 0, 6, 16 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(XMM2, "XMM2"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(XMM2, Register_Position(offset++, 0, 6, 16 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(XMM3, "XMM3"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(XMM3, Register_Position(offset++, 0, 6, 16 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(XMM4, "XMM4"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(XMM4, Register_Position(offset++, 0, 6, 16 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(XMM5, "XMM5"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(XMM5, Register_Position(offset++, 0, 6, 16 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(XMM6, "XMM6"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(XMM6, Register_Position(offset++, 0, 6, 16 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(XMM7, "XMM7"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(XMM7, Register_Position(offset++, 0, 6, 16 * 2)));
#ifdef _WIN64
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(XMM8, "XMM8"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(XMM8, Register_Position(offset++, 0, 6, 16 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(XMM9, "XMM9"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(XMM9, Register_Position(offset++, 0, 6, 16 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(XMM10, "XMM10"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(XMM10, Register_Position(offset++, 0, 6, 16 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(XMM11, "XMM11"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(XMM11, Register_Position(offset++, 0, 6, 16 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(XMM12, "XMM12"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(XMM12, Register_Position(offset++, 0, 6, 16 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(XMM13, "XMM13"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(XMM13, Register_Position(offset++, 0, 6, 16 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(XMM14, "XMM14"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(XMM14, Register_Position(offset++, 0, 6, 16 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(XMM15, "XMM15"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(XMM15, Register_Position(offset++, 0, 6, 16 * 2)));
#endif

		offset++;

		mRegisterMapping.insert(pair<REGISTER_NAME, string>(YMM0, "YMM0"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(YMM0, Register_Position(offset++, 0, 6, 32 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(YMM1, "YMM1"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(YMM1, Register_Position(offset++, 0, 6, 32 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(YMM2, "YMM2"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(YMM2, Register_Position(offset++, 0, 6, 32 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(YMM3, "YMM3"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(YMM3, Register_Position(offset++, 0, 6, 32 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(YMM4, "YMM4"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(YMM4, Register_Position(offset++, 0, 6, 32 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(YMM5, "YMM5"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(YMM5, Register_Position(offset++, 0, 6, 32 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(YMM6, "YMM6"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(YMM6, Register_Position(offset++, 0, 6, 32 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(YMM7, "YMM7"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(YMM7, Register_Position(offset++, 0, 6, 32 * 2)));
#ifdef _WIN64
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(YMM8, "YMM8"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(YMM8, Register_Position(offset++, 0, 6, 32 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(YMM9, "YMM9"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(YMM9, Register_Position(offset++, 0, 6, 32 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(YMM10, "YMM10"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(YMM10, Register_Position(offset++, 0, 6, 32 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(YMM11, "YMM11"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(YMM11, Register_Position(offset++, 0, 6, 32 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(YMM12, "YMM12"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(YMM12, Register_Position(offset++, 0, 6, 32 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(YMM13, "YMM13"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(YMM13, Register_Position(offset++, 0, 6, 32 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(YMM14, "YMM14"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(YMM14, Register_Position(offset++, 0, 6, 32 * 2)));
		mRegisterMapping.insert(pair<REGISTER_NAME, string>(YMM15, "YMM15"));
		mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(YMM15, Register_Position(offset++, 0, 6, 32 * 2)));
#endif
	}

	offset++;

	mRegisterMapping.insert(pair<REGISTER_NAME, string>(DR0, "DR0"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(DR0, Register_Position(offset++, 0, 4, sizeof(duint) * 2)));
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(DR1, "DR1"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(DR1, Register_Position(offset++, 0, 4, sizeof(duint) * 2)));
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(DR2, "DR2"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(DR2, Register_Position(offset++, 0, 4, sizeof(duint) * 2)));
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(DR3, "DR3"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(DR3, Register_Position(offset++, 0, 4, sizeof(duint) * 2)));
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(DR6, "DR6"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(DR6, Register_Position(offset++, 0, 4, sizeof(duint) * 2)));
	mRegisterMapping.insert(pair<REGISTER_NAME, string>(DR7, "DR7"));
	mRegisterPlaces.insert(pair<REGISTER_NAME, Register_Position>(DR7, Register_Position(offset++, 0, 4, sizeof(duint) * 2)));

	mRowsNeeded = offset + 1;

	std::map<int, int>lineWidth;
	CString str;
	for (auto itr = mRegisterPlaces.begin(); itr != mRegisterPlaces.end(); itr++)
	{		
		str = mRegisterMapping[itr->first].data();
		int nameLen = str.GetLength();
		if (!lineWidth.count(itr->second.line))
			lineWidth.insert(pair<int, int>(itr->second.line, nameLen + itr->second.labelwidth + itr->second.valuesize));
		else
		{
			int value = lineWidth.find(itr->second.line)->second + nameLen + itr->second.labelwidth + itr->second.valuesize;
			lineWidth.insert(pair<int, int>(itr->second.line, value));
		}
	}
	mCharCount = 0;
	for (const auto & line : lineWidth)
		mCharCount = max(mCharCount, line.second);
	mCharCount += 10;
}

/**
* @brief                   Get the label associated with the register
* @param register_selected the register
* @return                  the label
*/
CString CRegistersView::getRegisterLabel(REGISTER_NAME register_selected)
{
	char label_text[MAX_LABEL_SIZE] = "";
	char module_text[MAX_MODULE_SIZE] = "";
	char string_text[MAX_STRING_SIZE] = "";

	CString valueText;
	CString formatText = _T("%0");
	formatText.AppendFormat(_T("%dX"), mRegisterPlaces[register_selected].valuesize);

	valueText.Format(formatText, *((duint*)registerValue(&wRegDumpStruct, register_selected)));
	duint register_value = (*((duint*)registerValue(&wRegDumpStruct, register_selected)));
	CString newText = _T("");

	bool hasString = DbgGetStringAt(register_value, string_text);
	bool hasLabel = DbgGetLabelAt(register_value, SEG_DEFAULT, label_text);
	bool hasModule = DbgGetModuleAt(register_value, module_text);

	if (hasString && mONLYMODULEANDLABELDISPLAY.find(register_selected) == mONLYMODULEANDLABELDISPLAY.end())
	{
		newText = CA2W(string_text, CP_UTF8);
	}
	else if (hasLabel && hasModule)
	{
		//newText = "<" + QString(module_text) + "." + QString(label_text) + ">";
		newText = "<";
		newText += CA2W(module_text, CP_UTF8);
		newText += ".";
		newText += CA2W(label_text, CP_UTF8);
		newText += ">";
	}
	else if (hasModule)
	{
		//newText = QString(module_text) + "." + valueText;
		newText = CA2W(module_text, CP_UTF8);
		newText += ".";
		newText += valueText;
	}
	else if (hasLabel)
	{
		//newText = "<" + QString(label_text) + ">";
		newText = "<";
		newText += CA2W(label_text, CP_UTF8);
		newText += ">";
	}
	else if (mONLYMODULEANDLABELDISPLAY.find(register_selected) == mONLYMODULEANDLABELDISPLAY.end())
	{
		if (register_value == (register_value & 0xFF))
		{
			/*
			QChar c = QChar((char)register_value);
			if (c.isPrint())
			{
				newText = QString("'%1'").arg((char)register_value);
			}
			*/
			if (isprint(register_value))
			{
				TCHAR c = (TCHAR)register_value;
				newText = _T("'");
				newText += c;
				newText += "'";
			}
		}
		else if (register_value == (register_value & 0xFFF)) //UNICODE?
		{
			/*
			QChar c = QChar((wchar_t)register_value);
			if (c.isPrint())
			{
				newText = "L'" + QString(c) + "'";
			}
			*/
			if (iswprint((int)register_value))
			{
				TCHAR c = (TCHAR)register_value;
				newText = _T("L'");
				newText += c;
				newText += "'";
			}
		}
	}

	return std::move(newText);
}

/**
* @brief RegistersView::helpRegister Get the help information of a register. The help information is from Intel's manual.
* @param reg The register name
* @return The help information, possibly translated to the native language, or empty if the help information is not available yet.
*/
CString CRegistersView::helpRegister(REGISTER_NAME reg)
{
	switch (reg)
	{
		//We don't display help messages for general purpose registers as they are very well-known.
	case CF:
		return _T("CF (bit 0) : Carry flag - Set if an arithmetic operation generates a carry or a borrow out of the mostsignificant bit of the result; cleared otherwise.\n")
			_T("This flag indicates an overflow condition for unsigned-integer arithmetic. It is also used in multiple-precision arithmetic.");
	case PF:
		return _T("PF (bit 2) : Parity flag - Set if the least-significant byte of the result contains an even number of 1 bits; cleared otherwise.");
	case AF:
		return _T("AF (bit 4) : Auxiliary Carry flag - Set if an arithmetic operation generates a carry or a borrow out of bit\n")
			_T("3 of the result; cleared otherwise. This flag is used in binary-coded decimal (BCD) arithmetic.");
	case ZF:
		return _T("ZF (bit 6) : Zero flag - Set if the result is zero; cleared otherwise.");
	case SF:
		return _T("SF (bit 7) : Sign flag - Set equal to the most-significant bit of the result, which is the sign bit of a signed\n")
			_T("integer. (0 indicates a positive value and 1 indicates a negative value.)");
	case OF:
		return _T("OF (bit 11) : Overflow flag - Set if the integer result is too large a positive number or too small a negative\n")
			_T("number (excluding the sign-bit) to fit in the destination operand; cleared otherwise. This flag indicates an overflow\n")
			_T("condition for signed-integer (two’s complement) arithmetic.");
	case DF:
		return _T("DF (bit 10) : The direction flag controls string instructions (MOVS, CMPS, SCAS, LODS, and STOS). Setting the DF flag causes the string instructions\n")
			_T("to auto-decrement (to process strings from high addresses to low addresses). Clearing the DF flag causes the string instructions to auto-increment\n")
			_T("(process strings from low addresses to high addresses).");
	case TF:
		return _T("TF (bit 8) : Trap flag - Set to enable single-step mode for debugging; clear to disable single-step mode.");
	case IF:
		return _T("IF (bit 9) : Interrupt enable flag - Controls the response of the processor to maskable interrupt requests. Set to respond to maskable interrupts; cleared to inhibit maskable interrupts.");
	case x87ControlWord:
		return _T("The 16-bit x87 FPU control word controls the precision of the x87 FPU and rounding method used. It also contains the x87 FPU floating-point exception mask bits.");
	case x87StatusWord:
		return _T("The 16-bit x87 FPU status register indicates the current state of the x87 FPU.");
	case x87TagWord:
		return _T("The 16-bit tag word indicates the contents of each the 8 registers in the x87 FPU data-register stack (one 2-bit tag per register).");

	case x87CW_PC:
		return _T("The precision-control (PC) field (bits 8 and 9 of the x87 FPU control word) determines the precision (64, 53, or 24 bits) of floating-point calculations made by the x87 FPU");
	case x87CW_RC:
		return _T("The rounding-control (RC) field of the x87 FPU control register (bits 10 and 11) controls how the results of x87 FPU floating-point instructions are rounded.");
	case x87CW_IC:
		return _T("The infinity control flag (bit 12 of the x87 FPU control word) is provided for compatibility with the Intel 287 Math Coprocessor;\n")
			_T("it is not meaningful for later version x87 FPU coprocessors or IA-32 processors.");
	case x87CW_IM:
		return _T("The invalid operation exception mask (bit 0). When the mask bit is set, its corresponding exception is blocked from being generated.");
	case x87CW_DM:
		return _T("The denormal-operand exception mask (bit 2). When the mask bit is set, its corresponding exception is blocked from being generated.");
	case x87CW_ZM:
		return _T("The floating-point divide-by-zero exception mask (bit 3). When the mask bit is set, its corresponding exception is blocked from being generated.");
	case x87CW_OM:
		return _T("The floating-point numeric overflow exception mask (bit 4). When the mask bit is set, its corresponding exception is blocked from being generated.");
	case x87CW_UM:
		return _T("The potential floating-point numeric underflow condition mask (bit 5). When the mask bit is set, its corresponding exception is blocked from being generated.");
	case x87CW_PM:
		return _T("The inexact-result/precision exception mask (bit 6). When the mask bit is set, its corresponding exception is blocked from being generated.");

	case x87SW_B:
		return _T("The busy flag (bit 15) indicates if the FPU is busy (B=1) while executing an instruction, or is idle (B=0).\n")
			_T("The B-bit (bit 15) is included for 8087 compatibility only. It reflects the contents of the ES flag.");
	/*
	case x87SW_C0:
		return _T("The C%1 condition code flag (bit %2) is used to indicate the results of floating-point comparison and arithmetic operations.").arg(0).arg(8);
	case x87SW_C1:
		return _T("The C%1 condition code flag (bit %2) is used to indicate the results of floating-point comparison and arithmetic operations.").arg(1).arg(9);
	case x87SW_C2:
		return _T("The C%1 condition code flag (bit %2) is used to indicate the results of floating-point comparison and arithmetic operations.").arg(2).arg(10);
	case x87SW_C3:
		return _T("The C%1 condition code flag (bit %2) is used to indicate the results of floating-point comparison and arithmetic operations.").arg(3).arg(14);		
	*/
	
	case x87SW_ES:
		return _T("The error/exception summary status flag (bit 7) is set when any of the unmasked exception flags are set.");
	case x87SW_SF:
		return _T("The stack fault flag (bit 6 of the x87 FPU status word) indicates that stack overflow or stack underflow has occurred with data\nin the x87 FPU data register stack.");
	case x87SW_TOP:
		return _T("A pointer to the x87 FPU data register that is currently at the top of the x87 FPU register stack is contained in bits 11 through 13\n")
			_T("of the x87 FPU status word. This pointer, which is commonly referred to as TOP (for top-of-stack), is a binary value from 0 to 7.");
	case x87SW_I:
		return _T("The processor reports an invalid operation exception (bit 0) in response to one or more invalid arithmetic operands.");
	case x87SW_D:
		return _T("The processor reports the denormal-operand exception (bit 2) if an arithmetic instruction attempts to operate on a denormal operand.");
	case x87SW_Z:
		return _T("The processor reports the floating-point divide-by-zero exception (bit 3) whenever an instruction attempts to divide a finite non-zero operand by 0.");
	case x87SW_O:
		return _T("The processor reports a floating-point numeric overflow exception (bit 4) whenever the rounded result of an instruction exceeds the largest allowable finite value that will fit into the destination operand.");
	case x87SW_U:
		return _T("The processor detects a potential floating-point numeric underflow condition (bit 5) whenever the result of rounding with unbounded exponent is non-zero and tiny.");
	case x87SW_P:
		return _T("The inexact-result/precision exception (bit 6) occurs if the result of an operation is not exactly representable in the destination format.");

	case MxCsr:
		return _T("The 32-bit MXCSR register contains control and status information for SIMD floating-point operations.");
	case MxCsr_IE:
		return _T("Bit 0 (IE) : Invalid Operation Flag; indicate whether a SIMD floating-point exception has been detected.");
	case MxCsr_DE:
		return _T("Bit 1 (DE) : Denormal Flag; indicate whether a SIMD floating-point exception has been detected.");
	case MxCsr_ZE:
		return _T("Bit 2 (ZE) : Divide-by-Zero Flag; indicate whether a SIMD floating-point exception has been detected.");
	case MxCsr_OE:
		return _T("Bit 3 (OE) : Overflow Flag; indicate whether a SIMD floating-point exception has been detected.");
	case MxCsr_UE:
		return _T("Bit 4 (UE) : Underflow Flag; indicate whether a SIMD floating-point exception has been detected.");
	case MxCsr_PE:
		return _T("Bit 5 (PE) : Precision Flag; indicate whether a SIMD floating-point exception has been detected.");
	case MxCsr_IM:
		return _T("Bit 7 (IM) : Invalid Operation Mask. When the mask bit is set, its corresponding exception is blocked from being generated.");
	case MxCsr_DM:
		return _T("Bit 8 (DM) : Denormal Mask. When the mask bit is set, its corresponding exception is blocked from being generated.");
	case MxCsr_ZM:
		return _T("Bit 9 (ZM) : Divide-by-Zero Mask. When the mask bit is set, its corresponding exception is blocked from being generated.");
	case MxCsr_OM:
		return _T("Bit 10 (OM) : Overflow Mask. When the mask bit is set, its corresponding exception is blocked from being generated.");
	case MxCsr_UM:
		return _T("Bit 11 (UM) : Underflow Mask. When the mask bit is set, its corresponding exception is blocked from being generated.");
	case MxCsr_PM:
		return _T("Bit 12 (PM) : Precision Mask. When the mask bit is set, its corresponding exception is blocked from being generated.");
	case MxCsr_FZ:
		return _T("Bit 15 (FZ) of the MXCSR register enables the flush-to-zero mode, which controls the masked response to a SIMD floating-point underflow condition.");
	case MxCsr_DAZ:
		return _T("Bit 6 (DAZ) of the MXCSR register enables the denormals-are-zeros mode, which controls the processor’s response to a SIMD floating-point\n")
			_T("denormal operand condition.");
	case MxCsr_RC:
		return _T("Bits 13 and 14 of the MXCSR register (the rounding control [RC] field) control how the results of SIMD floating-point instructions are rounded.");
	case LastError:
	{
		char dat[1024];
		LASTERROR* error;
		error = (LASTERROR*)registerValue(&wRegDumpStruct, LastError);
		CString fmt;
		fmt.Format(_T("{winerror@%X}"), error->code);

		if (DbgFunctions()->StringFormatInline(CT2A(fmt, CP_UTF8), sizeof(dat), dat))
			return (CString)CA2T(dat);
		else
			return _T("The value of GetLastError(). This value is stored in the TEB.");
	}
	case LastStatus:
	{
		char dat[1024];
		LASTSTATUS* error;
		error = (LASTSTATUS*)registerValue(&wRegDumpStruct, LastStatus);
		CString fmt;
		fmt.Format(_T("{ntstatus@%X}"), error->code);
		if (DbgFunctions()->StringFormatInline(CT2A(fmt, CP_UTF8), sizeof(dat), dat))
			return (CString)CA2T(dat);
		else
			return _T("The NTSTATUS in the LastStatusValue field of the TEB.");
	}
#ifdef _WIN64
	case GS:
		return _T("The TEB of the current thread can be accessed as an offset of segment register GS (x64).\nThe TEB can be used to get a lot of information on the process without calling Win32 API.");
#else //x86
	case FS:
		return _T("The TEB of the current thread can be accessed as an offset of segment register FS (x86).\nThe TEB can be used to get a lot of information on the process without calling Win32 API.");
#endif //_WIN64
	default:
		return CString();
	}
}

char* CRegistersView::registerValue(const REGDUMP* regd, const REGISTER_NAME reg)
{
	static int null_value = 0;
	// this is probably the most efficient general method to access the values of the struct
	// TODO: add an array with something like: return array[reg].data, this is more fast :-)

	switch (reg)
	{
	case CAX:
		return (char*)&regd->regcontext.cax;
	case CBX:
		return (char*)&regd->regcontext.cbx;
	case CCX:
		return (char*)&regd->regcontext.ccx;
	case CDX:
		return (char*)&regd->regcontext.cdx;
	case CSI:
		return (char*)&regd->regcontext.csi;
	case CDI:
		return (char*)&regd->regcontext.cdi;
	case CBP:
		return (char*)&regd->regcontext.cbp;
	case CSP:
		return (char*)&regd->regcontext.csp;

	case CIP:
		return (char*)&regd->regcontext.cip;
	case EFLAGS:
		return (char*)&regd->regcontext.eflags;
#ifdef _WIN64
	case R8:
		return (char*)&regd->regcontext.r8;
	case R9:
		return (char*)&regd->regcontext.r9;
	case R10:
		return (char*)&regd->regcontext.r10;
	case R11:
		return (char*)&regd->regcontext.r11;
	case R12:
		return (char*)&regd->regcontext.r12;
	case R13:
		return (char*)&regd->regcontext.r13;
	case R14:
		return (char*)&regd->regcontext.r14;
	case R15:
		return (char*)&regd->regcontext.r15;
#endif
		// CF,PF,AF,ZF,SF,TF,IF,DF,OF
	case CF:
		return (char*)&regd->flags.c;
	case PF:
		return (char*)&regd->flags.p;
	case AF:
		return (char*)&regd->flags.a;
	case ZF:
		return (char*)&regd->flags.z;
	case SF:
		return (char*)&regd->flags.s;
	case TF:
		return (char*)&regd->flags.t;
	case IF:
		return (char*)&regd->flags.i;
	case DF:
		return (char*)&regd->flags.d;
	case OF:
		return (char*)&regd->flags.o;

		// GS,FS,ES,DS,CS,SS
	case GS:
		return (char*)&regd->regcontext.gs;
	case FS:
		return (char*)&regd->regcontext.fs;
	case ES:
		return (char*)&regd->regcontext.es;
	case DS:
		return (char*)&regd->regcontext.ds;
	case CS:
		return (char*)&regd->regcontext.cs;
	case SS:
		return (char*)&regd->regcontext.ss;

	case LastError:
		return (char*)&regd->lastError;
	case LastStatus:
		return (char*)&regd->lastStatus;

	case DR0:
		return (char*)&regd->regcontext.dr0;
	case DR1:
		return (char*)&regd->regcontext.dr1;
	case DR2:
		return (char*)&regd->regcontext.dr2;
	case DR3:
		return (char*)&regd->regcontext.dr3;
	case DR6:
		return (char*)&regd->regcontext.dr6;
	case DR7:
		return (char*)&regd->regcontext.dr7;

	case MM0:
		return (char*)&regd->mmx[0];
	case MM1:
		return (char*)&regd->mmx[1];
	case MM2:
		return (char*)&regd->mmx[2];
	case MM3:
		return (char*)&regd->mmx[3];
	case MM4:
		return (char*)&regd->mmx[4];
	case MM5:
		return (char*)&regd->mmx[5];
	case MM6:
		return (char*)&regd->mmx[6];
	case MM7:
		return (char*)&regd->mmx[7];

	case x87r0:
		return (char*)&regd->x87FPURegisters[0];
	case x87r1:
		return (char*)&regd->x87FPURegisters[1];
	case x87r2:
		return (char*)&regd->x87FPURegisters[2];
	case x87r3:
		return (char*)&regd->x87FPURegisters[3];
	case x87r4:
		return (char*)&regd->x87FPURegisters[4];
	case x87r5:
		return (char*)&regd->x87FPURegisters[5];
	case x87r6:
		return (char*)&regd->x87FPURegisters[6];
	case x87r7:
		return (char*)&regd->x87FPURegisters[7];

	case x87TagWord:
		return (char*)&regd->regcontext.x87fpu.TagWord;

	case x87ControlWord:
		return (char*)&regd->regcontext.x87fpu.ControlWord;

	case x87TW_0:
		return (char*)&regd->x87FPURegisters[0].tag;
	case x87TW_1:
		return (char*)&regd->x87FPURegisters[1].tag;
	case x87TW_2:
		return (char*)&regd->x87FPURegisters[2].tag;
	case x87TW_3:
		return (char*)&regd->x87FPURegisters[3].tag;
	case x87TW_4:
		return (char*)&regd->x87FPURegisters[4].tag;
	case x87TW_5:
		return (char*)&regd->x87FPURegisters[5].tag;
	case x87TW_6:
		return (char*)&regd->x87FPURegisters[6].tag;
	case x87TW_7:
		return (char*)&regd->x87FPURegisters[7].tag;

	case x87CW_IC:
		return (char*)&regd->x87ControlWordFields.IC;
	case x87CW_PM:
		return (char*)&regd->x87ControlWordFields.PM;
	case x87CW_UM:
		return (char*)&regd->x87ControlWordFields.UM;
	case x87CW_OM:
		return (char*)&regd->x87ControlWordFields.OM;
	case x87CW_ZM:
		return (char*)&regd->x87ControlWordFields.ZM;
	case x87CW_DM:
		return (char*)&regd->x87ControlWordFields.DM;
	case x87CW_IM:
		return (char*)&regd->x87ControlWordFields.IM;
	case x87CW_RC:
		return (char*)&regd->x87ControlWordFields.RC;
	case x87CW_PC:
		return (char*)&regd->x87ControlWordFields.PC;

	case x87StatusWord:
		return (char*)&regd->regcontext.x87fpu.StatusWord;

	case x87SW_B:
		return (char*)&regd->x87StatusWordFields.B;
	case x87SW_C3:
		return (char*)&regd->x87StatusWordFields.C3;
	case x87SW_C2:
		return (char*)&regd->x87StatusWordFields.C2;
	case x87SW_C1:
		return (char*)&regd->x87StatusWordFields.C1;
	case x87SW_O:
		return (char*)&regd->x87StatusWordFields.O;
	case x87SW_ES:
		return (char*)&regd->x87StatusWordFields.ES;
	case x87SW_SF:
		return (char*)&regd->x87StatusWordFields.SF;
	case x87SW_P:
		return (char*)&regd->x87StatusWordFields.P;
	case x87SW_U:
		return (char*)&regd->x87StatusWordFields.U;
	case x87SW_Z:
		return (char*)&regd->x87StatusWordFields.Z;
	case x87SW_D:
		return (char*)&regd->x87StatusWordFields.D;
	case x87SW_I:
		return (char*)&regd->x87StatusWordFields.I;
	case x87SW_C0:
		return (char*)&regd->x87StatusWordFields.C0;
	case x87SW_TOP:
		return (char*)&regd->x87StatusWordFields.TOP;

	case MxCsr:
		return (char*)&regd->regcontext.MxCsr;

	case MxCsr_FZ:
		return (char*)&regd->MxCsrFields.FZ;
	case MxCsr_PM:
		return (char*)&regd->MxCsrFields.PM;
	case MxCsr_UM:
		return (char*)&regd->MxCsrFields.UM;
	case MxCsr_OM:
		return (char*)&regd->MxCsrFields.OM;
	case MxCsr_ZM:
		return (char*)&regd->MxCsrFields.ZM;
	case MxCsr_IM:
		return (char*)&regd->MxCsrFields.IM;
	case MxCsr_DM:
		return (char*)&regd->MxCsrFields.DM;
	case MxCsr_DAZ:
		return (char*)&regd->MxCsrFields.DAZ;
	case MxCsr_PE:
		return (char*)&regd->MxCsrFields.PE;
	case MxCsr_UE:
		return (char*)&regd->MxCsrFields.UE;
	case MxCsr_OE:
		return (char*)&regd->MxCsrFields.OE;
	case MxCsr_ZE:
		return (char*)&regd->MxCsrFields.ZE;
	case MxCsr_DE:
		return (char*)&regd->MxCsrFields.DE;
	case MxCsr_IE:
		return (char*)&regd->MxCsrFields.IE;
	case MxCsr_RC:
		return (char*)&regd->MxCsrFields.RC;

	case XMM0:
		return (char*)&regd->regcontext.XmmRegisters[0];
	case XMM1:
		return (char*)&regd->regcontext.XmmRegisters[1];
	case XMM2:
		return (char*)&regd->regcontext.XmmRegisters[2];
	case XMM3:
		return (char*)&regd->regcontext.XmmRegisters[3];
	case XMM4:
		return (char*)&regd->regcontext.XmmRegisters[4];
	case XMM5:
		return (char*)&regd->regcontext.XmmRegisters[5];
	case XMM6:
		return (char*)&regd->regcontext.XmmRegisters[6];
	case XMM7:
		return (char*)&regd->regcontext.XmmRegisters[7];
#ifdef _WIN64
	case XMM8:
		return (char*)&regd->regcontext.XmmRegisters[8];
	case XMM9:
		return (char*)&regd->regcontext.XmmRegisters[9];
	case XMM10:
		return (char*)&regd->regcontext.XmmRegisters[10];
	case XMM11:
		return (char*)&regd->regcontext.XmmRegisters[11];
	case XMM12:
		return (char*)&regd->regcontext.XmmRegisters[12];
	case XMM13:
		return (char*)&regd->regcontext.XmmRegisters[13];
	case XMM14:
		return (char*)&regd->regcontext.XmmRegisters[14];
	case XMM15:
		return (char*)&regd->regcontext.XmmRegisters[15];
#endif //_WIN64

	case YMM0:
		return (char*)&regd->regcontext.YmmRegisters[0];
	case YMM1:
		return (char*)&regd->regcontext.YmmRegisters[1];
	case YMM2:
		return (char*)&regd->regcontext.YmmRegisters[2];
	case YMM3:
		return (char*)&regd->regcontext.YmmRegisters[3];
	case YMM4:
		return (char*)&regd->regcontext.YmmRegisters[4];
	case YMM5:
		return (char*)&regd->regcontext.YmmRegisters[5];
	case YMM6:
		return (char*)&regd->regcontext.YmmRegisters[6];
	case YMM7:
		return (char*)&regd->regcontext.YmmRegisters[7];
#ifdef _WIN64
	case YMM8:
		return (char*)&regd->regcontext.YmmRegisters[8];
	case YMM9:
		return (char*)&regd->regcontext.YmmRegisters[9];
	case YMM10:
		return (char*)&regd->regcontext.YmmRegisters[10];
	case YMM11:
		return (char*)&regd->regcontext.YmmRegisters[11];
	case YMM12:
		return (char*)&regd->regcontext.YmmRegisters[12];
	case YMM13:
		return (char*)&regd->regcontext.YmmRegisters[13];
	case YMM14:
		return (char*)&regd->regcontext.YmmRegisters[14];
	case YMM15:
		return (char*)&regd->regcontext.YmmRegisters[15];
#endif //_WIN64
	}

	return (char*)&null_value;
}

#ifndef _NTDEF_
typedef __success(return >= 0) LONG NTSTATUS;
typedef NTSTATUS *PNTSTATUS;
#endif

SIZE_T CRegistersView::GetSizeRegister(const REGISTER_NAME reg_name)
{
	SIZE_T size;

	if (mUINTDISPLAY.find(reg_name) != mUINTDISPLAY.end())
		size = sizeof(duint);
	else if (mUSHORTDISPLAY.find(reg_name) != mUSHORTDISPLAY.end() || mFIELDVALUE.find(reg_name) != mFIELDVALUE.end())
		size = sizeof(unsigned short);
	else if (mDWORDDISPLAY.find(reg_name) != mDWORDDISPLAY.end())
		size = sizeof(DWORD);
	else if (mBOOLDISPLAY.find(reg_name) != mBOOLDISPLAY.end())
		size = sizeof(bool);
	else if (mFPUx87_80BITSDISPLAY.find(reg_name) != mFPUx87_80BITSDISPLAY.end())
		size = 10;
	else if (mFPUMMX.find(reg_name) != mFPUMMX.end())
		size = 8;
	else if (mFPUXMM.find(reg_name) != mFPUXMM.end())
		size = 16;
	else if (mFPUYMM.find(reg_name) != mFPUYMM.end())
		size = 32;
	else if (reg_name == LastError)
		size = sizeof(DWORD);
	else if (reg_name == LastStatus)
		size = sizeof(NTSTATUS);
	else
		size = 0;

	return size;
}

int CRegistersView::CompareRegisters(const REGISTER_NAME reg_name, REGDUMP* regdump1, REGDUMP* regdump2)
{
	SIZE_T size = GetSizeRegister(reg_name);
	char* reg1_data = registerValue(regdump1, reg_name);
	char* reg2_data = registerValue(regdump2, reg_name);

	if (size != 0)
		return memcmp(reg1_data, reg2_data, size);

	return -1;
}

void CRegistersView::setRegister(REGISTER_NAME reg, duint value)
{
	// is register-id known?
	if (mRegisterMapping.count(reg))
	{
		// map "cax" to "eax" or "rax"
		string wRegName = mRegisterMapping[reg];

		// flags need to '_' infront
		if (mFlags.count(reg))
			wRegName = "_" + wRegName;

		// we change the value (so highlight it)
		mRegisterUpdates.insert(reg);
		// tell everything the compiler
		if (mFPU.count(reg))
			wRegName = "_" + wRegName;

		DbgValToString(wRegName.data(), value);

		// force repaint
		//emit refresh();
		Invalidate();
	}
}

void CRegistersView::setRegisters(REGDUMP* reg)
{
	// tests if new-register-value == old-register-value holds
	if (mCip != reg->regcontext.cip) //CIP changed
	{
		wCipRegDumpStruct = wRegDumpStruct;
		mRegisterUpdates.clear();
		mCip = reg->regcontext.cip;
	}

	// iterate all ids (CAX, CBX, ...)
	for (auto itr = mRegisterMapping.begin(); itr != mRegisterMapping.end(); itr++)
	{
		if (CompareRegisters(itr->first, reg, &wCipRegDumpStruct) != 0)
			mRegisterUpdates.insert(itr->first);
		else if (mRegisterUpdates.find(itr->first) != mRegisterUpdates.end()) //registers are equal
			mRegisterUpdates.erase(itr->first);
	}

	// now we can save the values
	wRegDumpStruct = (*reg);

	if (mCip != reg->regcontext.cip)
		wCipRegDumpStruct = wRegDumpStruct;

	// force repaint
	//emit refresh();
	Invalidate();
}

#define X87FPU_TAGWORD_NONZERO 0
#define X87FPU_TAGWORD_ZERO 1
#define X87FPU_TAGWORD_SPECIAL 2
#define X87FPU_TAGWORD_EMPTY 3

#ifndef QT_TRANSLATE_NOOP
#define QT_TRANSLATE_NOOP(context, source) source
#endif //QT_TRANSLATE_NOOP

STRING_VALUE_TABLE_t TagWordValueStringTable[] =
{
	{ QT_TRANSLATE_NOOP("RegistersView_ConstantsOfRegisters", "Nonzero"), X87FPU_TAGWORD_NONZERO },
	{ QT_TRANSLATE_NOOP("RegistersView_ConstantsOfRegisters", "Zero"), X87FPU_TAGWORD_ZERO },
	{ QT_TRANSLATE_NOOP("RegistersView_ConstantsOfRegisters", "Special"), X87FPU_TAGWORD_SPECIAL },
	{ QT_TRANSLATE_NOOP("RegistersView_ConstantsOfRegisters", "Empty"), X87FPU_TAGWORD_EMPTY }
};

//WARNING: The following function is commented out because it is not used currently. If it is used again, it should be modified to keep working under internationized environment.
/*
unsigned int RegistersView::GetTagWordValueFromString(const char* string)
{
int i;

for(i = 0; i < (sizeof(TagWordValueStringTable) / sizeof(*TagWordValueStringTable)); i++)
{
if(tr(TagWordValueStringTable[i].string) == string)
return TagWordValueStringTable[i].value;
}

return i;
}

*/
CString CRegistersView::GetTagWordStateString(unsigned short state)
{
	int i;

	for (i = 0; i < (sizeof(TagWordValueStringTable) / sizeof(*TagWordValueStringTable)); i++)
	{
		if (TagWordValueStringTable[i].value == state)			
		{
			//return QApplication::translate("RegistersView_ConstantsOfRegisters", TagWordValueStringTable[i].string);
			CString str;
			str = TagWordValueStringTable[i].string;
			return str;
		}
			
	}
	return _T("Unknown");
}

#define MxCsr_RC_NEAR 0
#define MxCsr_RC_NEGATIVE 1
#define MxCsr_RC_POSITIVE   2
#define MxCsr_RC_TOZERO 3

STRING_VALUE_TABLE_t MxCsrRCValueStringTable[] =
{
	{ QT_TRANSLATE_NOOP("RegistersView_ConstantsOfRegisters", "Toward Zero"), MxCsr_RC_TOZERO },
	{ QT_TRANSLATE_NOOP("RegistersView_ConstantsOfRegisters", "Toward Positive"), MxCsr_RC_POSITIVE },
	{ QT_TRANSLATE_NOOP("RegistersView_ConstantsOfRegisters", "Toward Negative"), MxCsr_RC_NEGATIVE },
	{ QT_TRANSLATE_NOOP("RegistersView_ConstantsOfRegisters", "Round Near"), MxCsr_RC_NEAR }
};
//WARNING: The following function is commented out because it is not used currently. If it is used again, it should be modified to keep working under internationized environment.
/*
unsigned int RegistersView::GetMxCsrRCValueFromString(const char* string)
{
int i;

for(i = 0; i < (sizeof(MxCsrRCValueStringTable) / sizeof(*MxCsrRCValueStringTable)); i++)
{
if(MxCsrRCValueStringTable[i].string == string)
return MxCsrRCValueStringTable[i].value;
}

return i;
}
*/
CString CRegistersView::GetMxCsrRCStateString(unsigned short state)
{
	int i;

	for (i = 0; i < (sizeof(MxCsrRCValueStringTable) / sizeof(*MxCsrRCValueStringTable)); i++)
	{
		if (MxCsrRCValueStringTable[i].value == state)
		{
			//return QApplication::translate("RegistersView_ConstantsOfRegisters", MxCsrRCValueStringTable[i].string);
			CString str;
			str = MxCsrRCValueStringTable[i].string;
			return str;
		}			
	}
	return _T("Unknown");
}

#define x87CW_RC_NEAR 0
#define x87CW_RC_DOWN 1
#define x87CW_RC_UP   2
#define x87CW_RC_TRUNCATE 3

STRING_VALUE_TABLE_t ControlWordRCValueStringTable[] =
{
	{ QT_TRANSLATE_NOOP("RegistersView_ConstantsOfRegisters", "Truncate"), x87CW_RC_TRUNCATE },
	{ QT_TRANSLATE_NOOP("RegistersView_ConstantsOfRegisters", "Round Up"), x87CW_RC_UP },
	{ QT_TRANSLATE_NOOP("RegistersView_ConstantsOfRegisters", "Round Down"), x87CW_RC_DOWN },
	{ QT_TRANSLATE_NOOP("RegistersView_ConstantsOfRegisters", "Round Near"), x87CW_RC_NEAR }
};
//WARNING: The following function is commented out because it is not used currently. If it is used again, it should be modified to keep working under internationized environment.
/*
unsigned int RegistersView::GetControlWordRCValueFromString(const char* string)
{
int i;

for(i = 0; i < (sizeof(ControlWordRCValueStringTable) / sizeof(*ControlWordRCValueStringTable)); i++)
{
if(tr(ControlWordRCValueStringTable[i].string) == string)
return ControlWordRCValueStringTable[i].value;
}

return i;
}
*/
CString CRegistersView::GetControlWordRCStateString(unsigned short state)
{
	int i;

	for (i = 0; i < (sizeof(ControlWordRCValueStringTable) / sizeof(*ControlWordRCValueStringTable)); i++)
	{
		if (ControlWordRCValueStringTable[i].value == state)
		{
			//return QApplication::translate("RegistersView_ConstantsOfRegisters", ControlWordRCValueStringTable[i].string);
			CString str;
			str = ControlWordRCValueStringTable[i].string;
			return str;
		}			
	}
	return _T("Unknown");
}

#define x87CW_PC_REAL4 0
#define x87CW_PC_NOTUSED 1
#define x87CW_PC_REAL8   2
#define x87CW_PC_REAL10 3

STRING_VALUE_TABLE_t ControlWordPCValueStringTable[] =
{
	{ QT_TRANSLATE_NOOP("RegistersView_ConstantsOfRegisters", "Real4"), x87CW_PC_REAL4 },
	{ QT_TRANSLATE_NOOP("RegistersView_ConstantsOfRegisters", "Not Used"), x87CW_PC_NOTUSED },
	{ QT_TRANSLATE_NOOP("RegistersView_ConstantsOfRegisters", "Real8"), x87CW_PC_REAL8 },
	{ QT_TRANSLATE_NOOP("RegistersView_ConstantsOfRegisters", "Real10"), x87CW_PC_REAL10 }
};
/*
//WARNING: The following function is commented out because it is not used currently. If it is used again, it should be modified to keep working under internationized environment.
unsigned int RegistersView::GetControlWordPCValueFromString(const char* string)
{
int i;

for(i = 0; i < (sizeof(ControlWordPCValueStringTable) / sizeof(*ControlWordPCValueStringTable)); i++)
{
if(tr(ControlWordPCValueStringTable[i].string) == string)
return ControlWordPCValueStringTable[i].value;
}

return i;
}
*/

CString CRegistersView::GetControlWordPCStateString(unsigned short state)
{
	int i;

	for (i = 0; i < (sizeof(ControlWordPCValueStringTable) / sizeof(*ControlWordPCValueStringTable)); i++)
	{
		if (ControlWordPCValueStringTable[i].value == state)
		{
			//return QApplication::translate("RegistersView_ConstantsOfRegisters", ControlWordPCValueStringTable[i].string);
			CString str;
			str = ControlWordPCValueStringTable[i].string;
			return str;
		}			
	}

	return _T("Unknown");
}

#define x87SW_TOP_0 0
#define x87SW_TOP_1 1
#define x87SW_TOP_2 2
#define x87SW_TOP_3 3
#define x87SW_TOP_4 4
#define x87SW_TOP_5 5
#define x87SW_TOP_6 6
#define x87SW_TOP_7 7
// This string needs not to be internationalized.
STRING_VALUE_TABLE_t StatusWordTOPValueStringTable[] =
{
	{ "x87r0", x87SW_TOP_0 },
	{ "x87r1", x87SW_TOP_1 },
	{ "x87r2", x87SW_TOP_2 },
	{ "x87r3", x87SW_TOP_3 },
	{ "x87r4", x87SW_TOP_4 },
	{ "x87r5", x87SW_TOP_5 },
	{ "x87r6", x87SW_TOP_6 },
	{ "x87r7", x87SW_TOP_7 }
};
//WARNING: The following function is commented out because it is not used currently. If it is used again, it should be modified to keep working under internationized environment.
/*
unsigned int RegistersView::GetStatusWordTOPValueFromString(const char* string)
{
	int i;

	for(i = 0; i < (sizeof(StatusWordTOPValueStringTable) / sizeof(*StatusWordTOPValueStringTable)); i++)
	{
	if(StatusWordTOPValueStringTable[i].string == string)
	return StatusWordTOPValueStringTable[i].value;
	}

	return i;
}
*/
CString CRegistersView::GetStatusWordTOPStateString(unsigned short state)
{
	int i;

	for (i = 0; i < (sizeof(StatusWordTOPValueStringTable) / sizeof(*StatusWordTOPValueStringTable)); i++)
	{
		if (StatusWordTOPValueStringTable[i].value == state)
			return CString(StatusWordTOPValueStringTable[i].string);
	}
	return _T("Unknown");
}

/**
* @brief RegistersView::GetRegStringValueFromValue Get the textual representation of the register value.
* @param reg The name of the register
* @param value The current value of the register
* @return The textual representation of the register value
*
* This value does not return hex representation all the times for SIMD registers. The actual representation of SIMD registers depends on the user settings.
*/
CString CRegistersView::GetRegStringValueFromValue(REGISTER_NAME reg, const char* value)
{
	CString valueText;
	CString formatText = _T("%0");
	formatText.AppendFormat(_T("%dX"), mRegisterPlaces[reg].valuesize);
	
	if (mUINTDISPLAY.find(reg) != mUINTDISPLAY.end())
		//valueText = QString("%1").arg((*((const duint*)value)), mRegisterPlaces[reg].valuesize, 16, QChar('0')).toUpper();
		valueText.Format(formatText, *((const duint*)value));
	else if (mUSHORTDISPLAY.find(reg) != mUSHORTDISPLAY.end())
		//valueText = QString("%1").arg((*((const unsigned short*)value)), mRegisterPlaces[reg].valuesize, 16, QChar('0')).toUpper();
		valueText.Format(formatText, *((const unsigned short*)value));
	else if (mDWORDDISPLAY.find(reg) != mDWORDDISPLAY.end())
		//valueText = QString("%1").arg((*((const DWORD*)value)), mRegisterPlaces[reg].valuesize, 16, QChar('0')).toUpper();
		valueText.Format(formatText, *((const DWORD *)value));
	else if (mBOOLDISPLAY.find(reg) != mBOOLDISPLAY.end())
		//valueText = QString("%1").arg((*((const bool*)value)), mRegisterPlaces[reg].valuesize, 16, QChar('0')).toUpper();
		valueText.Format(formatText, *((const bool *)value));
	else if (mFIELDVALUE.find(reg) != mFIELDVALUE.end())
	{
		if (mTAGWORD.find(reg) != mTAGWORD.end())
		{
			/*
			valueText = QString("%1").arg((*((const unsigned short*)value)), 1, 16, QChar('0')).toUpper();
			valueText += QString(" (");
			valueText += GetTagWordStateString((*((const unsigned short*)value)));
			valueText += QString(")");
			*/
			valueText.Format(_T("01X"), *((const unsigned short*)value));
			valueText += " (";
			valueText += GetTagWordStateString((*((const unsigned short*)value)));
			valueText += ")";
		}
		if (reg == MxCsr_RC)
		{
			/*
			valueText = QString("%1").arg((*((const unsigned short*)value)), 1, 16, QChar('0')).toUpper();
			valueText += QString(" (");
			valueText += GetMxCsrRCStateString((*((const unsigned short*)value)));
			valueText += QString(")");
			*/
			valueText.Format(_T("01X"), *((const unsigned short*)value));
			valueText += " (";
			valueText += GetMxCsrRCStateString((*((const unsigned short*)value)));
			valueText += ")";
		}
		else if (reg == x87CW_RC)
		{
			/*
			valueText = QString("%1").arg((*((const unsigned short*)value)), 1, 16, QChar('0')).toUpper();
			valueText += QString(" (");
			valueText += GetControlWordRCStateString((*((const unsigned short*)value)));
			valueText += QString(")");
			*/
			valueText.Format(_T("01X"), *((const unsigned short*)value));
			valueText += " (";
			valueText += GetControlWordRCStateString((*((const unsigned short*)value)));
			valueText += ")";
		}
		else if (reg == x87CW_PC)
		{
			/*
			valueText = QString("%1").arg((*((const unsigned short*)value)), 1, 16, QChar('0')).toUpper();
			valueText += QString(" (");
			valueText += GetControlWordPCStateString((*((const unsigned short*)value)));
			valueText += QString(")");
			*/
			valueText.Format(_T("01X"), *((const unsigned short*)value));
			valueText += " (";
			valueText += GetControlWordPCStateString((*((const unsigned short*)value)));
			valueText += ")";
		}
		else if (reg == x87SW_TOP)
		{
			/*
			valueText = QString("%1").arg((*((const unsigned short*)value)), 1, 16, QChar('0')).toUpper();
			valueText += QString(" (ST0=");
			valueText += GetStatusWordTOPStateString((*((const unsigned short*)value)));
			valueText += QString(")");
			*/
			valueText.Format(_T("01X"), *((const unsigned short*)value));
			valueText += " (";
			valueText += GetStatusWordTOPStateString((*((const unsigned short*)value)));
			valueText += ")";
		}
	}
	else if (reg == LastError)
	{
		LASTERROR* data = (LASTERROR*)value;
		if (*data->name)
			//valueText = QString().sprintf("%08X (%s)", data->code, data->name);
			valueText.Format(_T("%08X (%S)"), data->code, data->name);
		else
			//valueText = QString().sprintf("%08X", data->code);
			valueText.Format(_T("%08X"), data->code);
		mRegisterPlaces[LastError].valuesize = valueText.GetLength();
	}
	else if (reg == LastStatus)
	{
		LASTSTATUS* data = (LASTSTATUS*)value;
		if (*data->name)
			//valueText = QString().sprintf("%08X (%s)", data->code, data->name);
			valueText.Format(_T("%08X (%S)"), data->code, data->name);
		else
			//valueText = QString().sprintf("%08X", data->code);
			valueText.Format(_T("%08X"), data->code);
		mRegisterPlaces[LastStatus].valuesize = valueText.GetLength();
	}
	else
	{
		SIZE_T size = GetSizeRegister(reg);
		bool bFpuRegistersLittleEndian = ConfigBool("Gui", "FpuRegistersLittleEndian");
		if (size != 0)
		{
			if (mFPUXMM.find(reg) != mFPUXMM.end())
				valueText = composeRegTextXMM(value, wSIMDRegDispMode, bFpuRegistersLittleEndian);
			else if (mFPUYMM.find(reg) != mFPUYMM.end())
			{
				if (wSIMDRegDispMode == SIMD_REG_DISP_HEX)
					valueText = fillValue(value, size, bFpuRegistersLittleEndian);
				else if (bFpuRegistersLittleEndian)
					valueText = composeRegTextXMM(value, wSIMDRegDispMode, bFpuRegistersLittleEndian) + ' ' + composeRegTextXMM(value + 16, wSIMDRegDispMode, bFpuRegistersLittleEndian);
				else
					valueText = composeRegTextXMM(value + 16, wSIMDRegDispMode, bFpuRegistersLittleEndian) + ' ' + composeRegTextXMM(value, wSIMDRegDispMode, bFpuRegistersLittleEndian);
			}
			else
				valueText = fillValue(value, size, bFpuRegistersLittleEndian);
		}
		else
			valueText = _T("???");
	}

	return std::move(valueText);
}

void CRegistersView::drawRegister(CDC* painter, REGISTER_NAME reg, char* value)
{
	TEXTMETRIC tm;
	painter->GetTextMetrics(&tm);
	int mCharWidth = tm.tmAveCharWidth;
	// is the register-id known?
	if (mRegisterMapping.find(reg) != mRegisterMapping.end())
	{
		// padding to the left is at least one character (looks better)
		mMaxLine = mMaxLine < mRegisterPlaces[reg].line ? mRegisterPlaces[reg].line : mMaxLine;
		int x = mCharWidth * (1 + mRegisterPlaces[reg].start);
		int ySpace = yTopSpacing;
		if (mVScrollOffset != 0)
			ySpace = 0;
		unsigned int mRowHeight = getRowHeight();
		int y = mRowHeight * (mRegisterPlaces[reg].line + mVScrollOffset) + ySpace;

		//draw raster
		/*
		p->save();
		p->setPen(QColor("#FF0000"));
		p->drawLine(0, y, this->viewport()->width(), y);
		p->restore();
		*/

		// draw name of value
		//int width = fontMetrics.width(mRegisterMapping[reg]);
		CString str;
		str = mRegisterMapping[reg].data();
		int width = painter->GetTextExtent(str).cx;
		

		// set the color of the register label
#ifdef _WIN64
		switch (reg)
		{
		case CCX: //arg1
		case CDX: //arg2
		case R8: //arg3
		case R9: //arg4
		case XMM0:
		case XMM1:
		case XMM2:
		case XMM3:
			//p->setPen(ConfigColor("RegistersArgumentLabelColor"));
			painter->SetTextColor(ConfigColor("RegistersArgumentLabelColor").ToCOLORREF());
			break;
		default:
#endif //_WIN64
			//p->setPen(ConfigColor("RegistersLabelColor"));
			painter->SetTextColor(ConfigColor("RegistersLabelColor").ToCOLORREF());
#ifdef _WIN64
			break;
		}
#endif //_WIN64

		//draw the register name
		auto regName = mRegisterMapping[reg];
		CString textName; 
		textName = regName.data();
		//p->drawText(x, y, width, mRowHeight, Qt::AlignVCenter, regName);
		painter->DrawText(textName, CRect(x, y, x + width, y + mRowHeight), DT_VCENTER);
		//highlight the register based on access
		uint8_t highlight = 0;
		for (const auto & reg : mHighlightRegs)
		{
			CString firstName;
			firstName = reg.first;
			if (!CapstoneTokenizer::tokenTextPoolEquals(textName, firstName))
				continue;
			highlight = reg.second;
			break;
		}
		if (highlight)
		{
			const char* name = "";
			switch (highlight & ~(Zydis::RAIImplicit | Zydis::RAIExplicit))
			{
			case Zydis::RAIRead:
				name = "RegistersHighlightReadColor";
				break;
			case Zydis::RAIWrite:
				name = "RegistersHighlightWriteColor";
				break;
			case Zydis::RAIRead | Zydis::RAIWrite:
				name = "RegistersHighlightReadWriteColor";
				break;
			}
			auto highlightColor = ConfigColor(name);
			if (highlightColor.GetAlpha())
			{
				CPen highlightPen;
				highlightPen.CreatePen(PS_SOLID, 1, highlightColor.ToCOLORREF());
				painter->SelectObject(&highlightPen);
				//painter->drawLine(x + 1, y + mRowHeight - 1, x + mCharWidth * regName.length() - 1, y + mRowHeight - 1);
				painter->MoveTo(x + 1, y + mRowHeight - 1);
				painter->LineTo(x + mCharWidth * regName.length() - 1, y + mRowHeight - 1);
			}
		}

		x += (mRegisterPlaces[reg].labelwidth) * mCharWidth;
		//p->drawText(offset,mRowHeight*(mRegisterPlaces[reg].line+1),mRegisterMapping[reg]);

		//set highlighting
		if (DbgIsDebugging() && mRegisterUpdates.find(reg) != mRegisterUpdates.end())
			//p->setPen(ConfigColor("RegistersModifiedColor"));
			painter->SetTextColor(ConfigColor("RegistersModifiedColor").ToCOLORREF());
		else
			//p->setPen(ConfigColor("RegistersColor"));
			painter->SetTextColor(ConfigColor("RegistersColor").ToCOLORREF());

		//get register value
		CString valueText = GetRegStringValueFromValue(reg, value);

		//selection
		width = painter->GetTextExtent(valueText).cx;
		if (mSelected == reg)
		{
			//p->fillRect(x, y, width, mRowHeight, QBrush(ConfigColor("RegistersSelectionColor")));
			painter->FillSolidRect(CRect(x, y, x + width, y + mRowHeight), ConfigColor("RegistersSelectionColor").ToCOLORREF());
			//p->fillRect(QRect(x + (mRegisterPlaces[reg].labelwidth)*mCharWidth ,mRowHeight*(mRegisterPlaces[reg].line)+2, mRegisterPlaces[reg].valuesize*mCharWidth, mRowHeight), QBrush(ConfigColor("RegistersSelectionColor")));
		}

		// draw value
		//p->drawText(x, y, width, mRowHeight, Qt::AlignVCenter, valueText);
		painter->DrawText(valueText, CRect(x, y, x + width, y + mRowHeight), DT_VCENTER);
		//p->drawText(x + (mRegisterPlaces[reg].labelwidth)*mCharWidth ,mRowHeight*(mRegisterPlaces[reg].line+1),QString("%1").arg(value, mRegisterPlaces[reg].valuesize, 16, QChar('0')).toUpper());

		x += width;

		if (mFPUx87_80BITSDISPLAY.find(reg) != mFPUx87_80BITSDISPLAY.end() && DbgIsDebugging())
		{
			//p->setPen(ConfigColor("RegistersExtraInfoColor"));
			painter->SetTextColor(ConfigColor("RegistersExtraInfoColor").ToCOLORREF());
			x += 1 * mCharWidth; //1 space
			CString newText;
			if (mRegisterUpdates.find(x87SW_TOP) != mRegisterUpdates.end())
				//p->setPen(ConfigColor("RegistersModifiedColor"));
				painter->SetTextColor(ConfigColor("RegistersModifiedColor").ToCOLORREF());

			newText.Format(_T("ST%d "), ((X87FPUREGISTER*)registerValue(&wRegDumpStruct, reg))->st_value);
			width = painter->GetTextExtent(newText).cx;
			//p->drawText(x, y, width, mRowHeight, Qt::AlignVCenter, newText);
			painter->DrawText(newText, CRect(x, y, x + width, y + mRowHeight), DT_VCENTER);

			x += width;

			newText = _T("");

			//p->setPen(ConfigColor("RegistersExtraInfoColor"));
			painter->SetTextColor(ConfigColor("RegistersColor").ToCOLORREF());

			if (reg == x87r0 && mRegisterUpdates.find(x87TW_0) != mRegisterUpdates.end())
			{
				//p->setPen(ConfigColor("RegistersModifiedColor"));
				painter->SetTextColor(ConfigColor("RegistersModifiedColor").ToCOLORREF());
			}
			else if (reg == x87r1 && mRegisterUpdates.find(x87TW_1) != mRegisterUpdates.end())
			{
				//p->setPen(ConfigColor("RegistersModifiedColor"));
				painter->SetTextColor(ConfigColor("RegistersModifiedColor").ToCOLORREF());
			}
			else if (reg == x87r2 && mRegisterUpdates.find(x87TW_2) != mRegisterUpdates.end())
			{
				//p->setPen(ConfigColor("RegistersModifiedColor"));
				painter->SetTextColor(ConfigColor("RegistersModifiedColor").ToCOLORREF());
			}
			else if (reg == x87r3 && mRegisterUpdates.find(x87TW_3) != mRegisterUpdates.end())
			{
				//p->setPen(ConfigColor("RegistersModifiedColor"));
				painter->SetTextColor(ConfigColor("RegistersModifiedColor").ToCOLORREF());
			}
			else if (reg == x87r4 && mRegisterUpdates.find(x87TW_4) != mRegisterUpdates.end())
			{
				//p->setPen(ConfigColor("RegistersModifiedColor"));
				painter->SetTextColor(ConfigColor("RegistersModifiedColor").ToCOLORREF());
			}
			else if (reg == x87r5 && mRegisterUpdates.find(x87TW_5) != mRegisterUpdates.end())
			{
				//p->setPen(ConfigColor("RegistersModifiedColor"));
				painter->SetTextColor(ConfigColor("RegistersModifiedColor").ToCOLORREF());
			}
			else if (reg == x87r6 && mRegisterUpdates.find(x87TW_6) != mRegisterUpdates.end())
			{
				//p->setPen(ConfigColor("RegistersModifiedColor"));
				painter->SetTextColor(ConfigColor("RegistersModifiedColor").ToCOLORREF());
			}
			else if (reg == x87r7 && mRegisterUpdates.find(x87TW_7) != mRegisterUpdates.end())
			{
				//p->setPen(ConfigColor("RegistersModifiedColor"));
				painter->SetTextColor(ConfigColor("RegistersModifiedColor").ToCOLORREF());
			}

			newText += GetTagWordStateString(((X87FPUREGISTER*)registerValue(&wRegDumpStruct, reg))->tag);
			newText += " ";

			width = painter->GetTextExtent(newText).cx;
			//p->drawText(x, y, width, mRowHeight, Qt::AlignVCenter, newText);
			painter->DrawText(newText, CRect(x, y, x + width, y + mRowHeight), DT_VCENTER);

			x += width;

			newText = "";

			//p->setPen(ConfigColor("RegistersExtraInfoColor"));
			painter->SetTextColor(ConfigColor("RegistersExtraInfoColor").ToCOLORREF());

			if (DbgIsDebugging() && mRegisterUpdates.find(reg) != mRegisterUpdates.end())
				//p->setPen(ConfigColor("RegistersModifiedColor"));
				painter->SetTextColor(ConfigColor("RegistersModifiedColor").ToCOLORREF());

			newText += ToLongDoubleString(((X87FPUREGISTER*)registerValue(&wRegDumpStruct, reg))->data);
			width = painter->GetTextExtent(newText).cx;
			//p->drawText(x, y, width, mRowHeight, Qt::AlignVCenter, newText);
			painter->DrawText(newText, CRect(x, y, x + width, y + mRowHeight), DT_VCENTER);
		}

		// do we have a label ?
		if (mLABELDISPLAY.find(reg) != mLABELDISPLAY.end())
		{
			x += 5 * mCharWidth; //5 spaces

			CString newText = getRegisterLabel(reg);

			// are there additional informations?
			if (newText != "")
			{
				width = painter->GetTextExtent(newText).cx;
				painter->SetTextColor(ConfigColor("RegistersExtraInfoColor").ToCOLORREF());
				painter->DrawText(newText, CRect(x, y, x + width, y + mRowHeight), DT_VCENTER | DT_NOPREFIX);
				//p->drawText(x,mRowHeight*(mRegisterPlaces[reg].line+1),newText);
			}
		}
	}
}

void CRegistersView::updateRegistersSlot()
{
	// read registers
	REGDUMP z;
	DbgGetRegDumpEx(&z, sizeof(REGDUMP));
	// update gui
	initScorll();
	setRegisters(&z);
}

void CRegistersView::initScorll()
{
	int Rows = 0;
	int charCount = 0;
	if (DbgIsDebugging())
	{
		Rows = mRowsNeeded;
		charCount = mCharCount;
	}
	CRect viewRect;
	GetClientRect(&viewRect);
	
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	memset(&si, 0, si.cbSize);
	GetScrollInfo(SB_VERT, &si);
	float ratio = (float)si.nPos / (float)(si.nMax - si.nPage);
	si.fMask = SIF_RANGE | SIF_PAGE;
	si.nMin = 0;
	si.nMax = Rows - 1;
	si.nPage = viewRect.Height() / getRowHeight();
	//si.nPos = ratio * (si.nMax - si.nPage);
	SetScrollInfo(SB_VERT, &si);		

	TEXTMETRIC  tm;
	int cxChar = 7;
	GetTextMetrics(::GetDC(GetSafeHwnd()), &tm);
	cxChar = tm.tmAveCharWidth;

	si.nMin = 0;
	si.nMax = charCount;
	si.nPage = viewRect.Width() / cxChar;
	SetScrollInfo(SB_HORZ, &si);	
}

/**
* @brief retrieves the register id from given corrdinates of the viewport
* @param line
* @param offset (x-coord)
* @param resulting register-id
* @return true if register found
*/
bool CRegistersView::identifyRegister(const int line, const int offset, REGISTER_NAME* clickedReg)
{
	// we start by an unknown register id
	if (clickedReg)
		*clickedReg = UNKNOWN;
	bool found_flag = false;
	map<REGISTER_NAME, Register_Position>::const_iterator it = mRegisterPlaces.begin();
	// iterate all registers that being displayed
	while (it != mRegisterPlaces.end())
	{
		if ((it->second.line == (line - mVScrollOffset))    /* same line ? */
			&& ((1 + it->second.start) <= offset)   /* between start ... ? */
			&& (offset <= (1 + it->second.start + it->second.labelwidth + it->second.valuesize)) /* ... and end ? */
			)
		{
			// we found a matching register in the viewport
			if (clickedReg)
				*clickedReg = (REGISTER_NAME)it->first;
			found_flag = true;
			break;

		}
		++it;
	}
	return found_flag;
}

void CRegistersView::ModifyFields(const CString & title, STRING_VALUE_TABLE_t* table, SIZE_T size)
{
	/*
	SelectFields mSelectFields(this);
	QListWidget* mQListWidget = mSelectFields.GetList();

	QStringList items;
	unsigned int i;

	for (i = 0; i < size; i++)
		items << QApplication::translate("RegistersView_ConstantsOfRegisters", table[i].string) + QString(" (%1)").arg(table[i].value, 0, 16);

	mQListWidget->addItems(items);

	mSelectFields.setWindowTitle(title);
	if (mSelectFields.exec() != QDialog::Accepted)
		return;

	if (mQListWidget->selectedItems().count() != 1)
		return;

	QListWidgetItem* item = mQListWidget->takeItem(mQListWidget->currentRow());

	duint value;

	for (i = 0; i < size; i++)
	{
		if (QApplication::translate("RegistersView_ConstantsOfRegisters", table[i].string) + QString(" (%1)").arg(table[i].value, 0, 16) == item->text())
			break;
	}
	
	value = table[i].value;

	setRegister(mSelected, (duint)value);
	delete item;
	*/
}
#define SIZE_TABLE(table) (sizeof(table) / sizeof(*table))
#define MODIFY_FIELDS_DISPLAY(prefix, title, table) ModifyFields((CString)prefix + ' ' + CString(title), (STRING_VALUE_TABLE_t *) & table, SIZE_TABLE(table) )

/**
* @brief   This function displays the appropriate edit dialog according to selected register
* @return  Nothing.
*/

void CRegistersView::displayEditDialog()
{
	if (mFPU.count(mSelected))
	{
		if (mTAGWORD.count(mSelected))
		{
			CString title = _T("Edit Tag ");
			title += mRegisterMapping[mSelected].data();
			ModifyFields(title, (STRING_VALUE_TABLE_t *)& TagWordValueStringTable, SIZE_TABLE(TagWordValueStringTable));
		}
		/*	
		else if (mSelected == MxCsr_RC)
			MODIFY_FIELDS_DISPLAY(tr("Edit"), "MxCsr_RC", MxCsrRCValueStringTable);
		else if (mSelected == x87CW_RC)
			MODIFY_FIELDS_DISPLAY(tr("Edit"), "x87CW_RC", ControlWordRCValueStringTable);
		else if (mSelected == x87CW_PC)
			MODIFY_FIELDS_DISPLAY(tr("Edit"), "x87CW_PC", ControlWordPCValueStringTable);
		else if (mSelected == x87SW_TOP)
			MODIFY_FIELDS_DISPLAY(tr("Edit"), "x87SW_TOP", StatusWordTOPValueStringTable);
			
		else if (mFPUYMM.count(mSelected))
		{
			EditFloatRegister mEditFloat(256, this);
			mEditFloat.setWindowTitle(tr("Edit YMM register"));
			mEditFloat.loadData(registerValue(&wRegDumpStruct, mSelected));
			mEditFloat.show();
			mEditFloat.selectAllText();
			if (mEditFloat.exec() == QDialog::Accepted)
				setRegister(mSelected, (duint)mEditFloat.getData());
		}
		else if (mFPUXMM.count(mSelected))
		{
			EditFloatRegister mEditFloat(128, this);
			mEditFloat.setWindowTitle(tr("Edit XMM register"));
			mEditFloat.loadData(registerValue(&wRegDumpStruct, mSelected));
			mEditFloat.show();
			mEditFloat.selectAllText();
			if (mEditFloat.exec() == QDialog::Accepted)
				setRegister(mSelected, (duint)mEditFloat.getData());
		}
		else if (mFPUMMX.count(mSelected))
		{
			EditFloatRegister mEditFloat(64, this);
			mEditFloat.setWindowTitle(tr("Edit MMX register"));
			mEditFloat.loadData(registerValue(&wRegDumpStruct, mSelected));
			mEditFloat.show();
			mEditFloat.selectAllText();
			if (mEditFloat.exec() == QDialog::Accepted)
				setRegister(mSelected, (duint)mEditFloat.getData());
		}
		*/
		else
		{
			bool errorinput = false;
			
			CString editText = GetRegStringValueFromValue(mSelected, registerValue(&wRegDumpStruct, mSelected));
			
			CString title = _T("Edit FPU register");
			CLineEditDlg LineEditDlg(this, title, editText);

			auto sizeRegister = int(GetSizeRegister(mSelected));
			if (sizeRegister == 10)
				LineEditDlg.setFpuMode();
			LineEditDlg.ForceSize(sizeRegister * 2);
			do
			{
				errorinput = false;
				if (LineEditDlg.DoModal() != IDOK)
					return; //pressed cancel
				else
				{
					bool ok = false;
					duint fpuvalue;

					if (mUSHORTDISPLAY.count(mSelected))
					{
						//fpuvalue = (duint)mLineEdit.editText.toUShort(&ok, 16);
						std::string s;
						s = CT2A(LineEditDlg.m_editText);
						fpuvalue = strtol(s.data(), NULL, 16);
						fpuvalue = StrToInt(LineEditDlg.m_editText);
						fpuvalue = atoi(s.data());
						fpuvalue = _ttoi(LineEditDlg.m_editText);
					}
						
					else if (mDWORDDISPLAY.count(mSelected))
						//fpuvalue = mLineEdit.editText.toUInt(&ok, 16);
						fpuvalue = _ttoi(LineEditDlg.m_editText);
					else if (mFPUx87_80BITSDISPLAY.count(mSelected))
					{
						CString editTextLower = LineEditDlg.m_editText;
						editTextLower.MakeLower();
						if (sizeRegister == 10 && ((LineEditDlg.m_editText.Find('.') !=  -1) || editTextLower == "nan" || editTextLower == "inf"
							|| editTextLower == "+inf" || editTextLower == "-inf"))
						{
							char number[10];
							std::string s;
							s = CT2A(LineEditDlg.m_editText);
							str2ld(s.data(), number);
							setRegister(mSelected, reinterpret_cast<duint>(number));
							return;
						}
						else
						{
							CByteArray pArray;
							string s;
							s = CT2A(LineEditDlg.m_editText);
							int size = s.size();
							pArray.SetSize(size);
							for (int i = 0; i < size; i++)
							{
								pArray[i] = s[i];
							}

							if (pArray.GetSize() == sizeRegister * 2)
							{
								char* pData = (char*)calloc(1, sizeof(char) * sizeRegister);

								if (pData != NULL)
								{
									ok = true;
									char actual_char[3];
									for (int i = 0; i < sizeRegister; i++)
									{
										memset(actual_char, 0, sizeof(actual_char));
										memcpy(actual_char, (char*)pArray.GetData() + (i * 2), 2);
										if (!isxdigit(actual_char[0]) || !isxdigit(actual_char[1]))
										{
											ok = false;
											break;
										}
										pData[i] = (char)strtol(actual_char, NULL, 16);
									}

									if (ok)
									{
										if (!ConfigBool("Gui", "FpuRegistersLittleEndian")) // reverse byte order if it is big-endian
										{
											//pArray = ByteReverse(QByteArray(pData, sizeRegister));
											CByteArray byteArray;
											byteArray.SetSize(sizeRegister);
											for (int i = 0; i < sizeRegister; i++)
											{
												byteArray[i] = pData[sizeRegister - i - 1];
											}
											
											setRegister(mSelected, reinterpret_cast<duint>(byteArray.GetData()));
										}
										else
											setRegister(mSelected, reinterpret_cast<duint>(pData));
									}

									free(pData);

									if (ok)
										return;
								}
							}
						}
					}
					if (!ok)
					{
						errorinput = true;
						//SimpleWarningBox(this, tr("ERROR CONVERTING TO HEX"), tr("ERROR CONVERTING TO HEX"));
						MessageBox(_T("ERROR CONVERTING TO HEX!"), _T("ERROR CONVERTING!"), MB_OKCANCEL);
					}
					else
						setRegister(mSelected, fpuvalue);
				}
			} while (errorinput);
		}		
	}
	
	else if (mSelected == LastError)
	{
		bool errorinput = false;
		LASTERROR* error = (LASTERROR*)registerValue(&wRegDumpStruct, LastError);
		CString editText;
		editText.Format(_T("%d"), error->code);
		CString title = _T("Set Last Error");
		CLineEditDlg LineEditDlg(this, title, editText);
		do
		{
			errorinput = true;
			if (LineEditDlg.DoModal() == IDCANCEL)
				return;
			string editText;
			editText = CT2A(LineEditDlg.m_editText);
			if (DbgIsValidExpression(editText.data()))
				errorinput = false;
		} while (errorinput);
		setRegister(LastError, DbgValFromString(CT2A(LineEditDlg.m_editText)));
	}
	
	else if (mSelected == LastStatus)
	{
		LASTSTATUS* status = (LASTSTATUS*)registerValue(&wRegDumpStruct, LastStatus);
		CString editText;
		editText.Format(_T("%d"), status->code);
		CString title = _T("Set Last Status");
		CLineEditDlg LineEditDlg(this, title, editText);		
		if (LineEditDlg.DoModal() == IDOK)
		{			
			string editText;
			editText = CT2A(LineEditDlg.m_editText);
			if (DbgIsValidExpression(editText.data()))
				setRegister(LastStatus, DbgValFromString(editText.data()));
		}		
	}
	
	else
	{
		/*
		WordEditDialog wEditDial(this);
		wEditDial.setup(tr("Edit"), (*((duint*)registerValue(&wRegDumpStruct, mSelected))), sizeof(dsint));
		if (wEditDial.exec() == QDialog::Accepted) //OK button clicked
			setRegister(mSelected, wEditDial.getVal());
		*/		
		CString title = _T("修改");
		title += mRegisterMapping[mSelected].data();

		CWordEditDlg modifyDlg(title, (*((duint*)registerValue(&wRegDumpStruct, mSelected))), sizeof(dsint), this);
		if (modifyDlg.DoModal() == IDOK)
			setRegister(mSelected, modifyDlg.getValue());
	}
}

void CRegistersView::onChangeFPUViewAction()
{
	if (mShowFpu == true)
		ShowFPU(false);
	else
		ShowFPU(true);
}

void CRegistersView::appendRegister(CString & text, REGISTER_NAME reg, const char* name64, const char* name32)
{
	CString symbol;
#ifdef _WIN64
	UNUSED(name32);
	text += name64;
#else //x86
	UNUSED(name64);
	text += name32;
#endif //_WIN64
	text += GetRegStringValueFromValue(reg, registerValue(&wRegDumpStruct, reg));
	symbol = getRegisterLabel(reg);
	if (symbol != "")
	{
		text += "     ";
		text += symbol;
	}
	text += ("\r\n");
}

bool CRegistersView::setTextToClipboard(CString& text)
{
	if (OpenClipboard())
	{
		EmptyClipboard();
		int len = text.GetLength() + 1;
		HGLOBAL hglbCopy = GlobalAlloc(0, len * sizeof(TCHAR));
		if (hglbCopy == NULL)
		{
			CloseClipboard();
			return false;
		}
		LPTSTR lptstrCopy = (LPTSTR)GlobalLock(hglbCopy);
		memcpy(lptstrCopy, text.GetBuffer(),
			len * sizeof(TCHAR));
		lptstrCopy[len] = (TCHAR)0;    // null character 
		GlobalUnlock(hglbCopy);
		SetClipboardData(CF_UNICODETEXT, hglbCopy);
		CloseClipboard();
		return true;
	}
	return false;
}

void CRegistersView::onCopy()
{
	CString text = GetRegStringValueFromValue(mSelected, registerValue(&wRegDumpStruct, mSelected));
	setTextToClipboard(text);
}

void CRegistersView::onCopyAll()
{
	CString text;
	appendRegister(text, REGISTER_NAME::CAX, "RAX : ", "EAX : ");
	appendRegister(text, REGISTER_NAME::CBX, "RBX : ", "EBX : ");
	appendRegister(text, REGISTER_NAME::CCX, "RCX : ", "ECX : ");
	appendRegister(text, REGISTER_NAME::CDX, "RDX : ", "EDX : ");
	appendRegister(text, REGISTER_NAME::CBP, "RBP : ", "EBP : ");
	appendRegister(text, REGISTER_NAME::CSP, "RSP : ", "ESP : ");
	appendRegister(text, REGISTER_NAME::CSI, "RSI : ", "ESI : ");
	appendRegister(text, REGISTER_NAME::CDI, "RDI : ", "EDI : ");
#ifdef _WIN64
	appendRegister(text, REGISTER_NAME::R8, "R8  : ", "R8  : ");
	appendRegister(text, REGISTER_NAME::R9, "R9  : ", "R9  : ");
	appendRegister(text, REGISTER_NAME::R10, "R10 : ", "R10 : ");
	appendRegister(text, REGISTER_NAME::R11, "R11 : ", "R11 : ");
	appendRegister(text, REGISTER_NAME::R12, "R12 : ", "R12 : ");
	appendRegister(text, REGISTER_NAME::R13, "R13 : ", "R13 : ");
	appendRegister(text, REGISTER_NAME::R14, "R14 : ", "R14 : ");
	appendRegister(text, REGISTER_NAME::R15, "R15 : ", "R15 : ");
#endif
	appendRegister(text, REGISTER_NAME::CIP, "RIP : ", "EIP : ");
	appendRegister(text, REGISTER_NAME::EFLAGS, "RFLAGS : ", "EFLAGS : ");
	appendRegister(text, REGISTER_NAME::ZF, "ZF : ", "ZF : ");
	appendRegister(text, REGISTER_NAME::OF, "OF : ", "OF : ");
	appendRegister(text, REGISTER_NAME::CF, "CF : ", "CF : ");
	appendRegister(text, REGISTER_NAME::PF, "PF : ", "PF : ");
	appendRegister(text, REGISTER_NAME::SF, "SF : ", "SF : ");
	appendRegister(text, REGISTER_NAME::TF, "TF : ", "TF : ");
	appendRegister(text, REGISTER_NAME::AF, "AF : ", "AF : ");
	appendRegister(text, REGISTER_NAME::DF, "DF : ", "DF : ");
	appendRegister(text, REGISTER_NAME::IF, "IF : ", "IF : ");
	appendRegister(text, REGISTER_NAME::LastError, "LastError : ", "LastError : ");
	appendRegister(text, REGISTER_NAME::LastStatus, "LastStatus : ", "LastStatus : ");
	appendRegister(text, REGISTER_NAME::GS, "GS : ", "GS : ");
	appendRegister(text, REGISTER_NAME::ES, "ES : ", "ES : ");
	appendRegister(text, REGISTER_NAME::CS, "CS : ", "CS : ");
	appendRegister(text, REGISTER_NAME::FS, "FS : ", "FS : ");
	appendRegister(text, REGISTER_NAME::DS, "DS : ", "DS : ");
	appendRegister(text, REGISTER_NAME::SS, "SS : ", "SS : ");
	if (mShowFpu)
	{
		appendRegister(text, REGISTER_NAME::x87r0, "x87r0 : ", "x87r0 : ");
		appendRegister(text, REGISTER_NAME::x87r1, "x87r1 : ", "x87r1 : ");
		appendRegister(text, REGISTER_NAME::x87r2, "x87r2 : ", "x87r2 : ");
		appendRegister(text, REGISTER_NAME::x87r3, "x87r3 : ", "x87r3 : ");
		appendRegister(text, REGISTER_NAME::x87r4, "x87r4 : ", "x87r4 : ");
		appendRegister(text, REGISTER_NAME::x87r5, "x87r5 : ", "x87r5 : ");
		appendRegister(text, REGISTER_NAME::x87r6, "x87r6 : ", "x87r6 : ");
		appendRegister(text, REGISTER_NAME::x87r7, "x87r7 : ", "x87r7 : ");
		appendRegister(text, REGISTER_NAME::x87TagWord, "x87TagWord : ", "x87TagWord : ");
		appendRegister(text, REGISTER_NAME::x87ControlWord, "x87ControlWord : ", "x87ControlWord : ");
		appendRegister(text, REGISTER_NAME::x87StatusWord, "x87StatusWord : ", "x87StatusWord : ");
		appendRegister(text, REGISTER_NAME::x87TW_0, "x87TW_0 : ", "x87TW_0 : ");
		appendRegister(text, REGISTER_NAME::x87TW_1, "x87TW_1 : ", "x87TW_1 : ");
		appendRegister(text, REGISTER_NAME::x87TW_2, "x87TW_2 : ", "x87TW_2 : ");
		appendRegister(text, REGISTER_NAME::x87TW_3, "x87TW_3 : ", "x87TW_3 : ");
		appendRegister(text, REGISTER_NAME::x87TW_4, "x87TW_4 : ", "x87TW_4 : ");
		appendRegister(text, REGISTER_NAME::x87TW_5, "x87TW_5 : ", "x87TW_5 : ");
		appendRegister(text, REGISTER_NAME::x87TW_6, "x87TW_6 : ", "x87TW_6 : ");
		appendRegister(text, REGISTER_NAME::x87TW_7, "x87TW_7 : ", "x87TW_7 : ");
		appendRegister(text, REGISTER_NAME::x87SW_B, "x87SW_B : ", "x87SW_B : ");
		appendRegister(text, REGISTER_NAME::x87SW_C3, "x87SW_C3 : ", "x87SW_C3 : ");
		appendRegister(text, REGISTER_NAME::x87SW_TOP, "x87SW_TOP : ", "x87SW_TOP : ");
		appendRegister(text, REGISTER_NAME::x87SW_C2, "x87SW_C2 : ", "x87SW_C2 : ");
		appendRegister(text, REGISTER_NAME::x87SW_C1, "x87SW_C1 : ", "x87SW_C1 : ");
		appendRegister(text, REGISTER_NAME::x87SW_O, "x87SW_O : ", "x87SW_O : ");
		appendRegister(text, REGISTER_NAME::x87SW_ES, "x87SW_ES : ", "x87SW_ES : ");
		appendRegister(text, REGISTER_NAME::x87SW_SF, "x87SW_SF : ", "x87SW_SF : ");
		appendRegister(text, REGISTER_NAME::x87SW_P, "x87SW_P : ", "x87SW_P : ");
		appendRegister(text, REGISTER_NAME::x87SW_U, "x87SW_U : ", "x87SW_U : ");
		appendRegister(text, REGISTER_NAME::x87SW_Z, "x87SW_Z : ", "x87SW_Z : ");
		appendRegister(text, REGISTER_NAME::x87SW_D, "x87SW_D : ", "x87SW_D : ");
		appendRegister(text, REGISTER_NAME::x87SW_I, "x87SW_I : ", "x87SW_I : ");
		appendRegister(text, REGISTER_NAME::x87SW_C0, "x87SW_C0 : ", "x87SW_C0 : ");
		appendRegister(text, REGISTER_NAME::x87CW_IC, "x87CW_IC : ", "x87CW_IC : ");
		appendRegister(text, REGISTER_NAME::x87CW_RC, "x87CW_RC : ", "x87CW_RC : ");
		appendRegister(text, REGISTER_NAME::x87CW_PC, "x87CW_PC : ", "x87CW_PC : ");
		appendRegister(text, REGISTER_NAME::x87CW_PM, "x87CW_PM : ", "x87CW_PM : ");
		appendRegister(text, REGISTER_NAME::x87CW_UM, "x87CW_UM : ", "x87CW_UM : ");
		appendRegister(text, REGISTER_NAME::x87CW_OM, "x87CW_OM : ", "x87CW_OM : ");
		appendRegister(text, REGISTER_NAME::x87CW_ZM, "x87CW_ZM : ", "x87CW_ZM : ");
		appendRegister(text, REGISTER_NAME::x87CW_DM, "x87CW_DM : ", "x87CW_DM : ");
		appendRegister(text, REGISTER_NAME::x87CW_IM, "x87CW_IM : ", "x87CW_IM : ");
		appendRegister(text, REGISTER_NAME::MxCsr, "MxCsr : ", "MxCsr : ");
		appendRegister(text, REGISTER_NAME::MxCsr_FZ, "MxCsr_FZ : ", "MxCsr_FZ : ");
		appendRegister(text, REGISTER_NAME::MxCsr_PM, "MxCsr_PM : ", "MxCsr_PM : ");
		appendRegister(text, REGISTER_NAME::MxCsr_UM, "MxCsr_UM : ", "MxCsr_UM : ");
		appendRegister(text, REGISTER_NAME::MxCsr_OM, "MxCsr_OM : ", "MxCsr_OM : ");
		appendRegister(text, REGISTER_NAME::MxCsr_ZM, "MxCsr_ZM : ", "MxCsr_ZM : ");
		appendRegister(text, REGISTER_NAME::MxCsr_IM, "MxCsr_IM : ", "MxCsr_IM : ");
		appendRegister(text, REGISTER_NAME::MxCsr_DM, "MxCsr_DM : ", "MxCsr_DM : ");
		appendRegister(text, REGISTER_NAME::MxCsr_DAZ, "MxCsr_DAZ : ", "MxCsr_DAZ : ");
		appendRegister(text, REGISTER_NAME::MxCsr_PE, "MxCsr_PE : ", "MxCsr_PE : ");
		appendRegister(text, REGISTER_NAME::MxCsr_UE, "MxCsr_UE : ", "MxCsr_UE : ");
		appendRegister(text, REGISTER_NAME::MxCsr_OE, "MxCsr_OE : ", "MxCsr_OE : ");
		appendRegister(text, REGISTER_NAME::MxCsr_ZE, "MxCsr_ZE : ", "MxCsr_ZE : ");
		appendRegister(text, REGISTER_NAME::MxCsr_DE, "MxCsr_DE : ", "MxCsr_DE : ");
		appendRegister(text, REGISTER_NAME::MxCsr_IE, "MxCsr_IE : ", "MxCsr_IE : ");
		appendRegister(text, REGISTER_NAME::MxCsr_RC, "MxCsr_RC : ", "MxCsr_RC : ");
		appendRegister(text, REGISTER_NAME::MM0, "MM0 : ", "MM0 : ");
		appendRegister(text, REGISTER_NAME::MM1, "MM1 : ", "MM1 : ");
		appendRegister(text, REGISTER_NAME::MM2, "MM2 : ", "MM2 : ");
		appendRegister(text, REGISTER_NAME::MM3, "MM3 : ", "MM3 : ");
		appendRegister(text, REGISTER_NAME::MM4, "MM4 : ", "MM4 : ");
		appendRegister(text, REGISTER_NAME::MM5, "MM5 : ", "MM5 : ");
		appendRegister(text, REGISTER_NAME::MM6, "MM6 : ", "MM6 : ");
		appendRegister(text, REGISTER_NAME::MM7, "MM7 : ", "MM7 : ");
		appendRegister(text, REGISTER_NAME::XMM0, "XMM0  : ", "XMM0  : ");
		appendRegister(text, REGISTER_NAME::XMM1, "XMM1  : ", "XMM1  : ");
		appendRegister(text, REGISTER_NAME::XMM2, "XMM2  : ", "XMM2  : ");
		appendRegister(text, REGISTER_NAME::XMM3, "XMM3  : ", "XMM3  : ");
		appendRegister(text, REGISTER_NAME::XMM4, "XMM4  : ", "XMM4  : ");
		appendRegister(text, REGISTER_NAME::XMM5, "XMM5  : ", "XMM5  : ");
		appendRegister(text, REGISTER_NAME::XMM6, "XMM6  : ", "XMM6  : ");
		appendRegister(text, REGISTER_NAME::XMM7, "XMM7  : ", "XMM7  : ");
#ifdef _WIN64
		appendRegister(text, REGISTER_NAME::XMM8, "XMM8  : ", "XMM8  : ");
		appendRegister(text, REGISTER_NAME::XMM9, "XMM9  : ", "XMM9  : ");
		appendRegister(text, REGISTER_NAME::XMM10, "XMM10 : ", "XMM10 : ");
		appendRegister(text, REGISTER_NAME::XMM11, "XMM11 : ", "XMM11 : ");
		appendRegister(text, REGISTER_NAME::XMM12, "XMM12 : ", "XMM12 : ");
		appendRegister(text, REGISTER_NAME::XMM13, "XMM13 : ", "XMM13 : ");
		appendRegister(text, REGISTER_NAME::XMM14, "XMM14 : ", "XMM14 : ");
		appendRegister(text, REGISTER_NAME::XMM15, "XMM15 : ", "XMM15 : ");
#endif
		appendRegister(text, REGISTER_NAME::YMM0, "YMM0  : ", "YMM0  : ");
		appendRegister(text, REGISTER_NAME::YMM1, "YMM1  : ", "YMM1  : ");
		appendRegister(text, REGISTER_NAME::YMM2, "YMM2  : ", "YMM2  : ");
		appendRegister(text, REGISTER_NAME::YMM3, "YMM3  : ", "YMM3  : ");
		appendRegister(text, REGISTER_NAME::YMM4, "YMM4  : ", "YMM4  : ");
		appendRegister(text, REGISTER_NAME::YMM5, "YMM5  : ", "YMM5  : ");
		appendRegister(text, REGISTER_NAME::YMM6, "YMM6  : ", "YMM6  : ");
		appendRegister(text, REGISTER_NAME::YMM7, "YMM7  : ", "YMM7  : ");
#ifdef _WIN64
		appendRegister(text, REGISTER_NAME::YMM8, "YMM8  : ", "YMM8  : ");
		appendRegister(text, REGISTER_NAME::YMM9, "YMM9  : ", "YMM9  : ");
		appendRegister(text, REGISTER_NAME::YMM10, "YMM10 : ", "YMM10 : ");
		appendRegister(text, REGISTER_NAME::YMM11, "YMM11 : ", "YMM11 : ");
		appendRegister(text, REGISTER_NAME::YMM12, "YMM12 : ", "YMM12 : ");
		appendRegister(text, REGISTER_NAME::YMM13, "YMM13 : ", "YMM13 : ");
		appendRegister(text, REGISTER_NAME::YMM14, "YMM14 : ", "YMM14 : ");
		appendRegister(text, REGISTER_NAME::YMM15, "YMM15 : ", "YMM15 : ");
#endif
	}
	appendRegister(text, REGISTER_NAME::DR0, "DR0 : ", "DR0 : ");
	appendRegister(text, REGISTER_NAME::DR1, "DR1 : ", "DR1 : ");
	appendRegister(text, REGISTER_NAME::DR2, "DR2 : ", "DR2 : ");
	appendRegister(text, REGISTER_NAME::DR3, "DR3 : ", "DR3 : ");
	appendRegister(text, REGISTER_NAME::DR6, "DR6 : ", "DR6 : ");
	appendRegister(text, REGISTER_NAME::DR7, "DR7 : ", "DR7 : ");

	/*
	auto clipboard = QApplication::clipboard();
	clipboard->setText(text);
	*/
	setTextToClipboard(text);	
}

BEGIN_MESSAGE_MAP(CRegistersView, CWnd)
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_CONTEXTMENU()
	ON_COMMAND_RANGE(ID_REGISTERS_ChangeFPUView, ID_REGISTERS_SetHBP, &CRegistersView::OnUnknownHandle)
	ON_COMMAND_RANGE(ID_REGISTERS_ModifyValue, ID_REGISTERS_Decrementx87Stack, &CRegistersView::OnLocalHandle)
END_MESSAGE_MAP()



// CRegistersView 消息处理程序




void CRegistersView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CWnd::OnPaint()
	/*
	if (mChangeViewButton != NULL)
	{
		if (mShowFpu)
			mChangeViewButton->setText(tr("Hide FPU"));
		else
			mChangeViewButton->setText(tr("Show FPU"));
	}
	*/

	CRect viewRect;
	GetClientRect(&viewRect);

	CDC MenDC;
	CBitmap MemMap;
	MenDC.CreateCompatibleDC(&dc);

	CFont font;
	VERIFY(font.CreateFontIndirect(&lf));
	MenDC.SelectObject(&font);	
	
	TEXTMETRIC tm;
	MenDC.GetTextMetrics(&tm);
	mCharWidth = tm.tmAveCharWidth;
	int totalWidth = (mCharCount + 1) * mCharWidth;
	int totalHight = (mRowsNeeded + 1)* getRowHeight();
	CRect drawRect = viewRect;
	if (totalWidth > viewRect.Width())
		drawRect.right = totalWidth;
	if (totalHight > viewRect.Height())
		drawRect.bottom = totalHight;
	MemMap.CreateCompatibleBitmap(&dc, drawRect.Width(), drawRect.Height());
	MenDC.SelectObject(&MemMap);
	MenDC.SetBkMode(TRANSPARENT);	
	MenDC.FillSolidRect(&drawRect, ConfigColor("RegistersBackgroundColor").ToCOLORREF());
	

	// Don't draw the registers if a program isn't actually running
	if (DbgIsDebugging())
	{
		// Iterate all registers
		for (auto itr = mRegisterMapping.begin(); itr != mRegisterMapping.end(); itr++)
		{
			// Paint register at given position			
			
			drawRegister(&MenDC, itr->first, registerValue(&wRegDumpStruct, itr->first));
		}		
	}
	hScrollValue = mCharWidth * GetScrollPos(SB_HORZ);
	vScrollValue = getRowHeight() * GetScrollPos(SB_VERT);
	//dc.FillSolidRect(&viewRect, ConfigColor("RegistersBackgroundColor").ToCOLORREF());
	dc.BitBlt(0, 0, viewRect.Width(), viewRect.Height(), &MenDC, hScrollValue, vScrollValue, SRCCOPY);	
}


void CRegistersView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	dsint nDelta = 0;
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	GetScrollInfo(SB_VERT, &si);
	switch (nSBCode)
	{
	case SB_TOP:
		nDelta = si.nMin - si.nPos;
		break;
	case SB_BOTTOM:
		nDelta = si.nMax - si.nPos;
		break;

	case SB_LINEUP:
		nDelta = -1;
		break;

	case SB_LINEDOWN:
		nDelta = 1;
		break;

	case SB_PAGEUP:
		nDelta -= si.nPage;
		break;

	case SB_PAGEDOWN:
		nDelta = si.nPage;
		break;

	case SB_THUMBTRACK:
		//si.nPos = si.nTrackPos;
		nDelta = si.nTrackPos - si.nPos;
		break;

	default:
		return;
	}

	if (nDelta != 0)
	{
		
		int wSliderPos = si.nPos + nDelta;
		wSliderPos = wSliderPos > GetScrollLimit(SB_VERT) ? GetScrollLimit(SB_VERT) : wSliderPos;
		wSliderPos = wSliderPos < 0 ? 0 : wSliderPos;

		int nLastPos = si.nPos;
		si.nPos = wSliderPos;

		si.fMask = SIF_POS;
		SetScrollInfo(SB_VERT, &si);
		if (nLastPos != GetScrollPos(SB_VERT))
		{
			//ScrollWindow(0, -nDelta * getRowHeight());
			Invalidate();
		}			
	}
	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CRegistersView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	dsint nDelta = 0;
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	GetScrollInfo(SB_HORZ, &si);
	switch (nSBCode)
	{
	case SB_TOP:
		nDelta = si.nMin - si.nPos;
		break;
	case SB_BOTTOM:
		nDelta = si.nMax - si.nPos;
		break;

	case SB_LINEUP:
		nDelta = -1;
		break;

	case SB_LINEDOWN:
		nDelta = 1;
		break;

	case SB_PAGEUP:
		nDelta -= si.nPage;
		break;

	case SB_PAGEDOWN:
		nDelta = si.nPage;
		break;

	case SB_THUMBTRACK:
		//si.nPos = si.nTrackPos;
		nDelta = si.nTrackPos - si.nPos;
		break;

	default:
		return;
	}

	if (nDelta != 0)
	{
		int wSliderPos = si.nPos + nDelta;
		wSliderPos = wSliderPos > GetScrollLimit(SB_HORZ) ? GetScrollLimit(SB_HORZ) : wSliderPos;
		wSliderPos = wSliderPos < 0 ? 0 : wSliderPos;

		int nLastPos = si.nPos;
		si.nPos = wSliderPos;

		si.fMask = SIF_POS;
		SetScrollInfo(SB_HORZ, &si);
		if (nLastPos != GetScrollPos(SB_HORZ))
		{
			//ScrollWindow(0, -nDelta * 7);
			Invalidate();
		}
	}

	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}


int CRegistersView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	TEXTMETRIC  tm;
	GetTextMetrics(::GetDC(GetSafeHwnd()), &tm);
	mCharWidth = tm.tmAveCharWidth;
	int cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * mCharWidth / 2;
	int mCharWidth = tm.tmHeight + tm.tmExternalLeading;

	return 0;
}


void CRegistersView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	initScorll();
}


BOOL CRegistersView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (zDelta)
	{
	case WHEEL_DELTA:
		SendMessage(WM_VSCROLL, SB_LINEUP, 0);
		break;
	case -WHEEL_DELTA:
		SendMessage(WM_VSCROLL, SB_LINEDOWN, 0);
		break;
	default:
		break;
	}

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}


void CRegistersView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (!DbgIsDebugging())
		return;
	// get mouse position
	const int y = (point.y + +vScrollValue - yTopSpacing) / getRowHeight();
	const int x = (point.x + hScrollValue) / mCharWidth;

	// do we find a corresponding register?
	if (!identifyRegister(y, x, 0))
		return;
	if (mSelected == CIP) //double clicked on CIP register
		DbgCmdExec("disasm cip");
	// is current register general purposes register or FPU register?
	else if (mMODIFYDISPLAY.count(mSelected))		
	{
		//wCM_Modify->trigger();
		displayEditDialog();
	}
	else if (mBOOLDISPLAY.count(mSelected)) // is flag ?
	{
		////wCM_ToggleValue->trigger();
		if (mBOOLDISPLAY.count(mSelected))
		{
			int value = (int)(*(bool*)registerValue(&wRegDumpStruct, mSelected));
			setRegister(mSelected, value ^ 1);
		}
	}
	else if (mCANSTOREADDRESS.count(mSelected))
	{
		//wCM_FollowInDisassembly->trigger();
		onFollowInDisassembly();
	}
	CWnd::OnLButtonDblClk(nFlags, point);
}

void CRegistersView::OnButtonDown(UINT nFlags, CPoint point)
{
	if (!DbgIsDebugging())
		return;
	/*
	if (event->y() < yTopSpacing - mButtonHeight)
	{
	onChangeFPUViewAction();
	}
	*/

	// get mouse position
	const int y = (point.y + vScrollValue - yTopSpacing) / getRowHeight();
	const int x = (point.x + hScrollValue) / mCharWidth;

	REGISTER_NAME r;
	// do we find a corresponding register?

	if (identifyRegister(y, x, &r))
	{
		CDisassembly* pCDisassembly = ((CMainFrame*)AfxGetApp()->GetMainWnd())->getCDisasmPane()->getCDisassembly();

		if (pCDisassembly->isHighlightMode())
		{
			if (mGPR.count(r) && r != REGISTER_NAME::EFLAGS)
				pCDisassembly->hightlightToken(CapstoneTokenizer::SingleToken(CapstoneTokenizer::TokenType::GeneralRegister, (CString)mRegisterMapping[r].data()));
			else if (mFPUMMX.count(r))
				pCDisassembly->hightlightToken(CapstoneTokenizer::SingleToken(CapstoneTokenizer::TokenType::MmxRegister, (CString)mRegisterMapping[r].data()));
			else if (mFPUXMM.count(r))
				pCDisassembly->hightlightToken(CapstoneTokenizer::SingleToken(CapstoneTokenizer::TokenType::XmmRegister, (CString)mRegisterMapping[r].data()));
			else if (mFPUYMM.count(r))
				pCDisassembly->hightlightToken(CapstoneTokenizer::SingleToken(CapstoneTokenizer::TokenType::YmmRegister, (CString)mRegisterMapping[r].data()));
			else if (mSEGMENTREGISTER.count(r))
				pCDisassembly->hightlightToken(CapstoneTokenizer::SingleToken(CapstoneTokenizer::TokenType::MemorySegment, (CString)mRegisterMapping[r].data()));
			else
				mSelected = r;
		}
		else
			mSelected = r;
		Invalidate();
	}
	else
		mSelected = UNKNOWN;
}

void CRegistersView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	OnButtonDown(nFlags, point);
	CWnd::OnLButtonDown(nFlags, point);
}


void CRegistersView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CWnd::OnRButtonUp(nFlags, point);
}


void CRegistersView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	OnButtonDown(nFlags, point);
	CWnd::OnRButtonDown(nFlags, point);
}

void CRegistersView::OnUnknownHandle(UINT id)
{
	switch (id)
	{
	case ID_REGISTERS_ChangeFPUView:
		onChangeFPUViewAction();
		break;
	case ID_REGISTERS_CopyAll:
		onCopyAll();
		break;
	case ID_REGISTERS_SwitchSIMDDispMode_Hexadecimal:
		wSIMDRegDispMode = SIMD_REG_DISP_HEX;
		Invalidate();
		break;
	case ID_REGISTERS_SwitchSIMDDispMode_Float:
		wSIMDRegDispMode = SIMD_REG_DISP_FLOAT;
		Invalidate();
		break;
	case ID_REGISTERS_SwitchSIMDDispMode_Double:
		wSIMDRegDispMode = SIMD_REG_DISP_DOUBLE;
		Invalidate();
		break;
	case ID_REGISTERS_SwitchSIMDDispMode_SignedWord:
		wSIMDRegDispMode = SIMD_REG_DISP_WORD_SIGNED;
		Invalidate();
		break;
	case ID_REGISTERS_SwitchSIMDDispMode_SignedDWord:
		wSIMDRegDispMode = SIMD_REG_DISP_WORD_UNSIGNED;
		Invalidate();
		break;
	case ID_REGISTERS_SwitchSIMDDispMode_SignedQWord:
		wSIMDRegDispMode = SIMD_REG_DISP_WORD_HEX;
		Invalidate();
		break;
	case ID_REGISTERS_SwitchSIMDDispMode_UnsignedWord:
		wSIMDRegDispMode = SIMD_REG_DISP_DWORD_SIGNED;
		Invalidate();
		break;
	case ID_REGISTERS_SwitchSIMDDispMode_UnsignedDWord:
		wSIMDRegDispMode = SIMD_REG_DISP_DWORD_UNSIGNED;
		Invalidate();
		break;
	case ID_REGISTERS_SwitchSIMDDispMode_UnsignedQWord:
		wSIMDRegDispMode = SIMD_REG_DISP_DWORD_HEX;
		Invalidate();
		break;
	case ID_REGISTERS_SwitchSIMDDispMode_HexadecimalWord:
		wSIMDRegDispMode = SIMD_REG_DISP_QWORD_SIGNED;
		Invalidate();
		break;
	case ID_REGISTERS_SwitchSIMDDispMode_HexadecimalDWord:
		wSIMDRegDispMode = SIMD_REG_DISP_QWORD_UNSIGNED;
		Invalidate();
		break;
	case ID_REGISTERS_SwitchSIMDDispMode_HexadecimalQWord:
		wSIMDRegDispMode = SIMD_REG_DISP_QWORD_HEX;
		Invalidate();
		break;
	case ID_REGISTERS_SetHBP:
		DbgCmdExec("bphws csp,rw");
		break;
	default:
		break;
	}
}

void CRegistersView::onFollowInDisassembly()
{
	if (mCANSTOREADDRESS.count(mSelected))
	{
		//QString addr = QString("%1").arg((*((duint*)registerValue(&wRegDumpStruct, mSelected))), mRegisterPlaces[mSelected].valuesize, 16, QChar('0')).toUpper();
		CString formatText = _T("%0");
		formatText.AppendFormat(_T("%dX"), mRegisterPlaces[mSelected].valuesize);
		CString addr = _T("disasm \"");
		addr.AppendFormat(formatText, *((duint*)registerValue(&wRegDumpStruct, mSelected)));
		addr += '"';
		string _addr;
		_addr = CT2A(addr);
		if (DbgMemIsValidReadPtr((*((duint*)registerValue(&wRegDumpStruct, mSelected)))))
			//DbgCmdExec(QString().sprintf("disasm \"%s\"", addr.toUtf8().constData()).toUtf8().constData());
			DbgCmdExec(_addr.data());
	}
}

void CRegistersView::onFollowInDump()
{
	if (mCANSTOREADDRESS.count(mSelected))
	{
		CString formatText = _T("%0");
		formatText.AppendFormat(_T("%dX"), mRegisterPlaces[mSelected].valuesize);
		CString addr = _T("dump \"");
		addr.AppendFormat(formatText, *((duint*)registerValue(&wRegDumpStruct, mSelected)));
		addr += '"';
		string _addr;
		_addr = CT2A(addr);		
		if (DbgMemIsValidReadPtr((*((duint*)registerValue(&wRegDumpStruct, mSelected)))))
			DbgCmdExec(_addr.data());
	}
}

void CRegistersView::onFollowInStack()
{
	if (mCANSTOREADDRESS.count(mSelected))
	{
		CString formatText = _T("%0");
		formatText.AppendFormat(_T("%dX"), mRegisterPlaces[mSelected].valuesize);
		CString addr = _T("sdump \"");
		addr.AppendFormat(formatText, *((duint*)registerValue(&wRegDumpStruct, mSelected)));
		addr += '"';
		string _addr;
		_addr = CT2A(addr);		
		if (DbgMemIsValidReadPtr((*((duint*)registerValue(&wRegDumpStruct, mSelected)))))
			DbgCmdExec(_addr.data());
	}
}

void CRegistersView::onFollowInMemoryMap()
{
	if (mCANSTOREADDRESS.count(mSelected))
	{
		CString formatText = _T("%0");
		formatText.AppendFormat(_T("%dX"), mRegisterPlaces[mSelected].valuesize);
		CString addr = _T("memmapdump \"");
		addr.AppendFormat(formatText, *((duint*)registerValue(&wRegDumpStruct, mSelected)));
		addr += '"';
		string _addr;
		_addr = CT2A(addr);		
		if (DbgMemIsValidReadPtr((*((duint*)registerValue(&wRegDumpStruct, mSelected)))))
			DbgCmdExec(_addr.data());
	}
}

void CRegistersView::onCopySymbol()
{
	if (mLABELDISPLAY.count(mSelected))
	{
		CString symbol = getRegisterLabel(mSelected);
		if (!symbol.IsEmpty())
			setTextToClipboard(symbol);
	}
}

void CRegistersView::onHighlight()
{
	CDisassembly* pCDisassembly = ((CMainFrame*)AfxGetApp()->GetMainWnd())->getCDisasmPane()->getCDisassembly();
	if (mGPR.count(mSelected) && mSelected != REGISTER_NAME::EFLAGS)
		pCDisassembly->hightlightToken(CapstoneTokenizer::SingleToken(CapstoneTokenizer::TokenType::GeneralRegister, (CString)mRegisterMapping[mSelected].data()));
	else if (mSEGMENTREGISTER.count(mSelected))
		pCDisassembly->hightlightToken(CapstoneTokenizer::SingleToken(CapstoneTokenizer::TokenType::MemorySegment, (CString)mRegisterMapping[mSelected].data()));
	else if (mFPUMMX.count(mSelected))
		pCDisassembly->hightlightToken(CapstoneTokenizer::SingleToken(CapstoneTokenizer::TokenType::MmxRegister, (CString)mRegisterMapping[mSelected].data()));
	else if (mFPUXMM.count(mSelected))
		pCDisassembly->hightlightToken(CapstoneTokenizer::SingleToken(CapstoneTokenizer::TokenType::XmmRegister, (CString)mRegisterMapping[mSelected].data()));
	else if (mFPUYMM.count(mSelected))
		pCDisassembly->hightlightToken(CapstoneTokenizer::SingleToken(CapstoneTokenizer::TokenType::YmmRegister, (CString)mRegisterMapping[mSelected].data()));
	pCDisassembly->reloadData();
}

void CRegistersView::onUndo()
{
	if (mUNDODISPLAY.count(mSelected))
	{
		if (mFPUMMX.count(mSelected) || mFPUXMM.count(mSelected) || mFPUYMM.count(mSelected) || mFPUx87_80BITSDISPLAY.count(mSelected))
			setRegister(mSelected, (duint)registerValue(&wCipRegDumpStruct, mSelected));
		else
			setRegister(mSelected, *(duint*)registerValue(&wCipRegDumpStruct, mSelected));
	}
}

void CRegistersView::onZero()
{
	if (mSETONEZEROTOGGLE.count(mSelected))
		setRegister(mSelected, 0);
}

void CRegistersView::onSetTo_1()
{
	if (mSETONEZEROTOGGLE.count(mSelected))
		setRegister(mSelected, 1);
}

void CRegistersView::onToggleValue()
{
	if (mBOOLDISPLAY.count(mSelected))
	{
		int value = (int)(*(bool*)registerValue(&wRegDumpStruct, mSelected));
		setRegister(mSelected, value ^ 1);
	}
}

void CRegistersView::onIncrementx87Stack()
{
	if (mFPUx87_80BITSDISPLAY.count(mSelected))
		setRegister(x87SW_TOP, ((*((duint*)registerValue(&wRegDumpStruct, x87SW_TOP))) + 1) % 8);
}

void CRegistersView::onDecrementx87Stack()
{
	if (mFPUx87_80BITSDISPLAY.count(mSelected))
		setRegister(x87SW_TOP, ((*((duint*)registerValue(&wRegDumpStruct, x87SW_TOP))) - 1) % 8);
}

void CRegistersView::onIncrement()
{
	if (mINCREMENTDECREMET.count(mSelected))
		setRegister(mSelected, (*((duint*)registerValue(&wRegDumpStruct, mSelected))) + 1);
}

void CRegistersView::onDecrement()
{
	if (mINCREMENTDECREMET.count(mSelected))
		setRegister(mSelected, (*((duint*)registerValue(&wRegDumpStruct, mSelected))) - 1);
}

void CRegistersView::onPush()
{
	duint csp = (*((duint*)registerValue(&wRegDumpStruct, CSP))) - sizeof(void*);
	duint regVal = 0;
	regVal = *((duint*)registerValue(&wRegDumpStruct, mSelected));
	setRegister(CSP, csp);
	DbgMemWrite(csp, (const unsigned char*)&regVal, sizeof(void*));
}

void CRegistersView::onPop()
{
	duint csp = (*((duint*)registerValue(&wRegDumpStruct, CSP)));
	duint newVal;
	DbgMemRead(csp, (unsigned char*)&newVal, sizeof(void*));
	setRegister(CSP, csp + sizeof(void*));
	setRegister(mSelected, newVal);
}

void CRegistersView::OnLocalHandle(UINT id)
{
	switch (id)
	{
	case ID_REGISTERS_ModifyValue:
		displayEditDialog();
		break;
	case ID_REGISTERS_FollowInDump:
		onFollowInDump();
		break;
	case ID_REGISTERS_FollowInDisassembly:
		onFollowInDisassembly();
		break;
	case ID_REGISTERS_FollowInMemoryMap:
		onFollowInMemoryMap();
	case ID_REGISTERS_FollowInStack:
		onFollowInStack();
		break;
	case ID_REGISTERS_Copy:
		onCopy();
		break;
	case ID_REGISTERS_CopySymbol:
		onCopySymbol();
		break;
	case ID_REGISTERS_HighLight:
		onHighlight();
		break;
	case ID_REGISTERS_Undo:
		onUndo();
		break;
	case ID_REGISTERS_Zero:
		onZero();
		break;
	case ID_REGISTERS_SetTo_1:
		onSetTo_1();
		break;
	case ID_REGISTERS_Toggle:
		onToggleValue();
		break;
	case ID_REGISTERS_Incrementx87Stack:
		onIncrementx87Stack();
		break;
	case ID_REGISTERS_Decrementx87Stack:
		onDecrementx87Stack();
		break;
	case ID_REGISTERS_Increment:
		onIncrement();
		break;
	case ID_REGISTERS_Decrement:
		onDecrement();
		break;
	case ID_REGISTERS_Push:
		onPush();
		break;
	case ID_REGISTERS_Pop:
		onPop();
		break;

	default:
		break;
	}
}

void CRegistersView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: 在此处添加消息处理程序代码
	
	if (!DbgIsDebugging())
		return;
	/*
	QMenu wMenu(this);
	QMenu* followInDumpNMenu = nullptr;
	const QAction* selectedAction = nullptr;
	*/
	/*
	UINT selectedSIMDRegDispMode = 0;
	switch (wSIMDRegDispMode)
	{
	case SIMD_REG_DISP_HEX:
		selectedAction = SIMDHex;
		break;
	case SIMD_REG_DISP_FLOAT:
		selectedAction = SIMDFloat;
		break;
	case SIMD_REG_DISP_DOUBLE:
		selectedAction = SIMDDouble;
		break;
	case SIMD_REG_DISP_WORD_SIGNED:
		selectedAction = SIMDSWord;
		break;
	case SIMD_REG_DISP_WORD_UNSIGNED:
		selectedAction = SIMDUWord;
		break;
	case SIMD_REG_DISP_WORD_HEX:
		selectedAction = SIMDHWord;
		break;
	case SIMD_REG_DISP_DWORD_SIGNED:
		selectedAction = SIMDSDWord;
		break;
	case SIMD_REG_DISP_DWORD_UNSIGNED:
		selectedAction = SIMDUDWord;
		break;
	case SIMD_REG_DISP_DWORD_HEX:
		selectedAction = SIMDHDWord;
		break;
	case SIMD_REG_DISP_QWORD_SIGNED:
		selectedAction = SIMDSQWord;
		break;
	case SIMD_REG_DISP_QWORD_UNSIGNED:
		selectedAction = SIMDUQWord;
		break;
	case SIMD_REG_DISP_QWORD_HEX:
		selectedAction = SIMDHQWord;
		break;
	}
	SIMDHex->setChecked(SIMDHex == selectedAction);
	SIMDFloat->setChecked(SIMDFloat == selectedAction);
	SIMDDouble->setChecked(SIMDDouble == selectedAction);
	SIMDSWord->setChecked(SIMDSWord == selectedAction);
	SIMDUWord->setChecked(SIMDUWord == selectedAction);
	SIMDHWord->setChecked(SIMDHWord == selectedAction);
	SIMDSDWord->setChecked(SIMDSDWord == selectedAction);
	SIMDUDWord->setChecked(SIMDUDWord == selectedAction);
	SIMDHDWord->setChecked(SIMDHDWord == selectedAction);
	SIMDSQWord->setChecked(SIMDSQWord == selectedAction);
	SIMDUQWord->setChecked(SIMDUQWord == selectedAction);
	SIMDHQWord->setChecked(SIMDHQWord == selectedAction);
	*/

	CMenu PopupMenu;
	// 创建弹出菜单
	BOOL bRet = PopupMenu.CreatePopupMenu();

	CMenu SecondLevelPopupMenu;
	// 创建二级弹出菜单
	bRet = SecondLevelPopupMenu.CreateMenu();

	CStringArray strSIMDDispModeArray;
	strSIMDDispModeArray.Add(L"Hexadecimal");
	strSIMDDispModeArray.Add(L"Float");
	strSIMDDispModeArray.Add(L"Double");
	strSIMDDispModeArray.Add(L"Signed Word");
	strSIMDDispModeArray.Add(L"Signed DWord");
	strSIMDDispModeArray.Add(L"Signed QWord");
	strSIMDDispModeArray.Add(L"Unsigned Word");
	strSIMDDispModeArray.Add(L"Unsigned DWord");
	strSIMDDispModeArray.Add(L"Unsigned QWord");
	strSIMDDispModeArray.Add(L"Hexadecimal Word");
	strSIMDDispModeArray.Add(L"Hexadecimal DWord");
	strSIMDDispModeArray.Add(L"Hexadecimal QWord");
	for (UINT i = 0; i < (ID_REGISTERS_SwitchSIMDDispMode_HexadecimalQWord - ID_REGISTERS_SwitchSIMDDispMode_Hexadecimal); i++)
	{
		UINT nFlags;
		if (i == (UINT)wSIMDRegDispMode)
			nFlags = MF_ENABLED | MF_CHECKED;
		else
		{
			nFlags = MF_ENABLED;
		}
		bRet = SecondLevelPopupMenu.AppendMenu(nFlags, i + ID_REGISTERS_SwitchSIMDDispMode_Hexadecimal, strSIMDDispModeArray[i]);
	}

	if (mSelected != UNKNOWN)
	{	
		
		if (mMODIFYDISPLAY.count(mSelected))
		{
			PopupMenu.AppendMenu(MF_ENABLED, ID_REGISTERS_ModifyValue, _T("修改"));
			PopupMenu.AppendMenu(MF_SEPARATOR);
		}

		if (mCANSTOREADDRESS.count(mSelected))
		{
			duint addr = (*((duint*)registerValue(&wRegDumpStruct, mSelected)));
			if (DbgMemIsValidReadPtr(addr))
			{
				PopupMenu.AppendMenu(MF_ENABLED, ID_REGISTERS_FollowInDump, _T("更随到内存"));
				PopupMenu.AppendMenu(MF_ENABLED, ID_REGISTERS_FollowInDisassembly, _T("更随到反汇编"));
				PopupMenu.AppendMenu(MF_ENABLED, ID_REGISTERS_FollowInMemoryMap, _T("更随到内存映射"));
				duint size = 0;
				duint base = DbgMemFindBaseAddr(DbgValFromString("csp"), &size);
				if (addr >= base && addr < base + size)
					PopupMenu.AppendMenu(MF_ENABLED, ID_REGISTERS_FollowInStack, _T("更随到堆栈"));
				PopupMenu.AppendMenu(MF_SEPARATOR);

			}
		}
		PopupMenu.AppendMenu(MF_ENABLED, ID_REGISTERS_Copy, _T("复制选中寄存器"));
		PopupMenu.AppendMenu(MF_ENABLED, ID_REGISTERS_CopyAll, _T("复制所有寄存器"));
		PopupMenu.AppendMenu(MF_SEPARATOR);
		
		if (mLABELDISPLAY.count(mSelected))
		{
			CString symbol = getRegisterLabel(mSelected);
			if (symbol != "")
			{
				PopupMenu.AppendMenu(MF_ENABLED, ID_REGISTERS_CopySymbol, _T("复制符号"));
				PopupMenu.AppendMenu(MF_SEPARATOR);
			}
		}

		if ((mGPR.count(mSelected) && mSelected != REGISTER_NAME::EFLAGS) || mSEGMENTREGISTER.count(mSelected) || mFPUMMX.count(mSelected) || mFPUXMM.count(mSelected) || mFPUYMM.count(mSelected))
		{
			PopupMenu.AppendMenu(MF_ENABLED, ID_REGISTERS_HighLight, _T("高亮"));
			PopupMenu.AppendMenu(MF_SEPARATOR);
		}

		if (mUNDODISPLAY.count(mSelected) && CompareRegisters(mSelected, &wRegDumpStruct, &wCipRegDumpStruct) != 0)
		{
			PopupMenu.AppendMenu(MF_ENABLED, ID_REGISTERS_Undo, _T("撤消"));
			PopupMenu.AppendMenu(MF_SEPARATOR);
		}

		if (mSETONEZEROTOGGLE.count(mSelected))
		{
			if ((*((duint*)registerValue(&wRegDumpStruct, mSelected))) >= 1)
				PopupMenu.AppendMenu(MF_ENABLED, ID_REGISTERS_Zero, _T("清0"));
			if ((*((duint*)registerValue(&wRegDumpStruct, mSelected))) == 0)
				PopupMenu.AppendMenu(MF_ENABLED, ID_REGISTERS_SetTo_1, _T("置1"));
		}

		if (mBOOLDISPLAY.count(mSelected))
		{
			PopupMenu.AppendMenu(MF_ENABLED, ID_REGISTERS_Toggle, _T("取反"));
			PopupMenu.AppendMenu(MF_SEPARATOR);
		}

		if (mFPUx87_80BITSDISPLAY.count(mSelected))
		{
			PopupMenu.AppendMenu(MF_ENABLED, ID_REGISTERS_Incrementx87Stack, _T("Increment x87S tack"));
			PopupMenu.AppendMenu(MF_ENABLED, ID_REGISTERS_Decrementx87Stack, _T("Decrement x87S tack"));
			PopupMenu.AppendMenu(MF_SEPARATOR);
		}

		if (mINCREMENTDECREMET.count(mSelected))
		{
			PopupMenu.AppendMenu(MF_ENABLED, ID_REGISTERS_Increment, _T("+1"));
			PopupMenu.AppendMenu(MF_ENABLED, ID_REGISTERS_Decrement, _T("-1"));
			PopupMenu.AppendMenu(MF_SEPARATOR);
		}

		if (mGPR.count(mSelected) || mSelected == CIP)
		{
			PopupMenu.AppendMenu(MF_ENABLED, ID_REGISTERS_Push, _T("入栈"));
			PopupMenu.AppendMenu(MF_ENABLED, ID_REGISTERS_Pop, _T("出栈"));
			PopupMenu.AppendMenu(MF_SEPARATOR);
		}

		if (mFPUMMX.count(mSelected) || mFPUXMM.count(mSelected) || mFPUYMM.count(mSelected))
		{
			PopupMenu.AppendMenu(MF_POPUP, (UINT)SecondLevelPopupMenu.m_hMenu, _T("切换SIMD寄存器显示模式"));
		}		
	}
	else
	{
		/*
		wMenu.addSeparator();
		wMenu.addAction(wCM_ChangeFPUView);
		wMenu.addAction(wCM_CopyAll);
		wMenu.addMenu(mSwitchSIMDDispMode);
		wMenu.addSeparator();
		QAction* wHwbpCsp = wMenu.addAction(DIcon("breakpoint.png"), tr("Set Hardware Breakpoint on %1").arg(ArchValue("ESP", "RSP")));
		QAction* wAction = wMenu.exec(this->mapToGlobal(pos));

		if (wAction == wHwbpCsp)
			DbgCmdExec("bphws csp,rw");		
		*/
		
		
		
		/*
		bRet = SwitchSIMDDispModePopupMenu.AppendMenu(MF_ENABLED | MF_CHECKED, ID_REGISTERS_SwitchSIMDDispMode_Hexadecimal, L"Hexadecimal");
		bRet = SwitchSIMDDispModePopupMenu.AppendMenu(MF_ENABLED, ID_REGISTERS_SwitchSIMDDispMode_Float, L"Float");
		bRet = SwitchSIMDDispModePopupMenu.AppendMenu(MF_ENABLED, ID_REGISTERS_SwitchSIMDDispMode_Double, L"Double");
		bRet = SwitchSIMDDispModePopupMenu.AppendMenu(MF_ENABLED, ID_REGISTERS_SwitchSIMDDispMode_SignedWord, L"Signed Word");
		bRet = SwitchSIMDDispModePopupMenu.AppendMenu(MF_ENABLED, ID_REGISTERS_SwitchSIMDDispMode_SignedDWord, L"Signed DWord");
		bRet = SwitchSIMDDispModePopupMenu.AppendMenu(MF_ENABLED, ID_REGISTERS_SwitchSIMDDispMode_SignedQWord, L"Signed QWord");
		bRet = SwitchSIMDDispModePopupMenu.AppendMenu(MF_ENABLED, ID_REGISTERS_SwitchSIMDDispMode_UnsignedWord, L"Unsigned Word");
		bRet = SwitchSIMDDispModePopupMenu.AppendMenu(MF_ENABLED, ID_REGISTERS_SwitchSIMDDispMode_UnsignedDWord, L"Unsigned DWord");
		bRet = SwitchSIMDDispModePopupMenu.AppendMenu(MF_ENABLED, ID_REGISTERS_SwitchSIMDDispMode_UnsignedQWord, L"Unsigned QWord");
		bRet = SwitchSIMDDispModePopupMenu.AppendMenu(MF_ENABLED, ID_REGISTERS_SwitchSIMDDispMode_HexadecimalWord, L"Hexadecimal Word");
		bRet = SwitchSIMDDispModePopupMenu.AppendMenu(MF_ENABLED, ID_REGISTERS_SwitchSIMDDispMode_HexadecimalDWord, L"Hexadecimal DWord");
		bRet = SwitchSIMDDispModePopupMenu.AppendMenu(MF_ENABLED, ID_REGISTERS_SwitchSIMDDispMode_HexadecimalQWord, L"Hexadecimal QWord");
		*/
		

		bRet = PopupMenu.AppendMenu(MF_ENABLED, ID_REGISTERS_ChangeFPUView, L"视图切换");
		bRet = PopupMenu.AppendMenu(MF_ENABLED, ID_REGISTERS_CopyAll, L"复制所有寄存器");
		bRet = PopupMenu.AppendMenu(MF_SEPARATOR);
		bRet = PopupMenu.AppendMenu(MF_POPUP, (UINT)SecondLevelPopupMenu.m_hMenu, _T("切换SIMD寄存器显示模式"));
		bRet = PopupMenu.AppendMenu(MF_SEPARATOR);
		bRet = PopupMenu.AppendMenu(MF_ENABLED, ID_REGISTERS_SetHBP, L"在ESP上设置硬件断点");
	}
	// 显示菜单
	int nCmd = PopupMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this); //TPM_RETURNCMD
}
