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
 * File:    PanelTrade.cpp
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading/MarketDepth
 * Created on October 28, 2021, 15:55
 */

#include <wx/sizer.h>
#include <wx/dcclient.h>

#include "PanelTrade.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth

namespace {
  const unsigned int FontHeight = 15;
  const unsigned int RowHeight = 20;
  const unsigned int BorderWidth = 5;
  const unsigned int FramedRows = 10; // when to move into frame then recenter
}

PanelTrade::PanelTrade(): wxWindow()
{
  Init();
};

PanelTrade::PanelTrade( /*wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size, long style*/
  wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style
)
{
  Init();
  Create( parent, id, pos, size, style );
}

PanelTrade::~PanelTrade() {}

void PanelTrade::Init() {
  m_nRowCount = 0;
  m_nVisibleRows = 0;
  m_nFramedRows = 0;
  m_nCenteredRows = 0;
}

bool PanelTrade::Create( /*wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size, long style*/
  wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style
) {

  SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
  wxWindow::Create( parent, id, pos, size, style );

  CreateControls();
  if ( GetSizer() ) {
    GetSizer()->SetSizeHints( this );
  }

  return true;
}

void PanelTrade::CreateControls( void ) {

  //PanelTrade* itemPanel1 = this;
  DrawRows();

  //Bind( wxEVT_PAINT, &PanelTrade::OnPaint, this, GetId() );

  Bind( wxEVT_SIZE, &PanelTrade::OnResize, this, GetId() );
  Bind( wxEVT_SIZING, &PanelTrade::OnResizing, this, GetId() );
  Bind( wxEVT_DESTROY, &PanelTrade::OnDestroy, this, GetId() );

}

void PanelTrade::OnPaint( wxPaintEvent& event ) {
  wxPaintDC dc( this );
}

void PanelTrade::DrawRows() {

  wxSize sizeClient = wxWindow::GetClientSize();

  auto BorderWidthTimes2 = 2 * BorderWidth;
  auto Height = sizeClient.GetHeight();

  if ( Height > BorderWidthTimes2 ) {

    //assert( 0 == m_nRowCount );
    m_nRowCount = ( sizeClient.GetHeight() - 2 * BorderWidth ) / RowHeight;

    if ( 1 < m_nRowCount ) {

      m_nVisibleRows = m_nRowCount - 1; // first row is header row
      m_nFramedRows = m_nVisibleRows / FramedRows;
      m_nCenteredRows = ( ( m_nVisibleRows - m_nFramedRows ) / 2 ) - 1; // eliminates up/down jitter

      int yOffset = BorderWidth; // start offset with border

      int ixWinRow = 0;
      m_vWinRow.resize( m_nRowCount );

      if ( 1 < m_nRowCount ) {

        // should this go into the vector?
        m_pWinRow_Header.reset();
        m_pWinRow_Header = WinRow::Construct( this, wxPoint( BorderWidth, yOffset ), RowHeight, true );
        //m_vRowElements[ ixRowElements ] = pRow;
        yOffset += RowHeight;
        //ixRowElements++;

        while ( ixWinRow < m_nRowCount ) {
          pWinRow_t pWinRow = WinRow::Construct( this, wxPoint( BorderWidth, yOffset ), RowHeight, false );
          m_vWinRow[ ixWinRow ] = pWinRow;
          yOffset += RowHeight;
          ixWinRow++;
        }

      }

    }
    else {
      m_nRowCount = 0;
    }
  }

}

void PanelTrade::OnResize( wxSizeEvent& event ) {
  CallAfter(
    [this](){
      if ( 0 != m_nRowCount ) {
        m_pWinRow_Header.reset();
        m_vWinRow.clear();
        m_nRowCount = 0;
      }
      DrawRows();
    });
  event.Skip(); // required when working with sizers
}

void PanelTrade::OnResizing( wxSizeEvent& event ) {
  m_pWinRow_Header.reset();
  m_vWinRow.clear();
  m_nRowCount = 0;
  event.Skip(); // required when working with sizers
}

void PanelTrade::OnDestroy( wxWindowDestroyEvent& event ) {

  if ( event.GetId() == GetId() ) {

    m_pWinRow_Header.reset();
    m_vWinRow.clear();

    //Unbind( wxEVT_PAINT, &PanelTrade::OnPaint, this, GetId() );

    Unbind( wxEVT_SIZE, &PanelTrade::OnResize, this, GetId() );
    Unbind( wxEVT_SIZING, &PanelTrade::OnResizing, this, GetId() );
    Unbind( wxEVT_DESTROY, &PanelTrade::OnDestroy, this, GetId() );

    //event.Skip();  // do not put this in
  }

}

} // market depth
} // namespace tf
} // namespace ou
