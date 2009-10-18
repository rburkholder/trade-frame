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

// TapeReaderView.h : interface of the CTapeReaderView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>

#include "LibIQFeed/IQFeed.h"

class CTapeReaderView : public CDialogImpl<CTapeReaderView>,
                        public CDialogResize<CTapeReaderView>
{
public:
	enum { IDD = IDD_TAPEREADER_FORM };

	BOOL PreTranslateMessage(MSG* pMsg);

//  http://www.codeproject.com/KB/wtl/wtldlgresize.aspx
//    * DLSZ_SIZE_X: Resize the width of the control as the dialog resizes horizontally.
//    * DLSZ_SIZE_Y: Resize the height of the control as the dialog resizes vertically.
//    * DLSZ_MOVE_X: Move the control horizontally as the dialog resizes horizontally.
//    * DLSZ_MOVE_Y: Move the control vertically as the dialog resizes vertically.
//    * DLSZ_REPAINT: Invalidate the control after every move/resize so it repaints every time.

  BEGIN_DLGRESIZE_MAP(CTapeReaderView)
    DLGRESIZE_CONTROL( IDC_EDTSYMBOL, DLSZ_SIZE_X )
    DLGRESIZE_CONTROL( IDC_BTNSTART, DLSZ_MOVE_X )
    DLGRESIZE_CONTROL( IDC_BTNSTOP, DLSZ_MOVE_X )
    DLGRESIZE_CONTROL( IDC_LISTTAPE, DLSZ_SIZE_X | DLSZ_SIZE_Y )
  END_DLGRESIZE_MAP()


// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
  LRESULT OnEnChangeEdtsymbol(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnBnClickedBtnstart(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnBnClickedBtnstop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnLvnItemchangedListtape(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);

  //HWND Create(HWND hWndParent, RECT&, LPARAM dwInitParam = NULL);
  HWND Create(HWND hWndParent, LPARAM dwInitParam = NULL);

  CTapeReaderView( void );
  ~CTapeReaderView( void );
protected:

  enum enumMessages {
    WM_IQFEED_INITIALIZED = WM_USER + 1,
    WM_IQFEED_CONNECTED,
    WM_IQFEED_SENDDONE,
    WM_IQFEED_DISCONNECTED,
    WM_IQFEED_ERROR, 

    WM_IQFEED_UPDATE,
    WM_IQFEED_SUMMARY,
    WM_IQFEED_NEWS,
    WM_IQFEED_FUNDAMENTAL,
    WM_IQFEED_TIME,
    WM_IQFEED_SYSTEM
  };

  enum enumUIEnableState {
    UI_STARTING,
    UI_NOSYMBOL,
    UI_SYMBOLENTRY,
    UI_STARTED
  } m_stateUI;

  void UpdateUIState( void );

  CIQFeed<CTapeReaderView>::structMessageDestinations m_Destinations;

	BEGIN_MSG_MAP_EX(CTapeReaderView)
    MESSAGE_HANDLER( WM_IQFEED_UPDATE, OnIQFeedUpdate )
    MESSAGE_HANDLER( WM_IQFEED_SUMMARY, OnIQFeedSummary )
    MESSAGE_HANDLER( WM_IQFEED_FUNDAMENTAL, OnIQFeedFundamental )

    MESSAGE_HANDLER( WM_IQFEED_INITIALIZED, OnIQFeedInitialized )
    MESSAGE_HANDLER( WM_IQFEED_CONNECTED, OnIQFeedConnected )
    MESSAGE_HANDLER( WM_IQFEED_DISCONNECTED, OnIQFeedDisconnected )
    MESSAGE_HANDLER( WM_IQFEED_SENDDONE, OnIQFeedSendDone )
    MESSAGE_HANDLER( WM_IQFEED_ERROR, OnIQFeedError )

    COMMAND_HANDLER(IDC_EDTSYMBOL, EN_CHANGE, OnEnChangeEdtsymbol)
    COMMAND_HANDLER(IDC_BTNSTART, BN_CLICKED, OnBnClickedBtnstart)
    COMMAND_HANDLER(IDC_BTNSTOP, BN_CLICKED, OnBnClickedBtnstop)
    NOTIFY_HANDLER(IDC_LISTTAPE, LVN_ITEMCHANGED, OnLvnItemchangedListtape)
    MSG_WM_DESTROY(OnDestroy)
    CHAIN_MSG_MAP(CDialogResize<CTapeReaderView>)
	END_MSG_MAP()

  CEdit m_edtSymbol;
  CButton m_btnStart;
  CButton m_btnStop;
  CListViewCtrl m_lvTape; // sub class this later and turn into own class for reuse

  LRESULT OnIQFeedInitialized( UINT, WPARAM, LPARAM, BOOL& );
  LRESULT OnIQFeedConnected( UINT, WPARAM, LPARAM, BOOL& );
  LRESULT OnIQFeedDisconnected( UINT, WPARAM, LPARAM, BOOL& );
  LRESULT OnIQFeedSendDone( UINT, WPARAM, LPARAM, BOOL& );
  LRESULT OnIQFeedError( UINT, WPARAM, LPARAM, BOOL& );

  LRESULT OnIQFeedUpdate( UINT, WPARAM, LPARAM, BOOL& );
  LRESULT OnIQFeedSummary( UINT, WPARAM, LPARAM, BOOL& );
  LRESULT OnIQFeedFundamental( UINT, WPARAM, LPARAM, BOOL& );

  void OnDestroy();

private:
  typedef CDialogImpl<CTapeReaderView> CThisClass;

  CIQFeed<CTapeReaderView>* m_pIQFeed;

  std::string m_sSymbol;


};
