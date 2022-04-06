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
 * File:    WinRowElement.cpp
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading/MarketDepth
 * Created on October 28, 2021, 16:29
 */

#include <wx/sizer.h>
#include <wx/event.h>
#include <wx/dcclient.h>

#include "WinRowElement.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth

WinRowElement::WinRowElement() {
}

WinRowElement::WinRowElement(
  wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style
) {
  Create(parent, id, pos, size, style);
}

WinRowElement::~WinRowElement() {
}

bool WinRowElement::Create(
  wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style
) {

  SetExtraStyle(wxWS_EX_BLOCK_EVENTS); // TODO: do we keep this or not?
  wxWindow::Create( parent, id, pos, size, style );

  Init();

  CreateControls();
  if ( GetSizer() ) {
    GetSizer()->SetSizeHints( this );
  }

  Paint();

  return true;
}

void WinRowElement::Init() {
  m_bFocusSet = false;
  m_bCanHaveFocus = false;
  //m_ColourBackground = wxSystemSettings::GetColour( wxSystemColour::wxSYS_COLOUR_WINDOW ).GetRGB();
  //m_ColourForeground = wxSystemSettings::GetColour( wxSystemColour::wxSYS_COLOUR_WINDOWTEXT ).GetRGB();
}

void WinRowElement::CreateControls() {

  Bind( wxEVT_ENTER_WINDOW, &WinRowElement::OnMouseEnterWindow, this );
  Bind( wxEVT_LEAVE_WINDOW, &WinRowElement::OnMouseLLeaveWindow, this );
  Bind( wxEVT_LEFT_UP, &WinRowElement::OnMouseLeftUp, this );
  Bind( wxEVT_PAINT, &WinRowElement::OnPaint, this, GetId() );
  Bind( wxEVT_DESTROY, &WinRowElement::OnDestroy, this, GetId() );
  //Bind( wxEVT_CONTEXT_MENU, &WinRowElement::OnContextMenu, this, GetId() );
  //Bind( wxEVT_SET_FOCUS, &WinRowElement::OnFocusSet, this, GetId() );
  //Bind( wxEVT_KILL_FOCUS, &WinRowElement::OnFocusKill, this, GetId() );

}

void WinRowElement::SetCanHaveFocus( bool bCanHaveFocus ) {
  m_bCanHaveFocus = bCanHaveFocus;
}

void WinRowElement::SetText( const std::string& sText ) {
  m_sText = sText;
  Paint();
}

void WinRowElement::Paint() {
  wxWindowDC dc( this );
  Render( dc );
}

void WinRowElement::Render( wxDC& dc ) {
  wxSize size = dc.GetSize();
  dc.Clear();
  if ( m_bFocusSet ) {
    size.DecBy( 2, 2 );
    dc.DrawRectangle( wxPoint( 1, 1 ), size );
  }

  wxCoord width;
  wxCoord height;
  wxCoord x( 1 );
  GetTextExtent( m_sText, &width, &height );
  if ( width <= size.x ) {
    x = ( size.x - width ) / 2;
  }
  dc.DrawText( m_sText, wxPoint( x, 1 ) );
}

void WinRowElement::OnPaint( wxPaintEvent& event ) {
  wxPaintDC dc( this );
  Render( dc );
}

// requires a click
void WinRowElement::OnFocusSet( wxFocusEvent& event ) {
  //std::cout << "OnFocusSet" << std::endl;
  //event.Skip();
}

void WinRowElement::OnFocusKill( wxFocusEvent& event ) {
  //std::cout << "OnFocusKill" << std::endl;
  //event.Skip();
}

void WinRowElement::OnMouseLeftUp( wxMouseEvent& event ) {
  event.Skip();
}

void WinRowElement::OnMouseEnterWindow( wxMouseEvent& event ) {
  if ( m_bCanHaveFocus ) {
    m_bFocusSet = true;
    Paint();
  }
  event.Skip();
}

void WinRowElement::OnMouseLLeaveWindow( wxMouseEvent& event ) {
  if ( m_bCanHaveFocus ) {
    m_bFocusSet = false;
    Paint();
  }
  event.Skip();
}

void WinRowElement::OnContextMenu( wxContextMenuEvent& event ) {
  event.Skip();
}

void WinRowElement::OnDestroy( wxWindowDestroyEvent& event ) {

  Unbind( wxEVT_ENTER_WINDOW, &WinRowElement::OnMouseEnterWindow, this );
  Unbind( wxEVT_LEAVE_WINDOW, &WinRowElement::OnMouseLLeaveWindow, this );
  Unbind( wxEVT_LEFT_UP, &WinRowElement::OnMouseLeftUp, this );
  Unbind( wxEVT_PAINT, &WinRowElement::OnPaint, this, GetId() );
  Unbind( wxEVT_DESTROY, &WinRowElement::OnDestroy, this, GetId() );
  //Unbind( wxEVT_CONTEXT_MENU, &WinRowElement::OnContextMenu, this, this->GetId() );
  //Unbind( wxEVT_SET_FOCUS, &WinRowElement::OnFocusSet, this, GetId() );
  //Unbind( wxEVT_KILL_FOCUS, &WinRowElement::OnFocusKill, this, GetId() );

  //event.Skip();  // no skip
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

