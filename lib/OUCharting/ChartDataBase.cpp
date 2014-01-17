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
// .91 1.5 2.4  3.9, 6.3, 10.2 16.5 26.6  54.0  69.7  112.8

ChartDataBase::ChartDataBase(void) 
  : m_dvChart(),
  m_bfTrades( 10 ),
  m_bfBuys( 10 ),
  m_bfSells( 10 ),
  m_bFirstTrade( true ),
  m_rtTickDiffs( m_pricesTickDiffs, seconds( 120 ) ),
  m_rocTickDiffs( m_pricesTickDiffsROC, seconds( 30 ) ),
  m_dblUpTicks( 0.0 ), m_dblMdTicks( 0.0 ), m_dblDnTicks( 0.0 ),
  m_dblUpVolume( 0.0 ), m_dblMdVolume( 0.0 ), m_dblDnVolume( 0.0 )
{

  m_vInfoBollinger.push_back( infoBollinger( m_quotes, boost::posix_time::seconds(  144 ) ) );
  m_vInfoBollinger.push_back( infoBollinger( m_quotes, boost::posix_time::seconds(  377 ) ) );
  m_vInfoBollinger.push_back( infoBollinger( m_quotes, boost::posix_time::seconds(  987 ) ) );
  m_vInfoBollinger.push_back( infoBollinger( m_quotes, boost::posix_time::seconds( 2584 ) ) );

/*
  m_quotes.Reserve( 500000 );
  m_trades.Reserve( 200000 );
  m_ceBars.Reserve( 50000 );
  m_rVolumes[ VDn ].Reserve( 50000 );
  m_rVolumes[ VUp ].Reserve( 50000 );
  m_ceTrade.Reserve( 200000 );
  m_ceQuoteUpper.Reserve( 500000 );
  m_ceQuoteLower.Reserve( 500000 );
  m_ceQuoteSpread.Reserve( 500000 );
  m_ceEma1.Reserve( 200000 );
  m_ceEma2.Reserve( 200000 );
  m_ceEma3.Reserve( 200000 );
  m_ceEma4.Reserve( 200000 );
  m_ceUpperBollinger1.Reserve( 200000 );
  m_ceLowerBollinger1.Reserve( 200000 );
  m_ceUpperBollinger2.Reserve( 200000 );
  m_ceLowerBollinger2.Reserve( 200000 );
  m_ceUpperBollinger3.Reserve( 200000 );
  m_ceLowerBollinger3.Reserve( 200000 );
  m_ceUpperBollinger4.Reserve( 200000 );
  m_ceLowerBollinger4.Reserve( 200000 );
  */
  m_dvChart.Add( 0, &m_ceQuoteUpper );
  m_dvChart.Add( 0, &m_ceQuoteLower );
  m_dvChart.Add( 0, &m_ceTrade );
  m_dvChart.Add( 2, &m_ceQuoteSpread );

  for ( int ix = 0; ix <= 3; ++ix ) {
    m_dvChart.Add( 0, &m_vInfoBollinger[ix].m_ceEma );
    m_dvChart.Add( 0, &m_vInfoBollinger[ix].m_ceUpperBollinger );
    m_dvChart.Add( 0, &m_vInfoBollinger[ix].m_ceLowerBollinger );
  }

  m_dvChart.Add( 0, &m_ceBars );

  m_dvChart.Add( 1, &m_rVolumes[ VUp ].ceVolumeUp );
  m_dvChart.Add( 1, &m_rVolumes[ VUp ].ceVolumeNeutral );
  m_dvChart.Add( 1, &m_rVolumes[ VUp ].ceVolumeDn );
  m_dvChart.Add( 1, &m_rVolumes[ VDn ].ceVolumeUp );
  m_dvChart.Add( 1, &m_rVolumes[ VDn ].ceVolumeNeutral );
  m_dvChart.Add( 1, &m_rVolumes[ VDn ].ceVolumeDn );

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

  m_ceQuoteSpread.SetName( "Spread" );

  m_rVolumes[ VUp ].ceVolumeUp.SetColour( ou::Colour::Green );
  m_rVolumes[ VUp ].ceVolumeNeutral.SetColour( ou::Colour::Yellow );
  m_rVolumes[ VUp ].ceVolumeDn.SetColour( ou::Colour::Red );
  m_rVolumes[ VDn ].ceVolumeUp.SetColour( ou::Colour::Green );
  m_rVolumes[ VDn ].ceVolumeNeutral.SetColour( ou::Colour::Yellow );
  m_rVolumes[ VDn ].ceVolumeDn.SetColour( ou::Colour::Red );

  {
    infoBollinger& ib( m_vInfoBollinger[0] );
    ib.m_ceEma.SetColour( ou::Colour::DarkOliveGreen );
    ib.m_ceUpperBollinger.SetColour( ou::Colour::DarkOliveGreen );
    ib.m_ceLowerBollinger.SetColour( ou::Colour::DarkOliveGreen );
    ib.m_ceEma.SetName( "Bollinger1 - 2.4" );
  }

  {
    infoBollinger& ib( m_vInfoBollinger[1] );
    ib.m_ceEma.SetColour( ou::Colour::Turquoise );
    ib.m_ceUpperBollinger.SetColour( ou::Colour::Turquoise );
    ib.m_ceLowerBollinger.SetColour( ou::Colour::Turquoise );
    ib.m_ceEma.SetName( "Bollinger2 - 6.3" );
  }

  {
    infoBollinger& ib( m_vInfoBollinger[2] );
    ib.m_ceEma.SetColour( ou::Colour::GreenYellow );
    ib.m_ceUpperBollinger.SetColour( ou::Colour::GreenYellow );
    ib.m_ceLowerBollinger.SetColour( ou::Colour::GreenYellow );
    ib.m_ceEma.SetName( "Bollinger3 - 16.5" );
  }

  {
    infoBollinger& ib( m_vInfoBollinger[3] );
    ib.m_ceEma.SetColour( ou::Colour::MediumSlateBlue );
    ib.m_ceUpperBollinger.SetColour( ou::Colour::MediumSlateBlue );
    ib.m_ceLowerBollinger.SetColour( ou::Colour::MediumSlateBlue );
    ib.m_ceEma.SetName( "Bollinger4 - 54.0" );
  }

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

  for ( int ix = 0; ix <= 3; ++ix ) {
    double lastEma, sd;
    infoBollinger& ib( m_vInfoBollinger[ix] );
    ib.m_ceEma.Append( dt, ib.m_ema.Ago( 0 ).Value() );
    lastEma = ib.m_ema.Ago( 0 ).Value();
    sd = 2.0 * ib.m_stats.SD();
    ib.m_ceUpperBollinger.Append( dt, lastEma + sd );
    ib.m_ceLowerBollinger.Append( dt, lastEma - sd );
  }

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