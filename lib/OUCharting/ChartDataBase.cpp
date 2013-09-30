/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

#include "ChartDataBase.h"

namespace ou { // One Unified

ChartDataBase::ChartDataBase(void) 
  : m_dvChart( "ChartDataBase", "Symbol" ),
  m_ema1( m_quotes, boost::posix_time::seconds(   55 ) ),
  m_ema2( m_quotes, boost::posix_time::seconds(  233 ) ),
  m_ema3( m_quotes, boost::posix_time::seconds( 1597 ) ),
  m_bfTrades( 10 ),
  m_bfBuys( 10 ),
  m_bfSells( 10 ),
  m_bFirstTrade( true ),
  m_rtTickDiffs( m_pricesTickDiffs, seconds( 120 ) ),
  m_rocTickDiffs( m_pricesTickDiffsROC, seconds( 30 ) ),
  m_dblUpTicks( 0.0 ), m_dblMdTicks( 0.0 ), m_dblDnTicks( 0.0 ),
  m_dblUpVolume( 0.0 ), m_dblMdVolume( 0.0 ), m_dblDnVolume( 0.0 )
{
  

  m_dvChart.Add( 0, m_ceBars );

  m_dvChart.Add( 0, m_ceEma1 );
  m_dvChart.Add( 0, m_ceEma2 );
  m_dvChart.Add( 0, m_ceEma3 );

  m_dvChart.Add( 1, m_rVolumes[ VUp ].ceVolumeUp );
  m_dvChart.Add( 1, m_rVolumes[ VUp ].ceVolumeNeutral );
  m_dvChart.Add( 1, m_rVolumes[ VUp ].ceVolumeDn );
  m_dvChart.Add( 1, m_rVolumes[ VDn ].ceVolumeUp );
  m_dvChart.Add( 1, m_rVolumes[ VDn ].ceVolumeNeutral );
  m_dvChart.Add( 1, m_rVolumes[ VDn ].ceVolumeDn );

  m_ceEma1.SetColour( ou::Colour::MediumVioletRed );
  m_ceEma2.SetColour( ou::Colour::RoyalBlue );
  m_ceEma3.SetColour( ou::Colour::MediumSpringGreen );

  m_rVolumes[ VUp ].ceVolumeUp.SetColour( ou::Colour::Green );
  m_rVolumes[ VUp ].ceVolumeNeutral.SetColour( ou::Colour::Yellow );
  m_rVolumes[ VUp ].ceVolumeDn.SetColour( ou::Colour::Red );
  m_rVolumes[ VDn ].ceVolumeUp.SetColour( ou::Colour::Green );
  m_rVolumes[ VDn ].ceVolumeNeutral.SetColour( ou::Colour::Yellow );
  m_rVolumes[ VDn ].ceVolumeDn.SetColour( ou::Colour::Red );

  m_bfTrades.SetOnBarComplete( MakeDelegate( this, &ChartDataBase::HandleBarCompletionTrades ) );
  m_bfBuys.SetOnBarComplete( MakeDelegate( this, &ChartDataBase::HandleBarCompletionBuys ) );
  m_bfSells.SetOnBarComplete( MakeDelegate( this, &ChartDataBase::HandleBarCompletionSells ) );
}

ChartDataBase::~ChartDataBase(void) { 
  m_bfTrades.SetOnBarComplete( 0 );
  m_bfBuys.SetOnBarComplete( 0 );
  m_bfSells.SetOnBarComplete( 0 );
}

void ChartDataBase::HandleBarCompletionTrades( const ou::tf::Bar& bar ) {
  m_ceBars.AddBar( bar );
}

void ChartDataBase::HandleBarCompletionBuys( const ou::tf::Bar& bar ) {

  ptime dt( bar.DateTime() );

  if ( bar.Open() == bar.Close() ) {
    m_rVolumes[ VUp ].ceVolumeUp.Add( dt, 0.0 );
    m_rVolumes[ VUp ].ceVolumeNeutral.Add( dt, bar.Volume() );
    m_rVolumes[ VUp ].ceVolumeDn.Add( dt, 0.0 );
  }
  else {
    if ( bar.Close() > bar.Open() ) {
      m_rVolumes[ VUp ].ceVolumeUp.Add( dt, bar.Volume() );
      m_rVolumes[ VUp ].ceVolumeNeutral.Add( dt, 0.0 );
      m_rVolumes[ VUp ].ceVolumeDn.Add( dt, 0.0 );
    }
    else {
      m_rVolumes[ VUp ].ceVolumeUp.Add( dt, 0.0 );
      m_rVolumes[ VUp ].ceVolumeNeutral.Add( dt, 0.0 );
      m_rVolumes[ VUp ].ceVolumeDn.Add( dt, bar.Volume() );
    }
  }
}

void ChartDataBase::HandleBarCompletionSells( const ou::tf::Bar& bar ) {

  ptime dt( bar.DateTime() );

  if ( bar.Open() == bar.Close() ) {
    m_rVolumes[ VDn ].ceVolumeUp.Add( dt, 0.0 );
    m_rVolumes[ VDn ].ceVolumeNeutral.Add( dt, -bar.Volume() );
    m_rVolumes[ VDn ].ceVolumeDn.Add( dt, 0.0 );
  }
  else {
    if ( bar.Close() > bar.Open() ) {
      m_rVolumes[ VDn ].ceVolumeUp.Add( dt, -bar.Volume() );
      m_rVolumes[ VDn ].ceVolumeNeutral.Add( dt, 0.0 );
      m_rVolumes[ VDn ].ceVolumeDn.Add( dt, 0.0 );
    }
    else {
      m_rVolumes[ VDn ].ceVolumeUp.Add( dt, 0.0 );
      m_rVolumes[ VDn ].ceVolumeNeutral.Add( dt, 0.0 );
      m_rVolumes[ VDn ].ceVolumeDn.Add( dt, -bar.Volume() );
    }
  }
}

void ChartDataBase::HandleZigZagPeak( const ou::tf::ZigZag&, ptime dt, double price, ou::tf::ZigZag::EDirection ) {
  m_ceZigZag.Add( dt, price );
  //m_zigzagPrice.SetFilterWidth( 0.17 * sqrt( price ) );
}

void ChartDataBase::HandleZigZagUpDp( const ou::tf::ZigZag& ) {
}

void ChartDataBase::HandleZigZagDnDp( const ou::tf::ZigZag& ) {
}

void ChartDataBase::HandleTrade( const ou::tf::Trade& trade ) {

  ptime dt( trade.DateTime() );
  ou::tf::Trade::price_t price = trade.Price();

  if ( m_bFirstTrade ) {
    m_bFirstTrade = false;
    //m_zigzagPrice.SetFilterWidth( 0.17 * sqrt( trade.Trade() ) );
    m_zigzagPrice.SetFilterWidth( 4.0 );
  }

  m_trades.Append( trade );
  m_bfTrades.Add( trade );

  m_zigzagPrice.Check( dt, price );

  double mid = m_quoteLast.Midpoint();
  if ( price == mid ) {
    switch ( m_TradeDirection ) {
    case ETradeDirUnkn:
      m_TradeDirection = ETradeDirUp;  // default to up
      //break; fall through instead
    case ETradeDirUp:  // leave as is
      break;
    case ETradeDirDn: // leave as is
      break;
    }
    ++m_dblMdTicks;
    //m_dblMdVolume += trade.Volume();
  }
  else {
    if ( price > mid ) {
      m_TradeDirection = ETradeDirUp;   // definitively up
      //++m_dblMdTicks;
      m_pricesTickDiffs.Append( ou::tf::Price( dt, +1.0 ) );
      ++m_dblUpTicks;
      m_dblUpVolume += trade.Volume();
    }
    else {
      m_TradeDirection = ETradeDirDn;  // definitively dow
      //--m_dblMdTicks;
      m_pricesTickDiffs.Append( ou::tf::Price( dt, -1.0 ) );
      ++m_dblDnTicks;
      m_dblDnVolume += trade.Volume();
    }
  }

  switch ( m_TradeDirection ) {
  case ETradeDirUp:
    m_bfBuys.Add( trade );
    break;
  case ETradeDirDn:
    m_bfSells.Add( trade );
    break;
  case ETradeDirUnkn:
    break;
  }

//  m_rtTickDiffs.Update();
  double dif = m_rtTickDiffs.Net();
  m_pricesTickDiffsROC.Append( ou::tf::Price( dt, dif ) );

  if (  45 < dif ) dif = 45;
  if ( -45 > dif ) dif = -45;
  m_ceTickDiffs.Add( dt, dif );

//  m_rocTickDiffs.Update();
  dif = m_rocTickDiffs.RateOfChange();
  if (  45 < dif ) dif = 45;
  if ( -45 > dif ) dif = -45;
  m_ceTickDiffsRoc.Add( dt, dif );
}

void ChartDataBase::HandleQuote( const ou::tf::Quote& quote ) {

  if ( !quote.IsValid() ) {
    return;
  }
  // should also check that a price within 2 - 3 sigma of last

  // problems occur when long trend happens and can't get out of opposing position.

  ptime dt( quote.DateTime() );

  m_quoteLast = quote;
  m_quotes.Append( quote );
  double midpoint = quote.Midpoint();

  m_ceEma1.Add( dt, m_ema1.Ago( 0 ).Value() );
  m_ceEma2.Add( dt, m_ema2.Ago( 0 ).Value() );
  m_ceEma3.Add( dt, m_ema3.Ago( 0 ).Value() );

  if ( 500 < m_quotes.Size() ) {
  }

}


} // namespace ou