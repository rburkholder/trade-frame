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
  m_Destinations( this, WM_IQFEED_INITIALIZED, WM_IQFEED_CONNECTED, WM_IQFEED_SENDDONE, WM_IQFEED_DISCONNECTED, WM_IQFEED_ERROR,
  WM_IQFEED_UPDATE, WM_IQFEED_SUMMARY, 0, WM_IQFEED_FUNDAMENTAL, 0, 0 ),
  m_stateUI( UI_STARTING )
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

  DlgResize_Init( false, true );

  m_edtSymbol = GetDlgItem( IDC_EDTSYMBOL );
  m_btnStart = GetDlgItem( IDC_BTNSTART );
  m_btnStop = GetDlgItem( IDC_BTNSTOP );
  m_lvTape = GetDlgItem( IDC_LISTTAPE );

  int i1 = m_lvTape.AddColumn( "Time", 0 );
  int i2 = m_lvTape.AddColumn( "BATE", 1 );
  int i3 = m_lvTape.AddColumn( "Size", 2 );
  int i4 = m_lvTape.AddColumn( "Price", 3 );

  m_pIQFeed = new CIQFeed<CTapeReaderView>( &_Module, m_Destinations );

  return h;
}

void CTapeReaderView::OnDestroy( void ) {
  m_pIQFeed->Disconnect();
  delete m_pIQFeed;
}

LRESULT CTapeReaderView::OnBnClickedBtnstart(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{

  m_stateUI = UI_STARTING;
  UpdateUIState();

  typedef std::string::value_type char_t ;
  int l = m_edtSymbol.GetWindowTextLengthA();
  char_t* pText = new char_t[ l + 1 ];
  m_edtSymbol.GetWindowText( (LPTSTR) pText, l + 1 ); 
  m_sSymbol = pText;
  delete[] pText;

  std::string sSend = _T( "w" );
  sSend += m_sSymbol;
  sSend += _T( "\n" );

  m_pIQFeed->Send( sSend );

  return 0;
}

LRESULT CTapeReaderView::OnBnClickedBtnstop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{

  std::string sSend = _T( "r" );
  sSend += m_sSymbol;
  sSend += _T( "\n" );

  m_pIQFeed->Send( sSend );

  m_stateUI = UI_SYMBOLENTRY;
  UpdateUIState();

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

LRESULT CTapeReaderView::OnIQFeedInitialized( UINT, WPARAM, LPARAM, BOOL& bHandled ) {
  m_pIQFeed->Connect();
  bHandled = true;
  return 1;
}

LRESULT CTapeReaderView::OnIQFeedConnected( UINT, WPARAM, LPARAM, BOOL& bHandled ) {

  m_stateUI = UI_NOSYMBOL;
  UpdateUIState();

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

  if ( 'N' == *msg->FieldBegin( CIQFUpdateMessage::QPLast ) ) {
    CWindow::MessageBoxA( "Symbol Not Found", "Error", MB_OK );
    m_stateUI = UI_SYMBOLENTRY;
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
        switch ( sLastTradeTime[ 8 ] ) {
          case 't':
            break;
          case 'T':
            break;
          case 'b':
            break;
          case 'a':
            break;
          case 'o':
            break;
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
