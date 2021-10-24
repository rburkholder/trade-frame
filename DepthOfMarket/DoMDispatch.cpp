/************************************************************************
 * Copyright(c) 2021, One Unified. All rights reserved.                 *
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
 * File:    DoMDispatch.cpp
 * Author:  raymond@burkholder.net
 * Project: DepthOfMarket
 * Created on October 17, 2021 11:45
 */

#include "DoMDispatch.h"

using inherited_t = ou::tf::iqfeed::l2::Dispatcher<DoMDispatch>;

DoMDispatch::DoMDispatch( const std::string& sWatch )
: m_sWatch( sWatch )
{
}

DoMDispatch::~DoMDispatch() {
}

void DoMDispatch::Connect() {
  inherited_t::Connect();
}

void DoMDispatch::Disconnect() {
  inherited_t::Disconnect();
}

void DoMDispatch::OnNetworkConnected() {
  StartMarketByOrder( m_sWatch );
}

void DoMDispatch::OnNetworkDisconnected() {
}

void DoMDispatch::OnMBOAdd( const ou::tf::iqfeed::l2::msg::OrderArrival::decoded& msg ) {

  assert( '3' == msg.chMsgType );

  mapOrder_t::iterator iter = m_mapOrder.find( msg.nOrderId );
  if ( m_mapOrder.end() != iter ) {
    std::cout << "map add order already exists: " << msg.nOrderId << std::endl;
  }
  else {
    m_mapOrder.emplace(
      std::pair(
        msg.nOrderId,
        Order( msg ) )
      );
  }

  switch ( msg.chOrderSide ) {
    case 'A':
      AuctionAdd( m_mapAuctionAsk, msg );
      break;
    case 'B':
      AuctionAdd( m_mapAuctionBid, msg );
      break;
  }
}

void DoMDispatch::OnMBOSummary( const ou::tf::iqfeed::l2::msg::OrderArrival::decoded& msg ) {
  OnMBOAdd( msg );  // will this work as expected?
}

void DoMDispatch::OnMBOUpdate( const ou::tf::iqfeed::l2::msg::OrderArrival::decoded& msg ) {

  assert( '4' == msg.chMsgType );

  mapOrder_t::iterator iter = m_mapOrder.find( msg.nOrderId );
  if ( m_mapOrder.end() == iter ) {
    std::cout << "map update order does not exist: " << msg.nOrderId << std::endl;
  }
  else {

    switch ( msg.chOrderSide ) {
      case 'A':
        AuctionUpdate( m_mapAuctionAsk, iter->second, msg );
        break;
      case 'B':
        AuctionUpdate( m_mapAuctionBid, iter->second, msg );
        break;
    }

    iter->second.nQuantity = msg.nQuantity;
  }

}

void DoMDispatch::OnMBODelete( const ou::tf::iqfeed::l2::msg::OrderDelete::decoded& msg ) {

  assert( '5' == msg.chMsgType );

  mapOrder_t::iterator iter = m_mapOrder.find( msg.nOrderId );
  if ( m_mapOrder.end() == iter ) {
    std::cout << "map order delete does not exist: " << msg.nOrderId << std::endl;
  }
  else {

    switch ( msg.chOrderSide ) {
      case 'A':
        AuctionDel( m_mapAuctionAsk, iter->second );
        break;
      case 'B':
        AuctionDel( m_mapAuctionBid, iter->second );
        break;
    }

    m_mapOrder.erase( iter );
  }

}

void DoMDispatch::AuctionAdd( mapAuction_t& map, const ou::tf::iqfeed::l2::msg::OrderArrival::decoded& msg ) {
  mapAuction_t::iterator iter = map.find( msg.dblPrice );
  if ( map.end() == iter ) {
    map.emplace(
      std::pair(
        msg.dblPrice,
        Auction( msg )
      )
    );
  }
  else {
    iter->second.nQuantity += msg.nQuantity;
  }
}

void DoMDispatch::AuctionUpdate( mapAuction_t& map, const Order& order, const ou::tf::iqfeed::l2::msg::OrderArrival::decoded& msg ) {
  mapAuction_t::iterator iter = map.find( order.dblPrice );
  if ( map.end() == iter ) {
    std::cout << "AuctionUpdate price not found: " << order.dblPrice << std::endl;
  }
  else {
    auto& nQuantity( iter->second.nQuantity );
    assert( nQuantity >= order.nQuantity );
    nQuantity += msg.nQuantity;
    nQuantity -= order.nQuantity;
  }
}

void DoMDispatch::AuctionDel( mapAuction_t& map, const Order& order ) {

  mapAuction_t::iterator iter = map.find( order.dblPrice );
  if ( map.end() == iter ) {
    std::cout << "AuctionDel price not found: " << order.dblPrice << std::endl;
  }
  else {
    iter->second.nQuantity -= order.nQuantity;
  }

}