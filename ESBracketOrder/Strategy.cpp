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
: ou::ChartDVBasics(), m_idOrderNext( 1 )
{
  m_pIB = boost::dynamic_pointer_cast<ou::tf::IBTWS>( pWatch->GetProvider() );
  m_pWatch = pWatch;
  m_pWatch->OnQuote.Add( MakeDelegate( this, &Strategy::HandleQuote ) );
  m_pWatch->OnTrade.Add( MakeDelegate( this, &Strategy::HandleTrade ) );
}

Strategy::~Strategy() {
  m_pWatch->OnQuote.Remove( MakeDelegate( this, &Strategy::HandleQuote ) );
  m_pWatch->OnTrade.Remove( MakeDelegate( this, &Strategy::HandleTrade ) );
}

void Strategy::HandleButtonUpdate() {
}

void Strategy::HandleButtonSend( ou::tf::OrderSide::enumOrderSide side ) {
//void Strategy::HandleButtonSend() {
  // TODO: need to track orders, nothing new while existing ones active?
  //ou::tf::OrderSide::enumOrderSide side( ou::tf::OrderSide::Buy );
  if ( 0.0 < m_tradeLast.Price() ) {
    switch ( side ) {
      case ou::tf::OrderSide::enumOrderSide::Buy:
        m_pOrderEntry
          = boost::make_shared<ou::tf::Order>(
              m_pWatch->GetInstrument(),
              ou::tf::OrderType::enumOrderType::Limit,
              ou::tf::OrderSide::enumOrderSide::Buy,
              1,
              m_tradeLast.Price()
              // idPosition
              // dt order submitted
              );
        m_pOrderEntry->SetOrderId( m_idOrderNext++ );
        m_pOrderProfit
          = boost::make_shared<ou::tf::Order>(
              m_pWatch->GetInstrument(),
              ou::tf::OrderType::enumOrderType::Limit,
              ou::tf::OrderSide::enumOrderSide::Sell,
              1,
              m_tradeLast.Price() + 1.00
              // idPosition
              // dt order submitted
              );
        m_pOrderProfit->SetOrderId( m_idOrderNext++ );
        m_pOrderStop
          = boost::make_shared<ou::tf::Order>(
              m_pWatch->GetInstrument(),
              ou::tf::OrderType::enumOrderType::Stop,
              ou::tf::OrderSide::enumOrderSide::Sell,
              1,
              m_tradeLast.Price() - 1.00
              // idPosition
              // dt order submitted
              );
        m_pOrderStop->SetOrderId( m_idOrderNext++ );
        break;
      case ou::tf::OrderSide::enumOrderSide::Sell:
        m_pOrderEntry
          = boost::make_shared<ou::tf::Order>(
              m_pWatch->GetInstrument(),
              ou::tf::OrderType::enumOrderType::Market,
              ou::tf::OrderSide::enumOrderSide::Sell,
              1
              // idPosition
              // dt order submitted
              );
        m_pOrderProfit
          = boost::make_shared<ou::tf::Order>(
              m_pWatch->GetInstrument(),
              ou::tf::OrderType::enumOrderType::Limit,
              ou::tf::OrderSide::enumOrderSide::Buy,
              1,
              m_tradeLast.Price() - 1.00
              // idPosition
              // dt order submitted
              );
        m_pOrderStop
          = boost::make_shared<ou::tf::Order>(
              m_pWatch->GetInstrument(),
              ou::tf::OrderType::enumOrderType::Stop,
              ou::tf::OrderSide::enumOrderSide::Buy,
              1,
              m_tradeLast.Price() + 1.00
              // idPosition
              // dt order submitted
              );
        break;
    }
    m_pIB->PlaceBracketOrder( m_pOrderEntry, m_pOrderProfit, m_pOrderStop );
  }
}

void Strategy::HandleButtonCancel() {
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
  ou::ChartDVBasics::HandleTrade( trade );
}
