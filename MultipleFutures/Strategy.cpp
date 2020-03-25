/************************************************************************
 * Copyright(c) 2020, One Unified. All rights reserved.                 *
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
 * Project: MultipleFutures
 * Created on March 22, 2020, 18:30
 */

 // TODO: timer on entry (monitor for average length of entry and use 2x as cancel, enter in other direction)
 //    or use four bars or so as maximum
 // TODO: if stop rises by 50%, bump up the profit exit?
 //    if rise is in direction of ema?
 // TODO: run long(21 bar)/short(7 bar) ema and see if can trade on crossing
 //    needs minimum range movement so as not to over-trade
 //    then can influence entry direction, and maybe let profit run.
 // TODO: add P/L to chart
 // TODO: remove the cap, let the profit run
 // TODO: narrow the stop range, or, if let profit run, then leave at two bars
 // TODO: need shorter range bollinger bands
 // TODO: use the Merrill Pattern for trade entry statistics

#include <wx/utils.h>

#include "Strategy.h"

namespace {
  static const size_t nBars { 2 };
}

Strategy::Strategy( pWatch_t pWatch, uint16_t nSecondsPerBar )
: ou::ChartDVBasics()
, ou::tf::DailyTradeTimeFrame<Strategy>()
, m_nSecondsPerBar( nSecondsPerBar )
, m_bfBar( nSecondsPerBar )
, m_dblAverageBarSize {}
, m_cntBars {}
, m_state( EState::initial )
, m_tfLatest( TimeFrame::Closed )
, m_stochastic(
    const_cast<ou::tf::Quotes&>( pWatch->GetQuotes() ),
    14, seconds( nSecondsPerBar ),
    [this]( const ou::tf::Price& k ){
      m_ceStochastic.Append( k );
      m_tsK.Append( k );
    }
    )
, m_stateStochastic( EStateStochastic::Quiesced )
, m_smaK( m_tsK, 1, seconds( nSecondsPerBar ) )
, m_curK {}
, m_lowerK0( 14.5 ), m_lowerK1( 15.5 ), m_upperK0( 85.5 ), m_upperK1( 84.5 )
{

  m_bfBar.SetOnBarComplete( MakeDelegate( this, &Strategy::HandleBarComplete ) );
  m_pIB = boost::dynamic_pointer_cast<ou::tf::IBTWS>( pWatch->GetProvider() );
  m_pWatch = pWatch;
  m_pWatch->OnQuote.Add( MakeDelegate( this, &Strategy::HandleQuote ) );
  m_pWatch->OnTrade.Add( MakeDelegate( this, &Strategy::HandleTrade ) );
  m_pPosition = boost::make_shared<ou::tf::Position>( m_pWatch, m_pIB );
  m_pPosition->OnUnRealizedPL.Add( MakeDelegate( this, &Strategy::HandleUnRealizedPL ) );
  m_pPosition->OnExecution.Add( MakeDelegate( this, &Strategy::HandleExecution ) );
  dynamic_cast<ou::tf::Prices&>( m_smaK ).OnAppend.Add( MakeDelegate( this, &Strategy::UpdateStochasticSmoothed ) );

  ptime dtNow( ou::TimeSource::Instance().External() );  // provided in utc
  std::cout << "ou::TimeSource::Instance().External(): " << dtNow << std::endl;
  boost::gregorian::date dateMarketOpen( MarketOpenDate( dtNow ) );
  std::cout << "MarketOpenDate: " << dateMarketOpen << std::endl;
  InitForUS24HourFutures( dateMarketOpen );

  if ( true ) { // false for standard day time trading, true for 24 hour futures trading
    // this may be offset incorrectly.
    //SetRegularHoursOpen( Normalize( dt.date(), dt.time_of_day(), "America/New_York" ) );  // collect some data first
    ptime dtMo( GetMarketOpen() );
    if ( dtNow > dtMo ) {
      SetRegularHoursOpen( dtNow );
      // change later to 10 to collect enough data to start trading:
      //SetStartTrading( Normalize( dt.date(), dt.time_of_day() + boost::posix_time::minutes( 2 ), "America/New_York" ) );  // collect some data first
      SetStartTrading( dtNow + boost::posix_time::minutes( 2 ) );  // time for history to accumulate
    }
    else {
      SetRegularHoursOpen( dtMo + boost::posix_time::minutes( 2 ) );  // time for history to accumulate
      SetStartTrading( dtMo + boost::posix_time::minutes( 12 ) );  // time for history to accumulate
    }
  }

  m_ceStochastic.SetColour( ou::Colour::Aquamarine );
  m_ceStochasticSmoothed.SetName( "Fast Stoch" );

  m_ceStochasticLimits.AddMark( 100.0, ou::Colour::DarkGray, "Top" );
  //m_ceStochasticLimits.AddMark( m_upperK, ou::Colour::Black, "Upper" );
  m_ceStochasticLimits.AddMark(  50.0, ou::Colour::DarkGray, "Middle" );
  //m_ceStochasticLimits.AddMark( m_lowerK, ou::Colour::Black, "Lower" );
  m_ceStochasticLimits.AddMark(   0.0, ou::Colour::DarkGray, "Bottom" );

  m_dvChart.Add( 3, &m_ceStochastic );
  m_dvChart.Add( 3, &m_ceStochasticSmoothed );
  //m_dvChart.Add( 3, &m_ceStochasticLimits ); // stops chart from showing data, even with just one marker
}

Strategy::~Strategy() {
  dynamic_cast<ou::tf::Prices&>( m_smaK ).OnAppend.Remove( MakeDelegate( this, &Strategy::UpdateStochasticSmoothed ) );
  m_pPosition->OnUnRealizedPL.Remove( MakeDelegate( this, &Strategy::HandleUnRealizedPL) );
  m_pPosition->OnExecution.Remove( MakeDelegate( this, &Strategy::HandleExecution ) );
  m_pWatch->OnQuote.Remove( MakeDelegate( this, &Strategy::HandleQuote ) );
  m_pWatch->OnTrade.Remove( MakeDelegate( this, &Strategy::HandleTrade ) );
  m_bfBar.SetOnBarComplete( nullptr );
}

void Strategy::HandleButtonUpdate() {
}

void Strategy::Entry( ou::tf::OrderSide::enumOrderSide side ) {
  // TODO: need to track orders, nothing new while existing ones active?
  m_trade.offset = 3.0 * m_dblAverageBarSize;
  double tick = m_pWatch->GetInstrument()->GetMinTick();
  double min = 3.0 * tick;
  if ( min > m_trade.offset ) m_trade.offset = min;
  //const double dblEntry = m_tradeLast.Price();
  //const double dblEntry = m_quoteLast.Midpoint();
  //const double dblUpper = m_pWatch->GetInstrument()->NormalizeOrderPrice( dblEntry + dblOffset );
  //const double dblLower = m_pWatch->GetInstrument()->NormalizeOrderPrice( dblEntry - dblOffset );
  if ( 0.0 < m_tradeLast.Price() ) {
    m_trade.side = side;
    switch ( side ) {
      case ou::tf::OrderSide::enumOrderSide::Buy: {

        m_trade.entry = m_quoteLast.Ask();
        // TODO: adjust when entry is executed?
        //double dblProfit = m_trade.entry + 2.0 * tick;
        //m_trade.stop = m_trade.entry - 3.0 * tick;
        m_trade.trail = m_trade.stop = m_trade.entry - m_trade.offset;

        ou::ChartDVBasics::m_ceLongEntries.AddLabel( m_quoteLast.DateTime(), m_trade.entry, "long entry" );
        m_pOrderEntry = m_pPosition->ConstructOrder(
          ou::tf::OrderType::enumOrderType::Limit,
          ou::tf::OrderSide::enumOrderSide::Buy,
          1,
          m_trade.entry
          // idPosition
          // dt order submitted
          );
        m_pOrderEntry->SetDescription( "long" );
        m_pOrderEntry->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
        m_pOrderEntry->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );

        std::cout
          << m_quoteLast.DateTime()
          << " entry long " << m_trade.entry
          << ", " << m_trade.offset
          << ", " << m_trade.stop
          << "," << m_trade.trail
          << std::endl;
        wxBell();
/*
        ou::ChartDVBasics::m_ceLongEntries.AddLabel( m_quoteLast.DateTime(), dblProfit, "profit target" );
        m_pOrderProfit = m_pPosition->ConstructOrder(
          ou::tf::OrderType::enumOrderType::Limit,
          ou::tf::OrderSide::enumOrderSide::Sell,
          1,
          dblProfit
          // idPosition
          // dt order submitted
          );
        m_pOrderProfit->SetDescription( "profit" );
        //m_pOrderProfit->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
        m_pOrderProfit->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );

*/
        }
        break;
      case ou::tf::OrderSide::enumOrderSide::Sell: {

        m_trade.entry = m_quoteLast.Bid();
        //double dblProfit = dblEntry - 2.0 * tick;
        //m_trade.stop = m_trade.entry + 3.0 * tick;
        m_trade.trail = m_trade.stop = m_trade.entry + m_trade.offset;

        ou::ChartDVBasics::m_ceLongEntries.AddLabel( m_quoteLast.DateTime(), m_trade.entry, "short entry" );
        m_pOrderEntry = m_pPosition->ConstructOrder(
          ou::tf::OrderType::enumOrderType::Limit,
          ou::tf::OrderSide::enumOrderSide::Sell,
          1,
          m_trade.entry
          // idPosition
          // dt order submitted
          );
        m_pOrderEntry->SetDescription( "short" );
        m_pOrderEntry->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
        m_pOrderEntry->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );

        std::cout
          << m_quoteLast.DateTime()
          << " entry short " << m_trade.entry
          << ", " << m_trade.offset
          << ", " << m_trade.stop
          << "," << m_trade.trail
          << std::endl;
        wxBell();
/*
        ou::ChartDVBasics::m_ceLongEntries.AddLabel( m_quoteLast.DateTime(), dblProfit, "profit target" );
        m_pOrderProfit = m_pPosition->ConstructOrder(
          ou::tf::OrderType::enumOrderType::Limit,
          ou::tf::OrderSide::enumOrderSide::Buy,
          1,
          dblProfit
          // idPosition
          // dt order submitted
          );
        m_pOrderProfit->SetDescription( "profit" );
        //m_pOrderProfit->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
        m_pOrderProfit->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );

*/
        }
        break;
    }
    // TOOD: place through OrderManager at some point
    //    then can use Position to cancel orders
    //m_pIB->PlaceBracketOrder( m_pOrderEntry, m_pOrderProfit, m_pOrderStop );
    //m_pIB->PlaceComboOrder( m_pOrderEntry, m_pOrderStop );
    m_pPosition->PlaceOrder( m_pOrderEntry );
  }
}

void Strategy::HandleButtonCancel() {
  m_pPosition->CancelOrders();
  m_state = EState::quiesce;
}

void Strategy::HandleQuote( const ou::tf::Quote &quote ) {
  //std::cout << "quote: " << quote.Bid() << "," << quote.Ask() << std::endl;
  ou::tf::Quote::price_t bid( quote.Bid() );
  ou::tf::Quote::price_t ask( quote.Ask() );
  if ( ( 0.0 < bid ) && ( 0.0 < ask ) ) {
    ou::ChartDVBasics::HandleQuote( quote );
    TimeTick( quote );
    m_quoteLast = quote;
  }
}

void Strategy::HandleTrade( const ou::tf::Trade &trade ) {
  //std::cout << "trade: " << trade.Volume() << "@" << trade.Price() << std::endl;
  m_tradeLast = trade;
  ou::ChartDVBasics::HandleTrade( trade );

  double ema[ 4 ];

  ema[ 0 ] = ou::ChartDVBasics::m_vInfoBollinger[ 0 ].m_stats.MeanY(); // fastest moving
  ema[ 1 ] = ou::ChartDVBasics::m_vInfoBollinger[ 1 ].m_stats.MeanY();
  ema[ 2 ] = ou::ChartDVBasics::m_vInfoBollinger[ 2 ].m_stats.MeanY();
  ema[ 3 ] = ou::ChartDVBasics::m_vInfoBollinger[ 3 ].m_stats.MeanY(); // slowest moving

  m_bfBar.Add( trade ); // comes after ChartDVBasics as updated stats are required
}

void Strategy::HandleBarComplete( const ou::tf::Bar& bar ) {
  m_dblAverageBarSize = 0.9 * m_dblAverageBarSize + 0.1 * ( bar.High() - bar.Low() );
  TimeTick( bar );
}

void Strategy::UpdateStochasticSmoothed( const ou::tf::Price& price ) {
  // run the states.  let the trades run/stop to completion.
  // need indicator of where in trades are.

  m_ceStochasticSmoothed.Append( price );

  double K( price.Value() );

  switch ( m_stateStochastic ) {
    case EStateStochastic::Quiesced:
      // needs external source to force state change (after delta time)
      break;
    case EStateStochastic::WaitForNeutral:
      if ( ( m_upperK1 > K ) && ( m_lowerK1 < K ) ) {
        m_stateStochastic = EStateStochastic::WaitForFirstCrossing;
      }
      break;
    case EStateStochastic::WaitForFirstCrossing:
      if ( m_upperK0 < K ) {
        m_stateStochastic = EStateStochastic::HiCrossedUp;
      }
      if ( m_lowerK0 > K ) {
        m_stateStochastic = EStateStochastic::LoCrossedDown;
      }
      break;
    case EStateStochastic::WaitForHiCrossUp:
      if ( m_upperK0 < K ) {
        m_stateStochastic = EStateStochastic::HiCrossedUp;
      }
      break;
    case EStateStochastic::HiCrossedUp:
      if ( m_upperK1 > K ) {
        if ( EState::entry_wait == m_state ) {
          m_state = EState::entry_filling;
          Entry( ou::tf::OrderSide::Sell );
        }
        m_stateStochastic = EStateStochastic::WaitForLoCrossDown;
      }
      break;
//    case EStateStochastic::HiCrossedDown:
//      break;
    case EStateStochastic::WaitForLoCrossDown:
      if ( m_lowerK0 > K ) {
        m_stateStochastic = EStateStochastic::LoCrossedDown;
      }
      break;
    case EStateStochastic::LoCrossedDown:
      if ( m_lowerK1 < K ) {
        if ( EState::entry_wait == m_state ) {
          m_state = EState::entry_filling;
          Entry( ou::tf::OrderSide::Buy );
        }
        m_stateStochastic = EStateStochastic::WaitForHiCrossUp;
      }
      break;
//    case EStateStochastic::LoCrossedUp:
//      break;
  }
}

void Strategy::HandleOrderCancelled( const ou::tf::Order& order ) {
  // TODO: may need to be more detailed here,
  //   need to check that the primery entry order has indeed been cancelled
  //   if only a few of the orders were cancelled, then may need to perform a close
  std::cout << "HandleOrderCancelled: " << order.GetOrderId() << "," << order.GetDescription() << std::endl;
  if ( EState::entry_cancelling == m_state ) {
    m_state = EState::entry_wait;
  }
  else {
    // TODO: migth be in entry state or something (need to validate)
    std::cout << "HandleOrderCancelled no entry_wait: "<< order.GetOrderId() << "," << order.GetDescription() << std::endl;
  }
}

void Strategy::HandleOrderFilled( const ou::tf::Order& order ) {
  std::string sMessage = "unknown ";
  switch ( m_state ) {
    case EState::initial:
      break;
    case EState::entry_wait:
      sMessage = "entry wait ";
      break;
    case EState::entry_cancelling:
      std::cout << "HandleOrderFilled in entry_cancelling, need to fix the state machine!" << std::endl;
      // fall through to handle the fill and proceed
    case EState::entry_filling:
      sMessage = "filled ";
      m_state = EState::exit_tracking;
      switch ( order.GetOrderSide() ) {
        case ou::tf::OrderSide::Buy:
          ou::ChartDVBasics::m_ceShortEntries.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), sMessage + "filled" );
          break;
        case ou::tf::OrderSide::Sell:
          ou::ChartDVBasics::m_ceLongEntries.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), sMessage + "filled" );
          break;
      }
      break;
    case EState::exit_tracking:
      {

      }
      break;
    case EState::exit_filling:
      {
        sMessage = "exit ";
        m_state = EState::entry_wait; // start over
      }
      break;
    case EState::cancel_wait:
      sMessage = "cancel ";
      break;
    case EState::quiesce:
      sMessage = "quiesce ";
      break;
  }
}

void Strategy::HandleRHTrading( const ou::tf::Quote& quote ) {
  switch ( m_state ) {
    case EState::exit_tracking: {
        switch ( m_trade.side ) {
          case ou::tf::OrderSide::Buy: {
            if ( m_trade.trail > quote.Ask() ) {
              m_state = EState::exit_filling;
              ou::ChartDVBasics::m_ceLongEntries.AddLabel( quote.DateTime(), m_trade.trail, "stop mkt" );
              m_pOrderStop = m_pPosition->ConstructOrder(
                ou::tf::OrderType::enumOrderType::Market,
                ou::tf::OrderSide::enumOrderSide::Sell,
                1
                //dblLoss,
                //dblEntry - dblLoss
                // idPosition
                // dt order submitted
                );
              m_pOrderStop->SetDescription( "stop" );
              //m_pOrderStop->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
              m_pOrderStop->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );

              std::cout
                << m_quoteLast.DateTime()
                << " stop long " << m_trade.entry
                << ", " << m_trade.offset
                << ", " << m_trade.stop
                << "," << m_trade.trail
                << std::endl;
              wxBell();

              m_pPosition->PlaceOrder( m_pOrderStop );
            }
            else {
              double trail = quote.Bid() - m_trade.offset;
              if ( trail > m_trade.trail ) m_trade.trail = trail;
            }
            }
            break;
          case ou::tf::OrderSide::Sell: {
            if ( m_trade.trail < quote.Bid() ) {
              m_state = EState::exit_filling;
              ou::ChartDVBasics::m_ceLongEntries.AddLabel( quote.DateTime(), m_trade.trail, "stop mkt" );
              m_pOrderStop = m_pPosition->ConstructOrder(
                ou::tf::OrderType::enumOrderType::Market,
                ou::tf::OrderSide::enumOrderSide::Buy,
                1
                //dblLoss,
                //dblLoss - dblEntry
                // idPosition
                // dt order submitted
                );
              m_pOrderStop->SetDescription( "stop" );

              //m_pOrderStop->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
              m_pOrderStop->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );

              std::cout
                << m_quoteLast.DateTime()
                << " stop short " << m_trade.entry
                << ", " << m_trade.offset
                << ", " << m_trade.stop
                << "," << m_trade.trail
                << std::endl;
              wxBell();

              m_pPosition->PlaceOrder( m_pOrderStop );
            }
            else {
              double trail = quote.Ask() + m_trade.offset;
              if ( trail < m_trade.trail ) m_trade.trail = trail;

            }
            }
            break;
        }
      }
      break;
  }
}

void Strategy::HandleRHTrading( const ou::tf::Bar& bar ) {
  // threading problem here with gui looking at m_mapMatching
  // at 20 seconds, will take 3 - 4 minutes to stabilize
  if ( 0 < m_cntBars ) {
    switch ( m_state ) {
      case EState::initial:
        if ( nBars <= m_cntBars ) {
          m_state = EState::entry_wait;
          m_stateStochastic = EStateStochastic::WaitForFirstCrossing;
        }
        break;
      case EState::entry_wait:
        {
          // trades generated in UpdateStochasticSmoothed
        }
        break;
      case EState::entry_filling:
        // primarily managed in HandleOrderFilled
//          m_pIB->CancelOrder( m_pOrderEntry ); // TODO: need to check that the two exit orders are also cancelled
        break;
      case EState::entry_cancelling:
        // either HandleOrderCancelled or HandleOrderFilled will move to the next state
        break;
      case EState::exit_tracking: {

        }
        break;
      case EState::exit_filling:
        break;
      case EState::cancel_wait:
        break;
      case EState::quiesce:
        break;
    }
  }
  m_barLast = bar;
  m_cntBars++;
}

void Strategy::CancelOrders() {
  if ( m_pOrderEntry ) {
    m_pIB->CancelOrder( m_pOrderEntry );
  }
  if ( m_pOrderProfit ) {
    m_pIB->CancelOrder( m_pOrderProfit );
  }
  if ( m_pOrderStop ) {
    m_pIB->CancelOrder( m_pOrderStop );
  }
}

void Strategy::HandleCancelling( const ou::tf::Bar& bar ) {
  if ( m_tfLatest != CurrentTimeFrame() ) {
    std::cout << "Time Frame: Cancelling" << std::endl;
    // a one-shot trigger
    m_tfLatest = CurrentTimeFrame();
    //m_pPosition->CancelOrders();
    // orders not placed through Position/OrderManager, so can not cancel orders via that avenue
    CancelOrders();
  }
  else {
    // wait for transition to next state
  }
}

void Strategy::HandleGoingNeutral( const ou::tf::Bar& bar ) {
  if ( m_tfLatest != CurrentTimeFrame() ) {
    std::cout << "Time Frame: Going Neutral" << std::endl;
    m_tfLatest = CurrentTimeFrame();
    m_pPosition->ClosePosition();
  }
  else {

  }
}

void Strategy::EmitBarSummary() {
}

void Strategy::HandleUnRealizedPL( const ou::tf::Position::PositionDelta_delegate_t& delta ) {
  //std::cout << "unrealized p/l from " << delta.get<1>() << " to " << delta.get<2>() << std::endl;
}

void Strategy::HandleExecution( const ou::tf::Position::PositionDelta_delegate_t& delta ) {
  std::cout << "realized p/l from " << delta.get<1>() << " to " << delta.get<2>() << std::endl;
}
