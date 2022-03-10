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
 * File:    TradeLifeTime.cpp
 * Author:  raymond@burkholder.net
 * Project: IndicatorTrading
 * Created: March 9, 2022 16:38
 */

#include <TFVuTrading/PanelOrderButtons_structs.h>

#include "TradeLifeTime.h"

namespace {

ou::tf::OrderType::enumOrderType XlateOrderType( ou::tf::PanelOrderButtons_Order::EPositionEntryMethod method ) {
  ou::tf::OrderType::enumOrderType eOrderType;
  switch ( method ) {
    case ou::tf::PanelOrderButtons_Order::EPositionEntryMethod::Market:
      eOrderType = ou::tf::OrderType::Market;
      break;
    case ou::tf::PanelOrderButtons_Order::EPositionEntryMethod::Limit:
      eOrderType = ou::tf::OrderType::Limit;
      break;
    default:
      eOrderType = ou::tf::OrderType::Market;
      break;
  }
  return eOrderType;
}

}

TradeLifeTime::TradeLifeTime( pPosition_t pPosition, const ou::tf::PanelOrderButtons_Order& order )
: m_statePosition( EPositionState::Looking ), m_pPosition( pPosition )
{
}

TradeLifeTime::~TradeLifeTime() {
}

void TradeLifeTime::HandleOrderCancelled( const ou::tf::Order& order ) {
  std::cout << "order " << order.GetOrderId() << " cancelled" << std::endl;
  switch ( m_statePosition ) {
    case EPositionState::Looking:
      break;
    case EPositionState::Buying:
      break;
    case EPositionState::Long:
      break;
    case EPositionState::Selling:
      break;
    case EPositionState::Short:
      break;
  }
}

void TradeLifeTime::HandleOrderFilled( const ou::tf::Order& order ) {
  std::cout << "order " << order.GetOrderId() << " filled" << std::endl;
  m_statePosition = EPositionState::Looking;
}

// =====

TradeWithABuy::TradeWithABuy( pPosition_t pPosition, const ou::tf::PanelOrderButtons_Order& order )
: TradeLifeTime( pPosition, order )
{
  std::cout << "buying" << std::endl;
  m_statePosition = EPositionState::Buying;
  m_pOrderEntry = m_pPosition->ConstructOrder( XlateOrderType( order.m_ePositionEntryMethod ), ou::tf::OrderSide::Buy, 1 );
  m_pOrderEntry->OnOrderCancelled.Add( MakeDelegate( this, &TradeWithABuy::HandleOrderCancelled ) );
  m_pOrderEntry->OnOrderFilled.Add( MakeDelegate( this, &TradeWithABuy::HandleOrderFilled ) );
  //m_ceLongEntry.AddLabel( m_quote.DateTime(), m_quote.Midpoint(), "Buy Submit" );
  m_pPosition->PlaceOrder( m_pOrderEntry );
}

void TradeWithABuy::HandleOrderCancelled( const ou::tf::Order& order ) {
  TradeLifeTime::HandleOrderCancelled( order );
}

void TradeWithABuy::HandleOrderFilled( const ou::tf::Order& order ) {
  TradeLifeTime::HandleOrderFilled( order );
  //m_ceLongFill.AddLabel( m_quote.DateTime(), m_quote.Midpoint(), "Buy Fill" );
  //m_ceShortFill.AddLabel( m_quote.DateTime(), m_quote.Midpoint(), "Sell Fill" );
}

// =====

TradeWithASell::TradeWithASell( pPosition_t pPosition, const ou::tf::PanelOrderButtons_Order& order )
: TradeLifeTime( pPosition, order )
{
  std::cout << "selling" << std::endl;
  m_statePosition = EPositionState::Selling;
  m_pOrderEntry = m_pPosition->ConstructOrder( XlateOrderType( order.m_ePositionEntryMethod ), ou::tf::OrderSide::Sell, 1 );
  m_pOrderEntry->OnOrderCancelled.Add( MakeDelegate( this, &TradeWithASell::HandleOrderCancelled ) );
  m_pOrderEntry->OnOrderFilled.Add( MakeDelegate( this, &TradeWithASell::HandleOrderFilled ) );
  //m_ceLongEntry.AddLabel( m_quote.DateTime(), m_quote.Midpoint(), "Sell Submit" );
  m_pPosition->PlaceOrder( m_pOrderEntry );
}

void TradeWithASell::HandleOrderCancelled( const ou::tf::Order& order ) {
  TradeLifeTime::HandleOrderCancelled( order );
}

void TradeWithASell::HandleOrderFilled( const ou::tf::Order& order ) {
  TradeLifeTime::HandleOrderFilled( order );
  //m_ceLongFill.AddLabel( m_quote.DateTime(), m_quote.Midpoint(), "Buy Fill" );
  //m_ceShortFill.AddLabel( m_quote.DateTime(), m_quote.Midpoint(), "Sell Fill" );
}

