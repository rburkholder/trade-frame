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
 * File:    PriceLevelOrder.cpp (from AppDoM)
 * Author:  raymond@burkholder.net
 * Project: lib/TFVuTrading/MarketDepth
 * Created: 2022/11/27 12:41:04
 */

 #include "PriceLevelOrder.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth

PriceLevelOrder::PriceLevelOrder() {}

PriceLevelOrder::PriceLevelOrder( pOrder_t pOrder )
: m_pOrder( pOrder )
{
  if ( m_fUpdateQuantity ) m_fUpdateQuantity( m_pOrder->GetQuanRemaining() );
  SetEvents();
}

PriceLevelOrder::PriceLevelOrder( PriceLevelOrder&& rhs ) {
  rhs.ClearEvents();
  m_pOrder = std::move( rhs.m_pOrder );
  m_fUpdateQuantity = std::move( rhs.m_fUpdateQuantity );
  SetEvents();
}

PriceLevelOrder& PriceLevelOrder::operator=( pOrder_t pOrder ) {
  ClearEvents();
  m_pOrder = std::move( pOrder );
  if ( m_fUpdateQuantity ) m_fUpdateQuantity( m_pOrder->GetQuanRemaining() );
  SetEvents();
  return *this;
}

PriceLevelOrder& PriceLevelOrder::operator=( PriceLevelOrder&& rhs ) {
  if ( this != &rhs ) {
    rhs.ClearEvents();
    m_pOrder = std::move( rhs.m_pOrder );
    m_fUpdateQuantity = std::move( rhs.m_fUpdateQuantity );
    SetEvents();
  }
  return *this;
}

PriceLevelOrder::~PriceLevelOrder() {
  ClearEvents();
  m_pOrder.reset();
  m_fUpdateQuantity = nullptr;
}

void PriceLevelOrder::Set( fUpdateQuantity_t&& fUpdateQuantity, fExecution_t&& fExecution ) {
  m_fUpdateQuantity = std::move( fUpdateQuantity );
  m_fExecution = std::move( fExecution );
}

void PriceLevelOrder::SetEvents() {
  if ( m_pOrder ) {
    m_pOrder->OnPartialFill.Add( MakeDelegate( this, &PriceLevelOrder::HandleOnPartialFill ) );
    m_pOrder->OnOrderFilled.Add( MakeDelegate( this, &PriceLevelOrder::HandleOnOrderFilled ) );
    m_pOrder->OnOrderCancelled.Add( MakeDelegate( this, &PriceLevelOrder::HandleOnOrderCancelled ) );
    m_pOrder->OnExecution.Add( MakeDelegate( this, &PriceLevelOrder::HandleOnOrderExecution ) );
  }
}

void PriceLevelOrder::ClearEvents() {
  if ( m_pOrder ) {
    m_pOrder->OnPartialFill.Remove( MakeDelegate( this, &PriceLevelOrder::HandleOnPartialFill ) );
    m_pOrder->OnOrderFilled.Remove( MakeDelegate( this, &PriceLevelOrder::HandleOnOrderFilled ) );
    m_pOrder->OnOrderCancelled.Remove( MakeDelegate( this, &PriceLevelOrder::HandleOnOrderCancelled ) );
    m_pOrder->OnExecution.Remove( MakeDelegate( this, &PriceLevelOrder::HandleOnOrderExecution ) );
  }
}

void PriceLevelOrder::HandleOnPartialFill( const ou::tf::Order& order ) { // only intermediate fill?
if ( m_fUpdateQuantity ) m_fUpdateQuantity( m_pOrder->GetQuanRemaining() );
  std::cout << "Partial fill order#" << order.GetOrderId() << " quan " << order.GetQuanFilled() << std::endl;
}

void PriceLevelOrder::HandleOnOrderFilled( const ou::tf::Order& order ) { // only final fill?
  if ( m_fUpdateQuantity ) m_fUpdateQuantity( m_pOrder->GetQuanRemaining() );
  std::cout << "Filled order#" << order.GetOrderId() << " quan " << order.GetQuanFilled() << std::endl;
}

void PriceLevelOrder::HandleOnOrderCancelled( const ou::tf::Order& order ) {
  if ( m_fUpdateQuantity ) m_fUpdateQuantity( 0 );
  std::cout << "Cancelled order#" << order.GetOrderId() << " quan " << order.GetQuanOrdered() << std::endl;
}

void PriceLevelOrder::HandleOnOrderExecution( const std::pair<const ou::tf::Order&, const ou::tf::Execution&>& pair ) {
  std::cout << "Execution order#" << pair.first.GetOrderId() << " of " << pair.second.GetSize() << std::endl;
  if ( m_fExecution ) m_fExecution( pair.second );
}

} // market depth
} // namespace tf
} // namespace ou
