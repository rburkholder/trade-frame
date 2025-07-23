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
 * File:    OptionChainView.cpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: July 18, 2025 11:30:21
 */

#include "OptionChainView.hpp"

OptionChainView::OptionChainView(): wxDataViewCtrl() {
  Init();
}

OptionChainView::OptionChainView( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
: wxDataViewCtrl()
{
  Init();
  Create(parent, id, pos, size, style, name );
}

OptionChainView::~OptionChainView() {
}

void OptionChainView::Init() {
}

bool OptionChainView::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) {

  wxDataViewCtrl::Create(parent, id, pos, size, style );

  CreateControls();

  return true;
}

void OptionChainView::CreateControls() {

  Bind( wxEVT_DESTROY, &OptionChainView::OnDestroy, this );

  auto fRendererRight = []()->wxDataViewTextRenderer*{
    auto renderer = new wxDataViewTextRenderer();
    renderer->SetAlignment( wxALIGN_RIGHT );
    return renderer;
  };

  {
    assert( AppendColumn( new wxDataViewColumn( "c oi",   fRendererRight(), EChainColums::c_oi ) ) );
    assert( AppendColumn( new wxDataViewColumn( "c iv",   fRendererRight(), EChainColums::c_iv ) ) );
    assert( AppendColumn( new wxDataViewColumn( "c dlt",  fRendererRight(), EChainColums::c_dlt ) ) );
    assert( AppendColumn( new wxDataViewColumn( "c bid",  fRendererRight(), EChainColums::c_bid ) ) );
    assert( AppendColumn( new wxDataViewColumn( "c ask",  fRendererRight(), EChainColums::c_ask ) ) );
    assert( AppendColumn( new wxDataViewColumn( "strike", new wxDataViewTextRenderer(), EChainColums::strike ) ) );
    assert( AppendColumn( new wxDataViewColumn( "p bid",  fRendererRight(), EChainColums::p_bid ) ) );
    assert( AppendColumn( new wxDataViewColumn( "p ask",  fRendererRight(), EChainColums::p_ask ) ) );
    assert( AppendColumn( new wxDataViewColumn( "p dlt",  fRendererRight(), EChainColums::p_dlt ) ) );
    assert( AppendColumn( new wxDataViewColumn( "p iv",   fRendererRight(), EChainColums::p_iv ) ) );
    assert( AppendColumn( new wxDataViewColumn( "p oi",   fRendererRight(), EChainColums::p_oi ) ) );
    assert( AppendColumn( new wxDataViewColumn( "",       fRendererRight(), EChainColums::empty ) ) );
  }
}

void OptionChainView::OnDestroy( wxWindowDestroyEvent& event ) {

  assert( Unbind( wxEVT_DESTROY, &OptionChainView::OnDestroy, this ) );

  event.Skip( true );  // auto followed by Destroy();
}

wxBitmap OptionChainView::GetBitmapResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullBitmap;
}

wxIcon OptionChainView::GetIconResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullIcon;
}

