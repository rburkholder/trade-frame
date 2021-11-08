/************************************************************************
 * Copyright(c) 2021, One Unified. All rights reserved.                 *
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
 * File:    WinElement.cpp
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading/MarketDepth
 * Created on October 28, 2021, 16:29
 */

#include <iostream>

#include <wx/sizer.h>
#include <wx/event.h>
#include <wx/dcclient.h>

#include "WinElement.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth

WinElement::WinElement() {
  Init();
}

WinElement::WinElement(
  wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style
) {
  Init();
  Create(parent, id, pos, size, style);
}

WinElement::~WinElement() {}

bool WinElement::Create(
  wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style
) {

  SetExtraStyle(wxWS_EX_BLOCK_EVENTS); // TODO: do we keep this or not?
  wxWindow::Create( parent, id, pos, size, style );

  CreateControls();
  if (GetSizer())
  {
      GetSizer()->SetSizeHints(this);
  }
  //Centre();

  return true;

}

void WinElement::Init() {
  m_bFocusSet = false;
}

void WinElement::CreateControls() {

  Bind( wxEVT_PAINT, &WinElement::OnPaint, this, this->GetId() );
  Bind( wxEVT_SET_FOCUS, &WinElement::OnFocusSet, this, this->GetId() );
  Bind( wxEVT_KILL_FOCUS, &WinElement::OnFocusKill, this, this->GetId() );
  Bind( wxEVT_CONTEXT_MENU, &WinElement::OnContextMenu, this, this->GetId() );
  Bind( wxEVT_DESTROY, &WinElement::OnDestroy, this );
  Bind( wxEVT_LEFT_UP, &WinElement::OnMouseLeftUp, this );
  Bind( wxEVT_ENTER_WINDOW, &WinElement::OnMouseEnterWindow, this );
  Bind( wxEVT_LEAVE_WINDOW, &WinElement::OnMouseLLeaveWindow, this );

}

void WinElement::SetText( const std::string& sText ) {
  m_sText = sText;
  Refresh();
}

void WinElement::OnPaint( wxPaintEvent& event ) {
  std::cout << "OnPaint" << std::endl;
  wxPaintDC dc(this);
  dc.Clear();
  if ( m_bFocusSet ) {
    wxSize size = dc.GetSize();
    size.DecBy( 2, 2 );
    dc.DrawRectangle( wxPoint( 1, 1 ), size );
    dc.DrawText( "in focus", wxPoint( 4,4 ) );
  }
  else {
    dc.DrawText( "no focus", wxPoint( 4,4 ) );
  }
  //dc.DrawText( m_sText, wxPoint( 4,4 ) );
  //event.Skip();
}

// requires a click
void WinElement::OnFocusSet( wxFocusEvent& event ) {
  //std::cout << "OnFocusSet" << std::endl;
  //event.Skip();
}

void WinElement::OnFocusKill( wxFocusEvent& event ) {
  std::cout << "OnFocusKill" << std::endl;
  //event.Skip();
}

void WinElement::OnMouseLeftUp( wxMouseEvent& event ) {
  event.Skip();
}

void WinElement::OnMouseEnterWindow( wxMouseEvent& event ) {
  m_bFocusSet = true;
  Refresh();
  //event.Skip();
}

void WinElement::OnMouseLLeaveWindow( wxMouseEvent& event ) {
  m_bFocusSet = false;
  Refresh();
  //event.Skip();
}

void WinElement::OnContextMenu( wxContextMenuEvent& event ) {
  event.Skip();
}

void WinElement::OnDestroy( wxWindowDestroyEvent& event ) {

  Unbind( wxEVT_PAINT, &WinElement::OnPaint, this, this->GetId() );
  Unbind( wxEVT_SET_FOCUS, &WinElement::OnFocusSet, this, this->GetId() );
  Unbind( wxEVT_KILL_FOCUS, &WinElement::OnFocusKill, this, this->GetId() );
  Unbind( wxEVT_LEFT_UP, &WinElement::OnMouseLeftUp, this );
  Unbind( wxEVT_ENTER_WINDOW, &WinElement::OnMouseEnterWindow, this );
  Unbind( wxEVT_LEAVE_WINDOW, &WinElement::OnMouseLLeaveWindow, this );
  Unbind( wxEVT_CONTEXT_MENU, &WinElement::OnContextMenu, this, this->GetId() );
  Unbind( wxEVT_DESTROY, &WinElement::OnDestroy, this );

  event.Skip();  // auto followed by Destroy();
}


} // market depth
} // namespace tf
} // namespace ou

/*
 wxEVT_LEFT_DOWN
 wxEVT_LEFT_UP
 wxEVT_MIDDLE_DOWN
 wxEVT_MIDDLE_UP
 wxEVT_RIGHT_DOWN
 wxEVT_RIGHT_UP
 wxEVT_MOTION
 wxEVT_ENTER_WINDOW
 wxEVT_LEAVE_WINDOW
 wxEVT_LEFT_DCLICK
 wxEVT_MIDDLE_DCLICK
 wxEVT_RIGHT_DCLICK
*/

