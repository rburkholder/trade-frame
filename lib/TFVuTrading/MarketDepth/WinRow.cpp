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
  static int colourNormalCell( ou::Colour::White ); // TODO: need the neutral background

  static int colourColumnHeader( ou::Colour::LightGoldenrodYellow );

  static int colourAccount1( ou::Colour::LightSeaGreen );
  static int colourAccount2( ou::Colour::LightGreen );

  static int colourPrice( ou::Colour::LightSteelBlue );
  static int colourPriceHighlight( ou::Colour::SkyBlue );

  static int colourBidHighlight( ou::Colour::DodgerBlue );
  static int colourAskHighlight( ou::Colour::Magenta );

  using EField = ou::tf::l2::WinRow::EField;
  using EColour = ou::Colour::EColour;

  struct Element {
    EField field;
    int width;
    std::string header;
    EColour colour;
  };

  using vElement_t = std::vector<Element>;
  vElement_t vElement = {
     { EField::BidSize,  60, "BidSize", EColour::LightSlateGray } // changes
   , { EField::Price,    60, "Price",   EColour::LightSteelBlue } // static
   , { EField::AskSize,  60, "AskSize", EColour::LightSlateGray } // changes
   , { EField::Ticks,    50, "Ticks",   EColour::LightSlateGray } // count of trades
   , { EField::Volume,   50, "Vol",     EColour::LightSlateGray } // sum of volume
   , { EField::Static,   80, "SttcInd", EColour::LightSlateGray } // static indicators - pivots, ...
   , { EField::Dynamic, 100, "DynInd",  EColour::LightSlateGray } // dynamic indicators - ema, ...
  };
}

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth

WinRow::WinRow( wxWindow* parent, const wxPoint& origin, int RowHeight, bool bIsHeader ) {

  int xPos = origin.x;

  for ( const vElement_t::value_type& element: vElement ) {
    WinRowElement* pwre = new WinRowElement( parent, wxID_ANY, wxPoint( xPos, origin.y ), wxSize( element.width -1 , RowHeight - 1 ) );
    pwre->SetCanHaveFocus( !bIsHeader );
    if ( bIsHeader ) {
      pwre->SetText( element.header );
      pwre->SetBackgroundColour( colourColumnHeader );
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
  m_vWinRowElement.clear(); // wxWindows destroys children
}

void WinRow::HighlightBid( bool bHighlight ) {
  if ( bHighlight ) {
    m_vWinRowElement[ (int)EField::BidSize ]->SetBackgroundColour( colourBidHighlight );
    m_vWinRowElement[ (int)EField::Price ]->SetBackgroundColour( colourBidHighlight );
  }
  else {
    m_vWinRowElement[ (int)EField::BidSize ]->SetBackgroundColour( colourNormalCell );
    m_vWinRowElement[ (int)EField::Price ]->SetBackgroundColour( colourPrice );
   }
}

void WinRow::HighlightAsk( bool bHighlight ) {
  if ( bHighlight ) {
    m_vWinRowElement[ (int)EField::AskSize ]->SetBackgroundColour( colourAskHighlight );
    m_vWinRowElement[ (int)EField::Price ]->SetBackgroundColour( colourAskHighlight );
  }
  else {
    m_vWinRowElement[ (int)EField::AskSize ]->SetBackgroundColour( colourNormalCell );
    m_vWinRowElement[ (int)EField::Price ]->SetBackgroundColour( colourPrice );
   }
}

void WinRow::HighlightPrice( bool bHighlight ) {
  if ( bHighlight ) {
    m_vWinRowElement[ (int)EField::Price ]->SetBackgroundColour( colourPriceHighlight );
  }
  else {
    m_vWinRowElement[ (int)EField::Price ]->SetBackgroundColour( colourPrice );
   }
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
