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

#include <thread>

#include <boost/lexical_cast.hpp>

#include <wx/sizer.h>
#include <wx/dcclient.h>

#include <TFTimeSeries/DatedDatum.h>

#include "WinRow.hpp"
#include "WinRowElement.hpp"
#include "DataRowElement.hpp"
#include "PanelSideBySide.hpp"

/*
  TODO:
    calculate order imbalance in L2Base
    create simple panel to show top of book and imbalance as it changes:
      price, imbalance bid <-> ask
    use vector to show value for top n positions
    as bid/ask price levels are updated and cleared, algorithm for entry 0 should be clear, or do we shift?
      ie, when bid or ask level goes to zero, we get a differnt set of levels to match
      ie, orders at a level have to 'disappear' in order for bid/ask/spread to move up/down
    then need to deduce for how long does the signal last?
*/

// 2022/04/29 @ESM22 had 10x to 50x usual message volume at main market close
//   will the code be able to keep up?
// will need to watch for disconnect and resync the order list

namespace {

  enum class EField: int {
    BSizeAgg, BSize, BPrice, APrice, ASize, ASizeAgg, Imbalance
  };

  using EColour = ou::Colour::wx::EColour;
  //using EField = ou::tf::l2::DataRow::EField;

  static const ou::tf::l2::WinRow::vElement_t vElement = {
    { (int)EField::BSizeAgg,  45, "Agg",    wxCENTER, EColour::LightSkyBlue,  EColour::Black, EColour::DodgerBlue    }
  , { (int)EField::BSize,     45, "Size",   wxCENTER, EColour::LightSkyBlue,  EColour::Black, EColour::DodgerBlue    }
  , { (int)EField::BPrice,    65, "Bid",    wxCENTER, EColour::LightSeaGreen, EColour::Black, EColour::LightYellow   }
  , { (int)EField::APrice,    65, "Ask",    wxCENTER, EColour::LightSeaGreen, EColour::Black, EColour::LightYellow   }
  , { (int)EField::ASize,     45, "Size",   wxCENTER, EColour::LightPink,     EColour::Black, EColour::Magenta       }
  , { (int)EField::ASizeAgg,  45, "Agg",    wxCENTER, EColour::LightPink,     EColour::Black, EColour::Magenta       }
  , { (int)EField::Imbalance, 50, "Imbal",  wxCENTER, EColour::DimGray,       EColour::White, EColour::PaleGoldenrod }
  };

  const ou::tf::l2::WinRow::vElement_t vLeft = { // TBD
  };

  const ou::tf::l2::WinRow::vElement_t vMiddle = { // TBD
  };

  const ou::tf::l2::WinRow::vElement_t vRight = { // TBD
  };

  struct DataRow_Book { // one for Bid, one for Ask
    ou::tf::l2::DataRowElement<double> m_drePrice;
    ou::tf::l2::DataRowElement<unsigned int> m_dreSize;
    ou::tf::l2::DataRowElement<unsigned int> m_dreSizeAgg;
  };

  struct DataRow_Statistics {
    ou::tf::l2::DataRowElement<double> m_dreImbalance;
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

  m_timerRefresh.SetOwner( this );
  Bind( wxEVT_TIMER, &PanelSideBySide::HandleTimerRefresh, this, m_timerRefresh.GetId() );
  m_timerRefresh.Start( 200 ); // 5 times a second
}

void PanelSideBySide::OnL2Ask( double price, int volume, bool bOnAdd ) {
  UpdateMap( m_mapAskPriceLevel, price, volume );
}

void PanelSideBySide::OnL2Bid( double price, int volume, bool bOnAdd ) {
  UpdateMap( m_mapBidPriceLevel, price, volume );
}

void PanelSideBySide::UpdateMap( mapPriceLevel_t& map, double price, int volume ) {

  // scoped_lock: brute force & ignorance for now, probably ultimately

  mapPriceLevel_t::iterator iter = map.find( price );
  if ( map.end() == iter ) {
    if ( 0 < volume ) {
      std::scoped_lock<std::mutex> lock( m_mutexMaps );
      auto result = map.emplace( price, PriceLevel( volume) );
      assert( result.second );
    }
  }
  else {
    if ( 0 == volume ) {
      std::scoped_lock<std::mutex> lock( m_mutexMaps );
      map.erase( iter );
    }
    else {
      iter->second.nVolume = volume;
    }
  }
}

double Imbalance( int volBid, int volAsk ) {
  int top = volBid - volAsk;
  int bot = volBid + volAsk;
  return ( (double)top / (double)bot );
}

void PanelSideBySide::CalculateStatistics() { // need to fix this, as cross thread problems in the maps exist (add the DataRow thingy)

  // brute force & ignorance for now, probably ultimately, just need lock on the map add/delete portions
  std::scoped_lock<std::mutex> lock( m_mutexMaps );

  if ( ( 2 <= m_mapAskPriceLevel.size() ) && ( 2 <= m_mapBidPriceLevel.size() ) && ( 2 <= m_vWinRow.size() ) ) { // TODO: tune the test
    int nVolumeAggregateAsk {};
    int nVolumeAggregateBid {};

    mapPriceLevel_t::iterator iterMapAsk = m_mapAskPriceLevel.begin();
    //iterMapAsk->second.nVolumeAggregate = 0;
    mapPriceLevel_t::reverse_iterator iterMapBid = m_mapBidPriceLevel.rbegin();
    //iterMapBid->second.nVolumeAggregate = 0;
    vWinRow_t::iterator iterWinRow = m_vWinRow.begin();

    for ( int ix = 0; ix < 2; ix++ ) {
      nVolumeAggregateAsk += iterMapAsk->second.nVolume;
      nVolumeAggregateBid += iterMapBid->second.nVolume;
      WinRow& row( **iterWinRow );

      row[ (int)EField::APrice ]->SetText( boost::lexical_cast<std::string>( iterMapAsk->first ) ); // will need to convert to proper data element
      row[ (int)EField::ASize ]->SetText( boost::lexical_cast<std::string>( iterMapAsk->second.nVolume ) ); // will need to convert to proper data element
      row[ (int)EField::ASizeAgg ]->SetText( boost::lexical_cast<std::string>( nVolumeAggregateAsk ) ); // will need to convert to proper data element

      row[ (int)EField::BPrice ]->SetText( boost::lexical_cast<std::string>( iterMapBid->first ) ); // will need to convert to proper data element
      row[ (int)EField::BSize ]->SetText( boost::lexical_cast<std::string>( iterMapBid->second.nVolume ) ); // will need to convert to proper data element
      row[ (int)EField::BSizeAgg ]->SetText( boost::lexical_cast<std::string>( nVolumeAggregateBid ) ); // will need to convert to proper data element

      double imbalance = Imbalance( nVolumeAggregateBid, nVolumeAggregateAsk );
      row[ (int)EField::Imbalance ]->SetText( boost::lexical_cast<std::string>( imbalance ) ); // will need to convert to proper data element

      iterMapAsk++;
      iterMapBid++;
      iterWinRow++;
    }
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

    Unbind( wxEVT_TIMER, &PanelSideBySide::HandleTimerRefresh, this, m_timerRefresh.GetId() );

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
