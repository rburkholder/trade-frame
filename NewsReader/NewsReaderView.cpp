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

#include <boost/foreach.hpp>

#include "NewsReaderView.h"

CNewsReaderView::CNewsReaderView() 
: CDialogImpl<CNewsReaderView>(), CDialogResize<CNewsReaderView>(),
//  m_Destinations( this, WM_IQFEED_CONNECTED, WM_IQFEED_SENDDONE, WM_IQFEED_DISCONNECTED, WM_IQFEED_ERROR,
//  0, 0, WM_IQFEED_NEWS, 0, 0, 0 ),
  m_MsgIdsForIQFeed( this, WM_IQFEED_CONNECTED, 0, 0, 0, 0, 0, WM_IQFEED_NEWS, 0, 0, 0 ),
  m_MsgIdsForNewsQuery( this, WM_QUERY_CONNECTED, 0, 0, 0 ),
  m_stateStoryRetrieval( INACTIVE )
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
  int i2 = m_lvHeadlines.AddColumn( "DateTime", 1 );
  int i3 = m_lvHeadlines.AddColumn( "Symbols", 2 );
  int i4 = m_lvHeadlines.AddColumn( "Headline", 3 );

  m_lvHeadlines.SetColumnWidth( 0, 50 );
  m_lvHeadlines.SetColumnWidth( 1, 100 );
  m_lvHeadlines.SetColumnWidth( 2, 70 );
  m_lvHeadlines.SetColumnWidth( 3, 400 );

  m_pIQFeed = new CIQFeed<CNewsReaderView>( &_Module, m_MsgIdsForIQFeed );
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

  m_pIQFeed->SetNewsOn();

  m_pIQFeedNewsQuery = new CIQFeedNewsQuery<CNewsReaderView>( &_Module, m_MsgIdsForNewsQuery );
  m_pIQFeedNewsQuery->Connect();

  bHandled = true;
  return 1;
}

LRESULT CNewsReaderView::OnQueryConnected( UINT, WPARAM, LPARAM, BOOL& bHandled ) {

  m_pIQFeedNewsQuery->RetrieveConfiguration();

  bHandled = true;
  return 1;
}

/*
LRESULT CNewsReaderView::OnIQFeedDisconnected( UINT, WPARAM, LPARAM, BOOL& bHandled ) {

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
*/
LRESULT CNewsReaderView::OnIQFeedNews( UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled ) {

  CIQFNewsMessage* msg = reinterpret_cast<CIQFNewsMessage*>( lParam );
  CIQFNewsMessage::fielddelimiter_t fd;

  structNewsItem item;
  m_NewsItems.push_back( item );

  vNewsItems_t::size_type ix = m_NewsItems.size() - 1;

  structNewsItem& ref = m_NewsItems[ ix ];

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

  m_lvHeadlines.InsertItem(  0,    ref.Distributor.c_str() );
  m_lvHeadlines.SetItemText( 0, 1, ref.DateTime.c_str() );
  m_lvHeadlines.SetItemText( 0, 2, ref.SymbolList.c_str() );
  m_lvHeadlines.SetItemText( 0, 3, ref.Headline.c_str() );

  m_lvHeadlines.SetItemData( 0, ix );

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

LRESULT CNewsReaderView::OnIQFeedStoryLine( UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled ) {

  typedef CIQFeedNewsQuery<CNewsReaderView> query_t;

  vNewsItems_t::size_type ix = static_cast<vNewsItems_t::size_type>( lParam );

  typedef query_t::inherited_t::linebuffer_t linebuffer_t;

  linebuffer_t* buf = reinterpret_cast<linebuffer_t*>( wParam );
  linebuffer_t::const_iterator bgn = (*buf).begin();
  linebuffer_t::const_iterator end = (*buf).end();

  structNewsItem& ref = m_NewsItems[ ix ];
  if ( 0 != ref.Story.size() ) {
    ref.Story.append( "\r\n" );
  }
  ref.Story.append( bgn, end );

  m_pIQFeedNewsQuery->PostProcessedMessage( wParam );

  bHandled = true;
  return 1;
}

LRESULT CNewsReaderView::OnIQFeedStoryDone( UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled ) {

  vNewsItems_t::size_type ix = static_cast<vNewsItems_t::size_type>( lParam );

  m_NewsItems[ ix ].StoryLoaded = true;
  m_stateStoryRetrieval = INACTIVE;
  DisplayStory( ix );

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

  LVHITTESTINFO info; 
  info.pt = pNM->ptAction;
  int ix = m_lvHeadlines.HitTest( &info );
/*
#ifdef _DEBUG
  std::stringstream ss;
  ss << typeid( this ).name() << " HotTrack "
    << " item: " << pNM->iItem
    << " sub: " << pNM->iSubItem
    << " ix: " << ix
    << std::endl;
  OutputDebugString( ss.str().c_str() );
#endif
*/
  bHandled = true;
  return 0;
}

void CNewsReaderView::DisplayStory( vNewsItems_t::size_type ix ) {
  m_edtStory.Clear();
  m_edtStory.SetWindowText( m_NewsItems[ ix ].Story.c_str() );
}

LRESULT CNewsReaderView::OnLVHeadlinesClick( int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled) {

  LPNMITEMACTIVATE pNM = reinterpret_cast<LPNMITEMACTIVATE>( pNMHDR );

  if ( INACTIVE == m_stateStoryRetrieval ) {
    // check what got hit
    LVHITTESTINFO info;
    info.pt = pNM->ptAction;
    int ix = m_lvHeadlines.HitTest( &info );

    if ( -1 != ix ) {  // a valid entry was hit

      DWORD_PTR ixStory = m_lvHeadlines.GetItemData(ix);

      if ( m_NewsItems[ ixStory ].StoryLoaded ) {
        DisplayStory( ixStory );
      }
      else {

        m_stateStoryRetrieval = RETRIEVING;

        m_edtStory.Clear();

        m_pIQFeedNewsQuery->RetrieveStory( 
          m_NewsItems[ixStory].StoryId, this, ixStory, WM_IQFEED_STORY_LINE, WM_IQFEED_STORY_DONE );
      }
    }
  }

  bHandled = true;
  return 0;
}

LRESULT CNewsReaderView::OnLVHeadlinesHover( int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled) {

  //LPNMLISTVIEW pNM = reinterpret_cast<LPNMLISTVIEW>( pNMHDR );

  bHandled = true;
  return 0;
}

LRESULT CNewsReaderView::OnLVHeadlinesRClick( int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled) {

  //LPNMITEMACTIVATE pNM = reinterpret_cast<LPNMITEMACTIVATE>( pNMHDR );

  bHandled = true;
  return 0;
}

LRESULT CNewsReaderView::OnLVHeadlinesDispInfo( int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled) {

  //NMLVDISPINFO* pNM = reinterpret_cast<NMLVDISPINFO*>( pNMHDR );

  bHandled = true;
  return 0;
}

