/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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
/*
 * File:    BookOfOptionChains.vpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: July 18, 2025 10:26:28
 */

#include "BookOfOptionChains.hpp"
#include "wx/treebook.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

BookOfOptionChains::BookOfOptionChains(): wxTreebook() {
  Init();
}

BookOfOptionChains::BookOfOptionChains( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
: wxTreebook()
{
  Init();
  Create(parent, id, pos, size, style, name );
}

BookOfOptionChains::~BookOfOptionChains() {
}

void BookOfOptionChains::Init() {
  m_bEventsAreBound = false;
  m_fOnPageChanged = nullptr;
  m_fOnPageChanging = nullptr;
}

bool BookOfOptionChains::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) {

  wxTreebook::Create(parent, id, pos, size, style, name );

  CreateControls();

  return true;
}

void BookOfOptionChains::CreateControls() {
  Bind( wxEVT_DESTROY, &BookOfOptionChains::OnDestroy, this );
  BindBookEvents();

}

void BookOfOptionChains::BindBookEvents() {
  if ( !m_bEventsAreBound ) {
    // Page Change events cause issues during OnDestroy
    Bind( wxEVT_TREEBOOK_PAGE_CHANGING, &BookOfOptionChains::OnPageChanging, this );
    Bind( wxEVT_TREEBOOK_PAGE_CHANGED, &BookOfOptionChains::OnPageChanged, this );
    Bind( wxEVT_TREEBOOK_NODE_COLLAPSED, &BookOfOptionChains::OnNodeCollapsed, this );
    Bind( wxEVT_TREEBOOK_NODE_EXPANDED, &BookOfOptionChains::OnNodeExpanded, this );
    m_bEventsAreBound = true;
  }
}

void BookOfOptionChains::UnbindBookEvents() {
  if ( m_bEventsAreBound ) {
    // Note: page change events occur during Deletion of Pages
    assert( Unbind( wxEVT_TREEBOOK_PAGE_CHANGING, &BookOfOptionChains::OnPageChanging, this ) );
    assert( Unbind( wxEVT_TREEBOOK_PAGE_CHANGED, &BookOfOptionChains::OnPageChanged, this ) );
    assert( Unbind( wxEVT_TREEBOOK_NODE_COLLAPSED, &BookOfOptionChains::OnNodeCollapsed, this ) );
    assert( Unbind( wxEVT_TREEBOOK_NODE_EXPANDED, &BookOfOptionChains::OnNodeExpanded, this ) );
    m_bEventsAreBound = false;
  }
}

void BookOfOptionChains::Set(
  fOnPageEvent_t&& fOnPageChanging // departed
, fOnPageEvent_t&& fOnPageChanged  // arrival
, fOnNodeEvent_t&& fOnNodeCollapsed
, fOnNodeEvent_t&& fOnNodeExpanded
) {
  m_fOnPageChanging = std::move( fOnPageChanging );
  m_fOnPageChanged  = std::move( fOnPageChanged );
  m_fOnNodeCollapsed = std::move( fOnNodeCollapsed );
  m_fOnNodeExpanded = std::move( fOnNodeExpanded );
}

// start leaving old page
void BookOfOptionChains::OnPageChanging( wxBookCtrlEvent& event ) {
  int ixTab = event.GetOldSelection();
  if ( -1 != ixTab ) {
  }
  event.Skip();
}

// finishing arriving at new page
void BookOfOptionChains::OnPageChanged( wxBookCtrlEvent& event ) {
  int ixTab = event.GetSelection();
  event.Skip();
}

void BookOfOptionChains::OnNodeCollapsed( wxBookCtrlEvent& event ) {
  event.Skip();
}

void BookOfOptionChains::OnNodeExpanded( wxBookCtrlEvent& event ) {
  event.Skip();
}

void BookOfOptionChains::OnDestroy( wxWindowDestroyEvent& event ) {

  UnbindBookEvents();

  //DeleteAllPages();
  while ( 0 != GetPageCount() ) {
   //DeletePage( 0 );
   RemovePage( 0 );
  }

  assert( Unbind( wxEVT_DESTROY, &BookOfOptionChains::OnDestroy, this ) );

  event.Skip( true );  // auto followed by Destroy();
}

wxBitmap BookOfOptionChains::GetBitmapResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullBitmap;
}

wxIcon BookOfOptionChains::GetIconResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullIcon;
}

} // namespace tf
} // namespace ou
