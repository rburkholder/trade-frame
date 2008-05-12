// TFCharting.h : main header file for the TFCharting application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CTFChartingApp:
// See TFCharting.cpp for the implementation of this class
//

class CTFChartingApp : public CWinApp
{
public:
	CTFChartingApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CTFChartingApp theApp;