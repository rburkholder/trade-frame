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

// 55, 89, 144, 233, 377, 610, 987, 1597, 2584, 4181, 6765

ChartDataBase::ChartDataBase(void) 
  : m_dvChart(),
  m_ema1( m_quotes, boost::posix_time::seconds(  144 ) ),
  m_ema2( m_quotes, boost::posix_time::seconds(  377 ) ),
  m_ema3( m_quotes, boost::posix_time::seconds(  987 ) ),
  m_ema4( m_quotes, boost::posix_time::seconds(  2584 ) ),
  m_stats1( m_quotes, boost::posix_time::seconds(  144 ) ),
  m_stats2( m_quotes, boost::posix_time::seconds(  377 ) ),
  m_stats3( m_quotes, boost::posix_time::seconds(  987 ) ),
  m_stats4( m_quotes, boost::posix_time::seconds(  2584 ) ),
  m_bfTrades( 10 ),
  m_bfBuys( 10 ),
  m_bfSells( 10 ),
  m_bFirstTrade( true ),
  m_rtTickDiffs( m_pricesTickDiffs, seconds( 120 ) ),
  m_rocTickDiffs( m_pricesTickDiffsROC, seconds( 30 ) ),
  m_dblUpTicks( 0.0 ), m_dblMdTicks( 0.0 ), m_dblDnTicks( 0.0 ),
  m_dblUpVolume( 0.0 ), m_dblMdVolume( 0.0 ), m_dblDnVolume( 0.0 )
{
  
  m_dvChart.Add( 0, &m_ceQuoteUpper );
  m_dvChart.Add( 0, &m_ceQuoteLower );
  m_dvChart.Add( 0, &m_ceTrade );
  m_dvChart.Add( 2, &m_ceQuoteSpread );

  m_dvChart.Add( 0, &m_ceEma1 );
  m_dvChart.Add( 0, &m_ceEma2 );
  m_dvChart.Add( 0, &m_ceEma3 );
  m_dvChart.Add( 0, &m_ceEma4 );

  m_dvChart.Add( 0, &m_ceBars );

  m_dvChart.Add( 1, &m_rVolumes[ VUp ].ceVolumeUp );
  m_dvChart.Add( 1, &m_rVolumes[ VUp ].ceVolumeNeutral );
  m_dvChart.Add( 1, &m_rVolumes[ VUp ].ceVolumeDn );
  m_dvChart.Add( 1, &m_rVolumes[ VDn ].ceVolumeUp );
  m_dvChart.Add( 1, &m_rVolumes[ VDn ].ceVolumeNeutral );
  m_dvChart.Add( 1, &m_rVolumes[ VDn ].ceVolumeDn );

  m_dvChart.Add( 0, &m_ceUpperBollinger1 );
  m_dvChart.Add( 0, &m_ceLowerBollinger1 );
  m_dvChart.Add( 0, &m_ceUpperBollinger2 );
  m_dvChart.Add( 0, &m_ceLowerBollinger2 );
  m_dvChart.Add( 0, &m_ceUpperBollinger3 );
  m_dvChart.Add( 0, &m_ceLowerBollinger3 );
  m_dvChart.Add( 0, &m_ceUpperBollinger4 );
  m_dvChart.Add( 0, &m_ceLowerBollinger4 );
  /*
  m_dvChart.Add( 2, m_ce11 );
  m_dvChart.Add( 2, m_ce12 );
  m_dvChart.Add( 2, m_ce13 );
  m_dvChart.Add( 2, m_ce14 );
  m_dvChart.Add( 3, m_ce21 );
  m_dvChart.Add( 3, m_ce22 );
  m_dvChart.Add( 3, m_ce23 );
  m_dvChart.Add( 3, m_ce24 );

  m_dvChart.Add( 4, m_ce31 );
  
  m_ce11.SetColour( ou::Colour::Red );
  m_ce12.SetColour( ou::Colour::Orange );
  m_ce13.SetColour( ou::Colour::Yellow );
  m_ce14.SetColour( ou::Colour::Green );
  m_ce21.SetColour( ou::Colour::Red );
  m_ce22.SetColour( ou::Colour::Orange );
  m_ce23.SetColour( ou::Colour::Yellow );
  m_ce24.SetColour( ou::Colour::Green );
  */

  m_ceQuoteUpper.SetColour( ou::Colour::Red );
  m_ceQuoteLower.SetColour( ou::Colour::Blue );
  m_ceTrade.SetColour( ou::Colour::DarkGreen );
  m_ceQuoteSpread.SetColour( ou::Colour::Black );

  m_rVolumes[ VUp ].ceVolumeUp.SetColour( ou::Colour::Green );
  m_rVolumes[ VUp ].ceVolumeNeutral.SetColour( ou::Colour::Yellow );
  m_rVolumes[ VUp ].ceVolumeDn.SetColour( ou::Colour::Red );
  m_rVolumes[ VDn ].ceVolumeUp.SetColour( ou::Colour::Green );
  m_rVolumes[ VDn ].ceVolumeNeutral.SetColour( ou::Colour::Yellow );
  m_rVolumes[ VDn ].ceVolumeDn.SetColour( ou::Colour::Red );

  //m_ceEma1.SetColour( ou::Colour::MediumVioletRed );
  m_ceEma1.SetColour( ou::Colour::DarkOliveGreen );
  m_ceUpperBollinger1.SetColour( ou::Colour::DarkOliveGreen );
  m_ceLowerBollinger1.SetColour( ou::Colour::DarkOliveGreen );

  //m_ceEma2.SetColour( ou::Colour::RoyalBlue );
  m_ceEma2.SetColour( ou::Colour::Turquoise );
  m_ceUpperBollinger2.SetColour( ou::Colour::Turquoise );
  m_ceLowerBollinger2.SetColour( ou::Colour::Turquoise );

  //m_ceEma3.SetColour( ou::Colour::MediumSpringGreen );
  m_ceEma3.SetColour( ou::Colour::GreenYellow );
  m_ceUpperBollinger3.SetColour( ou::Colour::GreenYellow );
  m_ceLowerBollinger3.SetColour( ou::Colour::GreenYellow );

  m_ceEma4.SetColour( ou::Colour::ForestGreen );
  m_ceUpperBollinger4.SetColour( ou::Colour::ForestGreen );
  m_ceLowerBollinger4.SetColour( ou::Colour::ForestGreen );

//  m_ceBollinger1Offset.SetColour( ou::Colour::DarkOliveGreen );
//  m_ceBollinger2Offset.SetColour( ou::Colour::Turquoise );
//  m_ceBollinger3Offset.SetColour( ou::Colour::GreenYellow );

//  m_ceSlopeOfBollinger2Offset.SetColour( ou::Colour::DarkMagenta );

  m_ceZigZag.SetColour( ou::Colour::DarkBlue );

  m_bfTrades.SetOnBarComplete( MakeDelegate( this, &ChartDataBase::HandleBarCompletionTrades ) );
  m_bfBuys.SetOnBarComplete( MakeDelegate( this, &ChartDataBase::HandleBarCompletionBuys ) );
  m_bfSells.SetOnBarComplete( MakeDelegate( this, &ChartDataBase::HandleBarCompletionSells ) );

  m_zigzagPrice.SetOnPeakFound( MakeDelegate( this, &ChartDataBase::HandleZigZagPeak ) );
  m_zigzagPrice.SetUpDecisionPointFound( MakeDelegate( this, &ChartDataBase::HandleZigZagUpDp ) );
  m_zigzagPrice.SetDnDecisionPointFound( MakeDelegate( this, &ChartDataBase::HandleZigZagDnDp ) );
}

ChartDataBase::~ChartDataBase(void) { 
  m_bfTrades.SetOnBarComplete( 0 );
  m_bfBuys.SetOnBarComplete( 0 );
  m_bfSells.SetOnBarComplete( 0 );
}

void ChartDataBase::HandleBarCompletionTrades( const ou::tf::Bar& bar ) {
  m_ceBars.AppendBar( bar );
}

void ChartDataBase::HandleBarCompletionBuys( const ou::tf::Bar& bar ) {

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

void ChartDataBase::HandleBarCompletionSells( const ou::tf::Bar& bar ) {

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

void ChartDataBase::HandleZigZagPeak( const ou::tf::ZigZag&, ptime dt, double price, ou::tf::ZigZag::EDirection ) {
  m_ceZigZag.Append( dt, price );
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
    m_ceTickDiffs.Append( dt, dif );

  //  m_rocTickDiffs.Update();
    dif = m_rocTickDiffs.RateOfChange();
    if (  45 < dif ) dif = 45;
    if ( -45 > dif ) dif = -45;
    m_ceTickDiffsRoc.Append( dt, dif );
    /*
    double lastEma, sd;
    lastEma = m_ema1.Ago( 0 ).Value();
    sd = 2.0 * m_variance1.Ago( 0 ).Value();
    m_ceUpperBollinger1.Append( dt, lastEma + sd );
    m_ceLowerBollinger1.Append( dt, lastEma - sd );
    
    lastEma = m_ema2.Ago( 0 ).Value();
    sd = 2.0 * m_variance2.Ago( 0 ).Value();
    m_ceUpperBollinger2.Append( dt, lastEma + sd );
    m_ceLowerBollinger2.Append( dt, lastEma - sd );

    lastEma = m_ema3.Ago( 0 ).Value();
    sd = 2.0 * m_variance3.Ago( 0 ).Value();
    m_ceUpperBollinger3.Append( dt, lastEma + sd );
    m_ceLowerBollinger3.Append( dt, lastEma - sd );
    */
  }

  
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

  m_ceQuoteUpper.Append( dt, quote.Ask() );
  m_ceQuoteLower.Append( dt, quote.Bid() );
  m_ceQuoteSpread.Append( dt, quote.Ask() - quote.Bid() );

  m_ceEma1.Append( dt, m_ema1.Ago( 0 ).Value() );
  m_ceEma2.Append( dt, m_ema2.Ago( 0 ).Value() );
  m_ceEma3.Append( dt, m_ema3.Ago( 0 ).Value() );
  m_ceEma4.Append( dt, m_ema4.Ago( 0 ).Value() );

    double lastEma, sd;

    lastEma = m_ema1.Ago( 0 ).Value();
    sd = 2.0 * m_stats1.SD();
    m_ceUpperBollinger1.Append( dt, lastEma + sd );
    m_ceLowerBollinger1.Append( dt, lastEma - sd );
    
    lastEma = m_ema2.Ago( 0 ).Value();
    sd = 2.0 * m_stats2.SD();
    m_ceUpperBollinger2.Append( dt, lastEma + sd );
    m_ceLowerBollinger2.Append( dt, lastEma - sd );

    lastEma = m_ema3.Ago( 0 ).Value();
    sd = 2.0 * m_stats3.SD();
    m_ceUpperBollinger3.Append( dt, lastEma + sd );
    m_ceLowerBollinger3.Append( dt, lastEma - sd );

    lastEma = m_ema4.Ago( 0 ).Value();
    sd = 2.0 * m_stats4.SD();
    m_ceUpperBollinger4.Append( dt, lastEma + sd );
    m_ceLowerBollinger4.Append( dt, lastEma - sd );

    /*
    m_ce11.Append( dt, m_variance1.m_pma1->m_vEMA[1]->GetEMA() );
    m_ce12.Append( dt, m_variance1.m_pma1->m_vEMA[5]->GetEMA() );
    m_ce13.Append( dt, m_variance1.m_pma1->m_vEMA[9]->GetEMA() );
    m_ce14.Append( dt, m_variance1.m_pma1->GetMA() );
    m_ce21.Append( dt, m_variance1.m_ma2.m_vEMA[1]->GetEMA() );
    m_ce22.Append( dt, m_variance1.m_ma2.m_vEMA[5]->GetEMA() );
    m_ce23.Append( dt, m_variance1.m_ma2.m_vEMA[9]->GetEMA() );
    m_ce24.Append( dt, m_variance1.m_ma2.GetMA() );

    m_ce31.Append( dt, m_variance1.Ago( 0 ).Value() );
    */
  if ( 500 < m_quotes.Size() ) {
  }

}


} // namespace ou