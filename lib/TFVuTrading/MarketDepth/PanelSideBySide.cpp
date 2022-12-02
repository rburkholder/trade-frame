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

#include <memory>
#include <thread>
#include <algorithm>

#include <boost/lexical_cast.hpp>

#include <wx/sizer.h>
#include <wx/dcclient.h>

#include <TFTimeSeries/DatedDatum.h>

#include <TFIndicators/RunningStats.h>

#include "WinRow.hpp"
#include "WinRowElement.hpp"
#include "PanelSideBySide.hpp"

// 2022/04/29 @ESM22 had 10x to 50x usual message volume at main market close
//   will the code be able to keep up?
// will need to watch for disconnect and resync the order list

// TODO: need to fix the SetWinRowElement lines with the false/true setting

//namespace {

  using EColour = ou::Colour::wx::EColour;

  using EStatsField = ou::tf::l2::PanelSideBySide::EStatsField;

  // TODO: this may ultimately be a composite of three structures (try boost::hana)
  const ou::tf::l2::vElement_t vElement = {
    { (int)EStatsField::BSizeAgg,  40, "Agg",    wxCENTER, EColour::LightSkyBlue,  EColour::Black, EColour::DodgerBlue    }
  , { (int)EStatsField::BSize,     40, "Size",   wxCENTER, EColour::LightSkyBlue,  EColour::Black, EColour::DodgerBlue    }
  , { (int)EStatsField::BPrice,    65, "Bid",    wxCENTER, EColour::LightSeaGreen, EColour::Black, EColour::LightYellow   }
  , { (int)EStatsField::Imbalance, 50, "Imbal",  wxCENTER, EColour::DimGray,       EColour::White, EColour::PaleGoldenrod }
  , { (int)EStatsField::APrice,    65, "Ask",    wxCENTER, EColour::LightSeaGreen, EColour::Black, EColour::LightYellow   }
  , { (int)EStatsField::ASize,     40, "Size",   wxCENTER, EColour::LightPink,     EColour::Black, EColour::Magenta       }
  , { (int)EStatsField::ASizeAgg,  40, "Agg",    wxCENTER, EColour::LightPink,     EColour::Black, EColour::Magenta       }
  };

//} // anonymous

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth

namespace {
  const unsigned int FontHeight = 15; // pixels
  const unsigned int RowHeight = 18;  // pixels
  const unsigned int BorderWidth = 4; // pixels
  const unsigned int FramedRows = 10; // when to move into frame then recenter
}

const std::string PanelSideBySide::sFmtInteger( "%i" );
const std::string PanelSideBySide::sFmtPrice( "%0.2f" );
const std::string PanelSideBySide::sFmtString( "%s" );

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
  m_fImbalanceStats = nullptr;
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
  Bind( wxEVT_DESTROY, &PanelSideBySide::OnDestroy, this );
  Bind( wxEVT_CLOSE_WINDOW, &PanelSideBySide::OnCloseWindow, this );

  m_timerRefresh.SetOwner( this );
  Bind( wxEVT_TIMER, &PanelSideBySide::HandleTimerRefresh, this, m_timerRefresh.GetId() );
  m_timerRefresh.Start( 200 ); // 5 times a second
}

void PanelSideBySide::OnL2Ask( double price, int volume, bool bOnAdd ) {
  UpdateMap( m_mapAskPriceLevel, price, volume, bOnAdd );
}

void PanelSideBySide::OnL2Bid( double price, int volume, bool bOnAdd ) {
  UpdateMap( m_mapBidPriceLevel, price, volume, bOnAdd );
}

void PanelSideBySide::UpdateMap( mapPriceLevel_t& map, double price, int volume, bool bOnAdd ) {

  // scoped_lock: brute force & ignorance for now, probably ultimately
  std::scoped_lock<std::mutex> lock( m_mutexMaps );

  try {
    mapPriceLevel_t::iterator iter = map.find( price );
    if ( bOnAdd ) {
      if ( map.end() == iter ) {
        if ( 0 < volume ) {
          //auto result = map.emplace( price, PriceLevel( volume) );
          auto result = map.emplace( price, DataRow_Book( price, volume) );
          assert( result.second );
        }
      }
      else {
        if ( 0 == volume ) {
          map.erase( iter );
        }
        else {
          //iter->second.nVolume = volume;
          iter->second.m_dreSize.Set( volume );
        }
      }
    }
    else {
      if ( map.end() != iter ) {
        map.erase( iter );
      }
      else {
        // because there are some zero volume items above which may eliminated it?
      }

    }
  }
  catch (...) {
    std::cout << "PanelSideBySide::UpdateMap problems" << std::endl;
  }

}

double Imbalance( int volBid, int volAsk ) {
  int top = volBid - volAsk;
  int bot = volBid + volAsk;
  return ( (double)top / (double)bot );
}

void PanelSideBySide::CalculateStatistics() { // need to fix this, as cross thread problems in the maps exist (add the DataRow thingy)

  // brute force & ignorance for now, probably ultimately, just need lock on the map add/delete portions
  // alternative:  optimized to pull values from lib/TFIQFeed/Level2/FeatureSet_Level.hpp

  size_t nRows;
  ou::tf::RunningStats rs;

  {
    std::scoped_lock<std::mutex> lock( m_mutexMaps );

    nRows = m_cntWinRows_Data;
    nRows = std::min<int>( nRows, m_mapAskPriceLevel.size() );
    nRows = std::min<int>( nRows, m_mapBidPriceLevel.size() );

    if ( 0 < nRows ) {

      int nVolumeAggregateAsk {};
      int nVolumeAggregateBid {};

      mapPriceLevel_t::iterator iterMapAsk = m_mapAskPriceLevel.begin();
      mapPriceLevel_t::reverse_iterator iterMapBid = m_mapBidPriceLevel.rbegin();
      vWinRow_t::iterator iterWinRow = m_vWinRow.begin();

      for ( size_t ix = 0; ix < nRows; ix++ ) {

        nVolumeAggregateAsk += iterMapAsk->second.m_dreSize.Get();
        nVolumeAggregateBid += iterMapBid->second.m_dreSize.Get();
        WinRow& row( **iterWinRow );

        DataRow_Book& bookAsk( iterMapAsk->second );
        DataRow_Book& bookBid( iterMapBid->second );

        assert( ix < m_vStatistics.size() );
        DataRow_Statistics& stats( *m_vStatistics[ ix ] );
        double imbalance = Imbalance( nVolumeAggregateBid, nVolumeAggregateAsk );
        stats.m_dreImbalance.Set( imbalance );
        stats.Update();

        rs.Add( ix, imbalance );

        bookAsk.m_drePrice.SetWinRowElement(   row[ (int)EStatsField::APrice ] ); // can this be performed during map update?
        bookAsk.m_dreSize.SetWinRowElement(    row[ (int)EStatsField::ASize ] );
        bookAsk.m_dreSizeAgg.SetWinRowElement( row[ (int)EStatsField::ASizeAgg ] );
        bookAsk.m_dreSizeAgg.Set( nVolumeAggregateAsk );

        bookBid.m_drePrice.SetWinRowElement(   row[ (int)EStatsField::BPrice ] ); // can this be performed during map update?
        bookBid.m_dreSize.SetWinRowElement(    row[ (int)EStatsField::BSize ] );
        bookBid.m_dreSizeAgg.SetWinRowElement( row[ (int)EStatsField::BSizeAgg ] );
        bookBid.m_dreSizeAgg.Set( nVolumeAggregateBid );

        // can't take these out of the lock as the maps are async updated
        // would need to create a second map
        bookAsk.Update();
        bookBid.Update();

        iterMapAsk++;
        iterMapBid++;
        iterWinRow++;
      } // end of for nRows
    }
  }

  if ( 0 < nRows ) {
    rs.CalcStats(); // obtain b0, b1, will want to turn this into an indicator elsewhere
    if ( m_fImbalanceStats ) m_fImbalanceStats( rs.MeanY(), rs.Slope() );
  }
}

void PanelSideBySide::HandleTimerRefresh( wxTimerEvent& event ) {
  //if ( m_fTimer ) m_fTimer();
  //std::scoped_lock<std::mutex> lock( m_mutexTimer );
  if ( 0 < m_cntWinRows_Data ) {
    CalculateStatistics();
    //for ( int ix = m_ixFirstPriceRow; ix <= m_ixLastPriceRow; ix++ ) {
    //  m_PriceRows[ ix ].Refresh(); // TODO: this requires a lookup, maybe do an interation instead
    //}
  }
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
      m_pWinRow_Header = WinRow::Construct( this, ::vElement, wxPoint( BorderWidth, yOffset ), RowHeight, true );

      yOffset += RowHeight;

      int ixWinRow = 0;
      m_vWinRow.resize( m_cntWinRows_Data );
      m_vStatistics.resize( m_cntWinRows_Data );

      while ( ixWinRow < m_cntWinRows_Data ) {
        pWinRow_t pWinRow = WinRow::Construct( this, ::vElement, wxPoint( BorderWidth, yOffset ), RowHeight, false );
        m_vWinRow[ ixWinRow ] = pWinRow;

        pDataRow_Statistics_t pDataRow_Statistics = std::make_unique<DataRow_Statistics>();
        pDataRow_Statistics->m_dreImbalance.SetWinRowElement( (*pWinRow)[ (int)EStatsField::Imbalance ]);
        m_vStatistics[ ixWinRow ] = std::move( pDataRow_Statistics );

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
  }

}

void PanelSideBySide::OnCloseWindow( wxCloseEvent& event ) {

  if ( event.GetId() == GetId() ) {

    //Unbind( wxEVT_PAINT, &PanelTrade::OnPaint, this, GetId() );

    Unbind( wxEVT_TIMER, &PanelSideBySide::HandleTimerRefresh, this, m_timerRefresh.GetId() );

    Unbind( wxEVT_SIZE, &PanelSideBySide::OnResize, this, GetId() );
    Unbind( wxEVT_SIZING, &PanelSideBySide::OnResizing, this, GetId() );
    Unbind( wxEVT_DESTROY, &PanelSideBySide::OnDestroy, this );
    Unbind( wxEVT_CLOSE_WINDOW, &PanelSideBySide::OnCloseWindow, this );
    //Unbind( wxEVT_TIMER, &PanelTrade::HandleTimerRefresh, this, m_timerRefresh.GetId() );

    m_mapAskPriceLevel.clear();
    m_mapBidPriceLevel.clear();
    m_vStatistics.clear();
    m_pWinRow_Header.reset();
    m_vWinRow.clear();

    //event.Skip();  // do not put this in
  }

}

} // market depth
} // namespace tf
} // namespace ou
