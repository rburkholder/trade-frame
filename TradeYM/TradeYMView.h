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

// TradeYMView.h : interface of the CTradeYMView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Process.h"

class CTradeYMView : public CDialogImpl<CTradeYMView>
{
public:
	enum { IDD = IDD_TRADEYM_FORM };

	BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP(CTradeYMView)
    COMMAND_HANDLER(IDC_BTN_TEST_TRADE, BN_CLICKED, OnBnClickedBtnTestTrade)
  END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

  CTradeYMView( void );
  ~CTradeYMView( void );

protected:
private:

  CProcess m_process;

public:
  LRESULT OnBnClickedBtnTestTrade(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};
