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

#include <TFTimeSeries/DatedDatum.h>

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

  m_nFramedRows = 0;
  m_nCenteredRows = 0;


  m_cntTotalWinRows = 0;
  m_cntWinRows = 0;

  m_ixFirstVisibleRow = 0; // first visible integerized price
  m_ixLastVisibleRow = 0;  // last visible integerized price

  m_ixHiRecenterFrame = 0;
  m_ixLoRecenterFrame = 0;

  m_dblLastPrice = 0.0;

  m_ixLastAsk = 0.0;
  m_ixLastBid = 0.0;
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
    m_cntTotalWinRows = ( sizeClient.GetHeight() - 2 * BorderWidth ) / RowHeight;

    if ( 1 < m_cntTotalWinRows ) {

      m_cntWinRows = m_cntTotalWinRows - 1; // first row is header row
      m_nFramedRows = m_cntWinRows / FramedRows;
      m_nCenteredRows = ( ( m_cntWinRows - m_nFramedRows ) / 2 ) - 1; // eliminates up/down jitter

      int yOffset = BorderWidth; // start offset with border

      int ixWinRow = 0;
      m_vWinRow.resize( m_cntTotalWinRows );

      if ( 1 < m_cntTotalWinRows ) {

        // should this go into the vector?
        m_pWinRow_Header.reset();
        m_pWinRow_Header = WinRow::Construct( this, wxPoint( BorderWidth, yOffset ), RowHeight, true );
        //m_vRowElements[ ixRowElements ] = pRow;
        yOffset += RowHeight;
        //ixRowElements++;

        while ( ixWinRow < m_cntTotalWinRows ) {
          pWinRow_t pWinRow = WinRow::Construct( this, wxPoint( BorderWidth, yOffset ), RowHeight, false );
          m_vWinRow[ ixWinRow ] = pWinRow;
          yOffset += RowHeight;
          ixWinRow++;
        }

      }

    }
    else {
      m_cntTotalWinRows = 0;
    }
  }

}

void PanelTrade::DeleteAllRows() {

  WinRow* pWinRow;

  for (
    int ix = 0, iy = m_ixFirstVisibleRow;
    ix < m_cntTotalWinRows;
    ix++, iy++
  ) {
    DataRow& rowData( m_DataRows[ iy ] );
    rowData.DelRowElements();
    pWinRow_t pWinRow = std::move( m_vWinRow[ ix ] );
    //pRow->DestroyWindow();
    // delete pVRow; pWinRow auto deletes
  }
  m_cntTotalWinRows = 0;
  m_cntWinRows = 0;

}

void PanelTrade::OnResize( wxSizeEvent& event ) {
  CallAfter(
    [this](){
      if ( 0 != m_cntTotalWinRows ) {
        m_pWinRow_Header.reset();
        m_vWinRow.clear();
        m_cntTotalWinRows = 0;
      }
      DrawRows();
    });
  event.Skip(); // required when working with sizers
}

void PanelTrade::OnResizing( wxSizeEvent& event ) {
  m_pWinRow_Header.reset();
  m_vWinRow.clear();
  m_cntTotalWinRows = 0;
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

void PanelTrade::SetInterval( double interval ) {
  m_DataRows.SetInterval( interval );
}

void PanelTrade::OnQuote( const ou::tf::Quote& quote ) {

  int ixAskPrice = m_DataRows.Cast( quote.Ask() );
  if ( ( 0 != m_ixLastAsk ) && ( ixAskPrice != m_ixLastAsk ) ) {
    DataRow& row( m_DataRows[ m_ixLastAsk ] );
    row.SetAskVolume( 0 );
    CallAfter(
      [this,&row](){
        row.Refresh();  // TODO: will need to do this in different thread
      });
  }
  DataRow& rowAsk( m_DataRows[ ixAskPrice ] );
  rowAsk.SetAskVolume( quote.AskSize() );
  CallAfter(
    [this,&rowAsk](){
      rowAsk.Refresh();  // TODO: will need to do this in different thread
    });
  m_ixLastAsk = ixAskPrice;

  int ixBidPrice = m_DataRows.Cast( quote.Bid() );
  if ( ( 0 != m_ixLastBid ) && ( ixBidPrice != m_ixLastBid ) ) {
    DataRow& row( m_DataRows[ m_ixLastBid ] );
    row.SetBidVolume( 0 );
    CallAfter(
      [this,&row](){
        row.Refresh();  // TODO: will need to do this in different thread
      });
  }
  DataRow& rowBid( m_DataRows[ ixBidPrice ] );
  rowBid.SetBidVolume( quote.BidSize() );
  CallAfter(
    [this,&rowBid](){
      rowBid.Refresh();  // TODO: will need to do this in different thread
    });
  m_ixLastBid = ixBidPrice;

  int ixHiPrice = std::max( ixAskPrice, ixBidPrice );
  int ixLoPrice = std::max( ixAskPrice, ixBidPrice );
  int ixDiffPrice = ixHiPrice - ixLoPrice + 1;
  if ( ixDiffPrice <= m_nCenteredRows ) {
    // not sure where to recenter
  }
  else {
    int ixMidPoint = ( ixHiPrice + ixLoPrice ) / 2;
    ReCenterVisible( ixMidPoint );
  }

}

void PanelTrade::OnTrade( const ou::tf::Trade& trade ) {

  if ( 0.0 != m_dblLastPrice ) {
    DataRow& row( m_DataRows[ m_dblLastPrice ] );
    row.SetPrice( m_dblLastPrice ); // TODO: needs to reset back to the price, remove highlight
    CallAfter(
      [this,&row](){
        row.Refresh();  // TODO: will need to do this in different thread
      });
  }

  m_dblLastPrice = trade.Price();
  int ixPrice = m_DataRows.Cast( m_dblLastPrice );
  ReCenterVisible( ixPrice );
  DataRow& rowData( m_DataRows[ ixPrice ] );

  rowData.SetPrice( trade.Volume() ); // need to highlight the price level
  rowData.SetTicks( rowData.GetTicks() + 1 );
  rowData.SetVolume( rowData.GetVolume() + trade.Volume() );

  CallAfter(
    [this,&rowData](){
      rowData.Refresh();  // TODO: will need to do this in different thread
    });


  // TODO: add TickBuyVolume, TickSellVolume

}

void PanelTrade::ReCenterVisible( int ixPrice ) {
  // does this need a lock from background thread?
  // may need to put the recenter index somewhere for use by the the foreground thread

  // only does something if ixPrice moves outside of window

  // TODO: check flags so changes aren't made while one thread or the other are updating
  //   maybe count how many missed opportunties presetn themeselves

  if ( ( ixPrice <= m_ixLoRecenterFrame ) || ( ixPrice >= m_ixHiRecenterFrame ) ) {
    // recalibrate mappings
    if ( m_ixFirstVisibleRow != m_ixLastVisibleRow ) {
      for ( int iy = m_ixFirstVisibleRow; iy <= m_ixLastVisibleRow; iy++ ) {
        // remove existing string update events
        DataRow& rowData( m_DataRows[ iy ] );
        //pDRow -> SetOnStringUpdatedHandlers(NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
        rowData.DelRowElements();
      }
    }
    m_ixFirstVisibleRow = ixPrice - ( m_cntTotalWinRows / 2 );
    m_ixLastVisibleRow = m_ixFirstVisibleRow + m_cntWinRows - 1;
    m_ixHiRecenterFrame = m_ixLastVisibleRow - m_nFramedRows;
    m_ixLoRecenterFrame = m_ixFirstVisibleRow + m_nFramedRows;
    for (
      int ix = m_cntWinRows, iy = m_ixFirstVisibleRow;
      ix >= 1;
      ix--, iy++
    ) {
       pWinRow_t pWinRow = m_vWinRow[ ix ];
       DataRow& rowData( m_DataRows[ iy ] );
       rowData.SetRow( *pWinRow );
       rowData.Refresh();  // TODO: refactor out into timer
    }
  }


}

} // market depth
} // namespace tf
} // namespace ou
