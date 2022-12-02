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
 * File:    ExecutionControl.cpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFVuTrading/MarketDepth
 * Created: 2022/11/21 14:59:32
 */

#include <OUCommon/Colour.h>

#include "Fields.hpp"
#include "PanelTrade.hpp"
#include "ExecutionControl.hpp"

namespace {

  enum Index { NoOrder, LimitTracking, LimitSubmitted, StopTracking, StopSubmitted };

  // https://en.wikipedia.org/wiki/Web_colors
  using EColour = ou::Colour::wx::EColour;
  std::vector<EColour> OrderColours {
    EColour::LightYellow,    // no order
    EColour::Turquoise,      // limit, tracking
    EColour::LightSeaGreen,  // limit, submitted
    EColour::LightSalmon,    // stop, tracking
    EColour::IndianRed       // stop, submitted
  };

  //enum OrderColour {
  //  NoOrder=EColour::LightYellow
  //, LimitTracking = EColour::Turquoise
  //, LimitSubmitted = EColour::LightSeaGreen
  //, StopTracking = EColour::LightSalmon
  //, StopSubmitted = EColour::IndianRed
  //};
}

namespace ou {
namespace tf {
namespace l2 {

ExecutionControl::ExecutionControl( pPosition_t pPosition, unsigned int nDefaultOrder )
: m_pPanelTrade( nullptr )
, m_nDefaultOrder( nDefaultOrder )
, m_pPosition( std::move( pPosition ) )
{}

ExecutionControl::~ExecutionControl() {
    // TODO: will need a controlled cancellation of all orders
  if ( 0 != m_mapAskOrders.size() ) {
    std::cout << "ExecutionControl: outstanding ask orders in limbo" << std::endl;
    m_mapAskOrders.clear();
  }

  if ( 0 != m_mapBidOrders.size() ) {
    std::cout << "ExecutionControl: outstanding bid orders in limbo" << std::endl;
    m_mapBidOrders.clear();
  }

  if ( m_pPanelTrade ) {
    m_pPanelTrade->Set( nullptr );
    m_pPanelTrade = nullptr;
  }
}

// TODO: much of this shouild be moved to ExecModel, and leave the button decoding here
// Consider: to simulate market maker, orders are submitted and retracted automatically
//   based upon at which bid/ask rung the current price is nearest
//   - since stops need to be simulated outside of market hours, maybe simulate/track all orders
// TODO: use ExecutionModel to handle order submission/retraction?
//  * can take the feed for auto-submission
//  * then forms basis for any automated decision making
//  * light colour: tracking order locally
//  * dark colour:  order at exchange
//  * green:  limit order (light/dark)
//  * red:    stop order (light/dark)
void ExecutionControl::Set( ou::tf::l2::PanelTrade* pPanelTrade ) {

  m_pPanelTrade = pPanelTrade;

  if ( m_pPanelTrade ) {
    using EButton = ou::tf::Mouse::EButton;
    using EField  = ou::tf::l2::EField;
    m_pPanelTrade->Set(
      [this]( double price, EField field, EButton button, bool shift, bool control, bool alt ){
        switch ( field ) {
          case EField::AskOrder:
            switch ( button ) {
              case PriceRow::EButton::Left:
                if ( shift ) {
                  AskStop( price ); // need to simulate submission
                }
                else {
                  AskLimit( price );
                }
                break;
              case PriceRow::EButton::Middle:
                break;
              case PriceRow::EButton::Right:
                if ( shift ) {
                  // TODO: cancel all orders on this side
                }
                else {
                  AskCancel( price );
                }
                break;
            }
            break;
          case EField::BidOrder:
            switch ( button ) {
              case PriceRow::EButton::Left:
                if ( shift ) {
                  BidStop( price ); // need to simulate submission
                }
                else {
                  BidLimit( price );
                }
                break;
              case PriceRow::EButton::Middle:
                break;
              case PriceRow::EButton::Right:
                if ( shift ) {
                  // TODO: cancel all orders on this side
                }
                else {
                  BidCancel( price );
                }
                break;
            }
            break;
          default:
            assert( false );
            break;
        }

      } );
  }

}

// on each click, to increase quantity, cancel order & re-submit with new quantity
void ExecutionControl::AskLimit( double price ) {
  mapOrders_t::iterator iterOrders = m_mapAskOrders.find( price );
  if ( m_mapAskOrders.end() == iterOrders ) {
    pOrder_t pOrder = m_pPosition->PlaceOrder(
      ou::tf::OrderType::Limit, ou::tf::OrderSide::Sell, m_nDefaultOrder, price );
    std::cout << "Submitted limit order#" << pOrder->GetOrderId() << " at ask " << price << std::endl;
    auto pair = m_mapAskOrders.emplace( price, PriceLevelOrder() );
    assert( pair.second );
    mapOrders_t::iterator iterOrders( pair.first );
    PriceLevelOrder& plo( iterOrders->second );
    plo.Set( // fUpdateQuantity_t
      [this,price,iterOrders]( unsigned int quantity ){
        m_pPanelTrade->SetAsk( price, quantity, OrderColours[ 0 == quantity ? NoOrder : LimitSubmitted ] ); // set with plo instead
        if ( 0 == quantity ) { // based upon cancel, or fulfillment
          m_KillPriceLevelOrder = std::move( iterOrders->second );
          m_mapAskOrders.erase( iterOrders );
        }
      }
    );
    plo = pOrder;
  }
  else {
    std::cout << "order (ask) " << iterOrders->second.Order()->GetOrderId() << " exists" << std::endl;
  }
}

// on futures, only available during regular trading hours, will need to be simulated
// can't do a -1 on the order status, use colour instead
void ExecutionControl::AskStop( double price ) {
  mapOrders_t::iterator iterOrders = m_mapAskOrders.find( price );
  if ( m_mapAskOrders.end() == iterOrders ) {
    pOrder_t pOrder = m_pPosition->PlaceOrder(
      ou::tf::OrderType::Stop, ou::tf::OrderSide::Buy, m_nDefaultOrder, price );
    std::cout << "Submitted stop order#" << pOrder->GetOrderId() << " at ask " << price << std::endl;
    auto pair = m_mapAskOrders.emplace( price, PriceLevelOrder() );
    assert( pair.second );
    mapOrders_t::iterator iterOrders( pair.first );
    PriceLevelOrder& plo( iterOrders->second );
    plo.Set( // fUpdateQuantity_t
      [this,price,iterOrders]( unsigned int quantity ){
        m_pPanelTrade->SetAsk( price, quantity, OrderColours[ 0 == quantity ? NoOrder : StopTracking ] ); // set with plo instead
        if ( 0 == quantity ) { // based upon cancel, or fulfillment
          m_KillPriceLevelOrder = std::move( iterOrders->second );
          m_mapAskOrders.erase( iterOrders );
        }
      }
    );
    plo = pOrder;
  }
  else {
    std::cout << "order (ask) " << iterOrders->second.Order()->GetOrderId() << " exists" << std::endl;
  }
}

void ExecutionControl::AskCancel( double price ) {
  mapOrders_t::iterator iterOrders = m_mapAskOrders.find( price );
  if ( m_mapAskOrders.end() == iterOrders ) {}
  else {
    pOrder_t pOrder = iterOrders->second.Order();
    m_pPosition->CancelOrder( pOrder->GetOrderId() );
  }
}

// on each click, to increase quantity, cancel order & re-submit with new quantity
void ExecutionControl::BidLimit( double price ) {
  mapOrders_t::iterator iterOrders = m_mapBidOrders.find( price );
  if ( m_mapBidOrders.end() == iterOrders ) {
    pOrder_t pOrder = m_pPosition->PlaceOrder(
      ou::tf::OrderType::Limit, ou::tf::OrderSide::Buy, m_nDefaultOrder, price );
    std::cout << "Submitted limit order#" << pOrder->GetOrderId() << " at bid " << price << std::endl;
    auto pair = m_mapBidOrders.emplace( price, PriceLevelOrder() );
    assert( pair.second );
    mapOrders_t::iterator iterOrders( pair.first );
    PriceLevelOrder& plo( iterOrders->second );
    plo.Set( // fUpdateQuantity_t
      [this,price,iterOrders]( unsigned int quantity ){
        m_pPanelTrade->SetBid( price, quantity, OrderColours[ 0 == quantity ? NoOrder : LimitSubmitted ] ); // set with plo instead
        if ( 0 == quantity ) { // based upon cancel, or fulfillment
          m_KillPriceLevelOrder = std::move( iterOrders->second );
          m_mapBidOrders.erase( iterOrders );
        }
      }
    );
    plo = pOrder;
  }
  else {
    std::cout << "order (bid) " << iterOrders->second.Order()->GetOrderId() << " exists" << std::endl;
  }
}

// on futures, only available during regular trading hours, will need to be simulated
// can't do a -1 on the order status, use colour instead
void ExecutionControl::BidStop( double price ) {
  mapOrders_t::iterator iterOrders = m_mapBidOrders.find( price );
  if ( m_mapBidOrders.end() == iterOrders ) {
    pOrder_t pOrder = m_pPosition->PlaceOrder(
      ou::tf::OrderType::Stop, ou::tf::OrderSide::Sell, m_nDefaultOrder, price );
    std::cout << "Submitted stop order#" << pOrder->GetOrderId() << " at bid " << price << std::endl;
    auto pair = m_mapBidOrders.emplace( price, PriceLevelOrder() );
    assert( pair.second );
    mapOrders_t::iterator iterOrders( pair.first );
    PriceLevelOrder& plo( iterOrders->second );
    plo.Set( // fUpdateQuantity_t
      [this,price,iterOrders]( unsigned int quantity ){
        m_pPanelTrade->SetBid( price, quantity, OrderColours[ 0 == quantity ? NoOrder : StopTracking ] ); // set with plo instead
        if ( 0 == quantity ) { // based upon cancel, or fulfillment
          m_KillPriceLevelOrder = std::move( iterOrders->second );
          m_mapBidOrders.erase( iterOrders );
        }
      }
    );
    plo = pOrder;
  }
  else {
    std::cout << "order (bid) " << iterOrders->second.Order()->GetOrderId() << " exists" << std::endl;
  }
}

void ExecutionControl::BidCancel( double price ) {
  mapOrders_t::iterator iterOrders = m_mapBidOrders.find( price );
  if ( m_mapBidOrders.end() == iterOrders ) {}
  else {
    pOrder_t pOrder = iterOrders->second.Order();
    m_pPosition->CancelOrder( pOrder->GetOrderId() );
  }
}


} // market depth
} // namespace tf
} // namespace ou
