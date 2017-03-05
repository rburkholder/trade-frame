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

#include <cfloat>

#include <OUCommon/Colour.h>

#include "ChartDVBasics.h"

namespace ou { // One Unified

// 55, 89, 144, 233, 377, 610, 987, 1597, 2584, 4181, 6765
// .91 1.5 2.4  3.9, 6.3, 10.2 16.5 26.6  54.0  69.7  112.8

ChartDVBasics::ChartDVBasics(void) 
  : m_dvChart(),
  m_bfTrades( 10 ),
  m_bfBuys( 10 ),
  m_bfSells( 10 ),
  m_bFirstTrade( true ),
  m_rtTickDiffs( m_pricesTickDiffs, seconds( 120 ) ),
  m_rocTickDiffs( m_pricesTickDiffsROC, seconds( 30 ) ),
  m_dblUpTicks( 0.0 ), m_dblMdTicks( 0.0 ), m_dblDnTicks( 0.0 ),
  m_dblUpVolume( 0.0 ), m_dblMdVolume( 0.0 ), m_dblDnVolume( 0.0 ),
  m_ceShortEntries( ou::ChartEntryShape::EShort, ou::Colour::Red ),
  m_ceLongEntries( ou::ChartEntryShape::ELong, ou::Colour::Blue ),
  m_ceShortFills( ou::ChartEntryShape::EFillShort, ou::Colour::Red ),
  m_ceLongFills( ou::ChartEntryShape::EFillLong, ou::Colour::Blue ),
  m_ceShortExits( ou::ChartEntryShape::EShortStop, ou::Colour::Red ),
  m_ceLongExits( ou::ChartEntryShape::ELongStop, ou::Colour::Blue )

{

  m_vInfoBollinger.push_back( infoBollinger( m_quotes, boost::posix_time::seconds(  144 ) ) );
  m_vInfoBollinger.push_back( infoBollinger( m_quotes, boost::posix_time::seconds(  377 ) ) );
  m_vInfoBollinger.push_back( infoBollinger( m_quotes, boost::posix_time::seconds(  987 ) ) );
  m_vInfoBollinger.push_back( infoBollinger( m_quotes, boost::posix_time::seconds( 2584 ) ) );

  m_vInfoBollinger[0].SetProperties( ou::Colour::DarkOliveGreen, "Bollinger1 - 2.4" );
  m_vInfoBollinger[1].SetProperties( ou::Colour::Turquoise, "Bollinger2 - 6.3" );
  m_vInfoBollinger[2].SetProperties( ou::Colour::GreenYellow, "Bollinger3 - 16.5" );
  m_vInfoBollinger[3].SetProperties( ou::Colour::MediumSlateBlue, "Bollinger4 - 54.0" );

  m_dvChart.Add( 0, &m_ceQuoteUpper );
  m_dvChart.Add( 0, &m_ceQuoteLower );
  m_dvChart.Add( 0, &m_ceTrade );
  m_dvChart.Add( 2, &m_ceQuoteSpread );

  m_dvChart.Add( 0, &m_ceShortEntries );
  m_dvChart.Add( 0, &m_ceLongEntries );
  m_dvChart.Add( 0, &m_ceShortFills );
  m_dvChart.Add( 0, &m_ceLongFills );
  m_dvChart.Add( 0, &m_ceShortExits );
  m_dvChart.Add( 0, &m_ceLongExits );

  for ( int ix = 0; ix <= 3; ++ix ) {
    infoBollinger& ib( m_vInfoBollinger[ix] );
    m_dvChart.Add( 0, &ib.m_ceEma );
    m_dvChart.Add( 0, &ib.m_ceUpperBollinger );
    m_dvChart.Add( 0, &ib.m_ceLowerBollinger );
    //m_dvChart.Add( 5, &ib.m_ceRatio );
    m_dvChart.Add( 6, &ib.m_ceSlope );
    m_dvChart.Add( 7, &ib.m_ceSlopeBy2 );
    m_dvChart.Add( 8, &ib.m_ceSlopeBy3 );
  }

//  m_cemRatio.AddMark(  2.0, ou::Colour::Black, "+2sd" );
//  m_cemRatio.AddMark(  0.0, ou::Colour::Black, "" );//"zero" );
//  m_cemRatio.AddMark( -2.0, ou::Colour::Black, "-2sd" );
//  m_dvChart.Add( 5, &m_cemRatio );

  m_cemSlope.AddMark( 0.0, ou::Colour::Black, "" ); //"zero" );
  m_dvChart.Add( 6, &m_cemSlope );

  m_cemSlopeBy2.AddMark( 0.0, ou::Colour::Black, "" ); //"zero" );
  m_dvChart.Add( 7, &m_cemSlopeBy2 );

  m_cemSlopeBy3.AddMark( 0.0, ou::Colour::Black, "" ); //"zero" );
  m_dvChart.Add( 8, &m_cemSlopeBy3 );

  m_dvChart.Add( 0, &m_ceBars );

  m_dvChart.Add( 1, &m_rVolumes[ VUp ].ceVolumeUp );
  m_dvChart.Add( 1, &m_rVolumes[ VUp ].ceVolumeNeutral );
  m_dvChart.Add( 1, &m_rVolumes[ VUp ].ceVolumeDn );
  m_dvChart.Add( 1, &m_rVolumes[ VDn ].ceVolumeUp );
  m_dvChart.Add( 1, &m_rVolumes[ VDn ].ceVolumeNeutral );
  m_dvChart.Add( 1, &m_rVolumes[ VDn ].ceVolumeDn );

  m_ceQuoteUpper.SetColour( ou::Colour::Red );
  m_ceQuoteLower.SetColour( ou::Colour::Blue );
  m_ceTrade.SetColour( ou::Colour::DarkGreen );
  m_ceQuoteSpread.SetColour( ou::Colour::Black );

  m_ceQuoteSpread.SetName( "Spread" );
  //m_ceQuoteUpper.SetName( "QuoteUpper" );
  //m_ceQuoteLower.SetName( "QuoteLower" );
  //m_ceTrade.SetName( "Trade" );

  m_rVolumes[ VUp ].ceVolumeUp.SetColour( ou::Colour::Green );
  m_rVolumes[ VUp ].ceVolumeNeutral.SetColour( ou::Colour::Yellow );
  m_rVolumes[ VUp ].ceVolumeDn.SetColour( ou::Colour::Red );
  m_rVolumes[ VDn ].ceVolumeUp.SetColour( ou::Colour::Green );
  m_rVolumes[ VDn ].ceVolumeNeutral.SetColour( ou::Colour::Yellow );
  m_rVolumes[ VDn ].ceVolumeDn.SetColour( ou::Colour::Red );

  m_ceZigZag.SetColour( ou::Colour::DarkBlue );

  m_bfTrades.SetOnBarComplete( MakeDelegate( this, &ChartDVBasics::HandleBarCompletionTrades ) );
  m_bfBuys.SetOnBarComplete( MakeDelegate( this, &ChartDVBasics::HandleBarCompletionBuys ) );
  m_bfSells.SetOnBarComplete( MakeDelegate( this, &ChartDVBasics::HandleBarCompletionSells ) );

  m_zigzagPrice.SetOnPeakFound( MakeDelegate( this, &ChartDVBasics::HandleZigZagPeak ) );
  m_zigzagPrice.SetUpDecisionPointFound( MakeDelegate( this, &ChartDVBasics::HandleZigZagUpDp ) );
  m_zigzagPrice.SetDnDecisionPointFound( MakeDelegate( this, &ChartDVBasics::HandleZigZagDnDp ) );
}

ChartDVBasics::~ChartDVBasics(void) { 
  m_bfTrades.SetOnBarComplete( 0 );
  m_bfBuys.SetOnBarComplete( 0 );
  m_bfSells.SetOnBarComplete( 0 );
}

void ChartDVBasics::HandleBarCompletionTrades( const ou::tf::Bar& bar ) {
  m_ceBars.AppendBar( bar );
}

void ChartDVBasics::HandleBarCompletionBuys( const ou::tf::Bar& bar ) {

  ptime dt( bar.DateTime() );

  if ( bar.Open() == bar.Close() ) {
    m_rVolumes[ VUp ].ceVolumeUp.Append( dt, 0.0 );
    m_rVolumes[ VUp ].ceVolumeNeutral.Append( dt, bar.Volume() );
    m_rVolumes[ VUp ].ceVolumeDn.Append( dt, 0.0 );
  }
  else {
    if ( bar.Close() > bar.Open() ) {
      m_rVolumes[ VUp ].ceVolumeUp.Append( dt, bar.Volume() );
      m_rVolumes[ VUp ].ceVolumeNeutral.Append( dt, 0.0 );
      m_rVolumes[ VUp ].ceVolumeDn.Append( dt, 0.0 );
    }
    else {
      m_rVolumes[ VUp ].ceVolumeUp.Append( dt, 0.0 );
      m_rVolumes[ VUp ].ceVolumeNeutral.Append( dt, 0.0 );
      m_rVolumes[ VUp ].ceVolumeDn.Append( dt, bar.Volume() );
    }
  }
}

void ChartDVBasics::HandleBarCompletionSells( const ou::tf::Bar& bar ) {

  ptime dt( bar.DateTime() );

  if ( bar.Open() == bar.Close() ) {
    m_rVolumes[ VDn ].ceVolumeUp.Append( dt, 0.0 );
    m_rVolumes[ VDn ].ceVolumeNeutral.Append( dt, -bar.Volume() );
    m_rVolumes[ VDn ].ceVolumeDn.Append( dt, 0.0 );
  }
  else {
    if ( bar.Close() > bar.Open() ) {
      m_rVolumes[ VDn ].ceVolumeUp.Append( dt, -bar.Volume() );
      m_rVolumes[ VDn ].ceVolumeNeutral.Append( dt, 0.0 );
      m_rVolumes[ VDn ].ceVolumeDn.Append( dt, 0.0 );
    }
    else {
      m_rVolumes[ VDn ].ceVolumeUp.Append( dt, 0.0 );
      m_rVolumes[ VDn ].ceVolumeNeutral.Append( dt, 0.0 );
      m_rVolumes[ VDn ].ceVolumeDn.Append( dt, -bar.Volume() );
    }
  }
}

void ChartDVBasics::HandleZigZagPeak( const ou::tf::ZigZag&, ptime dt, double price, ou::tf::ZigZag::EDirection ) {
  m_ceZigZag.Append( dt, price );
  //m_zigzagPrice.SetFilterWidth( 0.17 * sqrt( price ) );
}

void ChartDVBasics::HandleZigZagUpDp( const ou::tf::ZigZag& ) {
}

void ChartDVBasics::HandleZigZagDnDp( const ou::tf::ZigZag& ) {
}

void ChartDVBasics::HandleTrade( const ou::tf::Trade& trade ) {

  ptime dt( trade.DateTime() );
  ou::tf::Trade::price_t price = trade.Price();

  if ( m_bFirstTrade ) {
    m_bFirstTrade = false;
    //m_zigzagPrice.SetFilterWidth( 0.17 * sqrt( trade.Trade() ) );
    m_zigzagPrice.SetFilterWidth( 4.0 );
  }

  m_ceTrade.Append( dt, price );

  m_trades.Append( trade );
  m_bfTrades.Add( trade );

  m_zigzagPrice.Check( dt, price );

  if ( 0 < m_quotes.Size() ) {
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
        m_TradeDirection = ETradeDirDn;  // definitively down
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
    m_ceTickDiffs.Append( dt, dif );

  //  m_rocTickDiffs.Update();
    dif = m_rocTickDiffs.RateOfChange();
    if (  45 < dif ) dif = 45;
    if ( -45 > dif ) dif = -45;
    m_ceTickDiffsRoc.Append( dt, dif );
  }
  
}

void ChartDVBasics::HandleQuote( const ou::tf::Quote& quote ) {

  if ( !quote.IsValid() ) {
    return;
  }
  // should also check that a price within 2 - 3 sigma of last

  // problems occur when long trend happens and can't get out of opposing position.

  ptime dt( quote.DateTime() );

  m_quoteLast = quote;
  m_quotes.Append( quote );
  double midpoint = quote.Midpoint();

  m_ceQuoteUpper.Append( dt, quote.Ask() );
  m_ceQuoteLower.Append( dt, quote.Bid() );
  m_ceQuoteSpread.Append( dt, quote.Ask() - quote.Bid() );

  for ( int ix = 0; ix <= 3; ++ix ) {
    double lastEma, sd;
    infoBollinger& ib( m_vInfoBollinger[ix] );
    ib.m_ceEma.Append( dt, ib.m_ema.Ago( 0 ).Value() );
    lastEma = ib.m_ema.Ago( 0 ).Value();
    sd = 2.0 * ib.m_stats.SD();
    ib.m_ceUpperBollinger.Append( dt, lastEma + sd );
    ib.m_ceLowerBollinger.Append( dt, lastEma - sd );
    ib.m_dblBollingerWidth = 2.0 * sd;
//    if ( 0.0 < sd ) {
//      ib.m_ceRatio.Append( dt, ( midpoint - lastEma ) / ( sd ) );
//    }
    double slope = ib.m_statsSlope.Slope();
    if ( 100.0 < std::abs( slope ) ) {
    }
    else {
      if ( ( slope <= DBL_MAX ) && ( slope >= -DBL_MAX ) ) {
        ib.m_ceSlope.Append( dt, slope );
        ib.m_tsStatsSlope.Append( ou::tf::Price( dt, slope ) );
        double slopeby2 = ib.m_statsSlopeBy2.Slope();
        if ( 100.0 < std::abs( slopeby2 ) ) {
        }
        else {
          ib.m_ceSlopeBy2.Append( dt, slopeby2 );

          ib.m_tsStatsSlopeBy2.Append( ou::tf::Price( dt, slopeby2 ) );
          double slopeby3 = ib.m_statsSlopeBy3.Slope();
          if ( 100.0 < std::abs( slopeby3 ) ) {
          }
          else {
            ib.m_ceSlopeBy3.Append( dt, slopeby3 );
//            if ( 0 == ix ) { // only on shortest time frame
//              switch ( ib.m_stateAccel.State( slopeby3 ) ) {
//              case ou::tf::Crossing<double>::EGTX: 
//                break;
//              case ou::tf::Crossing<double>::ELTX: 
//                break;
//              }
//            }
          }
        }
      }
    }

  }
}


} // namespace ou