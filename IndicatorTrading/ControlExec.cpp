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
 * File:    ControlExec.cpp
 * Author:  raymond@burkholder.net
 * Project: IndicatorTrading
 * Created: 2022/11/21 14:59:32
 */

#include <map>

#include <TFVuTrading/MarketDepth/PanelTrade.hpp>

#include "ControlExec.hpp"
#include "InteractiveChart.h"

ControlExec::ControlExec( pPosition_t pPosition, unsigned int nDefaultOrder )
: m_pPanelTrade( nullptr )
, m_pInteractiveChart( nullptr )
, m_nDefaultOrder( nDefaultOrder )
, m_pPosition( std::move( pPosition ) )
{
}

// TODO: much of this shouild be moved to ModelExec
void ControlExec::Set( ou::tf::l2::PanelTrade* pPanelTrade ) {
  m_pPanelTrade = pPanelTrade;

  if ( m_pPanelTrade ) {
    m_pPanelTrade->Set( // TODO set only one map is active
      [this](double price){ // fBidPlace
        mapOrders_t::iterator iterOrders = m_mapBidOrders.find( price );
        if ( m_mapBidOrders.end() == iterOrders ) {
          pOrder_t pOrder = m_pPosition->PlaceOrder(
            ou::tf::OrderType::Limit, ou::tf::OrderSide::Buy, m_nDefaultOrder, price );
          std::cout << "Submitted order#" << pOrder->GetOrderId() << " at bid " << price << std::endl;
          auto pair = m_mapBidOrders.emplace( price, PriceLevelOrder() );
          assert( pair.second );
          PriceLevelOrder& plo( pair.first->second );
          plo.Set( // fUpdateQuantity_t
            [this,price]( unsigned int quantity ){
              m_pPanelTrade->SetBid( price, quantity ); // set with plo instead
            }
          );
          plo = pOrder;
        }
        else {
        }
      },
      [this](double price){ // fBidCancel
        mapOrders_t::iterator iterOrders = m_mapBidOrders.find( price );
        if ( m_mapBidOrders.end() == iterOrders ) {}
        else {
          pOrder_t pOrder = iterOrders->second.Order();
          m_pPosition->CancelOrder( pOrder->GetOrderId() );
          m_mapBidOrders.erase( iterOrders ); // need elegant way to do this after cancellation
          m_pPanelTrade->SetBid( price, 0 );
        }
      },
      [this](double price){ // fAskPlace
        mapOrders_t::iterator iterOrders = m_mapAskOrders.find( price );
        if ( m_mapAskOrders.end() == iterOrders ) {
          pOrder_t pOrder = m_pPosition->PlaceOrder(
            ou::tf::OrderType::Limit, ou::tf::OrderSide::Sell, m_nDefaultOrder, price );
          std::cout << "Submitted order#" << pOrder->GetOrderId() << " at ask " << price << std::endl;
          auto pair = m_mapAskOrders.emplace( price, PriceLevelOrder() );
          assert( pair.second );
          PriceLevelOrder& plo( pair.first->second );
          plo.Set( // fUpdateQuantity_t
            [this,price]( unsigned int quantity ){
              m_pPanelTrade->SetAsk( price, quantity ); // set with plo instead
            }
          );
          plo = pOrder;
        }
        else {
        }
      },
      [this](double price){ // fAskCancel
        mapOrders_t::iterator iterOrders = m_mapAskOrders.find( price );
        if ( m_mapAskOrders.end() == iterOrders ) {}
        else {
          pOrder_t pOrder = iterOrders->second.Order();
          m_pPosition->CancelOrder( pOrder->GetOrderId() );
          m_mapAskOrders.erase( iterOrders ); // need elegant way to do this after cancellation
          m_pPanelTrade->SetAsk( price, 0 );
        }
      }
    );
  }
}

void ControlExec::Set( InteractiveChart* pInteractiveChart ) {
  m_pInteractiveChart = pInteractiveChart;
  if ( m_pInteractiveChart ) {
  }
}
