// TradeFrame.h : main header file for the TradeFrame application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CTradeFrameApp:
// See TradeFrame.cpp for the implementation of this class
//

class CTradeFrameApp : public CWinApp
{
public:
	CTradeFrameApp();

	CArray<HWND, HWND> m_aryFrames;
public:

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
protected:
	HMENU m_hMDIMenu;
	HACCEL m_hMDIAccel;

public:
	afx_msg void OnAppAbout();
	afx_msg void OnFileNewFrame();
	DECLARE_MESSAGE_MAP()
};

extern CTradeFrameApp theApp;