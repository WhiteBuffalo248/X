#pragma once
#include "HexDump.h"
class CCPUDump :
	public CHexDump
{
public:
	CCPUDump();
	~CCPUDump();



	

	void selectionUpdated();
	void onLButtonOnHeader(int wColIndex);
	
	void hexAscii();
	void hexUnicode();

protected:
	CString paintContent(CDC* painter, Graphics& graphics, dsint rowBase, int rowOffset, int col, int x, int y, int w, int h);

private:
	int mAsciiSeparator = 0;

	enum ViewEnum_t
	{
		ViewHexAscii = 0,
		ViewHexUnicode,
		ViewTextAscii,
		ViewTextUnicode,
		ViewIntegerSignedShort,
		ViewIntegerSignedLong,
		ViewIntegerSignedLongLong,
		ViewIntegerUnsignedShort,
		ViewIntegerUnsignedLong,
		ViewIntegerUnsignedLongLong,
		ViewIntegerHexShort,
		ViewIntegerHexLong,
		ViewIntegerHexLongLong,
		ViewFloatFloat,
		ViewFloatDouble,
		ViewFloatLongDouble,
		ViewAddress,
		ViewIntegerSignedByte,
		ViewIntegerUnsignedByte,
		ViewAddressAscii,
		ViewAddressUnicode,
		ViewHexCodepage,
		ViewTextCodepage
	};

	ViewEnum_t m_viewMode;
	

	void setView(ViewEnum_t view);
	

public:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

