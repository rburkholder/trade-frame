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
  const unsigned int FontHeight = 15; // pixels
  const unsigned int RowHeight = 20;  // pixels
  const unsigned int BorderWidth = 6; // pixels
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

  m_cntWinRows_Total = 0;
  m_cntWinRows_Data = 0;

  m_ixFirstDataRow = 0; // first visible integerized price
  m_ixLastDataRow = 0;  // last visible integerized price

  m_ixHiRecenterFrame = 0;
  m_ixLoRecenterFrame = 0;

  m_dblLastPrice = 0.0;

  m_ixLastAsk = 0.0;
  m_ixLastBid = 0.0;

  m_timerRefresh.SetOwner( this );
  Bind( wxEVT_TIMER, &PanelTrade::HandleTimerRefresh, this, m_timerRefresh.GetId() );
  m_timerRefresh.Start( 200 ); // 5 times a second
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
  DrawWinRows();

  //Bind( wxEVT_PAINT, &PanelTrade::OnPaint, this, GetId() );

  Bind( wxEVT_SIZE, &PanelTrade::OnResize, this, GetId() );
  Bind( wxEVT_SIZING, &PanelTrade::OnResizing, this, GetId() );
  Bind( wxEVT_DESTROY, &PanelTrade::OnDestroy, this, GetId() );

}

void PanelTrade::OnPaint( wxPaintEvent& event ) {
  wxPaintDC dc( this );
}

void PanelTrade::HandleTimerRefresh( wxTimerEvent& event ) {
  //std::scoped_lock<std::mutex> lock( m_mutexTimer );
  if ( 0 < m_cntWinRows_Data ) {
    for ( int ix = m_ixFirstDataRow; ix <= m_ixLastDataRow; ix++ ) {
      m_DataRows[ ix ].Refresh(); // TODO: this requires a lookup, maybe do an interation instead
    }
  }
}

void PanelTrade::DrawWinRows() {

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
      m_nFramedRows = m_cntWinRows_Data / FramedRows;
      m_nCenteredRows = ( ( m_cntWinRows_Data - m_nFramedRows ) / 2 ) - 1; // eliminates up/down jitter

      int yOffset = BorderWidth; // start offset with border

      // should this go into the vector?
      m_pWinRow_Header.reset();
      m_pWinRow_Header = WinRow::Construct( this, wxPoint( BorderWidth, yOffset ), RowHeight, true );

      yOffset += RowHeight;

      int ixWinRow = 0;
      m_vWinRow.resize( m_cntWinRows_Data );

      while ( ixWinRow < m_cntWinRows_Data ) {
        pWinRow_t pWinRow = WinRow::Construct( this, wxPoint( BorderWidth, yOffset ), RowHeight, false );
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

void PanelTrade::DeleteWinRows() {

  for (
    int ixWinRow = 0, iyDataRow = m_ixFirstDataRow;
    ixWinRow < m_cntWinRows_Data;
    ixWinRow++, iyDataRow++
  ) {
    DataRow& rowData( m_DataRows[ iyDataRow ] );
    rowData.DelRowElements();
    pWinRow_t pWinRow = std::move( m_vWinRow[ ixWinRow ] );
    //pRow->DestroyWindow();
    // delete pVRow; pWinRow auto deletes
  }
  m_cntWinRows_Total = 1;  // 0 or 1? // header row remains in tact
  m_cntWinRows_Data = 0;

}

void PanelTrade::OnResize( wxSizeEvent& event ) {  // TODO: need to fix this
  CallAfter(
    [this](){
      DrawWinRows(); // probably shouldn't do this
      //ReCenterVisible( ix );  // what can we use as ix?
    });
  event.Skip(); // required when working with sizers
}

void PanelTrade::OnResizing( wxSizeEvent& event ) {
  DrawWinRows();
  //m_pWinRow_Header.reset();
  //m_vWinRow.clear();
  //m_cntTotalRows = 0;
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
    Unbind( wxEVT_TIMER, &PanelTrade::HandleTimerRefresh, this, m_timerRefresh.GetId() );

    //event.Skip();  // do not put this in
  }

}

void PanelTrade::SetInterval( double interval ) {
  m_DataRows.SetInterval( interval );
}

void PanelTrade::OnQuote( const ou::tf::Quote& quote ) {
  // will need to use quote for tick analysis.
  // don't update the ladder, as it interferes with L2
  // maybe use to set the colour for best bid offer

  int ixAskPrice = m_DataRows.Cast( quote.Ask() );
  int ixBidPrice = m_DataRows.Cast( quote.Bid() );
/*
  if ( ( 0 != m_ixLastAsk ) && ( ixAskPrice != m_ixLastAsk ) ) {
    DataRow& row( m_DataRows[ m_ixLastAsk ] );
    row.SetAskVolume( 0 );
  }
  DataRow& rowAsk( m_DataRows[ ixAskPrice ] );
  rowAsk.SetAskVolume( quote.AskSize() );
  m_ixLastAsk = ixAskPrice;

  if ( ( 0 != m_ixLastBid ) && ( ixBidPrice != m_ixLastBid ) ) {
    DataRow& row( m_DataRows[ m_ixLastBid ] );
    row.SetBidVolume( 0 );
  }
  DataRow& rowBid( m_DataRows[ ixBidPrice ] );
  rowBid.SetBidVolume( quote.BidSize() );
  m_ixLastBid = ixBidPrice;

*/

  int ixHiPrice = std::max( ixAskPrice, ixBidPrice );
  int ixLoPrice = std::max( ixAskPrice, ixBidPrice );
  int ixDiffPrice = ixHiPrice - ixLoPrice + 1;
  if ( ixDiffPrice <= m_nCenteredRows ) {
    // not sure where to recenter
  }
  else {
    int ixMidPoint = ( ixHiPrice + ixLoPrice ) / 2;
    CallAfter(
      [this, ixMidPoint](){
        ReCenterVisible( ixMidPoint );
      });
  }
}

void PanelTrade::OnQuoteAsk( double price, int volume ) {
  int ixPrice = m_DataRows.Cast( price );
  DataRow& row( m_DataRows[ ixPrice ] );
  row.SetAskVolume( volume );
  CallAfter(
    [this, ixPrice](){
      ReCenterVisible( ixPrice );
    });
}

void PanelTrade::OnQuoteBid( double price, int volume ) {
  int ixPrice = m_DataRows.Cast( price );
  DataRow& row( m_DataRows[ ixPrice ] );
  row.SetBidVolume( volume );
  CallAfter(
    [this, ixPrice](){
      ReCenterVisible( ixPrice );
    });
}

void PanelTrade::OnTrade( const ou::tf::Trade& trade ) {

  if ( 0.0 != m_dblLastPrice ) {
    DataRow& row( m_DataRows[ m_dblLastPrice ] );
    row.SetPrice( m_dblLastPrice ); // TODO: needs to reset back to the price, remove highlight
  }

  m_dblLastPrice = trade.Price();
  int ixPrice = m_DataRows.Cast( m_dblLastPrice );
  DataRow& rowData( m_DataRows[ ixPrice ] );

  rowData.SetPrice( trade.Volume() ); // need to highlight the price level
  rowData.SetTicks( rowData.GetTicks() + 1 );
  rowData.SetVolume( rowData.GetVolume() + trade.Volume() );

  CallAfter(
    [this, ixPrice](){
      ReCenterVisible( ixPrice );
    });
  // TODO: add TickBuyVolume, TickSellVolume

}

void PanelTrade::ReCenterVisible( int ixPrice ) {
  // does this need a lock from background thread? - no longer, is run in foreground

  // only does something if ixPrice moves outside of window

  // TODO: check flags so changes aren't made while one thread or the other are updating
  //   maybe count how many missed opportunties presetn themeselves

  if ( ( ixPrice <= m_ixLoRecenterFrame ) || ( ixPrice >= m_ixHiRecenterFrame ) ) {
    //std::scoped_lock<std::mutex> lock( m_mutexTimer );
    // recalibrate mappings
    if ( m_ixFirstDataRow != m_ixLastDataRow ) {
      for ( int iy = m_ixFirstDataRow; iy <= m_ixLastDataRow; iy++ ) {
        // remove existing string update events
        DataRow& rowData( m_DataRows[ iy ] );
        //pDRow -> SetOnStringUpdatedHandlers(NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
        rowData.DelRowElements();
      }
    }
    m_ixFirstDataRow = ixPrice - ( m_cntWinRows_Data / 2 );
    m_ixLastDataRow = m_ixFirstDataRow + m_cntWinRows_Data - 1;
    m_ixHiRecenterFrame = m_ixLastDataRow - m_nFramedRows;
    m_ixLoRecenterFrame = m_ixFirstDataRow + m_nFramedRows;
    // need to check that same numbers for each
    for (
      int ix = 0, iy = m_ixLastDataRow;
      ix < m_cntWinRows_Data;
      ix++, iy--
    ) {
       pWinRow_t pWinRow = m_vWinRow[ ix ];
       DataRow& rowData( m_DataRows[ iy ] );
       rowData.SetRowElements( *pWinRow );
       //rowData.Refresh();  // TODO: refactor out into timer
    }
  }


}

} // market depth
} // namespace tf
} // namespace ou
