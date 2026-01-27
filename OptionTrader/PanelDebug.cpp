/************************************************************************
 * Copyright(c) 2026, One Unified. All rights reserved.                 *
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
 * File:    PanelDebug.cpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: January 26, 2026 19:56:26
 */

#include <boost/log/trivial.hpp>

#include <wx/sizer.h>

#include "PanelDebug.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame

PanelDebug::PanelDebug(): wxPanel() {
  Init();
}

PanelDebug::PanelDebug( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
: wxPanel()
{
  Init();
  Create( parent, id, pos, size, style, name );
}

PanelDebug::~PanelDebug() {
}

void PanelDebug::Init() {
}

bool PanelDebug::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) {

    SetExtraStyle( wxWS_EX_VALIDATE_RECURSIVELY );
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
    if ( GetSizer() ) {
      GetSizer()->SetSizeHints( this );
    }

  return true;
}

void PanelDebug::CreateControls() {

  PanelDebug* itemPanel1 = this;

  wxBoxSizer* itemBoxSizer1 = new wxBoxSizer( wxVERTICAL );
  itemPanel1->SetSizer( itemBoxSizer1 );

  //Layout();
  //GetParent()->Layout();

  Bind( wxEVT_DESTROY, &PanelDebug::OnDestroy, this );

}

void PanelDebug::OnDestroy( wxWindowDestroyEvent& event ) {

  assert( Unbind( wxEVT_DESTROY, &PanelDebug::OnDestroy, this ) );
  event.Skip( true );  // auto followed by Destroy();

}

wxBitmap PanelDebug::GetBitmapResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullBitmap;
}

} // namespace tf
} // namespace ou

