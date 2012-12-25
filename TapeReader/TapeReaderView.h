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

// TapeReaderView.h : interface of the TapeReaderView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <deque>

#include <boost/array.hpp>

#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/array/elem.hpp>
#include <boost/preprocessor/array/size.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>

#include <OUCommon/Colour.h>
#include <TFIQFeed/IQFeedMsgShim.h>

#define COLHDR_ARRAY_ELEMENT_SIZE 7
#define COLHDR_ARRAY_ROW_COUNT 8
#define COLHDR_ARRAY \
  (COLHDR_ARRAY_ROW_COUNT,  \
    ( /* Col 0,           1,      2,             3,   4,           5,       6 */ \
      (COLHDR_COL_Time  , "Time", LVCFMT_LEFT  , 60, std::string, vTime,    0  ), \
      (COLHDR_COL_BATE  , "BATE", LVCFMT_CENTER, 60, std::string, vBate,    "" ), \
      (COLHDR_COL_Bid   , "Bid",  LVCFMT_RIGHT , 57, double,      vBid,     0.0), \
      (COLHDR_COL_BidVol, "Vol",  LVCFMT_RIGHT , 36, int,         vBidVol,  0  ), \
      (COLHDR_COL_Tck   , "Tick", LVCFMT_RIGHT , 57, double,      vTick,    0.0), \
      (COLHDR_COL_TckVol, "Vol",  LVCFMT_RIGHT , 36, int,         vTickVol, 0  ), \
      (COLHDR_COL_Ask   , "Ask",  LVCFMT_RIGHT , 57, double,      vAsk,     0.0), \
      (COLHDR_COL_AskVol, "Vol",  LVCFMT_RIGHT , 36, int,         vAskVol,  0  )  \
      ) \
    ) \
  /**/

#define COLHDR_EXTRACT_COL_DETAILS(z, n, m, text) \
  BOOST_PP_TUPLE_ELEM( \
    COLHDR_ARRAY_ELEMENT_SIZE, m, \
      BOOST_PP_ARRAY_ELEM( n, COLHDR_ARRAY ) \
    )

#define COLHDR_EXTRACT_ENUM_LIST(z, n, text) \
  BOOST_PP_COMMA_IF(n) \
  COLHDR_EXTRACT_COL_DETAILS( z, n, 0, text )

#define COLHDR_EMIT_InsertColumn( z, n, VAR ) \
  m_lvTape.InsertColumn( VAR++, \
    _T(COLHDR_EXTRACT_COL_DETAILS(z, n, 1, ~)), \
    COLHDR_EXTRACT_COL_DETAILS(z, n, 2, ~), \
    COLHDR_EXTRACT_COL_DETAILS(z, n, 3, ~) \
    );

#define COLHDR_EMIT_DefineVars( z, n, text ) \
  COLHDR_EXTRACT_COL_DETAILS(z, n, 4, ~) \
  COLHDR_EXTRACT_COL_DETAILS(z, n, 5, ~)\
  ;

#define COLHDR_EMIT_StringVars( z, n, text ) \
  std::string \
  COLHDR_EXTRACT_COL_DETAILS(z, n, 5, ~)\
  ;

class TapeReaderView : public CDialogImpl<TapeReaderView>,
                        public CDialogResize<TapeReaderView>
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

  BEGIN_DLGRESIZE_MAP(TapeReaderView)
    DLGRESIZE_CONTROL( IDC_EDTSYMBOL, DLSZ_SIZE_X )
    DLGRESIZE_CONTROL( IDC_BTNSTART, DLSZ_MOVE_X )
    DLGRESIZE_CONTROL( IDC_BTNSTOP, DLSZ_MOVE_X )
    DLGRESIZE_CONTROL( IDC_LISTTAPE, DLSZ_SIZE_X | DLSZ_SIZE_Y )
  END_DLGRESIZE_MAP()

  TapeReaderView( void );
  ~TapeReaderView( void );
protected:

  enum enumMessages {
    WM_IQFEED_CONNECTED = WM_USER + 1,
    WM_IQFEED_SENDDONE,
    WM_IQFEED_DISCONNECTED,
    WM_IQFEED_ERROR, 

    WM_IQFEED_UPDATE,
    WM_IQFEED_SUMMARY,
    WM_IQFEED_NEWS,
    WM_IQFEED_FUNDAMENTAL,
    WM_IQFEED_TIME,
    WM_IQFEED_SYSTEM,
    WM_WAIT_FOR_DISCONNECT
  };

  enum enumUIEnableState {
    UI_STARTING,
    UI_NOSYMBOL,
    UI_SYMBOLENTRY,
    UI_STARTED,
    UI_DISCONNECTED
  } m_stateUI;

  enum enumColHdrCol {
    BOOST_PP_REPEAT( BOOST_PP_ARRAY_SIZE( COLHDR_ARRAY ), COLHDR_EXTRACT_ENUM_LIST, ~ )
  };
  // define variables to be viewed in the row
  struct structRowItems {
    BOOST_PP_REPEAT( BOOST_PP_ARRAY_SIZE( COLHDR_ARRAY ), COLHDR_EMIT_DefineVars, ~ )
  } m_prvValues;

  struct structRowForDisplay {
    ou::Colour::enumColour colourRow;
    BOOST_PP_REPEAT( BOOST_PP_ARRAY_SIZE( COLHDR_ARRAY ), COLHDR_EMIT_StringVars, ~ )
  };

  typedef std::deque<structRowForDisplay> rowqueue_t;

  bool m_bRunning; // need to store one row item before start of comparisons
  double m_dblMinTick;
  double m_dblMaxTick;

  void StopData( void );
  void UpdateUIState( void );

	BEGIN_MSG_MAP_EX(TapeReaderView)
//	BEGIN_MSG_MAP(TapeReaderView)
    MESSAGE_HANDLER( WM_IQFEED_UPDATE, OnIQFeedUpdate )
    MESSAGE_HANDLER( WM_IQFEED_SUMMARY, OnIQFeedSummary )
    MESSAGE_HANDLER( WM_IQFEED_FUNDAMENTAL, OnIQFeedFundamental )

    MESSAGE_HANDLER( WM_IQFEED_CONNECTED, OnIQFeedConnected )
    MESSAGE_HANDLER( WM_IQFEED_DISCONNECTED, OnIQFeedDisconnected )
    MESSAGE_HANDLER( WM_IQFEED_SENDDONE, OnIQFeedSendDone )
    MESSAGE_HANDLER( WM_IQFEED_ERROR, OnIQFeedError )

    COMMAND_HANDLER(IDC_EDTSYMBOL, EN_CHANGE, OnEnChangeEdtsymbol)
    COMMAND_HANDLER(IDC_BTNSTART, BN_CLICKED, OnBnClickedBtnstart)
    COMMAND_HANDLER(IDC_BTNSTOP, BN_CLICKED, OnBnClickedBtnstop)
    NOTIFY_HANDLER(IDC_LISTTAPE, LVN_ITEMCHANGED, OnLvnItemchangedListtape)

    MESSAGE_HANDLER( WM_WAIT_FOR_DISCONNECT, OnWaitForDisconnect )

    MSG_WM_INITDIALOG(OnInitDialog)
    MSG_WM_DESTROY(OnDestroy)
    MSG_WM_CLOSE(OnClose)
    CHAIN_MSG_MAP(CDialogResize<TapeReaderView>)
	END_MSG_MAP()

  CEdit m_edtSymbol;
  CButton m_btnStart;
  CButton m_btnStop;
  CListViewCtrl m_lvTape; // sub class this later and turn into own class for reuse

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
  LRESULT OnEnChangeEdtsymbol(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnBnClickedBtnstart(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnBnClickedBtnstop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnLvnItemchangedListtape(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);

  HWND Create(HWND hWndParent, LPARAM dwInitParam = NULL);

  LRESULT OnIQFeedConnected( UINT, WPARAM, LPARAM, BOOL& );
  LRESULT OnIQFeedDisconnected( UINT, WPARAM, LPARAM, BOOL& );
  LRESULT OnIQFeedSendDone( UINT, WPARAM, LPARAM, BOOL& );
  LRESULT OnIQFeedError( UINT, WPARAM, LPARAM, BOOL& );

  LRESULT OnIQFeedUpdate( UINT, WPARAM, LPARAM, BOOL& );
  LRESULT OnIQFeedSummary( UINT, WPARAM, LPARAM, BOOL& );
  LRESULT OnIQFeedFundamental( UINT, WPARAM, LPARAM, BOOL& );

  LRESULT OnWaitForDisconnect( UINT, WPARAM, LPARAM, BOOL& );

  BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
  void OnDestroy();
  void OnClose();

private:
  typedef CDialogImpl<TapeReaderView> CThisClass;

  ou::tf::iqfeed::MsgShim<TapeReaderView>::structMessageDestinations m_Destinations;
  ou::tf::iqfeed::MsgShim<TapeReaderView>* m_pIQFeed;
  typedef ou::tf::iqfeed::MsgShim<TapeReaderView>::linebuffer_t linebuffer_t;

  std::string m_sSymbol;

};
