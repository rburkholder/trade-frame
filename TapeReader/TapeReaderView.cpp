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

// TapeReaderView.cpp : implementation of the CTapeReaderView class
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


CTapeReaderView::CTapeReaderView( void ) 
: CDialogImpl<CTapeReaderView>(), CDialogResize<CTapeReaderView>(),
  m_Destinations( this, WM_IQFEED_CONNECTED, WM_IQFEED_SENDDONE, WM_IQFEED_DISCONNECTED, WM_IQFEED_ERROR,
  WM_IQFEED_UPDATE, WM_IQFEED_SUMMARY, 0, WM_IQFEED_FUNDAMENTAL, 0, 0 ),
  m_stateUI( UI_STARTING ),
  m_bRunning( false )
{
}

CTapeReaderView::~CTapeReaderView( void ) {
}

BOOL CTapeReaderView::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

HWND CTapeReaderView::Create(HWND hWndParent, LPARAM dwInitParam) {

  HWND h;
  h = CThisClass::Create( hWndParent, dwInitParam );

  return h;
}

BOOL CTapeReaderView::OnInitDialog(CWindow wndFocus, LPARAM lInitParam) {

  DlgResize_Init( false, true );

  m_edtSymbol = GetDlgItem( IDC_EDTSYMBOL );
  m_btnStart = GetDlgItem( IDC_BTNSTART );
  m_btnStop = GetDlgItem( IDC_BTNSTOP );
  m_lvTape = GetDlgItem( IDC_LISTTAPE );

  m_edtSymbol.SetFocus(); 

  int ix = 0;
  BOOST_PP_REPEAT( BOOST_PP_ARRAY_SIZE( COLHDR_ARRAY ), COLHDR_EMIT_InsertColumn, ix )

  m_pIQFeed = new CIQFeed<CTapeReaderView>( &_Module, m_Destinations );
  m_pIQFeed->Connect();

  return TRUE;
}

void CTapeReaderView::OnDestroy( void ) {
  StopData();
  m_pIQFeed->Disconnect();
  delete m_pIQFeed;
}

LRESULT CTapeReaderView::OnBnClickedBtnstart(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
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

void CTapeReaderView::StopData( void ) {
  if ( UI_STARTED == m_stateUI ) {
    std::string sSend = _T( "r" );
    sSend += m_sSymbol;
    sSend += _T( "\n" );

    m_pIQFeed->Send( sSend );

    m_stateUI = UI_SYMBOLENTRY;
    UpdateUIState();
  }
}

LRESULT CTapeReaderView::OnBnClickedBtnstop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{

  StopData();

  return 0;
}

LRESULT CTapeReaderView::OnEnChangeEdtsymbol(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  // TODO:  If this is a RICHEDIT control, the control will not
  // send this notification unless you override the CDialogImpl<CTapeReaderView>::OnInitDialog()
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

void CTapeReaderView::UpdateUIState( void ) {
  switch ( m_stateUI ) {
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


LRESULT CTapeReaderView::OnLvnItemchangedListtape(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
  LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
  

  return 0;
}

LRESULT CTapeReaderView::OnIQFeedConnected( UINT, WPARAM, LPARAM, BOOL& bHandled ) {

  m_stateUI = UI_NOSYMBOL;
  UpdateUIState();

  m_edtSymbol.SetFocus();

  bHandled = true;
  return 1;
}

LRESULT CTapeReaderView::OnIQFeedDisconnected( UINT, WPARAM, LPARAM, BOOL& bHandled ) {

  m_stateUI = UI_STARTING;
  UpdateUIState();

  bHandled = true;
  return 1;
}

LRESULT CTapeReaderView::OnIQFeedSendDone( UINT, WPARAM, LPARAM, BOOL& bHandled ) {
  bHandled = true;
  return 1;
}

LRESULT CTapeReaderView::OnIQFeedError( UINT, WPARAM, LPARAM, BOOL& bHandled ) {
  bHandled = true;
  return 1;
}

// Data Handling

// S,WATCHES,@ES,+CL#,@IA#,@QM#,@IE#,@QO#,BZ#,CRD#,@ES#,@ES@,#ES#,@LA#,+CLF#,@NQ#,@NS#,@NX#,#NX#,@QC#,@YM#

LRESULT CTapeReaderView::OnIQFeedUpdate( UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled ) {

  CIQFUpdateMessage* msg = reinterpret_cast<CIQFUpdateMessage*>( lParam );

  if ( 'N' == *msg->FieldBegin( CIQFUpdateMessage::QPLast ) ) {  // field has "Not Found" in numeric field
    CWindow::MessageBoxA( "Symbol Not Found", "Error", MB_OK );
    m_stateUI = UI_SYMBOLENTRY;
    UpdateUIState();
  }
  else {
    std::string sSymbol( 
      msg->FieldBegin( CIQFUpdateMessage::QPSymbol ),
      msg->FieldEnd( CIQFUpdateMessage::QPSymbol ) );
    if ( sSymbol == m_sSymbol ) {
      std::string sLastTradeTime( 
        msg->FieldBegin( CIQFUpdateMessage::QPLastTradeTime ),
        msg->FieldEnd( CIQFUpdateMessage::QPLastTradeTime ) );
      if ( 9 == sLastTradeTime.length() ) {
        std::string sBid( msg->FieldBegin( CIQFUpdateMessage::QPBid ), msg->FieldEnd( CIQFUpdateMessage::QPBid ) );
        std::string sBidVol( msg->FieldBegin( CIQFUpdateMessage::QPBidSize ), msg->FieldEnd( CIQFUpdateMessage::QPBidSize ) );
        std::string sTick( msg->FieldBegin( CIQFUpdateMessage::QPLast ), msg->FieldEnd( CIQFUpdateMessage::QPLast ) );
        std::string sTickVol( msg->FieldBegin( CIQFUpdateMessage::QPLastVol ), msg->FieldEnd( CIQFUpdateMessage::QPLastVol ) );
        std::string sAsk( msg->FieldBegin( CIQFUpdateMessage::QPAsk ), msg->FieldEnd( CIQFUpdateMessage::QPAsk ) );
        std::string sAskVol( msg->FieldBegin( CIQFUpdateMessage::QPAskSize ), msg->FieldEnd( CIQFUpdateMessage::QPAskSize ) );
//            sPrice.assign( msg->FieldBegin( CIQFUpdateMessage::QPExtTradeLast ), msg->FieldEnd( CIQFUpdateMessage::QPExtTradeLast ) );
//            sSize.assign( msg->FieldBegin( CIQFUpdateMessage::QPLastVol ), msg->FieldEnd( CIQFUpdateMessage::QPLastVol ) );

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
          ri.vBid = msg->Double( CIQFUpdateMessage::QPBid );
//          ri.vBidVol = msg->Integer( CIQFUpdateMessage::QPBidSize );
          ri.vTick = msg->Double( CIQFUpdateMessage::QPLast );
//          ri.vTickVol = msg->Integer( CIQFUpdateMessage::QPLastVol );
          ri.vAsk = msg->Double( CIQFUpdateMessage::QPAsk );
//          ri.vAskVol = msg->Integer( CIQFUpdateMessage::QPAskSize );
          
          COLORREF cBack = Colour::Beige;
          // set colours so green range for higher buy/sell, new high buy/sell
          // set colours so red range for lower buy/sell, new log buy/sell

          if ( m_bRunning ) {
            if ( 't' == sLastTradeTime[8] ) {  // treat as trade
              double dblMid = ( ri.vBid + ri.vAsk ) / 2.0;
              ri.vBate = "Trade";
              if ( ri.vTick > dblMid ) {
                cBack = Colour::LightGreen;
                ri.vBate = "Buy";
              }
              else {
                cBack = Colour::LightPink;
                ri.vBate = "Sell";
              }
              if ( ri.vTick > m_dblMaxTick ) {
                cBack = Colour::Blue;
              }
              if ( ri.vTick < m_dblMinTick ) {
                cBack = Colour::Orange;
              }
            }
            else {  // process bid/ask stuff
              if ( ( ri.vBid > m_prvValues.vBid ) && ( ri.vAsk < m_prvValues.vAsk ) ) {
                // new inside bid and new inside ask
                cBack = Colour::Gray;
                ri.vBate = "Best Bid/Ask";
              }
              else {
                if ( ( ri.vBid > m_prvValues.vBid ) || ( ri.vAsk < m_prvValues.vAsk ) ) {
                  if ( ri.vBid > m_prvValues.vBid ) {
                    cBack = Colour::Yellow;
                    ri.vBate = "Best Bid";
                  }
                  else {
                    cBack = Colour::White;
                    ri.vBate = "Best Ask";
                  }
                }
                else {  // regular bid or regular ask change
                  if ( 'b' == sLastTradeTime[8] ) {
                    ri.vBate = "Bid";
                    cBack = Colour::LightYellow;
                  }
                  if ( 'a' == sLastTradeTime[8] ) {
                    ri.vBate = "Ask";
                    cBack = Colour::Snow;
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

  m_pIQFeed->UpdateDone( wParam, lParam );

  bHandled = true;
  return 1;
}

LRESULT CTapeReaderView::OnIQFeedSummary( UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled ) {

  CIQFSummaryMessage* msg = reinterpret_cast<CIQFSummaryMessage*>( lParam );

  m_pIQFeed->SummaryDone( wParam, lParam );

  bHandled = true;
  return 1;
}

LRESULT CTapeReaderView::OnIQFeedFundamental( UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled ) {

  CIQFFundamentalMessage* msg = reinterpret_cast<CIQFFundamentalMessage*>( lParam );

  m_pIQFeed->FundamentalDone( wParam, lParam );

  bHandled = true;
  return 1;
}
