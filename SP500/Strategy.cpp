/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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
 * File:    Strategy.cpp
 * Author:  raymond@burkholder.net
 * Project: SP500
 * Created: April 14, 2025 20:32:29
 */

#include <boost/log/trivial.hpp>

#include "Strategy.hpp"

Strategy::Strategy(
  ou::ChartDataView& cdv
, fConstructWatch_t&& fConstructWatch
, fConstructPosition_t&& fConstructPosition
, fStart_t&& fStart
, fStop_t&& fStop
)
: m_cdv( cdv )
, m_stateTrade( ETradeState::Init )
, m_fConstructWatch( std::move( fConstructWatch ) )
, m_fConstructPosition( std::move( fConstructPosition ) )
, m_fStart( std::move( fStart ) )
, m_fStop( std::move( fStop ) )
, m_ceShortEntry( ou::ChartEntryShape::EShape::Short, ou::Colour::Red )
, m_ceLongEntry( ou::ChartEntryShape::EShape::Long, ou::Colour::Blue )
, m_ceShortFill( ou::ChartEntryShape::EShape::FillShort, ou::Colour::Red )
, m_ceLongFill( ou::ChartEntryShape::EShape::FillLong, ou::Colour::Blue )
, m_ceShortExit( ou::ChartEntryShape::EShape::ShortStop, ou::Colour::Red )
, m_ceLongExit( ou::ChartEntryShape::EShape::LongStop, ou::Colour::Blue )
, m_bfQuotes01Sec(  1 )
, m_dblAdv {}, m_dblDec {}, m_dblAdvDecRatio {}
, m_dblMid {}
, m_dblEma13 {}, m_dblEma29 {}
, m_dblEma50 {}, m_dblEma200 {}
, m_stateTickHi( ETickHi::Neutral ), m_stateTickLo( ETickLo::Neutral )
, m_nEnterLong {}, m_nEnterShort {}
//, m_dblLastTrin {}
, m_dblTickJ {}, m_dblTickL {}//, m_dblTickLmt {}
{
  SetupChart();

  m_bfQuotes01Sec.SetOnBarComplete( MakeDelegate( this, &Strategy::HandleBarQuotes01Sec ) );

  m_fConstructPosition(
    "SPY",
    [this]( pPosition_t pPosition ){
      m_pPosition = pPosition;
      pWatch_t pWatch = pPosition->GetWatch();
      pWatch->OnQuote.Add( fastdelegate::MakeDelegate( this, &Strategy::HandleQuote ) );
      pWatch->OnTrade.Add( fastdelegate::MakeDelegate( this, &Strategy::HandleTrade ) );
      pWatch->StartWatch();
      Start();
    } );

  m_fConstructWatch(
    "JT6T.Z",
    [this]( pWatch_t pWatch ){
      m_pTickJ = pWatch;
      m_pTickJ->OnTrade.Add( fastdelegate::MakeDelegate( this, &Strategy::HandleTickJ ) );
      m_pTickJ->StartWatch();
      Start();
    } );

  m_fConstructWatch(
    "LI6N.Z",
    [this]( pWatch_t pWatch ){
      m_pTickL = pWatch;
      m_pTickL->OnTrade.Add( fastdelegate::MakeDelegate( this, &Strategy::HandleTickL ) );
      m_pTickL->StartWatch();
      Start();
    } );

  m_fConstructWatch(
    "II6A.Z", // advancers
    [this]( pWatch_t pWatch ){
      m_pAdv = pWatch;
      m_pAdv->OnTrade.Add( fastdelegate::MakeDelegate( this, &Strategy::HandleAdv ) );
      m_pAdv->StartWatch();
      Start();
    } );

  m_fConstructWatch(
    "II6D.Z", // decliners
    [this]( pWatch_t pWatch ){
      m_pDec = pWatch;
      m_pDec->OnTrade.Add( fastdelegate::MakeDelegate( this, &Strategy::HandleDec ) );
      m_pDec->StartWatch();
      Start();
    } );

  }

Strategy::~Strategy() {
  if ( m_pDec ) {
    m_pDec->StopWatch();
    m_pDec->OnTrade.Remove( fastdelegate::MakeDelegate( this, &Strategy::HandleDec ) );
  }
  if ( m_pAdv ) {
    m_pAdv->StopWatch();
    m_pAdv->OnTrade.Remove( fastdelegate::MakeDelegate( this, &Strategy::HandleAdv ) );
  }
  if ( m_pTickJ ) {
    m_pTickJ->StopWatch();
    m_pTickJ->OnTrade.Remove( fastdelegate::MakeDelegate( this, &Strategy::HandleTickJ ) );
  }
  if ( m_pTickL ) {
    m_pTickL->StopWatch();
    m_pTickL->OnTrade.Remove( fastdelegate::MakeDelegate( this, &Strategy::HandleTickL ) );
  }
  if ( m_pPosition ) {
    pWatch_t pWatch = m_pPosition->GetWatch();
    pWatch->StopWatch();
    pWatch->OnQuote.Remove( fastdelegate::MakeDelegate( this, &Strategy::HandleQuote ) );
    pWatch->OnTrade.Remove( fastdelegate::MakeDelegate( this, &Strategy::HandleTrade ) );
  }
  m_bfQuotes01Sec.SetOnBarComplete( nullptr );
  m_cdv.Clear();
}

void Strategy::Start() {
  bool bOkToStart( true );
  bOkToStart &= nullptr != m_pPosition.get();
  bOkToStart &= nullptr != m_pTickJ.get();
  bOkToStart &= nullptr != m_pTickL.get();
  bOkToStart &= nullptr != m_pAdv.get();
  bOkToStart &= nullptr != m_pDec.get();
  if ( bOkToStart ) {
    m_fStart();
  }
}

void Strategy::SetupChart() {

  static const ou::Colour::EColour c_colourEma200( ou::Colour::OrangeRed );
  static const ou::Colour::EColour c_colourEma50(  ou::Colour::DarkMagenta );
  static const ou::Colour::EColour c_colourEma29(  ou::Colour::DarkTurquoise );
  static const ou::Colour::EColour c_colourEma13(  ou::Colour::Purple );
  static const ou::Colour::EColour c_colourPrice(  ou::Colour::DarkGreen );

  m_cemPosOne.AddMark( +1.0, ou::Colour::Black,   "+1" );
    m_cemZero.AddMark(  0.0, ou::Colour::Black, "zero" );
  m_cemNegOne.AddMark( -1.0, ou::Colour::Black,   "-1" );

  m_ceTrade.SetName( "Trade" );
  m_ceTrade.SetColour( c_colourPrice );
  m_cdv.Add( EChartSlot::Price, &m_ceTrade );

  m_ceEma13.SetName( "13s ema" );
  m_ceEma13.SetColour( c_colourEma13 );
  m_cdv.Add( EChartSlot::Price, &m_ceEma13 );

  m_ceEma29.SetName( "29s ema" );
  m_ceEma29.SetColour( c_colourEma29 );
  m_cdv.Add( EChartSlot::Price, &m_ceEma29 );

  m_ceEma50.SetName( "50s ema" );
  m_ceEma50.SetColour( c_colourEma50 );
  m_cdv.Add( EChartSlot::Price, &m_ceEma50 );

  m_ceEma200.SetName( "200s ema" );
  m_ceEma200.SetColour( c_colourEma200 );
  m_cdv.Add( EChartSlot::Price, &m_ceEma200 );

  m_cdv.Add( EChartSlot::Ratio, &m_cemZero );

  m_ceTrade_ratio.SetName( "Trade" );
  m_ceTrade_ratio.SetColour( c_colourPrice );
  m_cdv.Add( EChartSlot::Ratio, &m_ceTrade_ratio );

  m_ceEma13_ratio.SetName( "13s ema" );
  m_ceEma13_ratio.SetColour( c_colourEma13 );
  m_cdv.Add( EChartSlot::Ratio, &m_ceEma13_ratio );

  m_ceEma29_ratio.SetName( "29s ema" );
  m_ceEma29_ratio.SetColour( c_colourEma29 );
  m_cdv.Add( EChartSlot::Ratio, &m_ceEma29_ratio );

  m_ceEma50_ratio.SetName( "50s ema" );
  m_ceEma50_ratio.SetColour( c_colourEma50 );
  m_cdv.Add( EChartSlot::Ratio, &m_ceEma50_ratio );

  m_ceEma200_ratio.SetName( "200s ema" );
  m_ceEma200_ratio.SetColour( c_colourEma200 );
  m_cdv.Add( EChartSlot::Ratio, &m_ceEma200_ratio );

  //m_ceAsk.SetName( "Ask" );
  //m_ceAsk.SetColour( ou::Colour::Red );
  //m_cdv.Add( EChartSlot::Price, &m_ceAsk );

  //m_ceBid.SetName( "Bid" );
  //m_ceBid.SetColour( ou::Colour::Blue );
  //m_cdv.Add( EChartSlot::Price, &m_ceBid );

  m_ceVolume.SetName( "Volume" );
  m_cdv.Add( EChartSlot::Volume, &m_ceVolume );

  m_cdv.Add( EChartSlot::Tick, &m_cemZero );

  m_ceTickJ.SetName( "TickJ" );
  m_ceTickJ.SetColour( ou::Colour::Blue );
  m_cdv.Add( EChartSlot::Tick, &m_ceTickJ );

  m_ceTickL.SetName( "TickL" );
  m_ceTickL.SetColour( ou::Colour::DarkOrange );
  m_cdv.Add( EChartSlot::Tick, &m_ceTickL );
  //m_ceTickDiff.SetName( "Diff" );
  //m_ceTickDiff.SetColour( ou::Colour::Magenta );
  //m_cdv.Add( EChartSlot::Tick, &m_ceTickDiff );

  m_ceAdvDec.SetName( "AdvDec" );
  m_cdv.Add( EChartSlot::AdvDec, &m_cemZero );
  m_cdv.Add( EChartSlot::AdvDec, &m_ceAdvDec );

  m_ceProfitLoss.SetName( "P/L" );
  //m_cdv.Add( EChartSlot::PL, &m_cemZero );
  m_cdv.Add( EChartSlot::PL, &m_ceProfitLoss );

  m_cdv.Add( EChartSlot::Price, &m_ceLongEntry );
  m_cdv.Add( EChartSlot::Price, &m_ceLongFill );
  m_cdv.Add( EChartSlot::Price, &m_ceLongExit );
  m_cdv.Add( EChartSlot::Price, &m_ceShortEntry );
  m_cdv.Add( EChartSlot::Price, &m_ceShortFill );
  m_cdv.Add( EChartSlot::Price, &m_ceShortExit );
}

void Strategy::HandleQuote( const ou::tf::Quote& quote ) {
  m_quote = quote;
  //const auto dt( quote.DateTime() );
  //m_ceAsk.Append( dt, quote.Ask() );
  //m_ceBid.Append( dt, quote.Bid() );
  m_bfQuotes01Sec.Add( m_quote.DateTime(), m_quote.Midpoint(), 1 ); // provides a 1 sec pulse for checking the algorithm
}

void Strategy::HandleTrade( const ou::tf::Trade& trade ) {
  m_trade = trade;
  m_ceTrade.Append( trade.DateTime(), trade.Price() );
  m_ceVolume.Append( trade.DateTime(), trade.Volume() );
  TimeTick( trade );
}

void Strategy::HandleTickJ( const ou::tf::Trade& tick ) {
  const auto dt( tick.DateTime() );
  m_dblTickJ = tick.Price();
  m_ceTickJ.Append( dt, m_dblTickJ / 100.0 );
  //m_ceTickDiff.Append( dt, m_dblTickL - m_dblTickJ );
}

void Strategy::HandleTickL( const ou::tf::Trade& tick ) {
  const auto dt( tick.DateTime() );
  m_dblTickL = tick.Price();
  m_ceTickL.Append( dt, m_dblTickL / 200.0 );
  //m_ceTickDiff.Append( dt, m_dblTickL - m_dblTickJ );

  static const double hi( +200.0 );

  switch ( m_stateTickHi ) {
    case ETickHi::Neutral:
      if ( hi <= m_dblTickL ) {
        m_stateTickHi = ETickHi::UpOvr;
      }
      break;
    case ETickHi::UpOvr:
    case ETickHi::Up:
      m_stateTickHi = ( hi < m_dblTickL ) ? ETickHi::Up : ETickHi::DnOvr;
      break;
    case ETickHi::DnOvr:
      m_stateTickHi = ( hi > m_dblTickL ) ? ETickHi::Neutral : ETickHi::UpOvr;
      break;
  }

  static const double lo( -200.0 );

  switch ( m_stateTickLo ) {
    case ETickLo::Neutral:
      if ( lo >= m_dblTickL ) {
        m_stateTickLo = ETickLo::DnOvr;
      }
      break;
    case ETickLo::DnOvr:
    case ETickLo::Dn:
      m_stateTickLo = ( lo > m_dblTickL ) ? ETickLo::Dn : ETickLo::UpOvr;
      break;
    case ETickLo::UpOvr:
      m_stateTickLo = ( lo < m_dblTickL ) ? ETickLo::Neutral : ETickLo::DnOvr;
      break;
  }

}

void Strategy::HandleAdv( const ou::tf::Trade& tick ) {
  m_dblAdv = tick.Price();
  CalcAdvDec( tick.DateTime() );
}

void Strategy::HandleDec( const ou::tf::Trade& tick ) {
  m_dblDec = tick.Price();
  CalcAdvDec( tick.DateTime() );
}

void Strategy::CalcAdvDec( boost::posix_time::ptime dt ) {
  const double sum( m_dblAdv + m_dblDec );
  const double diff( m_dblAdv - m_dblDec );
  const double ratio( diff / sum );
  m_dblAdvDecRatio = ratio;
  m_ceAdvDec.Append( dt, ratio );
}

void Strategy::HandleBarQuotes01Sec( const ou::tf::Bar& bar ) {

  auto f// turn into template to facilitate constexpr
    = []( double seconds, const ou::tf::Bar& bar, double& ema, ou::ChartEntryIndicator& cei )-> void{
      const double cur( 1.0 / seconds );
      const double prv( 1.0 - cur );
      if ( 0.0 == ema ) {
        ema = bar.Close();
      }
      else {
        ema = prv * ema + cur * bar.Close();
      }
      cei.Append( bar.DateTime(), ema );
    };
  f(  13, bar, m_dblEma13, m_ceEma13 );
  f(  29, bar, m_dblEma29, m_ceEma29 );
  f(  50, bar, m_dblEma50, m_ceEma50 );
  f( 200, bar, m_dblEma200, m_ceEma200 );

  const rValues_t r = { m_dblEma200, m_dblEma50, m_dblEma29, m_dblEma13, m_trade.Price(), m_dblTickJ, m_dblTickL, m_dblAdvDecRatio };

  struct maxmin {
    double& max;
    double& min;
    maxmin( double& max_, double& min_, double init )
    : max( max_ ), min( min_ ) {
      max = min = init;
    }
    void test( const double value ) {
      if ( max < value ) max = value;
      if ( min > value ) min = value;
    }
  };

  double max;
  double min;
  maxmin mm( max, min, m_dblEma200 );
  mm.test( m_dblEma50 );
  mm.test( m_dblEma29 );
  mm.test( m_dblEma13 );
  // on purpose: no test on price

  if ( max > min ) {

    const double range( max - min );

    const double ratioEma200 = 2.0 * ( ( m_dblEma200 - min ) / range ) - 1.0; // zero other values to this - detrend timeseries
    const double ratioEma50  = 2.0 * ( ( m_dblEma50 - min ) / range ) - 1.0;
    const double ratioEma29  = 2.0 * ( ( m_dblEma29 - min ) / range ) - 1.0;
    const double ratioEma13  = 2.0 * ( ( m_dblEma13 - min ) / range ) - 1.0;
    const double ratioPrice  = 2.0 * ( ( m_trade.Price() - min ) / range ) - 1.0;

    // todo: assign to indicators for visual verification
    // todo: apply sigmoid/tanh to values to scale for lstm use

    m_ceEma200_ratio.Append( bar.DateTime(), ratioEma200 );
    m_ceEma50_ratio.Append( bar.DateTime(), ratioEma50 );
    m_ceEma29_ratio.Append( bar.DateTime(), ratioEma29 );
    m_ceEma13_ratio.Append( bar.DateTime(), ratioEma13 );
    m_ceTrade_ratio.Append( bar.DateTime(), ratioPrice );

  }



  m_rDataRaw.emplace_back( r );

  TimeTick( bar );
}

void Strategy::HandleRHTrading( const ou::tf::Trade& trade ) {
  const auto dt( trade.DateTime() );
  const auto price( trade.Price() );
  switch ( m_stateTrade ) {
    case ETradeState::Search:
      if ( ETickHi::DnOvr == m_stateTickHi ) {
        BOOST_LOG_TRIVIAL(trace) << "ETickHi::DnOvr enter";
        m_ceShortEntry.AddLabel( dt, price, "short" );
        ++m_nEnterShort;
        m_stateTrade = ETradeState::ShortSubmitted;
      }
      else {
        if ( ETickLo::UpOvr == m_stateTickLo ) {
          BOOST_LOG_TRIVIAL(trace) << "ETickLo::UpOvr enter";
          m_ceLongEntry.AddLabel( dt, price, "long" );
          ++m_nEnterLong;
          m_stateTrade = ETradeState::LongSubmitted;
        }
      }
      break;
    case ETradeState::LongSubmitted:
      if ( ETickLo::Neutral == m_stateTickLo ) {
        BOOST_LOG_TRIVIAL(trace) << "ETickLo::Neutral exit";
        m_stateTrade = ETradeState::Search;
      }
      break;
    case ETradeState::ShortSubmitted:
      if ( ETickHi::Neutral == m_stateTickHi ) {
        BOOST_LOG_TRIVIAL(trace) << "ETickHi::Neutral exit";
        m_stateTrade = ETradeState::Search;
      }
      break;
    case ETradeState::LongExit:
      break;
    case ETradeState::ShortExit:
      break;
    case ETradeState::LongExitSubmitted:
      break;
    case ETradeState::ShortExitSubmitted:
      break;
    case ETradeState::Neutral:
      BOOST_LOG_TRIVIAL(trace) << "neutral," << m_nEnterLong << ',' << m_nEnterShort;
      break;
    case ETradeState::EndOfDayCancel:
      BOOST_LOG_TRIVIAL(trace) << "eod cancel," << m_nEnterLong << ',' << m_nEnterShort;
      break;
    case ETradeState::EndOfDayNeutral:
      BOOST_LOG_TRIVIAL(trace) << "eod neutral," << m_nEnterLong << ',' << m_nEnterShort;
      break;
    case ETradeState::Done:
      break;
    case ETradeState::Init:
      m_stateTrade = ETradeState::Search;
      break;
  }
}

void Strategy::HandleRHTrading( const ou::tf::Bar& bar ) { // once a second
  double dblUnRealized, dblRealized, dblCommissionsPaid, dblTotal;
  m_pPosition->QueryStats( dblUnRealized, dblRealized, dblCommissionsPaid, dblTotal );
  m_ceProfitLoss.Append( bar.DateTime(), dblTotal );
}

void Strategy::HandleOrderCancelled( const ou::tf::Order& order ) {
  m_pOrder->OnOrderCancelled.Remove( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  m_pOrder->OnOrderFilled.Remove( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  switch ( m_stateTrade ) {
    case ETradeState::EndOfDayCancel:
    case ETradeState::EndOfDayNeutral:
      BOOST_LOG_TRIVIAL(info) << "order " << order.GetOrderId() << " cancelled - end of day";
      break;
    case ETradeState::LongExitSubmitted:
    case ETradeState::ShortExitSubmitted:
      //assert( false );  // TODO: need to figure out a plan to retry exit
      BOOST_LOG_TRIVIAL(warning) << "order " << order.GetOrderId() << " cancelled - state machine needs fixes";
      break;
    default:
      //m_stateTrade = ETradeState::Search;
      m_stateTrade = ETradeState::Neutral;
  }
  m_pOrder.reset();
}

void Strategy::HandleOrderFilled( const ou::tf::Order& order ) {
  m_pOrder->OnOrderCancelled.Remove( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  m_pOrder->OnOrderFilled.Remove( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  switch ( m_stateTrade ) {
    case ETradeState::LongSubmitted:
      m_ceLongFill.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "Long Fill" );
      m_stateTrade = ETradeState::LongExit;
      break;
    case ETradeState::ShortSubmitted:
      m_ceShortFill.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "Short Fill" );
      m_stateTrade = ETradeState::ShortExit;
      break;
    case ETradeState::LongExitSubmitted:
      m_ceShortFill.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "Long Exit Fill" );
      //m_stateTrade = ETradeState::Search;
      m_stateTrade = ETradeState::Neutral;
      break;
    case ETradeState::ShortExitSubmitted:
      m_ceLongFill.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "Short Exit Fill" );
      //m_stateTrade = ETradeState::Search;
      m_stateTrade = ETradeState::Neutral;
      break;
    case ETradeState::EndOfDayCancel:
    case ETradeState::EndOfDayNeutral:
      // figure out what labels to apply
      break;
    case ETradeState::Done:
      break;
    default:
      assert( false ); // TODO: unravel the state mess if we get here
  }
  m_pOrder.reset();
}

void Strategy::HandleCancel( boost::gregorian::date, boost::posix_time::time_duration td ) { // one shot
  BOOST_LOG_TRIVIAL(trace) << "event HandleCancel," << td << ',' << m_nEnterLong << ',' << m_nEnterShort;
  m_stateTrade = ETradeState::EndOfDayCancel;
  if ( m_pPosition ) {
    m_pPosition->CancelOrders();
  }
}

void Strategy::HandleGoNeutral( boost::gregorian::date, boost::posix_time::time_duration td ) { // one shot
  BOOST_LOG_TRIVIAL(trace) << "event HandleGoNeutral," << td << ',' << m_nEnterLong << ',' << m_nEnterShort;
  m_stateTrade = ETradeState::EndOfDayNeutral;
  if ( m_pPosition ) {
    m_pPosition->ClosePosition();
  }
}

