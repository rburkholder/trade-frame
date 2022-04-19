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

#include <OUCommon/Colour.h>

#include "WinRowElement.hpp"

#include "WinRow.hpp"

namespace { // local variables

  using EColour = ou::Colour::wx::EColour;

  const static EColour colourNormalCell( EColour::White ); // TODO: need the neutral background

  const static EColour colourColumnHeader( EColour::LightGoldenrodYellow );

  const static EColour colourAccount1( EColour::LightSeaGreen );
  const static EColour colourAccount2( EColour::LightGreen );

  using EField = ou::tf::l2::WinRow::EField;

  struct Element {
    EField field;
    int width;
    std::string header;
    long alignment; // not used at moment as WinRowElement uses dc.draw commands
    EColour colourBackground;
    EColour colourForeground;
    EColour colourHighlight;
  };

  using vElement_t = std::vector<Element>;
  vElement_t vElement = {
     { EField::BuyCount,   45, "Ticks",   wxCENTER, EColour::LightSkyBlue,  EColour::Black, EColour::PaleGoldenrod }
   , { EField::BuyVolume,  45, "Bought",  wxCENTER, EColour::LightSkyBlue,  EColour::Black, EColour::PaleGoldenrod }
   , { EField::BidSize,    45, "BidSize", wxCENTER, EColour::LightSkyBlue,  EColour::Black, EColour::DodgerBlue    }
   , { EField::Price,      60, "Price",   wxCENTER, EColour::LightSeaGreen, EColour::Black, EColour::LightYellow   }
   , { EField::AskSize,    45, "AskSize", wxCENTER, EColour::LightPink,     EColour::Black, EColour::Magenta       }
   , { EField::SellVolume, 45, "Sold",    wxCENTER, EColour::LightPink,     EColour::Black, EColour::PaleGoldenrod }
   , { EField::SellCount,  45, "Ticks",   wxCENTER, EColour::LightPink,     EColour::Black, EColour::PaleGoldenrod }
   , { EField::Ticks,      45, "Ticks",   wxCENTER, EColour::DimGray,       EColour::White, EColour::PaleGoldenrod } // count of trades
   , { EField::Volume,     60, "Vol",     wxCENTER, EColour::DimGray,       EColour::White, EColour::PaleGoldenrod } // sum of volume
   , { EField::Static,     80, "SttcInd", wxLEFT,   EColour::DimGray,       EColour::White, EColour::PaleGoldenrod } // static indicators - pivots, ...
   , { EField::Dynamic,   100, "DynInd",  wxLEFT,   EColour::DimGray,       EColour::White, EColour::PaleGoldenrod } // dynamic indicators - ema, ...
  };
}

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth

WinRow::WinRow( wxWindow* parent, const wxPoint& origin, int RowHeight, bool bIsHeader ) {

  int xPos = origin.x;

  for ( const vElement_t::value_type& element: vElement ) {
    WinRowElement* pwre = new WinRowElement( parent, wxID_ANY, wxPoint( xPos, origin.y ), wxSize( element.width -1 , RowHeight - 1 ), element.alignment );
    pwre->SetCanHaveFocus( !bIsHeader );
    if ( bIsHeader ) {
      pwre->SetText( element.header );
      pwre->SetBackgroundColour( colourColumnHeader );
    }
    else {
      pwre->SetColours( element.colourBackground, element.colourForeground, element.colourHighlight );
    }
    m_vWinRowElement.push_back( pwre );
    xPos += element.width; // maybe +1 for a border
  }

}

WinRow::pWinRow_t WinRow::Construct( wxWindow* parent, const wxPoint& origin, int RowHeight, bool bIsHeader ) {
  return std::make_shared<WinRow>( parent, origin, RowHeight, bIsHeader );
}

WinRow::~WinRow() {
  Clear();
  m_vWinRowElement.clear();
}

int WinRow::RowWidth() {
  int sum {};
  for ( const vElement_t::value_type& element: vElement ) {
    sum += element.width;
  }
  return sum;
}

WinRowElement* WinRow::operator[]( EField ix ) {
  return m_vWinRowElement[ (size_t)ix ];
}

void WinRow::Clear() {
  // TODO: interaction with parent?
  for ( vWinRowElement_t::value_type& element: m_vWinRowElement ) {
    if ( nullptr != element ) {
      bool bOk = element->Destroy();
      assert( bOk );
      element = nullptr;
    }
  }
}


} // market depth
} // namespace tf
} // namespace ou
