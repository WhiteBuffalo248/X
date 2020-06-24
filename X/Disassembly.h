#pragma once
#include "AbstractTableView.h"
#include "Imports.h"
#include "QBeaEngine.h"
#include "Memory/MemoryPage.h"
#include "CodeFolding.h"




class CDisassembly :
	public CAbstractTableView
{
public:
	CDisassembly();
	~CDisassembly();

	// Public Methods
	void reloadData() override;
	duint rvaToVa(dsint rva) const;
	const duint getBase() const;
	duint getSize() const;
	// Selection Management
	bool isSelected(CList<Instruction_t>* buffer, int index) const;
	dsint getInitialSelection() const;
	void setSingleSelection(dsint index);
	void expandSelectionUpTo(dsint to);
	dsint getSelectionStart() const;
	dsint getSelectionEnd() const;
	//misc
	bool isHighlightMode() const;
	bool hightlightToken(const CapstoneTokenizer::SingleToken & token);

	void disassembleClear();
	// history management
	void historyClear();

	CList<Instruction_t>* instructionsBuffer(); // ugly
	void disassembleAt(dsint parVA, dsint parCIP);
	void selectionChangedSlot(dsint parVA);
	void setTableOffset(dsint val);
	void tokenizerConfigUpdatedSlot();
	//signal
	void setSideBarRowSignal(int rowCount);
	

private:
	enum GuiState
	{
		NoState,
		MultiRowsSelectionState
	};
	enum GraphicDump
	{
		GD_Nothing,
		GD_FootToTop, //GD_FootToTop = '-
		GD_FootToBottom, //GD_FootToBottom = ,-
		GD_HeadFromTop, //GD_HeadFromTop = '->
		GD_HeadFromBottom, //GD_HeadFromBottom = ,->
		GD_HeadFromBoth, //GD_HeadFromBoth = |->
		GD_Vert, //GD_Vert = |
		GD_VertHori //GD_VertHori = |-
	};
	enum GraphicJumpDirection
	{
		GJD_Nothing,
		GJD_Up,
		GJD_Down
	};

	struct SelectionData
	{
		dsint firstSelectedIndex;
		dsint fromIndex;
		dsint toIndex;
	};
	struct HistoryData
	{
		dsint va;
		dsint tableOffset;
		CString windowTitle;
	};

	GuiState mGuiState;
	dsint mCipRva;
	int mCurrentVa;
	bool bSHIFT = false;
	
	SelectionData mSelection;	
	CList<HistoryData> mVaHistory;
	CList<Instruction_t> mInstBuffer;

	// Misc
	bool mRvaDisplayEnabled;
	bool mHighlightingMode;
	bool mNoCurrentModuleText;
	bool mShowMnemonicBrief = false;
	bool mPermanentHighlightingMode;
	bool mPopupEnabled;

	dsint mRvaDisplayPageBase;

	CodeFoldingHelper* mCodeFoldingManager;
	CapstoneTokenizer::SingleToken mHighlightToken;
	QBeaEngine* mDisasm;
	XREF_INFO mXrefInfo;
	duint mRvaDisplayBase;

	MemoryPage* mMemPage;

	// Configuration
	void updateFonts() override;
	void updateColors() override;
	// Function Graphic

	enum Function_t
	{
		Function_none,
		Function_single,
		Function_start,
		Function_middle,
		Function_loop_entry,
		Function_end
	};
	
	//弹出菜单事件
	void onCopySelection(bool copyBytes);
	void onFindStrings(UINT);

	// Reimplemented Functions
	void DrawBPCircle(CDC* painter, Graphics& graphics, int line, bool isbp, bool isbpdisabled);
	CString paintContent(CDC* painter, Graphics& graphics, dsint rowBase, int rowOffset, int col, int x, int y, int w, int h);
	int paintFunctionGraphic(CDC* painter, int x, int y, Function_t funcType, bool loop);
	// ScrollBar Management
	dsint sliderMovedHook(int type, dsint value, dsint delta) override;
	// Jumps Graphic
	int paintJumpsGraphic(CDC* painter, Gdiplus::Graphics& graphics, int x, int y, dsint addr, bool isjmp);

	// Instructions Management
	dsint getPreviousInstructionRVA(dsint rva, duint count);
	dsint getNextInstructionRVA(dsint rva, duint count, bool isGlobal = false);
	dsint getInstructionRVA(dsint index, dsint count);
	Instruction_t DisassembleAt(dsint rva);
	Instruction_t DisassembleAt(dsint rva, dsint count);	

	// Update/Reload/Refresh/Repaint
	void prepareData() override;
	void prepareDataRange(dsint startRva, dsint endRva, const std::function<bool(int, const Instruction_t &)> & disassembled);

	//disassemble
	void disassembleAt(dsint parVA, dsint parCIP, bool history, dsint newTableOffset);
	CString getAddrText(dsint cur_addr, char label[MAX_LABEL_SIZE], bool getLabel = true);
	RichTextPainter::List getRichBytes(const Instruction_t & instr) const;

	// Configuration
    Color	colorInstructionHighlight;
	Color	mTracedSelectedAddressBackgroundColor;
	Color	mSelectionColor;
	Color	mTracedAddressBackgroundColor;
	Color	mCipBackgroundColor;
	Color	mBreakpointBackgroundColor;
	Color	mBreakpointColor;
	Color	mCipColor;
	Color	mHardwareBreakpointBackgroundColor;
	Color	mHardwareBreakpointColor;
	Color	mBookmarkBackgroundColor;
	Color	mBookmarkColor;
	Color	mLabelColor;
	Color	mLabelBackgroundColor;
	Color	mSelectedAddressBackgroundColor;
	Color	mSelectedAddressColor;
	Color	mAddressBackgroundColor;
	Color	mAddressColor;	
	
	Color	mDisassemblyRelocationUnderlineColor;
	Color	mModifiedBytesColor;
	Color	mModifiedBytesBackgroundColor;
	Color	mRestoredBytesColor;
	Color	mRestoredBytesBackgroundColor;
	Color	mBytesColor;
	Color	mBytesBackgroundColor;
	Color	mAutoCommentColor;
	Color	mAutoCommentBackgroundColor;
	Color	mCommentColor;
	Color	mCommentBackgroundColor;
	Color	mMnemonicBriefColor;
	Color	mMnemonicBriefBackgroundColor;
	Color	mInstructionHighlightColor;
	Color	mByte00Color;
	Color	mByte00BackgroundColor;
	Color	mByte7FColor;
	Color	mByte7FBackgroundColor;
	Color	mByteFFColor;
	Color	mByteFFBackgroundColor;
	Color	mByteIsPrintColor;
	Color	mByteIsPrintBackgroundColor;
	Color	mUnconditionalJumpLineColor;
	Color	mConditionalJumpLineTrueColor;
	Color	mConditionalJumpLineFalseColor;
	Color	mLoopColor;
	Color	mFunctionColor;

	CPen	mLoopPen;
	CPen	mFunctionPen;
	CPen	mUnconditionalPen;
	CPen	mConditionalTruePen;
	CPen	mConditionalFalsePen;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnHandle(UINT id);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
};





