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

#include "TFTrading/TradingEnumerations.h"

#include "Strategy.h"

namespace {
  static const size_t nBars { 10 };
}

Strategy::Strategy( pWatch_t pWatch )
: ou::ChartDVBasics()
, m_bfBar( 20 )
, m_dblAverageBarSize {}
, m_cntBars {}
, m_state( EState::initial )
{
  m_bfBar.SetOnBarComplete( MakeDelegate( this, &Strategy::HandleBarComplete ) );
  m_pIB = boost::dynamic_pointer_cast<ou::tf::IBTWS>( pWatch->GetProvider() );
  m_pWatch = pWatch;
  m_pWatch->OnQuote.Add( MakeDelegate( this, &Strategy::HandleQuote ) );
  m_pWatch->OnTrade.Add( MakeDelegate( this, &Strategy::HandleTrade ) );
  m_pPosition = boost::make_shared<ou::tf::Position>( m_pWatch, m_pIB );
  m_pPosition->OnUnRealizedPL.Add( MakeDelegate( this, &Strategy::HandleUnRealizedPL ) );
  m_pPosition->OnExecution.Add( MakeDelegate( this, &Strategy::HandleExecution ) );
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
  const double dblEntry = m_tradeLast.Price();
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
          dblEntry
          // idPosition
          // dt order submitted
          );
        m_pOrderEntry->SetDescription( "long" );
        //m_pOrderEntry->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
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
          dblEntry - dblLower
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
          dblEntry
          // idPosition
          // dt order submitted
          );
        m_pOrderEntry->SetDescription( "short" );
        //m_pOrderEntry->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
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
          dblUpper - dblEntry
          // idPosition
          // dt order submitted
          );
        m_pOrderStop->SetDescription( "loss" );
        //m_pOrderStop->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
        m_pOrderStop->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );

        break;
    }
    // TOOD: place through OrderManager at some point
    m_pIB->PlaceBracketOrder( m_pOrderEntry, m_pOrderProfit, m_pOrderStop );
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
  }
}

void Strategy::HandleTrade( const ou::tf::Trade &trade ) {
  //std::cout << "trade: " << trade.Volume() << "@" << trade.Price() << std::endl;
  m_tradeLast = trade;
  m_bfBar.Add( trade );
  ou::ChartDVBasics::HandleTrade( trade );
}

void Strategy::HandleBarComplete( const ou::tf::Bar& bar ) {
  // threading problem here with gui looking at m_mapMatching
  // at 20 seconds, will take 3 - 4 minutes to stabilize
  m_dblAverageBarSize = 0.9 * m_dblAverageBarSize + 0.1 * ( bar.High() - bar.Low() );
  if ( 0 < m_cntBars ) {
    BarMatching key;
    key.Set( m_barLast, bar );
    OrderResults orsBlank;
    mapMatching_pair_t pair = m_mapMatching.try_emplace( key, orsBlank );
    pair.first->second.cntInstances++;
    switch ( m_state ) {
      case EState::initial:
        if ( nBars <= m_cntBars ) m_state = EState::entry_wait;
        break;
      case EState::entry_wait:
        {
          mapMatching_t::iterator entry( pair.first );
          m_keyMapMatching = entry->first;
          switch ( m_keyMapMatching.close ) {
            case 1:
              m_stateInfo.barMatching = key;
              m_state = EState::entry_filling;
              HandleButtonSend( ou::tf::OrderSide::Buy );
              break;
            case 0:
              // no entry
              break;
            case -1:
              m_stateInfo.barMatching = key;
              m_state = EState::entry_filling;
              HandleButtonSend( ou::tf::OrderSide::Sell );
              break;
          }
        }
        break;
      case EState::entry_filling:
        // managed in HandleOrderFilled
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

void Strategy::HandleOrderCancelled( const ou::tf::Order& order ) {
}

void Strategy::HandleOrderFilled( const ou::tf::Order& order ) {
  std::string sMessage = "unknown ";
  switch ( m_state ) {
    case EState::initial:
      break;
    case EState::entry_wait:
      sMessage = "entry wait ";
      break;
    case EState::entry_filling:
      sMessage = "entry filling ";
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
              entry->second.longs.dblProfit -= ( m_stateInfo.dblEntryPrice - order.GetAverageFillPrice() );
            }
            ou::ChartDVBasics::m_ceShortExits.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), sMessage + "filled" );
            break;
          case ou::tf::OrderSide::Sell:
            entry->second.shorts.cntOrders++;
            if ( "profit" == order.GetDescription() ) {
              entry->second.shorts.cntWins++;
              entry->second.longs.dblProfit += ( m_stateInfo.dblEntryPrice - order.GetAverageFillPrice() );
            }
            if ( "loss" == order.GetDescription() ) {
              entry->second.longs.cntLosses++;
              entry->second.longs.dblProfit -= ( order.GetAverageFillPrice() - m_stateInfo.dblEntryPrice );
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
