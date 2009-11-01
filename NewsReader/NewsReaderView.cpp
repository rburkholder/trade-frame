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
  m_Destinations( this, WM_IQFEED_CONNECTED, WM_IQFEED_SENDDONE, WM_IQFEED_DISCONNECTED, WM_IQFEED_ERROR,
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

  return h;
}

BOOL CNewsReaderView::OnInitDialog(CWindow wndFocus, LPARAM lInitParam) {

  DlgResize_Init( false, true );

  m_treeSources = GetDlgItem( IDC_TREESOURCES );
  m_lvHeadlines = GetDlgItem( IDC_LVHEADLINES );
  m_edtStory = GetDlgItem( IDC_EDITSTORY );

  int i1 = m_lvHeadlines.AddColumn( "Dist", 0 );
  int i2 = m_lvHeadlines.AddColumn( "Symbols", 1 );
  int i3 = m_lvHeadlines.AddColumn( "Headline", 2 );

  m_lvHeadlines.SetColumnWidth( 0, 50 );
  m_lvHeadlines.SetColumnWidth( 1, 70 );
  m_lvHeadlines.SetColumnWidth( 2, 400 );

  m_pIQFeed = new CIQFeed<CNewsReaderView>( &_Module, m_Destinations );
  m_pIQFeed->Connect();

  return TRUE;
}

void CNewsReaderView::OnDestroy( void ) {
  m_pIQFeedNewsQuery->Disconnect();
  delete m_pIQFeedNewsQuery;
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

LRESULT CNewsReaderView::OnIQFeedConnected( UINT, WPARAM, LPARAM, BOOL& bHandled ) {

//  m_stateUI = UI_NOSYMBOL;
//  UpdateUIState();
  m_pIQFeed->SetNewsOn();

  m_pIQFeedNewsQuery = new CIQFeedNewsQuery<CNewsReaderView>( &_Module );
  m_pIQFeedNewsQuery->Connect();

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
  // use the iterators for pulling out the strings in order to reduce the number of copies made

  CIQFNewsMessage* msg = reinterpret_cast<CIQFNewsMessage*>( lParam );
  CIQFNewsMessage::fielddelimiter_t fd;

  structNewsItem item;
  m_NewsItems.push_back( item );

  structNewsItem& ref = m_NewsItems[ m_NewsItems.size() - 1 ];

  fd = msg->Distributor_iter();
  ref.Distributor.assign( fd.first, fd.second );

  fd = msg->StoryId_iter();
  ref.StoryId.assign( fd.first, fd.second );

  fd = msg->SymbolList_iter();
  ref.SymbolList.assign( fd.first, fd.second );

  fd = msg->DateTime_iter();
  ref.DateTime.assign( fd.first, fd.second );

  fd = msg->HeadLine_iter();
  ref.Headline.assign( fd.first, fd.second );

  m_pIQFeed->NewsDone( wParam, lParam );

  std::stringstream ss;
  ss << "NN:" << ref.StoryId << ";";
  m_pIQFeedNewsQuery->Send( ss.str() );

  m_lvHeadlines.InsertItem( 0, ref.Distributor.c_str() );
  m_lvHeadlines.SetItemText( 0, 1, ref.SymbolList.c_str() );
  m_lvHeadlines.SetItemText( 0, 2, ref.Headline.c_str() );

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

LRESULT CNewsReaderView::OnLVHeadlinesItemActivate( int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled) {

  LPNMITEMACTIVATE pNM = reinterpret_cast<LPNMITEMACTIVATE>( pNMHDR );

  bHandled = true;
  return 0;
}

LRESULT CNewsReaderView::OnLVHeadlinesHotTrack( int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled) {

  LPNMLISTVIEW pNM = reinterpret_cast<LPNMLISTVIEW>( pNMHDR );
/*
#ifdef _DEBUG
  std::stringstream ss;
  ss << typeid( this ).name() << " HotTrack "
    << " item: " << pNM->iItem
    << " sub: " << pNM->iSubItem
    << std::endl;
  OutputDebugString( ss.str().c_str() );
#endif
*/
  bHandled = true;
  return 0;
}

LRESULT CNewsReaderView::OnLVHeadlinesClick( int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled) {

  LPNMITEMACTIVATE pNM = reinterpret_cast<LPNMITEMACTIVATE>( pNMHDR );

  bHandled = true;
  return 0;
}

LRESULT CNewsReaderView::OnLVHeadlinesHover( int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled) {

  //LPNMLISTVIEW pNM = reinterpret_cast<LPNMLISTVIEW>( pNMHDR );

  bHandled = true;
  return 0;
}

LRESULT CNewsReaderView::OnLVHeadlinesRClick( int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled) {

  LPNMITEMACTIVATE pNM = reinterpret_cast<LPNMITEMACTIVATE>( pNMHDR );

  bHandled = true;
  return 0;
}

LRESULT CNewsReaderView::OnLVHeadlinesDispInfo( int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled) {

  NMLVDISPINFO* pNM = reinterpret_cast<NMLVDISPINFO*>( pNMHDR );

  bHandled = true;
  return 0;
}

