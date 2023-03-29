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

#include <TFTrading/Watch.h>

#include "Config.hpp"
#include "Strategy.hpp"

using pWatch_t = ou::tf::Watch::pWatch_t;

Strategy::Strategy( ou::ChartDataView& cdv, const config::Options& options )
: ou::tf::DailyTradeTimeFrame<Strategy>()
, m_cdv( cdv )
, m_ceShortEntry( ou::ChartEntryShape::EShape::Short, ou::Colour::Red )
, m_ceLongEntry( ou::ChartEntryShape::EShape::Long, ou::Colour::Blue )
, m_ceShortFill( ou::ChartEntryShape::EShape::FillShort, ou::Colour::Red )
, m_ceLongFill( ou::ChartEntryShape::EShape::FillLong, ou::Colour::Blue )
, m_ceShortExit( ou::ChartEntryShape::EShape::ShortStop, ou::Colour::Red )
, m_ceLongExit( ou::ChartEntryShape::EShape::LongStop, ou::Colour::Blue )
, m_bfQuotes01Sec( 1 )
, m_stateTrade( ETradeState::Init )
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

  m_ceQuoteAsk.SetColour( ou::Colour::Red );
  m_ceQuoteBid.SetColour( ou::Colour::Blue );
  m_ceTrade.SetColour( ou::Colour::DarkGreen );

  m_ceQuoteAsk.SetName( "Ask" );
  m_ceTrade.SetName( "Tick" );
  m_ceQuoteBid.SetName( "Bid" );

  m_ceVolume.SetName( "Volume" );

  m_ceProfitLoss.SetName( "P/L" );

  m_bfQuotes01Sec.SetOnBarComplete( MakeDelegate( this, &Strategy::HandleBarQuotes01Sec ) );
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

  m_cdv.Add( EChartSlot::Volume, &m_ceVolume );

  m_cdv.Add( EChartSlot::PL, &m_ceProfitLoss );

}

void Strategy::SetPosition( pPosition_t pPosition ) {

  assert( pPosition );

  Clear();

  m_pPosition = pPosition;
  pWatch_t pWatch = m_pPosition->GetWatch();

  m_cdv.SetNames( "Moving Average Strategy", pWatch->GetInstrument()->GetInstrumentName() );

  SetupChart();

  time_duration td = time_duration( 0, 0, m_nPeriodWidth );

  m_vMA.emplace_back( MA( pWatch->GetQuotes(), m_vMAPeriods[0], td, ou::Colour::Gold, "ma1" ) );
  m_vMA.emplace_back( MA( pWatch->GetQuotes(), m_vMAPeriods[1], td, ou::Colour::Coral, "ma2" ) );
  m_vMA.emplace_back( MA( pWatch->GetQuotes(), m_vMAPeriods[2], td, ou::Colour::Brown, "ma3" ) );

  for ( vMA_t::value_type& ma: m_vMA ) {
    ma.AddToView( m_cdv );
  }

  pWatch->OnQuote.Add( MakeDelegate( this, &Strategy::HandleQuote ) );
  pWatch->OnTrade.Add( MakeDelegate( this, &Strategy::HandleTrade ) );

}

void Strategy::Clear() {
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

  if ( !quote.IsValid() ) {
    return;
  }

  ptime dt( quote.DateTime() );

  m_ceQuoteAsk.Append( dt, quote.Ask() );
  m_ceQuoteBid.Append( dt, quote.Bid() );

  m_dblMid = quote.Midpoint();

  for ( vMA_t::value_type& ma: m_vMA ) {
    ma.Update( dt );
  }

  m_bfQuotes01Sec.Add( dt, m_dblMid, 1 ); // provides a 1 sec pulse for checking the alogorithm

}

void Strategy::HandleTrade( const ou::tf::Trade& trade ) {

  ptime dt( trade.DateTime() );

  m_ceTrade.Append( dt, trade.Price() );
  m_ceVolume.Append( dt, trade.Volume() );

}

void Strategy::HandleBarQuotes01Sec( const ou::tf::Bar& bar ) {

  double dblUnRealized, dblRealized, dblCommissionsPaid, dblTotal;

  m_pPosition->QueryStats( dblUnRealized, dblRealized, dblCommissionsPaid, dblTotal );
  m_ceProfitLoss.Append( bar.DateTime(), dblTotal );

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

void Strategy::EnterLong( const ou::tf::Bar& bar ) {
  m_pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 100 );
  m_pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  m_pOrder->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  m_ceLongEntry.AddLabel( bar.DateTime(), bar.Close(), "Long Submit" );
  m_stateTrade = ETradeState::LongSubmitted;
  m_pPosition->PlaceOrder( m_pOrder );
}

void Strategy::EnterShort( const ou::tf::Bar& bar ) {
  m_pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 100 );
  m_pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  m_pOrder->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  m_ceShortEntry.AddLabel( bar.DateTime(), bar.Close(), "Short Submit" );
  m_stateTrade = ETradeState::ShortSubmitted;
  m_pPosition->PlaceOrder( m_pOrder );
}

void Strategy::ExitLong( const ou::tf::Bar& bar ) {
  m_pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 100 );
  m_pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  m_pOrder->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  m_ceLongExit.AddLabel( bar.DateTime(), bar.Close(), "Long Exit" );
  m_stateTrade = ETradeState::LongExitSubmitted;
  m_pPosition->PlaceOrder( m_pOrder );
}

void Strategy::ExitShort( const ou::tf::Bar& bar ) {
  m_pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 100 );
  m_pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  m_pOrder->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  m_ceShortExit.AddLabel( bar.DateTime(), bar.Close(), "Short Exit" );
  m_stateTrade = ETradeState::ShortExitSubmitted;
  m_pPosition->PlaceOrder( m_pOrder );
}

void Strategy::HandleRHTrading( const ou::tf::Bar& bar ) { // once a second

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

  switch ( m_stateTrade ) {
    case ETradeState::Search:

      if ( ( ma1 > ma3 ) && ( ma2 > ma3 ) && ( m_dblMid > ma1 ) ) {
        EnterLong( bar );
      }
      else {
        if ( ( ma1 < ma3 ) && ( ma2 < ma3 ) && ( m_dblMid < ma1 ) ) {
          EnterShort( bar );
        }
      }
      break;
    case ETradeState::LongSubmitted:
      // wait for order to execute
      break;
    case ETradeState::LongExit:
      if ( bar.Close() < ma2 ) {
        ExitLong( bar );
      }
      break;
    case ETradeState::ShortSubmitted:
      // wait for order to execute
      break;
    case ETradeState::ShortExit:
      if ( bar.Close() > ma2 ) {
        ExitShort( bar );
      }
      break;
    case ETradeState::LongExitSubmitted:
    case ETradeState::ShortExitSubmitted:
      // wait for order to execute
      break;
    case ETradeState::EndOfDayCancel:
    case ETradeState::EndOfDayNeutrall:
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

void Strategy::HandleOrderCancelled( const ou::tf::Order& order ) {
  m_pOrder->OnOrderCancelled.Remove( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  m_pOrder->OnOrderFilled.Remove( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  switch ( m_stateTrade ) {
    case ETradeState::EndOfDayCancel:
    case ETradeState::EndOfDayNeutrall:
      BOOST_LOG_TRIVIAL(info) << "order " << order.GetOrderId() << " cancelled - end of day";
      break;
    case ETradeState::LongExitSubmitted:
    case ETradeState::ShortExitSubmitted:
      //assert( false );  // TODO: need to figure out a plan to retry exit
      BOOST_LOG_TRIVIAL(error) << "order " << order.GetOrderId() << " cancelled - state machine needs fixes";
      break;
    default:
      m_stateTrade = ETradeState::Search;
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
      m_stateTrade = ETradeState::Search;
      break;
    case ETradeState::ShortExitSubmitted:
      m_ceLongFill.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "Short Exit Fill" );
      m_stateTrade = ETradeState::Search;
      break;
    case ETradeState::EndOfDayCancel:
    case ETradeState::EndOfDayNeutrall:
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
  m_stateTrade = ETradeState::EndOfDayNeutrall;
  if ( m_pPosition ) {
    m_pPosition->ClosePosition();
  }
}

void Strategy::SaveWatch( const std::string& sPrefix ) {
  if ( m_pPosition ) {
    m_pPosition->GetWatch()->SaveSeries( sPrefix );
  }
}

void Strategy::CloseAndDone() {
  std::cout << "Sending Close & Done" << std::endl;
  if ( m_pPosition ) {
    m_pPosition->ClosePosition();
  }
  m_stateTrade = ETradeState::Done;
}