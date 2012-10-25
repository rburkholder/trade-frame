/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <TFIQFeed/IQFeedHistoryQueryMsgShim.h>

#include "Process.h"

class CMainDlg : 
  public CDialogImpl<CMainDlg>, 
  public CUpdateUI<CMainDlg>,
	public CMessageFilter, 
  public CIdleHandler
{
public:
	enum { IDD = IDD_MAINDLG };

  typedef ou::tf::iqfeed::HistoryQueryMsgShim<CMainDlg> history_query_t;

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	BEGIN_UPDATE_UI_MAP(CMainDlg)
	END_UPDATE_UI_MAP()

  enum enumMessages {
    WM_IQFEEDHISTORY_CONNECTED = WM_USER + 1,
    WM_IQFEEDHISTORY_SENDCOMPLETE,
    WM_IQFEEDHISTORY_DISCONNECTED,
    WM_IQFEEDHISTORY_ERROR,
    WM_IQFEEDHISTORY_TICKDATAPOINT,
    WM_IQFEEDHISTORY_INTERVALDATA,
    WM_IQFEEDHISTORY_SUMMARYDATA,
    WM_IQFEEDHISTORY_REQUESTDONE,

    WM_MAINDLG_CLOSEDIALOG  // checks for disconnection then closes
  };

	BEGIN_MSG_MAP_EX(CMainDlg)
    MESSAGE_HANDLER(WM_IQFEEDHISTORY_CONNECTED, OnIQFeedHistoryConnected)
    MESSAGE_HANDLER(WM_IQFEEDHISTORY_SENDCOMPLETE, OnIQFeedHistorySendComplete)
    MESSAGE_HANDLER(WM_IQFEEDHISTORY_DISCONNECTED, OnIQFeedHistoryDisconnected)
    MESSAGE_HANDLER(WM_IQFEEDHISTORY_ERROR, OnIQFeedHistoryError)
    MESSAGE_HANDLER(WM_IQFEEDHISTORY_TICKDATAPOINT, OnIQFeedHistoryTickDataPoint)
    MESSAGE_HANDLER(WM_IQFEEDHISTORY_INTERVALDATA, OnIQFeedHistoryIntervalData)
    MESSAGE_HANDLER(WM_IQFEEDHISTORY_SUMMARYDATA, OnIQFeedHistorySummaryData)
    MESSAGE_HANDLER(WM_IQFEEDHISTORY_REQUESTDONE, OnIQFeedHistoryRequestDone)

		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
    //CHAIN_MSG_MAP(CIQFeedHistoryQuery<CMainDlg>)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

  LRESULT OnIQFeedHistoryConnected( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnIQFeedHistorySendComplete( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnIQFeedHistoryDisconnected( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnIQFeedHistoryError( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnIQFeedHistoryTickDataPoint( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnIQFeedHistoryIntervalData( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnIQFeedHistorySummaryData( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnIQFeedHistoryRequestDone( UINT, WPARAM, LPARAM, BOOL &bHandled );

	void CloseDialog(int nVal);

protected:
  ou::tf::iqfeed::HistoryQueryMsgShim<CMainDlg>::structMessageDestinations m_MsgIdsForIQFeedHistoryQuery;
  ou::tf::iqfeed::HistoryQueryMsgShim<CMainDlg>* m_pIQFeedHistoryQuery;
private:
  WORD m_DialogCloseCode;
  CProcess m_process;
};
