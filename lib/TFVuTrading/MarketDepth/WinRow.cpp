/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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
 * File:    WinRow.cpp
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading/MarketDepth
 * Created on April 4, 2022  12:14
 */

#include <wx/window.h>

#include "WinRow.hpp"
#include "WinRowElement.hpp"

namespace { // local variables

  using EColour = ou::Colour::wx::EColour;

  const static EColour colourNormalCell( EColour::White ); // TODO: need the neutral background

  const static EColour colourColumnHeader( EColour::LightGoldenrodYellow );

  const static EColour colourAccount1( EColour::LightSeaGreen );
  const static EColour colourAccount2( EColour::LightGreen );

} // namespace anonymous

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth

WinRow::WinRow( wxWindow* parent, const vElement_t& vElement, const wxPoint& origin, int RowHeight, bool bIsHeader ) {

  int xPos = origin.x;

  for ( const vElement_t::value_type& element: vElement ) {
    WinRowElement* pwre = new WinRowElement( parent, wxID_ANY, wxPoint( xPos, origin.y ), wxSize( element.width -1 , RowHeight - 1 ), element.alignment );
    pwre->SetCanHaveFocus( !bIsHeader );
    if ( bIsHeader ) {
      pwre->SetText( element.header );
      pwre->SetBackgroundColour( colourColumnHeader );
    }
    else {
      pwre->SetColours( element.colours );
    }
    m_vWinRowElement.push_back( pwre );
    xPos += element.width; // maybe +1 for a border
  }

  m_bParentIsAlive = true;
  parent->Bind( wxEVT_DESTROY, &WinRow::OnDestroy, this, parent->GetId() );

}

WinRow::pWinRow_t WinRow::Construct( wxWindow* parent, const vElement_t& vElement, const wxPoint& origin, int RowHeight, bool bIsHeader ) {
  return std::make_shared<WinRow>( parent, vElement, origin, RowHeight, bIsHeader );
}

WinRow::~WinRow() {
  Clear();
  m_vWinRowElement.clear();
}

int WinRow::RowWidth( const vElement_t& vElement ) {
  int sum {};
  for ( const vElement_t::value_type& element: vElement ) {
    sum += element.width;
  }
  return sum;
}

WinRowElement* WinRow::operator[]( int ix ) {
  return m_vWinRowElement[ ix ];
}

void WinRow::Clear() {
  if ( m_bParentIsAlive ) {
    for ( vWinRowElement_t::value_type& element: m_vWinRowElement ) {
      if ( nullptr != element ) {
        // TODO need a way to determine when to delete
        //   if exception occurs here, ensure all structures have been cleared
        bool bOk = element->Destroy();
        assert( bOk );
        element = nullptr;
      }
    }
  }
}

void WinRow::OnDestroy( wxWindowDestroyEvent& event ) {
  m_bParentIsAlive = false;
}

} // market depth
} // namespace tf
} // namespace ou
