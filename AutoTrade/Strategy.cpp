/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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
 * Project: AutoTrade
 * Created: February 14, 2022 10:59
 */

#include <boost/log/trivial.hpp>

#include <TFBitsNPieces/Stochastic.hpp>

#include "Config.hpp"
#include "Strategy.hpp"
#include "Strategy_impl.hpp"

namespace {
  static const int k_hi = 80;
  static const int k_lo = 20;
  static const double c_zigzag_hi = 96.0;
  static const double c_zigzag_lo =  4.0;
}

Strategy::Strategy( ou::ChartDataView& cdv, const config::Options& options )
: ou::tf::DailyTradeTimeFrame<Strategy>()
, m_cdv( cdv )
, m_ceShortEntry( ou::ChartEntryShape::EShape::Short, ou::Colour::Red )
, m_ceLongEntry( ou::ChartEntryShape::EShape::Long, ou::Colour::Blue )
, m_ceShortFill( ou::ChartEntryShape::EShape::FillShort, ou::Colour::Red )
, m_ceLongFill( ou::ChartEntryShape::EShape::FillLong, ou::Colour::Blue )
, m_ceShortExit( ou::ChartEntryShape::EShape::ShortStop, ou::Colour::Red )
, m_ceLongExit( ou::ChartEntryShape::EShape::LongStop, ou::Colour::Blue )
, m_bfQuotes01Sec(  1 )
, m_stateTrade( ETradeState::Init )
, m_dblMid {}, m_dblLastTick {}, m_dblLastTrin {}
, m_dblStochastic {}
, m_eZigZag( EZigZag::EndPoint1 ), m_dblEndPoint1( 0.0 ), m_dblEndPoint2( 0.0 ), m_dblZigZagDistance( 0.0 ), m_nZigZagLegs( 0 )
{

  assert( 0 < options.nPeriodWidth );

  m_nPeriodWidth = options.nPeriodWidth;
  m_vMAPeriods.push_back( options.nMA1Periods );
  m_vMAPeriods.push_back( options.nMA2Periods );
  m_vMAPeriods.push_back( options.nMA3Periods );

  assert( 3 == m_vMAPeriods.size() );
  for ( vMAPeriods_t::value_type value: m_vMAPeriods ) {
    assert( 0 < value );
  }

  m_cemZero.AddMark( 0.0, ou::Colour::Black,  "0" );
  m_cemOne.AddMark(  1.0, ou::Colour::Black,  "1" );

  m_nStochasticPeriods = options.nStochasticPeriods;

  m_cemStochastic.AddMark(  100, ou::Colour::Black,    "" );
  m_cemStochastic.AddMark( k_hi, ou::Colour::Red,   boost::lexical_cast<std::string>( k_hi ) + "%" );
  m_cemStochastic.AddMark(   50, ou::Colour::Green, "50%" );
  m_cemStochastic.AddMark( k_lo, ou::Colour::Blue,  boost::lexical_cast<std::string>( k_lo ) + "%" );
  m_cemStochastic.AddMark(    0, ou::Colour::Black,    "" );

  m_ceQuoteAsk.SetColour( ou::Colour::Red );
  m_ceQuoteBid.SetColour( ou::Colour::Blue );
  m_ceTrade.SetColour( ou::Colour::DarkGreen );

  m_ceZigZag.SetColour( ou::Colour::Purple );

  m_ceQuoteAsk.SetName( "Ask" );
  m_ceTrade.SetName( "Tick" );
  m_ceQuoteBid.SetName( "Bid" );

  m_ceVolume.SetName( "Volume" );

  m_ceProfitLoss.SetName( "P/L" );

  m_ceZigZag.SetName( "ZigZag" );

  m_bfQuotes01Sec.SetOnBarComplete( MakeDelegate( this, &Strategy::HandleBarQuotes01Sec ) );

  m_pStrategy_impl = std::make_unique<Strategy_impl>();
}

Strategy::~Strategy() {
  Clear();
}

void Strategy::SetupChart() {

  m_cdv.Add( EChartSlot::Price, &m_ceQuoteAsk );
  m_cdv.Add( EChartSlot::Price, &m_ceTrade );
  m_cdv.Add( EChartSlot::Price, &m_ceQuoteBid );

  m_cdv.Add( EChartSlot::Price, &m_ceLongEntry );
  m_cdv.Add( EChartSlot::Price, &m_ceLongFill );
  m_cdv.Add( EChartSlot::Price, &m_ceLongExit );
  m_cdv.Add( EChartSlot::Price, &m_ceShortEntry );
  m_cdv.Add( EChartSlot::Price, &m_ceShortFill );
  m_cdv.Add( EChartSlot::Price, &m_ceShortExit );

  m_cdv.Add( EChartSlot::Price, &m_ceZigZag );

  m_cdv.Add( EChartSlot::Volume, &m_ceVolume );

  m_cdv.Add( EChartSlot::Stoch, &m_cemStochastic );

  m_cdv.Add( EChartSlot::PL, &m_ceProfitLoss );

}

void Strategy::SetPosition( pPosition_t pPosition ) {

  assert( pPosition );

  Clear();

  m_pPosition = std::move( pPosition );
  pWatch_t pWatch = m_pPosition->GetWatch();

  m_cdv.SetNames( "Moving Average Strategy", pWatch->GetInstrument()->GetInstrumentName() );

  const time_duration td = time_duration( 0, 0, m_nPeriodWidth );

  m_pStochastic = std::make_unique<Stochastic>( "", pWatch->GetQuotes(), m_nStochasticPeriods, td, ou::Colour::DeepSkyBlue );
  m_pStochastic->AddToView( m_cdv , EChartSlot::Price, EChartSlot::Stoch );

  SetupChart();

  m_vMA.emplace_back( MA( pWatch->GetQuotes(), m_vMAPeriods[0], td, ou::Colour::Gold,  "ma1" ) );
  m_vMA.emplace_back( MA( pWatch->GetQuotes(), m_vMAPeriods[1], td, ou::Colour::Coral, "ma2" ) );
  m_vMA.emplace_back( MA( pWatch->GetQuotes(), m_vMAPeriods[2], td, ou::Colour::Brown, "ma3" ) );

  for ( vMA_t::value_type& ma: m_vMA ) {
    ma.AddToView( m_cdv );
  }

  pWatch->OnQuote.Add( MakeDelegate( this, &Strategy::HandleQuote ) );
  pWatch->OnTrade.Add( MakeDelegate( this, &Strategy::HandleTrade ) );

}

void Strategy::SetTick( pWatch_t pTick ) {
  assert( pTick );
  m_pTick = std::move( pTick );

  m_cdv.Add( EChartSlot::Tick, &m_cemZero );

  m_ceTick.SetName( "Tick" );
  m_cdv.Add( EChartSlot::Tick, &m_ceTick );

  m_pTick->OnTrade.Add( MakeDelegate( this, &Strategy::HandleTick ) );
  m_pTick->StartWatch();
}

void Strategy::SetTrin( pWatch_t pTrin ) {
  assert( pTrin );
  m_pTrin = std::move( pTrin );

  m_cdv.Add( EChartSlot::Trin, &m_cemOne );

  m_ceTrin.SetName( "Trin" );
  m_cdv.Add( EChartSlot::Trin, &m_ceTrin );

  m_pTrin->OnTrade.Add( MakeDelegate( this, &Strategy::HandleTrin ) );
  m_pTrin->StartWatch();
}

void Strategy::Clear() {
  if ( m_pTick ) {
    m_pTick->StopWatch();
    m_pTick->OnTrade.Remove( MakeDelegate( this, &Strategy::HandleTick ) );
    m_pTick.reset();
  }
  if ( m_pTrin ) {
    m_pTrin->StopWatch();
    m_pTrin->OnTrade.Remove( MakeDelegate( this, &Strategy::HandleTrin ) );
    m_pTrin.reset();
  }
  if  ( m_pPosition ) {
    pWatch_t pWatch = m_pPosition->GetWatch();
    pWatch->OnQuote.Remove( MakeDelegate( this, &Strategy::HandleQuote ) );
    pWatch->OnTrade.Remove( MakeDelegate( this, &Strategy::HandleTrade ) );
    m_cdv.Clear();
    m_vMA.clear();
    m_pPosition.reset();
  }
}

void Strategy::HandleQuote( const ou::tf::Quote& quote ) {
  // position has the quotes via the embedded watch
  // indicators are also attached to the embedded watch

  if ( quote.IsNonZero() ) {

    m_quote = quote;

    ptime dt( quote.DateTime() );

    m_ceQuoteAsk.Append( dt, quote.Ask() );
    m_ceQuoteBid.Append( dt, quote.Bid() );

    m_dblMid = quote.Midpoint();

    for ( vMA_t::value_type& ma: m_vMA ) {
      ma.Update( dt );
    }

    m_bfQuotes01Sec.Add( dt, m_dblMid, 1 ); // provides a 1 sec pulse for checking the alogorithm

  }
}

void Strategy::HandleTrade( const ou::tf::Trade& trade ) {

  ptime dt( trade.DateTime() );

  m_ceTrade.Append( dt, trade.Price() );
  m_ceVolume.Append( dt, trade.Volume() );

}

void Strategy::HandleTick( const ou::tf::Trade& trade ) {

  const ou::tf::Price::price_t tick = trade.Price();
  m_ceTick.Append( trade.DateTime(), tick );

  m_pStrategy_impl->Queue( NeuralNet::Input( m_dblStochastic, tick ) );

  if ( false ) {
    switch ( m_stateTrade ) {
      case ETradeState::Search:
        if ( 100.0 < tick ) {
          m_dblStopAbsolute = m_quote.Ask() - 0.06;
          EnterShort( trade.DateTime(), m_quote.Ask() );
          m_stateTrade = ETradeState::ShortSubmitted;
        }
        else {
          if ( -100.0 > tick ) {
            m_dblStopAbsolute = m_quote.Bid() + 0.06;
            EnterLong( trade.DateTime(), m_quote.Bid() );
            m_stateTrade = ETradeState::LongSubmitted;
          }
        }
        break;
      case ETradeState::LongExit:
        ExitLong( trade.DateTime(), m_dblStopAbsolute );
        m_stateTrade = ETradeState::LongExitSubmitted;
        break;
      case ETradeState::ShortExit:
        ExitShort( trade.DateTime(), m_dblStopAbsolute );
        m_stateTrade = ETradeState::ShortExitSubmitted;
        break;
      case ETradeState::Neutral:
        if ( ( 100.0 > tick ) && ( -100.0 < tick ) ) {
          m_stateTrade = ETradeState::Search;
        }
        break;
      case ETradeState::Init:
        m_stateTrade = ETradeState::Neutral;
        break;
      default:
        break;
    }
  }
  m_dblLastTick = tick;
}

void Strategy::HandleTrin( const ou::tf::Trade& trin ) {
  m_dblLastTrin = trin.Price();
  m_ceTrin.Append( trin.DateTime(), m_dblLastTrin );
}

/*
  https://www.investopedia.com/terms/a/arms.asp
  * If AD Volume creates a higher ratio than the AD Ratio, TRIN will be below one.
  * If AD Volume has a lower ratio than AD Ratio, TRIN will be above one.
  * A TRIN reading below one typically accompanies a strong price advance,
       since the strong volume in the rising stocks helps fuel the rally.
  * A TRIN reading above one typically accompanies a strong price decline,
       since the strong volume in the decliners helps fuel the selloff.
  * The Arms Index moves opposite the price trajectory of the Index.
    As discussed above, a strong price rally will see TRIN move to lower levels.
    A falling index will see TRIN push higher.
  * The market is said to be in a neutral state when the index equals 1.0,
  * A value that exceeds 3.00 indicates an oversold market and that bearish sentiment is too dramatic.
      This could mean an upward reversal in prices/index is coming.
  * a TRIN value that dips below 0.50 may indicate an overbought market
      and that bullish sentiment is overheating.
*/

void Strategy::HandleBarQuotes01Sec( const ou::tf::Bar& bar ) {

  double dblUnRealized, dblRealized, dblCommissionsPaid, dblTotal;

  m_pPosition->QueryStats( dblUnRealized, dblRealized, dblCommissionsPaid, dblTotal );
  m_ceProfitLoss.Append( bar.DateTime(), dblTotal );

  m_dblStochastic = m_pStochastic->Latest();
  switch ( m_eZigZag ) {
    case EZigZag::EndPoint1:
      m_dblEndPoint1 = m_quote.Midpoint();
      m_eZigZag = EZigZag::EndPoint2;
      break;
    case EZigZag::EndPoint2:
      if ( c_zigzag_hi < m_dblStochastic ) {
        m_dtZigZag = m_quote.DateTime();
        m_dblEndPoint2 = m_quote.Bid();
        m_pStrategy_impl->Submit( NeuralNet::Output( 1.0, 0.0, 0.0 ) );
        m_eZigZag = EZigZag::HighFound;
      }
      else {
        if ( c_zigzag_lo > m_dblStochastic ) {
          m_dtZigZag = m_quote.DateTime();
          m_dblEndPoint2 = m_quote.Ask();
          m_pStrategy_impl->Submit( NeuralNet::Output( 0.0, 0.0, 1.0 ) );
          m_eZigZag = EZigZag::LowFound;
        }
      }
      break;
    case EZigZag::HighFound:
      if ( c_zigzag_hi < m_dblStochastic ) {
        double bid = m_quote.Bid();
        if ( m_dblEndPoint2 < bid ) {
          m_dtZigZag = m_quote.DateTime();
          m_dblEndPoint2 = bid;
          m_pStrategy_impl->Submit( NeuralNet::Output( 1.0, 0.0, 0.0 ) );
        }
      }
      else {
        if ( c_zigzag_lo > m_dblStochastic ) {
          m_nZigZagLegs++;
          m_dblZigZagDistance += m_dblEndPoint2 - m_dblEndPoint1; // endpoint 2 higher than endpoint 1
          m_ceZigZag.Append( m_dtZigZag, m_dblEndPoint2 );
          m_dblEndPoint1 = m_dblEndPoint2;
          m_dblEndPoint2 = m_quote.Bid();
          m_pStrategy_impl->Submit( NeuralNet::Output( 0.0, 0.0, 1.0 ) );
          m_eZigZag = EZigZag::LowFound;
        }
      }
      break;
    case EZigZag::LowFound:
      if ( c_zigzag_lo > m_dblStochastic ) {
        double ask = m_quote.Ask();
        if ( m_dblEndPoint2 > ask ) {
          m_dtZigZag = m_quote.DateTime();
          m_dblEndPoint2 = ask;
          m_pStrategy_impl->Submit( NeuralNet::Output( 0.0, 0.0, 1.0 ) );
        }
      }
      else {
        if ( c_zigzag_hi < m_dblStochastic ) {
          m_nZigZagLegs++;
          m_dblZigZagDistance += m_dblEndPoint1 - m_dblEndPoint2; // endpoint 1 higher than endpoint 2
          m_ceZigZag.Append( m_dtZigZag, m_dblEndPoint2 );
          m_dblEndPoint1 = m_dblEndPoint2;
          m_dblEndPoint2 = m_quote.Ask();
          m_pStrategy_impl->Submit( NeuralNet::Output( 1.0, 0.0, 0.0 ) );
          m_eZigZag = EZigZag::HighFound;
        }
      }
      break;
  }

  TimeTick( bar );
}

/*
  // template to submit GTC limit order
  // strip off fractional seconds
  boost::posix_time::ptime dtQuote
    = quote.DateTime()
    - boost::posix_time::time_duration( 0, 0, 0, quote.DateTime( ).time_of_day().fractional_seconds() );

  m_pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Limit, ou::tf::OrderSide::Buy, quantity, price );

  m_pOrder->SetGoodTillDate( dtQuote + boost::posix_time::seconds( 30 ) );
  m_pOrder->SetTimeInForce( ou::tf::TimeInForce::GoodTillDate );
*/

void Strategy::EnterLong( const ptime dt, const double limit ) {
  m_pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 100 );
  //m_pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Limit, ou::tf::OrderSide::Buy, 100, limit );
  m_pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  m_pOrder->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  m_ceLongEntry.AddLabel( dt, limit, "Long Submit" );
  m_stateTrade = ETradeState::LongSubmitted;
  m_pPosition->PlaceOrder( m_pOrder );
}

void Strategy::EnterShort( const ptime dt, const double limit ) {
  m_pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 100 );
  //m_pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Limit, ou::tf::OrderSide::Sell, 100, limit );
  m_pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  m_pOrder->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  m_ceShortEntry.AddLabel( dt, limit, "Short Submit" );
  m_stateTrade = ETradeState::ShortSubmitted;
  m_pPosition->PlaceOrder( m_pOrder );
}

void Strategy::ExitLong( const ptime dt, const double limit ) {
  m_pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 100 );
  //m_pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Limit, ou::tf::OrderSide::Sell, 100, limit );
  m_pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  m_pOrder->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  m_ceLongExit.AddLabel( dt, limit, "Long Exit" );
  m_stateTrade = ETradeState::LongExitSubmitted;
  m_pPosition->PlaceOrder( m_pOrder );
}

void Strategy::ExitShort( const ptime dt, const double limit ) {
  m_pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 100 );
  //m_pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Limit, ou::tf::OrderSide::Buy, 100, limit );
  m_pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  m_pOrder->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  m_ceShortExit.AddLabel( dt, limit, "Short Exit" );
  m_stateTrade = ETradeState::ShortExitSubmitted;
  m_pPosition->PlaceOrder( m_pOrder );
}

void Strategy::HandleRHTrading( const ou::tf::Bar& bar ) { // once a second
  //HandleRHTrading_01Sec( bar );
}

void Strategy::HandleRHTrading_01Sec( const ou::tf::Bar& bar ) { // once a second

  // DailyTradeTimeFrame: Trading during regular active equity market hours
  // https://learnpriceaction.com/3-moving-average-crossover-strategy/
  // TODO: include the marketRule price difference here?

  double ma1 = m_vMA[0].Latest();
  double ma2 = m_vMA[1].Latest();
  double ma3 = m_vMA[2].Latest();

  double hi = ma1;
  if ( ma2 > hi ) hi = ma2;
  if ( ma3 > hi ) hi = ma3;

  double lo = ma1;
  if ( ma2 < lo ) lo = ma2;
  if ( ma3 < lo ) lo = ma3;

  if ( false ) {
    switch ( m_stateTrade ) {
      case ETradeState::Search:

        if ( ( ma1 > ma3 ) && ( ma2 > ma3 ) && ( m_dblMid > ma1 ) ) {
          EnterLong( bar.DateTime(), bar.Close() );
        }
        else {
          if ( ( ma1 < ma3 ) && ( ma2 < ma3 ) && ( m_dblMid < ma1 ) ) {
            EnterShort( bar.DateTime(), bar.Close() );
          }
        }
        break;
      case ETradeState::LongSubmitted:
        // wait for order to execute
        break;
      case ETradeState::LongExit:
        if ( bar.Close() < ma2 ) {
          ExitLong( bar.DateTime(), bar.Close() );
        }
        break;
      case ETradeState::ShortSubmitted:
        // wait for order to execute
        break;
      case ETradeState::ShortExit:
        if ( bar.Close() > ma2 ) {
          ExitShort( bar.DateTime(), bar.Close() );
        }
        break;
      case ETradeState::LongExitSubmitted:
      case ETradeState::ShortExitSubmitted:
        // wait for order to execute
        break;
      case ETradeState::EndOfDayCancel:
      case ETradeState::EndOfDayNeutral:
      case ETradeState::Done:
        // quiescent
        break;
      case ETradeState::Init:
        // market open statistics management here
        // will need to wait for ma to load & diverge (based upon width & period)
        m_stateTrade = ETradeState::Search;
        break;
    }
  }

  // TODO: need to deal with congestion, maybe bollinger bands or short duration stochastic
  //   maybe Trin will help

  if ( false ) {
    switch ( m_stateTrade ) {
      case ETradeState::Search:
        if ( 0.0 < m_dblLastTick ) {
          EnterLong( bar.DateTime(), bar.Close() );
        }
        else {
          if ( 0.0 > m_dblLastTick ) {
            EnterShort( bar.DateTime(), bar.Close() );
          }
        }
        break;
      case ETradeState::LongSubmitted:
        // wait for order to execute
        break;
      case ETradeState::LongExit:
        if ( 0.0 >= m_dblLastTick ) {
          ExitLong( bar.DateTime(), bar.Close() );
        }
        break;
      case ETradeState::ShortSubmitted:
        // wait for order to execute
        break;
      case ETradeState::ShortExit:
        if ( 0.0 <= m_dblLastTick ) {
          ExitShort( bar.DateTime(), bar.Close() );
        }
        break;
      case ETradeState::LongExitSubmitted:
      case ETradeState::ShortExitSubmitted:
        // wait for order to execute
        break;
      case ETradeState::EndOfDayCancel:
      case ETradeState::EndOfDayNeutral:
      case ETradeState::Done:
        // quiescent
        break;
      case ETradeState::Init:
        // market open statistics management here
        // will need to wait for ma to load & diverge (based upon width & period)
        m_stateTrade = ETradeState::Search;
        break;
    }
  }
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

void Strategy::HandleCancel( boost::gregorian::date, boost::posix_time::time_duration ) { // one shot
  m_stateTrade = ETradeState::EndOfDayCancel;
  if ( m_pPosition ) {
    m_pPosition->CancelOrders();
  }
}

void Strategy::HandleGoNeutral( boost::gregorian::date, boost::posix_time::time_duration ) { // one shot
  m_stateTrade = ETradeState::EndOfDayNeutral;
  if ( m_pPosition ) {
    m_pPosition->ClosePosition();
  }

  std::cout
    << "zigzag has " << m_nZigZagLegs
    << " legs with total travel of $" << m_dblZigZagDistance
    << " averaging $" << m_dblZigZagDistance / m_nZigZagLegs << " per leg"
    << std::endl;
}

void Strategy::SaveWatch( const std::string& sPrefix ) {
  if ( m_pPosition ) {
    m_pPosition->GetWatch()->SaveSeries( sPrefix );
  }
  if ( m_pTick ) {
    m_pTick->SaveSeries( sPrefix );
  }
  if ( m_pTrin ) {
    m_pTrin->SaveSeries( sPrefix );
  }
}

void Strategy::CloseAndDone() {
  std::cout << "Sending Close & Done" << std::endl;
  if ( m_pPosition ) {
    m_pPosition->ClosePosition();
  }
  m_stateTrade = ETradeState::Done;
}