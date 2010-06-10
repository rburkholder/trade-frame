/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

// TradeYMView.cpp : implementation of the CTradeYMView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "TradeYMView.h"

CTradeYMView::CTradeYMView( void )
: CDialogImpl<CTradeYMView>()
{
}

CTradeYMView::~CTradeYMView( void ) {
}

BOOL CTradeYMView::PreTranslateMessage(MSG* pMsg) {
	return CWindow::IsDialogMessage(pMsg);
}

LRESULT CTradeYMView::OnBnClickedBtnTestTrade(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {

  OutputDebugString( "test\n" );

  return 0;
}
