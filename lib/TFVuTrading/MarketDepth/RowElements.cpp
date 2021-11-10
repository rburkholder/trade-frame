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
 * File:    RowElements.cpp
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading/MarketDepth
 * Created: November 9, 2021 16:53
 */

 #include "RowElements.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth
namespace { // local variables

  using Field = RowElements::Field;

  struct ElementTrait {
    RowElements::Field field;
    int width;
    std::string title;
  };

  using vElementTrait_t = std::vector<ElementTrait>;
  const vElementTrait_t vElementTrait = {
    { Field::SellVol,  60, "BidVol" },
    { Field::Bid,      60, "Bid" },
    { Field::Price,    60, "Price" },
    { Field::Ask,      60, "Ask" },
    { Field::BuyVol,   60, "AskVol" },
    { Field::Ticks,    50, "Ticks" },
    { Field::Volume,   60, "Volume" },
    { Field::Volume,   80, "Static" },
    { Field::Dynamic, 100, "Dynamic" }
  };
}; // namespace anonymmous

RowElements::RowElements( wxWindow* pParent, const wxPoint& origin, int nRowHeight, bool bIsHeader ) {
  m_pParentWindow = pParent;
  m_vElements.resize( vElementTrait.size() );
  for ( vElements_t::value_type& element: m_vElements ) {
    element = nullptr;
  }
  Create( origin, nRowHeight, bIsHeader );
}

RowElements::~RowElements() {
  Clear();
}

int RowElements::RowWidth() { // TODO: constexpr?
  int width {};
  for ( const vElementTrait_t::value_type& trait: vElementTrait ) {
    width += trait.width;
  }
  return width;
}

void RowElements::Clear() {
  for ( vElements_t::value_type& element: m_vElements ) {
    if ( nullptr != element ) {
      bool bOk = element->Destroy();
      element = nullptr;
    }
  }
}

void RowElements::Create( const wxPoint& origin, int nRowHeight, bool bIsHeader ) {
  Clear();
  wxPoint location( origin );
  for ( const vElementTrait_t::value_type& trait: vElementTrait ) {
    WinRowElement* pWinRowElement
      = new WinRowElement( m_pParentWindow, wxID_ANY, location, wxSize( trait.width, nRowHeight ) );
    if ( bIsHeader ) {
      pWinRowElement->SetText( trait.title );
    }
    m_vElements[ (size_t)trait.field ] = pWinRowElement;
    location.x += trait.width;
  }
}

} // market depth
} // namespace tf
} // namespace ou
