#pragma once
#include "AbstractTableView.h"
#include "Memory/MemoryPage.h"
#include "RichTextPainter.h"
#include "VaHistory.h"


#include <functional>
class CHexDump :
	public CAbstractTableView
{
public:
	CHexDump();
	~CHexDump();

	

public:
	enum DataSize
	{
		Byte = 1,
		Word = 2,
		Dword = 4,
		Qword = 8,
		Tword = 10
	};

	enum ByteViewMode
	{
		HexByte,
		AsciiByte,
		SignedDecByte,
		UnsignedDecByte
	};

	enum WordViewMode
	{
		HexWord,
		UnicodeWord,
		SignedDecWord,
		UnsignedDecWord
	};

	enum DwordViewMode
	{
		HexDword,
		SignedDecDword,
		UnsignedDecDword,
		FloatDword //sizeof(float)=4
	};

	enum QwordViewMode
	{
		HexQword,
		SignedDecQword,
		UnsignedDecQword,
		DoubleQword //sizeof(double)=8
	};

	enum TwordViewMode
	{
		FloatTword
	};

	struct DataDescriptor
	{
		DataSize itemSize; // Items size
		union // View mode
		{
			ByteViewMode byteMode;
			WordViewMode wordMode;
			DwordViewMode dwordMode;
			QwordViewMode qwordMode;
			TwordViewMode twordMode;
		};
	};

	struct ColumnDescriptor
	{
		bool isData = true;
		int itemCount = 16;
		int separator = 0;
		//QTextCodec* textCodec = nullptr; //name of the text codec (leave empty if you want to keep your sanity)
		CString* textCodec = nullptr;
		DataDescriptor data;
		std::function<void()> columnSwitch;
	};

	VaHistory mHistory;
	vector<int> m_vColumnWidth;

public:
	
	void copyColumnWidth();
	int getLastColumnWidth(int index);


	void HexDumpClear();

	// Configuration
	void updateColors() override;
	void updateFonts() override;
	
	virtual void selectionUpdated() = 0;
	virtual void onLButtonOnHeader(int wColIndex) = 0;

	duint rvaToVa(dsint rva) const;
	int getItemPixelWidth(ColumnDescriptor desc);
	CString makeAddrText(duint va) const;
	// Selection Management
	bool isSelected(dsint rva) const;
	void expandSelectionUpTo(dsint rva);
	dsint getInitialSelection() const;
	dsint getSelectionStart() const;
	dsint getSelectionEnd() const;
	void setSingleSelection(dsint rva);
	void byteToString(duint rva, byte_t byte, ByteViewMode mode, RichTextPainter::CustomRichText_t & richText);
	void wordToString(duint rva, uint16 word, WordViewMode mode, RichTextPainter::CustomRichText_t & richText);
	static void dwordToString(duint rva, uint32 dword, DwordViewMode mode, RichTextPainter::CustomRichText_t & richText);
	static void qwordToString(duint rva, uint64 qword, QwordViewMode mode, RichTextPainter::CustomRichText_t & richText);
	static void twordToString(duint rva, void* tword, TwordViewMode mode, RichTextPainter::CustomRichText_t & richText);
	void toString(DataDescriptor desc, duint rva, byte_t* data, RichTextPainter::CustomRichText_t & richText);

	virtual void getColumnRichText(int col, dsint rva, RichTextPainter::List & richText);

	static int getSizeOf(DataSize size);

	int getItemPixelWidth(ColumnDescriptor desc) const;

	int getItemIndexFromX(int x) const;
	dsint getItemStartingAddress(int x, int y);

	//descriptor management
	void appendDescriptor(int width, CString title, bool clickable, ColumnDescriptor descriptor);
	void appendResetDescriptor(int width, CString title, bool clickable, ColumnDescriptor descriptor);
	void clearDescriptors();

	int getBytePerRowCount() const;
	void printDumpAt(dsint parVA);
	void printDumpAt(dsint parVA, bool select, bool repaint = true, bool updateTableOffset = true);
	CString paintContent(CDC* painter, Graphics& graphics, dsint rowBase, int rowOffset, int col, int x, int y, int w, int h) override;
	void printSelected(CDC* painter, Graphics& graphics, dsint rowBase, int rowOffset, int col, int x, int y, int w, int h);

protected:
	bool mRvaDisplayEnabled;
	bool bAlign = false;
	int mByteOffset;
	int mForceColumn = -1;
	dsint mRvaDisplayPageBase;
	duint mRvaDisplayBase;
	CList<ColumnDescriptor> mDescriptor;
	MemoryPage* mMemPage;
	

private:
	struct SelectionData
	{
		dsint firstSelectedIndex;
		dsint fromIndex;
		dsint toIndex;
	};

	SelectionData mSelection;

	enum GuiState
	{
		NoState,
		MultiRowsSelectionState
	};

	GuiState mGuiState = NoState;

	

	Color mModifiedBytesColor;
	Color mModifiedBytesBackgroundColor;
	Color mRestoredBytesColor;
	Color mRestoredBytesBackgroundColor;
	Color mByte00Color;
	Color mByte00BackgroundColor;
	Color mByte7FColor;
	Color mByte7FBackgroundColor;
	Color mByteFFColor;
	Color mByteFFBackgroundColor;
	Color mByteIsPrintColor;
	Color mByteIsPrintBackgroundColor;

	Color mUserModuleCodePointerHighlightColor;
	Color mUserModuleDataPointerHighlightColor;
	Color mSystemModuleCodePointerHighlightColor;
	Color mSystemModuleDataPointerHighlightColor;
	Color mUnknownCodePointerHighlightColor;
	Color mUnknownDataPointerHighlightColor;

	
public:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};

