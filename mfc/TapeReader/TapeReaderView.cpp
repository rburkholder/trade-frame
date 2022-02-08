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

// TapeReaderView.cpp : implementation of the TapeReaderView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

// custom on
// http://msdn.microsoft.com/en-us/library/e5ewb1h3.aspx
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
// custom off

#include "TapeReaderView.h"

TapeReaderView::TapeReaderView( void ) 
: CDialogImpl<TapeReaderView>(), 
  CDialogResize<TapeReaderView>(),
  m_Destinations( this, WM_IQFEED_CONNECTED, WM_IQFEED_SENDDONE, WM_IQFEED_DISCONNECTED, WM_IQFEED_ERROR,
  WM_IQFEED_UPDATE, WM_IQFEED_SUMMARY, 0, WM_IQFEED_FUNDAMENTAL, 0, 0 ),
  m_stateUI( UI_STARTING ),
  m_bRunning( false )
{
  m_pIQFeed = new ou::tf::iqfeed::MsgShim<TapeReaderView>( m_Destinations );
}

TapeReaderView::~TapeReaderView( void ) {
  delete m_pIQFeed;
}

BOOL TapeReaderView::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

HWND TapeReaderView::Create(HWND hWndParent, LPARAM dwInitParam) {

  HWND h;
  h = CThisClass::Create( hWndParent, dwInitParam );

  return h;
}

BOOL TapeReaderView::OnInitDialog(CWindow wndFocus, LPARAM lInitParam) {

  DlgResize_Init( false, true );

  m_edtSymbol = GetDlgItem( IDC_EDTSYMBOL );
  m_btnStart = GetDlgItem( IDC_BTNSTART );
  m_btnStop = GetDlgItem( IDC_BTNSTOP );
  m_lvTape = GetDlgItem( IDC_LISTTAPE );

  m_edtSymbol.SetFocus(); 

  int ix = 0;
  BOOST_PP_REPEAT( BOOST_PP_ARRAY_SIZE( COLHDR_ARRAY ), COLHDR_EMIT_InsertColumn, ix )

  m_pIQFeed->Connect();

  return TRUE;
}

void TapeReaderView::OnClose( void ) {  // doesn't appear to be called, needs to be used in main window
  // main thread would need to wait for disconnect and then do destroy
}

// wait for disconnect completion
LRESULT TapeReaderView::OnWaitForDisconnect( UINT, WPARAM, LPARAM, BOOL& bHandled ) {
  return TRUE;
}

void TapeReaderView::OnDestroy( void ) {
  StopData();
  m_pIQFeed->Disconnect();
}

LRESULT TapeReaderView::OnBnClickedBtnstart(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{

  m_stateUI = UI_STARTING;
  UpdateUIState();

  m_lvTape.DeleteAllItems();

  // determine symbol to process (should drop white space front and back)
  typedef std::string::value_type char_t ;
  int len = m_edtSymbol.GetWindowTextLengthA();
  char_t* pText = new char_t[ len + 1 ];
  m_edtSymbol.GetWindowText( (LPTSTR) pText, len + 1 ); 
  m_sSymbol = pText;
  delete[] pText;

  // turn on watch
  std::string sSend = _T( "w" );
  sSend += m_sSymbol;
  sSend += _T( "\n" );

  m_pIQFeed->Send( sSend );

  m_stateUI = UI_STARTED;
  UpdateUIState();

  return 0;
}

void TapeReaderView::StopData( void ) {
  if ( UI_STARTED == m_stateUI ) {
    std::string sSend = _T( "r" );
    sSend += m_sSymbol;
    sSend += _T( "\n" );

    m_pIQFeed->Send( sSend );

    m_stateUI = UI_SYMBOLENTRY;
    UpdateUIState();
  }
}

LRESULT TapeReaderView::OnBnClickedBtnstop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{

  StopData();

  return 0;
}

LRESULT TapeReaderView::OnEnChangeEdtsymbol(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  // TODO:  If this is a RICHEDIT control, the control will not
  // send this notification unless you override the CDialogImpl<TapeReaderView>::OnInitDialog()
  // function and call CRichEditCtrl().SetEventMask()
  // with the ENM_CHANGE flag ORed into the mask.

  // TODO:  Add your control notification handler code here
  if ( 0 == m_edtSymbol.GetWindowTextLengthA() ) {
    m_stateUI = UI_NOSYMBOL;
  }
  else {
    m_stateUI = UI_SYMBOLENTRY;
  }
  UpdateUIState();

  return 0;
}

void TapeReaderView::UpdateUIState( void ) {
  switch ( m_stateUI ) {
    case UI_DISCONNECTED:
    case UI_STARTING:
      m_btnStart.EnableWindow( false );
      m_btnStop.EnableWindow( false );
      m_edtSymbol.EnableWindow( false );
    case UI_NOSYMBOL: 
      m_btnStart.EnableWindow( false );
      m_btnStop.EnableWindow( false );
      m_edtSymbol.EnableWindow( true );
      break;
    case UI_SYMBOLENTRY:
      m_btnStart.EnableWindow( true );
      m_btnStop.EnableWindow( false );
      m_edtSymbol.EnableWindow( true );
      break;
    case UI_STARTED:
      m_btnStart.EnableWindow( false );
      m_btnStop.EnableWindow( true );
      m_edtSymbol.EnableWindow( false );
      break;
  }
}


LRESULT TapeReaderView::OnLvnItemchangedListtape(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
  LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
  

  return 0;
}

LRESULT TapeReaderView::OnIQFeedConnected( UINT, WPARAM, LPARAM, BOOL& bHandled ) {

  m_stateUI = UI_NOSYMBOL;
  UpdateUIState();

  m_edtSymbol.SetFocus();

  bHandled = true;
  return 1;
}

LRESULT TapeReaderView::OnIQFeedDisconnected( UINT, WPARAM, LPARAM, BOOL& bHandled ) {
  // this message never arrives

  m_stateUI = UI_DISCONNECTED;
  UpdateUIState();

  DestroyWindow();

  bHandled = true;
  return 1;
}

LRESULT TapeReaderView::OnIQFeedSendDone( UINT, WPARAM, LPARAM, BOOL& bHandled ) {
  bHandled = true;
  return 1;
}

LRESULT TapeReaderView::OnIQFeedError( UINT, WPARAM, LPARAM, BOOL& bHandled ) {
  bHandled = true;
  return 1;
}

// Data Handling

// S,WATCHES,@ES,+CL#,@IA#,@QM#,@IE#,@QO#,BZ#,CRD#,@ES#,@ES@,#ES#,@LA#,+CLF#,@NQ#,@NS#,@NX#,#NX#,@QC#,@YM#

LRESULT TapeReaderView::OnIQFeedUpdate( UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled ) {

  ou::tf::IQFUpdateMessage* msg = reinterpret_cast<ou::tf::IQFUpdateMessage*>( lParam );

  if ( 'N' == *msg->FieldBegin( ou::tf::IQFUpdateMessage::QPLast ) ) {  // field has "Not Found" in numeric field
    CWindow::MessageBoxA( "Symbol Not Found", "Error", MB_OK );
    m_stateUI = UI_SYMBOLENTRY;
    UpdateUIState();
  }
  else {
    std::string sSymbol( 
      msg->FieldBegin( ou::tf::IQFUpdateMessage::QPSymbol ),
      msg->FieldEnd( ou::tf::IQFUpdateMessage::QPSymbol ) );
    if ( sSymbol == m_sSymbol ) {
      std::string sLastTradeTime( 
        msg->FieldBegin(ou::tf:: IQFUpdateMessage::QPLastTradeTime ),
        msg->FieldEnd( ou::tf::IQFUpdateMessage::QPLastTradeTime ) );
      if ( 9 == sLastTradeTime.length() ) {
        std::string sBid( msg->FieldBegin( ou::tf::IQFUpdateMessage::QPBid ), msg->FieldEnd( ou::tf::IQFUpdateMessage::QPBid ) );
        std::string sBidVol( msg->FieldBegin( ou::tf::IQFUpdateMessage::QPBidSize ), msg->FieldEnd( ou::tf::IQFUpdateMessage::QPBidSize ) );
        std::string sTick( msg->FieldBegin( ou::tf::IQFUpdateMessage::QPLast ), msg->FieldEnd( ou::tf::IQFUpdateMessage::QPLast ) );
        std::string sTickVol( msg->FieldBegin( ou::tf::IQFUpdateMessage::QPLastVol ), msg->FieldEnd( ou::tf::IQFUpdateMessage::QPLastVol ) );
        std::string sAsk( msg->FieldBegin( ou::tf::IQFUpdateMessage::QPAsk ), msg->FieldEnd( ou::tf::IQFUpdateMessage::QPAsk ) );
        std::string sAskVol( msg->FieldBegin( ou::tf::IQFUpdateMessage::QPAskSize ), msg->FieldEnd( ou::tf::IQFUpdateMessage::QPAskSize ) );
//            sPrice.assign( msg->FieldBegin( IQFUpdateMessage::QPExtTradeLast ), msg->FieldEnd( IQFUpdateMessage::QPExtTradeLast ) );
//            sSize.assign( msg->FieldBegin( IQFUpdateMessage::QPLastVol ), msg->FieldEnd( IQFUpdateMessage::QPLastVol ) );

        int cntPerPage = m_lvTape.GetCountPerPage();
        if ( 0 < cntPerPage ) {

          int ix;
          while ( cntPerPage <= (ix = m_lvTape.GetItemCount() ) ) {
            m_lvTape.DeleteItem( ix - 1 );
          }

          m_lvTape.InsertItem( 0, sLastTradeTime.c_str() );
          m_lvTape.SetItemText( 0, COLHDR_COL_Bid, sBid.c_str() );
          m_lvTape.SetItemText( 0, COLHDR_COL_BidVol, sBidVol.c_str() );
          m_lvTape.SetItemText( 0, COLHDR_COL_Tck, sTick.c_str() );
          m_lvTape.SetItemText( 0, COLHDR_COL_TckVol, sTickVol.c_str() );
          m_lvTape.SetItemText( 0, COLHDR_COL_Ask, sAsk.c_str() );
          m_lvTape.SetItemText( 0, COLHDR_COL_AskVol, sAskVol.c_str() );

          structRowItems ri;
          ri.vTime = sLastTradeTime;
          ri.vBid = msg->Double( ou::tf::IQFUpdateMessage::QPBid );
//          ri.vBidVol = msg->Integer( IQFUpdateMessage::QPBidSize );
          ri.vTick = msg->Double( ou::tf::IQFUpdateMessage::QPLast );
//          ri.vTickVol = msg->Integer( IQFUpdateMessage::QPLastVol );
          ri.vAsk = msg->Double( ou::tf::IQFUpdateMessage::QPAsk );
//          ri.vAskVol = msg->Integer( IQFUpdateMessage::QPAskSize );
          
          COLORREF cBack = ou::Colour::Beige;
          // set colours so green range for higher buy/sell, new high buy/sell
          // set colours so red range for lower buy/sell, new log buy/sell

          if ( m_bRunning ) {
            if ( 't' == sLastTradeTime[8] ) {  // treat as trade
              double dblMid = ( ri.vBid + ri.vAsk ) / 2.0;
              ri.vBate = "Trade";
              if ( ri.vTick > dblMid ) {
                cBack = ou::Colour::LightGreen;
                ri.vBate = "Buy";
              }
              else {
                cBack = ou::Colour::LightPink;
                ri.vBate = "Sell";
              }
              if ( ri.vTick > m_dblMaxTick ) {
                cBack = ou::Colour::Blue;
              }
              if ( ri.vTick < m_dblMinTick ) {
                cBack = ou::Colour::Orange;
              }
            }
            else {  // process bid/ask stuff
              if ( ( ri.vBid > m_prvValues.vBid ) && ( ri.vAsk < m_prvValues.vAsk ) ) {
                // new inside bid and new inside ask
                cBack = ou::Colour::Gray;
                ri.vBate = "Best Bid/Ask";
              }
              else {
                if ( ( ri.vBid > m_prvValues.vBid ) || ( ri.vAsk < m_prvValues.vAsk ) ) {
                  if ( ri.vBid > m_prvValues.vBid ) {
                    cBack = ou::Colour::Yellow;
                    ri.vBate = "Best Bid";
                  }
                  else {
                    cBack = ou::Colour::White;
                    ri.vBate = "Best Ask";
                  }
                }
                else {  // regular bid or regular ask change
                  if ( 'b' == sLastTradeTime[8] ) {
                    ri.vBate = "Bid";
                    cBack = ou::Colour::LightYellow;
                  }
                  if ( 'a' == sLastTradeTime[8] ) {
                    ri.vBate = "Ask";
                    cBack = ou::Colour::Snow;
                  }
                }
              }
            }
            m_lvTape.SetItemText( 0, COLHDR_COL_BATE, ri.vBate.c_str() );
          }
          else {
            m_bRunning = true;
            m_dblMinTick = m_dblMaxTick = ri.vTick;
          }
          m_prvValues = ri;
          m_dblMinTick = std::min<double>( m_dblMinTick, ri.vTick );
          m_dblMaxTick = std::max<double>( m_dblMaxTick, ri.vTick );

          m_prvValues = ri;
        }
      }
    }
  }

  linebuffer_t* p = reinterpret_cast<linebuffer_t*>( wParam );
  m_pIQFeed->UpdateDone( p, msg );

  bHandled = true;
  return 1;
}


LRESULT TapeReaderView::OnIQFeedSummary( UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled ) {

  ou::tf::IQFSummaryMessage* msg = reinterpret_cast<ou::tf::IQFSummaryMessage*>( lParam );
  linebuffer_t* p = reinterpret_cast<linebuffer_t*>( wParam );

  m_pIQFeed->SummaryDone( p, msg );

  bHandled = true;
  return 1;
}

LRESULT TapeReaderView::OnIQFeedFundamental( UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled ) {

  ou::tf::IQFFundamentalMessage* msg = reinterpret_cast<ou::tf::IQFFundamentalMessage*>( lParam );
  linebuffer_t* p = reinterpret_cast<linebuffer_t*>( wParam );

  m_pIQFeed->FundamentalDone( p, msg );

  bHandled = true;
  return 1;
}
