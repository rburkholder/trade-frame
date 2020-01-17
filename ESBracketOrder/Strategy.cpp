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

Strategy::Strategy( pWatch_t pWatch )
: ou::ChartDVBasics(), m_bfBar( 20 ), m_dblAverageBarSize {}, m_cntBars {}
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
  const double dblOffset( 2.0 * m_dblAverageBarSize );
  const double dblEntry = m_tradeLast.Price();
  const double dblUpper = m_pWatch->GetInstrument()->NormalizeOrderPrice( dblEntry + dblOffset );
  const double dblLower = m_pWatch->GetInstrument()->NormalizeOrderPrice( dblEntry - dblOffset );
  if ( 0.0 < m_tradeLast.Price() ) {
    switch ( side ) {
      case ou::tf::OrderSide::enumOrderSide::Buy:
        m_pOrderEntry = m_pPosition->ConstructOrder(
          ou::tf::OrderType::enumOrderType::Limit,
          ou::tf::OrderSide::enumOrderSide::Buy,
          1,
          dblEntry
          // idPosition
          // dt order submitted
          );
        ou::ChartDVBasics::m_ceLongEntries.AddLabel( m_tradeLast.DateTime(), dblEntry, "long" );
        m_pOrderProfit = m_pPosition->ConstructOrder(
          ou::tf::OrderType::enumOrderType::Limit,
          ou::tf::OrderSide::enumOrderSide::Sell,
          1,
          dblUpper
          // idPosition
          // dt order submitted
          );
        ou::ChartDVBasics::m_ceLongEntries.AddLabel( m_tradeLast.DateTime(), dblUpper, "profit" );
        m_pOrderStop = m_pPosition->ConstructOrder(
          ou::tf::OrderType::enumOrderType::Stop,
          ou::tf::OrderSide::enumOrderSide::Sell,
          1,
          dblLower
          // idPosition
          // dt order submitted
          );
        ou::ChartDVBasics::m_ceLongEntries.AddLabel( m_tradeLast.DateTime(), dblLower, "loss" );
        break;
      case ou::tf::OrderSide::enumOrderSide::Sell:
        m_pOrderEntry = m_pPosition->ConstructOrder(
          ou::tf::OrderType::enumOrderType::Limit,
          ou::tf::OrderSide::enumOrderSide::Sell,
          1,
          dblEntry
          // idPosition
          // dt order submitted
          );
        ou::ChartDVBasics::m_ceLongEntries.AddLabel( m_tradeLast.DateTime(), dblEntry, "short" );
        m_pOrderProfit = m_pPosition->ConstructOrder(
          ou::tf::OrderType::enumOrderType::Limit,
          ou::tf::OrderSide::enumOrderSide::Buy,
          1,
          dblLower
          // idPosition
          // dt order submitted
          );
        ou::ChartDVBasics::m_ceLongEntries.AddLabel( m_tradeLast.DateTime(), dblLower, "profit" );
        m_pOrderStop = m_pPosition->ConstructOrder(
          ou::tf::OrderType::enumOrderType::Stop,
          ou::tf::OrderSide::enumOrderSide::Buy,
          1,
          dblUpper
          // idPosition
          // dt order submitted
          );
        ou::ChartDVBasics::m_ceLongEntries.AddLabel( m_tradeLast.DateTime(), dblUpper, "loss" );
        break;
    }
    // TOOD: place through OrderManager at some point
    m_pIB->PlaceBracketOrder( m_pOrderEntry, m_pOrderProfit, m_pOrderStop );
  }
}

void Strategy::HandleButtonCancel() {
  m_pPosition->CancelOrders();
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
  // at 20 seconds, will take 3 - 4 minutes to stabilize
  m_dblAverageBarSize = 0.9 * m_dblAverageBarSize + 0.1 * ( bar.High() - bar.Low() );
  if ( 0 < m_cntBars ) {
    BarMatching bm;
    bm.Compare( m_barLast, bar );
    OrderResults ors;
    mapMatching_pair_t pair = m_mapMatching.try_emplace( bm, ors );
    pair.first->second.cntBars++;
    std::cout
      << "bar "
      << m_mapMatching.size()
      << " "
      << ( pair.second ? "insert" : "update" )
      << " "
      << pair.first->second.cntBars
      << std::endl;
  }
  m_cntBars++;
  m_barLast = bar;
}

void Strategy::HandleUnRealizedPL( const ou::tf::Position::PositionDelta_delegate_t& delta ) {
  //std::cout << "unrealized p/l from " << delta.get<1>() << " to " << delta.get<2>() << std::endl;
}

void Strategy::HandleExecution( const ou::tf::Position::PositionDelta_delegate_t& delta ) {
  std::cout << "realized p/l from " << delta.get<1>() << " to " << delta.get<2>() << std::endl;
}
