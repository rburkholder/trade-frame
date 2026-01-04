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
 * File:    OptionComboGridView.cpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: December 28 2025 20:27:57
 */

#include <wx/icon.h>
#include <wx/sizer.h>

#include "OptionComboGridView.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame

OptionComboGridView::OptionComboGridView(): wxGrid() {
  Init();
}

OptionComboGridView::OptionComboGridView( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
: wxGrid()
{
  Init();
  Create( parent, id, pos, size, style, name );
}

OptionComboGridView::~OptionComboGridView() {
}

void OptionComboGridView::Init() {
}

bool OptionComboGridView::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) {

    SetExtraStyle( wxWS_EX_VALIDATE_RECURSIVELY );
    wxGrid::Create( parent, id, pos, size, style );

    CreateControls();
    if ( GetSizer() ) {
      GetSizer()->SetSizeHints( this );
    }

  return true;
}

void OptionComboGridView::CreateControls() {
  Bind( wxEVT_DESTROY, &OptionComboGridView::OnDestroy, this );
}

void OptionComboGridView::OnDestroy( wxWindowDestroyEvent& event ) {

  assert( Unbind( wxEVT_DESTROY, &OptionComboGridView::OnDestroy, this ) );
  event.Skip( true );  // auto followed by Destroy();

}

wxBitmap OptionComboGridView::GetBitmapResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullBitmap;
}

wxIcon OptionComboGridView::GetIconResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullIcon;
}


} // namespace tf
} // namespace ou
