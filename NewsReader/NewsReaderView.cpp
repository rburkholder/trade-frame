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

// NewsReaderView.cpp : implementation of the CNewsReaderView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include <sstream>

#include "NewsReaderView.h"

CNewsReaderView::CNewsReaderView() 
: CDialogImpl<CNewsReaderView>(), CDialogResize<CNewsReaderView>(),
  m_Destinations( this, WM_IQFEED_INITIALIZED, WM_IQFEED_CONNECTED, WM_IQFEED_SENDDONE, WM_IQFEED_DISCONNECTED, WM_IQFEED_ERROR,
  0, 0, WM_IQFEED_NEWS, 0, 0, 0 )
{
}

CNewsReaderView::~CNewsReaderView() {
}

BOOL CNewsReaderView::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

HWND CNewsReaderView::Create(HWND hWndParent, LPARAM dwInitParam) {

  HWND h;
  h = CThisClass::Create( hWndParent, dwInitParam );

  DlgResize_Init( false, true );

  m_treeSources = GetDlgItem( IDC_TREESOURCES );
  m_lvHeadlines = GetDlgItem( IDC_LVHEADLINES );
  m_edtStory = GetDlgItem( IDC_EDITSTORY );

  int i1 = m_lvHeadlines.AddColumn( "Dist", 0 );
  int i2 = m_lvHeadlines.AddColumn( "Symbols", 1 );
  int i3 = m_lvHeadlines.AddColumn( "Headline", 2 );

  m_pIQFeed = new CIQFeed<CNewsReaderView>( &_Module, m_Destinations );

  return h;
}

void CNewsReaderView::OnDestroy( void ) {
  m_pIQFeed->SetNewsOff();  // check to see if message gets fully processed before disconnect occurs
  m_pIQFeed->Disconnect();
  delete m_pIQFeed;
}

void CNewsReaderView::OnSize(UINT nType, CSize size) {
  std::stringstream ss;
  ss << "OnSize " << size.cx << ", " << size.cy << std::endl;
  OutputDebugString( ss.str().c_str() );
}

void CNewsReaderView::OnMove( CPoint ptPos ) {
  OutputDebugString( "OnMove" );
}

LRESULT CNewsReaderView::OnIQFeedInitialized( UINT, WPARAM, LPARAM, BOOL& bHandled ) {
  m_pIQFeed->Connect();
  bHandled = true;
  return 1;
}

LRESULT CNewsReaderView::OnIQFeedConnected( UINT, WPARAM, LPARAM, BOOL& bHandled ) {

//  m_stateUI = UI_NOSYMBOL;
//  UpdateUIState();
  m_pIQFeed->SetNewsOn();

  bHandled = true;
  return 1;
}

LRESULT CNewsReaderView::OnIQFeedDisconnected( UINT, WPARAM, LPARAM, BOOL& bHandled ) {

//  m_stateUI = UI_STARTING;
//  UpdateUIState();

  bHandled = true;
  return 1;
}

LRESULT CNewsReaderView::OnIQFeedSendDone( UINT, WPARAM, LPARAM, BOOL& bHandled ) {
  bHandled = true;
  return 1;
}

LRESULT CNewsReaderView::OnIQFeedError( UINT, WPARAM, LPARAM, BOOL& bHandled ) {
  bHandled = true;
  return 1;
}

LRESULT CNewsReaderView::OnIQFeedNews( UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled ) {
  // for local interpretation of the header

  CIQFNewsMessage* msg = reinterpret_cast<CIQFNewsMessage*>( lParam );

  m_lvHeadlines.InsertItem( 0, msg->Distributor().c_str() );
  m_lvHeadlines.SetItemText( 0, 1, msg->SymbolList().c_str() );
  m_lvHeadlines.SetItemText( 0, 2, msg->Headline().c_str() );

  m_pIQFeed->NewsDone( wParam, lParam );

  bHandled = true;
  return 1;
}

LRESULT CNewsReaderView::OnIQFeedNewsDone( UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled ) {
  // for when another module has processed the message and is passing it along back

  CIQFNewsMessage* msg = reinterpret_cast<CIQFNewsMessage*>( lParam );

  m_pIQFeed->NewsDone( wParam, lParam );

  bHandled = true;
  return 1;
}

