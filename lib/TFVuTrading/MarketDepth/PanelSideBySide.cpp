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
 * File:    PanelSideBySide.cpp
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading/MarketDepth
 * Created: April 27, 2022 16:38
 */

#include <wx/sizer.h>
#include <wx/tooltip.h>
#include <wx/dcclient.h>

#include <TFTimeSeries/DatedDatum.h>

#include "WinRow.hpp"
#include "PanelSideBySide.hpp"

/*
  TODO:
    calculate order imbalance in L2Base
    create simple planel to show top of book and imbalance as it changes:
      price, imbalance bid <-> ask
    use vector to show value for top n positions
    as bid/ask price levels are updated and cleared, algorithm for entry 0 should be clear, or do we shift?
      ie, when bid or ask level goes to zero, we get a differnt set of levels to match
      ie, orders at a level have to 'disappear' in order for bid/ask/spread to move up/down
    then need to deduce for how long does the signal last?
*/

namespace {

  enum class EField: int {
    BidVol, BidSize, Price, AskSize, AskVol, Imbalance
  };

  using EColour = ou::Colour::wx::EColour;
  //using EField = ou::tf::l2::DataRow::EField;

  const ou::tf::l2::WinRow::vElement_t vElement = {
    { (int)EField::BidVol,    45, "BVol",   wxCENTER, EColour::LightSkyBlue,  EColour::Black, EColour::DodgerBlue    }
  , { (int)EField::BidSize,   45, "BSize",  wxCENTER, EColour::LightSkyBlue,  EColour::Black, EColour::DodgerBlue    }
  , { (int)EField::Price,     60, "BPrice", wxCENTER, EColour::LightSeaGreen, EColour::Black, EColour::LightYellow   }
  , { (int)EField::Price,     60, "APrice", wxCENTER, EColour::LightSeaGreen, EColour::Black, EColour::LightYellow   }
  , { (int)EField::AskSize,   45, "ASize",  wxCENTER, EColour::LightPink,     EColour::Black, EColour::Magenta       }
  , { (int)EField::AskVol,    45, "AVol",   wxCENTER, EColour::LightPink,     EColour::Black, EColour::Magenta       }
  , { (int)EField::Imbalance, 50, "Imbal",  wxCENTER, EColour::DimGray,       EColour::White, EColour::PaleGoldenrod }
  };

  const ou::tf::l2::WinRow::vElement_t vLeft = { // TBD
  };

  const ou::tf::l2::WinRow::vElement_t vMiddle = { // TBD
  };

  const ou::tf::l2::WinRow::vElement_t vRight = { // TBD
  };

} // anonymous

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth

namespace {
  const unsigned int FontHeight = 15; // pixels
  const unsigned int RowHeight = 18;  // pixels
  const unsigned int BorderWidth = 4; // pixels
  const unsigned int FramedRows = 10; // when to move into frame then recenter
}

PanelSideBySide::PanelSideBySide(): wxWindow()
{
  Init();
};

PanelSideBySide::PanelSideBySide(
  wxWindow* parent,
  wxWindowID id,
  const wxPoint& pos,
  const wxSize& size,
  long style
) {
  Init();
  Create( parent, id, pos, size, style );
}

PanelSideBySide::~PanelSideBySide() {}

void PanelSideBySide::Init() {
  m_cntWinRows_Data = 0;
}

bool PanelSideBySide::Create(
  wxWindow* parent,
  wxWindowID id,
  const wxPoint& pos,
  const wxSize& size,
  long style
) {

  SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
  wxWindow::Create( parent, id, pos, size, style );

  CreateControls();
  if ( GetSizer() ) {
    GetSizer()->SetSizeHints( this );
  }

  return true;
}

void PanelSideBySide::CreateControls() {

  DrawWinRows();
  //Bind( wxEVT_PAINT, &PanelTrade::OnPaint, this, GetId() );

  Bind( wxEVT_SIZE, &PanelSideBySide::OnResize, this, GetId() );
  Bind( wxEVT_SIZING, &PanelSideBySide::OnResizing, this, GetId() );
  Bind( wxEVT_DESTROY, &PanelSideBySide::OnDestroy, this, GetId() );

}

void PanelSideBySide::DrawWinRows() {

  if ( 0 != m_cntWinRows_Data ) {
    //m_pWinRow_Header.reset();
    //m_vWinRow.clear();
    //m_cntTotalWinRows = 0;
    DeleteWinRows();
  }

  wxSize sizeClient = wxWindow::GetClientSize();

  //std::scoped_lock<std::mutex> lock( m_mutexTimer );
  auto BorderWidthTimes2 = 2 * BorderWidth;
  auto Height = sizeClient.GetHeight();

  if ( Height > BorderWidthTimes2 ) {

    //assert( 0 == m_nRowCount );
    m_cntWinRows_Total = ( sizeClient.GetHeight() - 2 * BorderWidth ) / RowHeight;

    if ( 1 < m_cntWinRows_Total ) { // space enough for at least header row, and one data row

      m_cntWinRows_Data = m_cntWinRows_Total - 1; // first row is header row

      int yOffset = BorderWidth; // start offset with border

      // should this go into the vector?
      m_pWinRow_Header.reset();
      m_pWinRow_Header = WinRow::Construct( this, vElement, wxPoint( BorderWidth, yOffset ), RowHeight, true );

      yOffset += RowHeight;

      int ixWinRow = 0;
      m_vWinRow.resize( m_cntWinRows_Data );

      while ( ixWinRow < m_cntWinRows_Data ) {
        pWinRow_t pWinRow = WinRow::Construct( this, vElement, wxPoint( BorderWidth, yOffset ), RowHeight, false );
        m_vWinRow[ ixWinRow ] = pWinRow;
        yOffset += RowHeight;
        ixWinRow++;
      }

    }
    else {
      m_cntWinRows_Total = 0;
    }
  }

}

void PanelSideBySide::DeleteWinRows() {}

void PanelSideBySide::OnPaint( wxPaintEvent& ) {}

void PanelSideBySide::OnResize( wxSizeEvent& event ) {  // TODO: need to fix this
  CallAfter(
    [this](){
      DrawWinRows(); // probably shouldn't do this
      //ReCenterVisible( m_DataRows.Cast( m_dblLastPrice ) );
    });
  event.Skip(); // required when working with sizers
}

void PanelSideBySide::OnResizing( wxSizeEvent& event ) {
  //DrawWinRows();
  //m_pWinRow_Header.reset();
  //m_vWinRow.clear();
  //m_cntTotalRows = 0;
  event.Skip(); // required when working with sizers
}

void PanelSideBySide::OnDestroy( wxWindowDestroyEvent& event ) {

  if ( event.GetId() == GetId() ) {

    m_pWinRow_Header.reset();
    m_vWinRow.clear();

    //Unbind( wxEVT_PAINT, &PanelTrade::OnPaint, this, GetId() );

    Unbind( wxEVT_SIZE, &PanelSideBySide::OnResize, this, GetId() );
    Unbind( wxEVT_SIZING, &PanelSideBySide::OnResizing, this, GetId() );
    Unbind( wxEVT_DESTROY, &PanelSideBySide::OnDestroy, this, GetId() );
    //Unbind( wxEVT_TIMER, &PanelTrade::HandleTimerRefresh, this, m_timerRefresh.GetId() );

    //event.Skip();  // do not put this in
  }

}

} // market depth
} // namespace tf
} // namespace ou
