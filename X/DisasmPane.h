#pragma once

#include "Disassembly.h"
#include "CPUSideBar.h"
#include "OutputBox.h"
#include "CPUInfoBox.h"
// CDisasmPane

class CDisasmPane : public CDockablePane
{
	DECLARE_DYNAMIC(CDisasmPane)

public:
	CDisasmPane();
	virtual ~CDisasmPane();
public:
	CDisassembly*	getCDisassembly() { return &m_wndDisasmView; }
	CCPUInfoBox*	getCCPUInfoBox() { return &m_wndOutputBox; }
	CCPUSideBar*	getCCPUSideBar() { return &m_wndCPUSideBar; }

public:
	enum GuiState
	{
		NoState,
		ReadyToResizeH,
		ReadyToResizeV,
		ResizeStateH,
		ResizeStateV
	};
	enum MouseButtonState
	{
		MouseButtonNoState,
		LButtonDown,
		RButtonDown,
		ButtonAllDown
	};
	enum FocusState
	{
		S,
		D,
		O
	};
	GuiState m_guiState;
	MouseButtonState m_mouseButtonState;
	FocusState m_focusState;

	

	struct ResizingData
	{
		CRect hResizeRect;
		CRect vResizeRect;
		int lastPosX;
		int lastPosY;		
	};
	ResizingData m_resizeDate;
	
	BOOL SaveBitmapToFile(HBITMAP   hBitmap, CString szfilename);
	void drawAdjustLine(CPoint);
	void MyAdjustLayout();
	FocusState getFocusState();
	void setFocusState(FocusState);
	void ActivePane();

private:
	CCPUSideBar	m_wndCPUSideBar;
	CDisassembly m_wndDisasmView;
	//COutputBox m_wndOutputBox;
	CCPUInfoBox m_wndOutputBox;

	bool m_bOnsize;
	bool m_ResizingX;
	bool m_ResizingY;
	int m_splitWidth;
	int m_splitHight;
	int m_defaultWidth;
	int m_defaultHight;
	int m_space;
	int iWidth;
	int iHight;

	//HBITMAP Q;
	//HDC m_windowHDC;
	CDC m_intDC;
	

	

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnMouseLeave();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
};


