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
 * Project: ESBracketOrder
 * Created: January 6, 2020, 11:41 AM
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

#include "TFTrading/TradingEnumerations.h"

#include "Strategy.h"

namespace {
  static const size_t nBars { 10 };
}

Strategy::Strategy( pWatch_t pWatch )
: ou::ChartDVBasics()
, ou::tf::DailyTradeTimeFrame<Strategy>()
, m_bfBar( 20 )
, m_dblAverageBarSize {}
, m_cntBars {}
, m_state( EState::initial )
, m_tfLatest( TimeFrame::Closed )
, m_mapEntry { // OverRide: Enter with OrderSdie based upon OrderResults statistics
    { { -1,-1,-1,-1 }, ou::tf::OrderSide::Buy  },
    { { -1,-1,-1, 1 }, ou::tf::OrderSide::Buy  },
    { { -1, 0,-1, 1 }, ou::tf::OrderSide::Buy  },
    { {  1, 1, 1,-1 }, ou::tf::OrderSide::Sell },
    { {  1, 1, 1, 1 }, ou::tf::OrderSide::Sell }
    }
{
  m_bfBar.SetOnBarComplete( MakeDelegate( this, &Strategy::HandleBarComplete ) );
  m_pIB = boost::dynamic_pointer_cast<ou::tf::IBTWS>( pWatch->GetProvider() );
  m_pWatch = pWatch;
  m_pWatch->OnQuote.Add( MakeDelegate( this, &Strategy::HandleQuote ) );
  m_pWatch->OnTrade.Add( MakeDelegate( this, &Strategy::HandleTrade ) );
  m_pPosition = boost::make_shared<ou::tf::Position>( m_pWatch, m_pIB );
  m_pPosition->OnUnRealizedPL.Add( MakeDelegate( this, &Strategy::HandleUnRealizedPL ) );
  m_pPosition->OnExecution.Add( MakeDelegate( this, &Strategy::HandleExecution ) );

  ptime dtNow( ou::TimeSource::Instance().External() );  // provided in utc
  std::cout << "ou::TimeSource::Instance().External(): " << dtNow << std::endl;
  boost::gregorian::date dateMarketOpen( MarketOpenDate( dtNow ) );
  std::cout << "MarketOpenDate: " << dateMarketOpen << std::endl;
  InitForUS24HourFutures( dateMarketOpen );
  // this may be offset incorrectly.
  //SetRegularHoursOpen( Normalize( dt.date(), dt.time_of_day(), "America/New_York" ) );  // collect some data first
//  ptime dtMo( GetMarketOpen() );
//  if ( dtNow > dtMo ) {
//    SetRegularHoursOpen( dtNow );  // collect some data first
    // change later to 10 to collect enough data to start trading:
    //SetStartTrading( Normalize( dt.date(), dt.time_of_day() + boost::posix_time::minutes( 2 ), "America/New_York" ) );  // collect some data first
//    SetStartTrading( dtNow + boost::posix_time::minutes( 2 ) );  // collect some data first
//  }
//  else {
//    SetRegularHoursOpen( dtMo + boost::posix_time::minutes( 2 ) );  // collect some data first
//    SetStartTrading( dtMo + boost::posix_time::minutes( 12 ) );  // collect some data first
//  }
}

Strategy::~Strategy() {
  m_pPosition->OnUnRealizedPL.Remove( MakeDelegate( this, &Strategy::HandleUnRealizedPL) );
  m_pPosition->OnExecution.Remove( MakeDelegate( this, &Strategy::HandleExecution ) );
  m_pWatch->OnQuote.Remove( MakeDelegate( this, &Strategy::HandleQuote ) );
  m_pWatch->OnTrade.Remove( MakeDelegate( this, &Strategy::HandleTrade ) );
  m_bfBar.SetOnBarComplete( MakeDelegate( this, &Strategy::HandleBarComplete ) );
}

void Strategy::HandleButtonUpdate() {
}

void Strategy::HandleButtonSend( ou::tf::OrderSide::enumOrderSide side ) {
  // TODO: need to track orders, nothing new while existing ones active?
  double dblOffset( 2.0 * m_dblAverageBarSize );
  double min = 2.0 * m_pWatch->GetInstrument()->GetMinTick();
  if ( min >= dblOffset ) dblOffset = min;
  //const double dblEntry = m_tradeLast.Price();
  const double dblEntry = m_quoteLast.Midpoint();
  const double dblUpper = m_pWatch->GetInstrument()->NormalizeOrderPrice( dblEntry + dblOffset );
  const double dblLower = m_pWatch->GetInstrument()->NormalizeOrderPrice( dblEntry - dblOffset );
  if ( 0.0 < m_tradeLast.Price() ) {
    switch ( side ) {
      case ou::tf::OrderSide::enumOrderSide::Buy:

        ou::ChartDVBasics::m_ceLongEntries.AddLabel( m_tradeLast.DateTime(), dblEntry, "long entry" );
        m_pOrderEntry = m_pPosition->ConstructOrder(
          ou::tf::OrderType::enumOrderType::Limit,
          ou::tf::OrderSide::enumOrderSide::Buy,
          1,
          m_quoteLast.Ask()
          // idPosition
          // dt order submitted
          );
        m_pOrderEntry->SetDescription( "long" );
        m_pOrderEntry->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
        m_pOrderEntry->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );

        ou::ChartDVBasics::m_ceLongEntries.AddLabel( m_tradeLast.DateTime(), dblUpper, "profit target" );
        m_pOrderProfit = m_pPosition->ConstructOrder(
          ou::tf::OrderType::enumOrderType::Limit,
          ou::tf::OrderSide::enumOrderSide::Sell,
          1,
          dblUpper
          // idPosition
          // dt order submitted
          );
        m_pOrderProfit->SetDescription( "profit" );
        //m_pOrderProfit->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
        m_pOrderProfit->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );

        ou::ChartDVBasics::m_ceLongEntries.AddLabel( m_tradeLast.DateTime(), dblLower, "loss target" );
        m_pOrderStop = m_pPosition->ConstructOrder(
          ou::tf::OrderType::enumOrderType::Trail,
          ou::tf::OrderSide::enumOrderSide::Sell,
          1,
          dblLower,
          m_quoteLast.Ask() - dblLower
          // idPosition
          // dt order submitted
          );
        m_pOrderStop->SetDescription( "loss" );
        //m_pOrderStop->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
        m_pOrderStop->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );

        break;
      case ou::tf::OrderSide::enumOrderSide::Sell:

        ou::ChartDVBasics::m_ceLongEntries.AddLabel( m_tradeLast.DateTime(), dblEntry, "short entry" );
        m_pOrderEntry = m_pPosition->ConstructOrder(
          ou::tf::OrderType::enumOrderType::Limit,
          ou::tf::OrderSide::enumOrderSide::Sell,
          1,
          m_quoteLast.Bid()
          // idPosition
          // dt order submitted
          );
        m_pOrderEntry->SetDescription( "short" );
        m_pOrderEntry->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
        m_pOrderEntry->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );

        ou::ChartDVBasics::m_ceLongEntries.AddLabel( m_tradeLast.DateTime(), dblLower, "profit target" );
        m_pOrderProfit = m_pPosition->ConstructOrder(
          ou::tf::OrderType::enumOrderType::Limit,
          ou::tf::OrderSide::enumOrderSide::Buy,
          1,
          dblLower
          // idPosition
          // dt order submitted
          );
        m_pOrderProfit->SetDescription( "profit" );
        //m_pOrderProfit->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
        m_pOrderProfit->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );

        ou::ChartDVBasics::m_ceLongEntries.AddLabel( m_tradeLast.DateTime(), dblUpper, "loss target" );
        m_pOrderStop = m_pPosition->ConstructOrder(
          ou::tf::OrderType::enumOrderType::Trail,
          ou::tf::OrderSide::enumOrderSide::Buy,
          1,
          dblUpper,
          dblUpper - m_quoteLast.Bid()
          // idPosition
          // dt order submitted
          );
        m_pOrderStop->SetDescription( "loss" );
        //m_pOrderStop->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
        m_pOrderStop->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );

        break;
    }
    // TOOD: place through OrderManager at some point
    //    then can use Position to cancel orders
    m_pIB->PlaceBracketOrder( m_pOrderEntry, m_pOrderProfit, m_pOrderStop );
    //m_pIB->PlaceComboOrder( m_pOrderEntry, m_pOrderStop );
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
    m_quoteLast = quote;
  }
}

void Strategy::HandleTrade( const ou::tf::Trade &trade ) {
  //std::cout << "trade: " << trade.Volume() << "@" << trade.Price() << std::endl;
  m_tradeLast = trade;
  m_bfBar.Add( trade );
  ou::ChartDVBasics::HandleTrade( trade );
}

void Strategy::HandleBarComplete( const ou::tf::Bar& bar ) {
  m_dblAverageBarSize = 0.9 * m_dblAverageBarSize + 0.1 * ( bar.High() - bar.Low() );
  TimeTick( bar );
}

void Strategy::HandleOrderCancelled( const ou::tf::Order& order ) {
  // TODO: may need to be more detailed here,
  //   need to check that the primery entry order has indeed been cancelled
  //   if only a few of the orders were cancelled, then may need to perform a close
  std::cout << "HandleOrderCancelled: " << order.GetOrderId() << "," << order.GetDescription() << std::endl;
  if ( EState::entry_cancelling == m_state ) {
    m_state = EState::entry_wait;
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
      m_state = EState::exit_filling;
      m_stateInfo.sideEntry = order.GetOrderSide();
      m_stateInfo.dblEntryPrice = order.GetAverageFillPrice();
      switch ( order.GetOrderSide() ) {
        case ou::tf::OrderSide::Buy:
          ou::ChartDVBasics::m_ceShortEntries.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), sMessage + "filled" );
          break;
        case ou::tf::OrderSide::Sell:
          ou::ChartDVBasics::m_ceLongEntries.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), sMessage + "filled" );
          break;
      }
      break;
    case EState::exit_filling:
      {
        sMessage = "exit ";
        m_state = EState::entry_wait; // start over
        mapMatching_t::iterator entry = m_mapMatching.find( m_stateInfo.barMatching );
        assert( m_mapMatching.end() != entry );
        switch ( m_stateInfo.sideEntry ) {
          case ou::tf::OrderSide::Buy:
            entry->second.longs.cntOrders++;
            if ( "profit" == order.GetDescription() ) {
              entry->second.longs.cntWins++;
              entry->second.longs.dblProfit += ( order.GetAverageFillPrice() - m_stateInfo.dblEntryPrice );
            }
            if ( "loss" == order.GetDescription() ) {
              entry->second.longs.cntLosses++;
              entry->second.longs.dblLoss -= ( m_stateInfo.dblEntryPrice - order.GetAverageFillPrice() );
            }
            ou::ChartDVBasics::m_ceShortExits.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), sMessage + "filled" );
            break;
          case ou::tf::OrderSide::Sell:
            entry->second.shorts.cntOrders++;
            if ( "profit" == order.GetDescription() ) {
              entry->second.shorts.cntWins++;
              entry->second.shorts.dblProfit += ( m_stateInfo.dblEntryPrice - order.GetAverageFillPrice() );
            }
            if ( "loss" == order.GetDescription() ) {
              entry->second.shorts.cntLosses++;
              entry->second.shorts.dblLoss -= ( order.GetAverageFillPrice() - m_stateInfo.dblEntryPrice );
            }
            ou::ChartDVBasics::m_ceLongExits.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), sMessage + "filled" );
            break;
        }
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

void Strategy::HandleRHTrading( const ou::tf::Bar& bar ) {
  // threading problem here with gui looking at m_mapMatching
  // at 20 seconds, will take 3 - 4 minutes to stabilize
  if ( 0 < m_cntBars ) {
    BarMatching key;
    key.Set( m_barLast, bar );
    OrderResults orsBlank;
    mapMatching_pair_t pair = m_mapMatching.try_emplace( key, orsBlank ); // find or create entry
    // pair.second => true if inserted, false if not
    auto& [mapKey, mapValue] = *pair.first;
    mapValue.cntInstances++;
    switch ( m_state ) {
      case EState::initial:
        if ( nBars <= m_cntBars ) m_state = EState::entry_wait;
        break;
      case EState::entry_wait:
        {
          mapEntry_t::iterator iter = m_mapEntry.find( key );
          if ( m_mapEntry.end() == iter ) {
            switch ( key.close ) {
              case 1:
                m_stateInfo.barMatching = key;
                m_stateInfo.nBarDuration = 0;
                m_state = EState::entry_filling;
                HandleButtonSend( ou::tf::OrderSide::Buy );
                break;
              case 0:
                // no entry
                break;
              case -1:
                m_stateInfo.barMatching = key;
                m_stateInfo.nBarDuration = 0;
                m_state = EState::entry_filling;
                HandleButtonSend( ou::tf::OrderSide::Sell );
                break;
            }
          }
          else {
            // use pre-calculated entry direction
            m_stateInfo.barMatching = key;
            m_state = EState::entry_filling;
            HandleButtonSend( iter->second );
          }
        }
        break;
      case EState::entry_filling:
        // primarily managed in HandleOrderFilled
        m_stateInfo.nBarDuration++;
        if ( 1 < m_stateInfo.nBarDuration ) {
          m_state = EState::entry_cancelling;
          //CancelOrders();
          m_pIB->CancelOrder( m_pOrderEntry ); // TODO: need to check that the two exit orders are also cancelled
        }
        break;
      case EState::entry_cancelling:
        // either HandleOrderCancelled or HandleOrderFilled will move to the next state
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
    m_tfLatest = CurrentTimeFrame();
    m_pPosition->ClosePosition();
  }
  else {

  }
}

void Strategy::EmitBarSummary() {
  std::cout << "bar count: " << m_mapMatching.size() << std::endl;
  std::for_each(
    m_mapMatching.begin(), m_mapMatching.end(),
    [](mapMatching_t::value_type& vt){
      const Results& longs(  vt.second.longs  );
      const Results& shorts( vt.second.shorts );
      std::cout
        << vt.second.cntInstances << "," << ( longs.cntOrders + shorts.cntOrders )
        << "," << vt.first.high << "," << vt.first.low << "," << vt.first.close << "," << vt.first.volume
        << "," << longs.cntOrders
               << "," << longs.cntWins   << "," << longs.dblProfit
               << "," << longs.cntLosses << "," << longs.dblLoss
        << "," << shorts.cntOrders
               << "," << shorts.cntWins   << "," << shorts.dblProfit
               << "," << shorts.cntLosses << "," << shorts.dblLoss
        << std::endl;
    }
    );
}

void Strategy::HandleUnRealizedPL( const ou::tf::Position::PositionDelta_delegate_t& delta ) {
  //std::cout << "unrealized p/l from " << delta.get<1>() << " to " << delta.get<2>() << std::endl;
}

void Strategy::HandleExecution( const ou::tf::Position::PositionDelta_delegate_t& delta ) {
  std::cout << "realized p/l from " << delta.get<1>() << " to " << delta.get<2>() << std::endl;
}
