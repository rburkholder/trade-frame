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
 * File:    Summary.cpp
 * Author:  raymond@burkholder.net
 * Project: TFIQFeed/Level2
 * Created on October 17, 2021 11:45
 */

#include "Summary.hpp"

using inherited_t = ou::tf::iqfeed::l2::Dispatcher<DoMDispatch>;

DoMDispatch::DoMDispatch( const std::string& sWatch )
: m_sWatch( sWatch )
{
}

DoMDispatch::~DoMDispatch() {
}

void DoMDispatch::Set( fVolumeAtPrice_t&& fBid, fVolumeAtPrice_t&& fAsk ) {
  m_fAskVolumeAtPrice = std::move( fAsk );
  m_fBidVolumeAtPrice = std::move( fBid );
}

void DoMDispatch::EmitMarketMakerMaps() {
  // will probably need a lock on this, as maps are in background thread
  // but mostly works as the deletion isn't in place yet

  // map to track price levels by market maker
  struct price_mm {
    double price;
    std::string mm;
    price_mm() : price {} {}
    price_mm( double price_, const std::string& mm_ )
    : price( price_ ), mm( mm_ ) {}
    bool operator()( const price_mm& lhs, const price_mm& rhs ) const {
      if ( lhs.price == rhs.price ) {
        return ( lhs.mm < rhs.mm );
      }
      else {
        return ( lhs.price < rhs.price );
      }
    }
  };
  using mapPriceLevels_t = std::map<price_mm, volume_t, price_mm>;
  mapPriceLevels_t mapPriceMMAsk;
  mapPriceLevels_t mapPriceMMBid;

  for ( const mapMM_t::value_type& vt: m_mapMMAsk ) {
    price_mm pmm( vt.second.price, vt.first );
    mapPriceMMAsk.emplace( pmm, vt.second.volume );

    //std::cout
    //  << "ask "
    //  << vt.first
    //  << ": " << vt.second.volume
    //  << "@" << vt.second.price
    //  << std::endl;
  }

  for ( const mapMM_t::value_type& vt: m_mapMMBid ) {
    price_mm pmm( vt.second.price, vt.first );
    mapPriceMMBid.emplace( pmm, vt.second.volume );

    //std::cout
    //  << "bid "
    //  << vt.first
    //  << ": " << vt.second.volume
    //  << "@" << vt.second.price
    //  << std::endl;
  }

  for (
    mapPriceLevels_t::const_reverse_iterator iter = mapPriceMMAsk.rbegin();
    iter != mapPriceMMAsk.rend();
    iter++
  ) {
    std::cout
      << "ask "
      << iter->first.price
      << "," << iter->first.mm
      << "=" << iter->second
      << std::endl;
  }

  std::cout << "----" << std::endl;

  for (
    mapPriceLevels_t::const_reverse_iterator iter = mapPriceMMBid.rbegin();
    iter != mapPriceMMBid.rend();
    iter++
  ) {
    std::cout
      << "bid "
      << iter->first.price
      << "," << iter->first.mm
      << "=" << iter->second
      << std::endl;
  }

}

void DoMDispatch::Connect() {
  inherited_t::Connect();
}

void DoMDispatch::Disconnect() {
  inherited_t::Disconnect();
}

void DoMDispatch::OnL2Initialized() {
  StartMarketByOrder( m_sWatch );
  //StartPriceLevel( m_sWatch );
}

// used with futures, not equities
void DoMDispatch::OnMBOAdd( const ou::tf::iqfeed::l2::msg::OrderArrival::decoded& msg ) {

  assert( ( '3' == msg.chMsgType ) || ( '6' == msg.chMsgType ) );

  if ( 0 == msg.nOrderId ) { // nasdaq type LII
    std::cout << "found nasdaq type lii add message" << std::endl;
  }
  else { // other type
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
        AuctionAdd( msg, m_fAskVolumeAtPrice, m_mapAuctionAsk );
        break;
      case 'B':
        AuctionAdd( msg, m_fBidVolumeAtPrice, m_mapAuctionBid );
        break;
    }
  }

}

void DoMDispatch::OnMBOSummary( const ou::tf::iqfeed::l2::msg::OrderArrival::decoded& msg ) {

  assert( '6' == msg.chMsgType );

  if ( 0 == msg.nOrderId ) { // nasdaq LII
    OnMBOOrderArrival( msg );
  }
  else { // regular futures
    OnMBOAdd( msg );  // will this work as expected?
  }

}

void DoMDispatch::OnMBOUpdate( const ou::tf::iqfeed::l2::msg::OrderArrival::decoded& msg ) {

  assert( '4' == msg.chMsgType );

  // for nasdaq l2 on equities, there is on orderid, so will need to lookup by mmid
  // so need a way to distinquish between futures and nasdaq and price levels

  // MBO Futures        WOR WPL L2O|
  // non-MBO Futures    --- WPL ---|
  // Equity Level2      WOR --- ---| has no orderid, use mmid for key?

  // will need to different maps, or skip the lookup in to m_mapOrder

  // equity, nasdaq l2:  '4,SPY,,NSDQ,B,451.4400,300,,4,11:33:27.030724,2022-04-01,'

  if ( 0 == msg.nOrderId ) { // equity/nasdaq lII, map by mm,order side
    OnMBOOrderArrival( msg );
  }
  else { // regular stuff
    mapOrder_t::iterator iter = m_mapOrder.find( msg.nOrderId );
    if ( m_mapOrder.end() == iter ) {
      std::cout << "map update order does not exist: " << msg.nOrderId << std::endl;
    }
    else {

      switch ( msg.chOrderSide ) {
        case 'A':
          AuctionUpdate( m_mapAuctionAsk, iter->second, msg, m_fAskVolumeAtPrice );
          break;
        case 'B':
          AuctionUpdate( m_mapAuctionBid, iter->second, msg, m_fBidVolumeAtPrice );
          break;
      }

      iter->second.nQuantity = msg.nQuantity;
    }
  }


}

// for nasdaq LII
void DoMDispatch::OnMBOOrderArrival( const ou::tf::iqfeed::l2::msg::OrderArrival::decoded& msg ) {

  //if ( "NSDQ" != msg.sMarketMaker ) {
  //  std::cout
  //    << "mmid: "
  //    << msg.sMarketMaker
  //    << "," << msg.chOrderSide
  //    << "," << msg.dblPrice
  //    << "," << msg.nQuantity
  //    << std::endl;
  //}

  switch ( msg.chOrderSide ) {
    case 'A':
      MMAuction_Update( msg, m_fAskVolumeAtPrice, m_mapMMAsk, m_mapAuctionAsk );
      break;
    case 'B':
      MMAuction_Update( msg, m_fBidVolumeAtPrice, m_mapMMBid, m_mapAuctionBid );
      break;
  }
}

void DoMDispatch::MMAuction_Update(
  const ou::tf::iqfeed::l2::msg::OrderArrival::decoded& msg,
  fVolumeAtPrice_t& f,
  mapMM_t& mapMM,
  mapAuction_t& mapAuction
) {

  price_level pl( msg.dblPrice, msg.nQuantity );

  mapMM_t::iterator mapMM_iter = mapMM.find( msg.sMarketMaker );
  if ( mapMM.end() == mapMM_iter ) {
    auto pair = mapMM.emplace( msg.sMarketMaker, pl );
    assert( pair.second );
    mapMM_iter = pair.first;
  }
  else {
    // remove volume from existing price level
    mapAuction_t::iterator mapAuction_iter = mapAuction.find( mapMM_iter->second.price );
    if ( mapAuction.end() != mapAuction_iter ) {
      mapAuction_iter->second.nQuantity -= mapMM_iter->second.volume; // remove old volume
      if ( 10000 < mapAuction_iter->second.nQuantity ) {
//        std::cout << mapAuction_iter->second.nQuantity << std::endl; // debug only
      }
      if ( f ) f( mapAuction_iter->first, mapAuction_iter->second.nQuantity );
    }
    else assert( false ); // how inconsistent are things?
    // assign new price level
    mapMM_iter->second = pl;
  }

  // update new price level
  bool bResult;
  mapAuction_t::iterator mapAuction_iter = mapAuction.find( msg.dblPrice );
  if ( mapAuction.end() == mapAuction_iter ) {
    auto pair = mapAuction.emplace( msg.dblPrice, Auction( msg.nQuantity ) ); // provide new price_level
    assert( pair.second );
    mapAuction_iter = pair.first;
  }
  else {
    mapAuction_iter->second.nQuantity += mapMM_iter->second.volume; // add new volume
  }

  if ( 10000 < mapAuction_iter->second.nQuantity ) {
//    std::cout << mapAuction_iter->second.nQuantity << std::endl; // debug only
  }
  if ( f ) f( mapAuction_iter->first, mapAuction_iter->second.nQuantity );

}

void DoMDispatch::OnMBODelete( const ou::tf::iqfeed::l2::msg::OrderDelete::decoded& msg ) {

  assert( '5' == msg.chMsgType );

  if ( 0 == msg.nOrderId ) { // nasdaq L II
    switch ( msg.chOrderSide ) {
      case 'A':
        MMAuction_Delete( msg, m_fAskVolumeAtPrice, m_mapMMAsk, m_mapAuctionAsk );
        break;
      case 'B':
        MMAuction_Delete( msg, m_fBidVolumeAtPrice, m_mapMMBid, m_mapAuctionBid );
        break;
    }
  }
  else { // futures MBO
    mapOrder_t::iterator iter = m_mapOrder.find( msg.nOrderId );
    if ( m_mapOrder.end() == iter ) {
      std::cout << "map order delete does not exist: " << msg.nOrderId << std::endl;
    }
    else {

      switch ( msg.chOrderSide ) {
        case 'A':
          AuctionDel( m_mapAuctionAsk, iter->second, m_fAskVolumeAtPrice );
          break;
        case 'B':
          AuctionDel( m_mapAuctionBid, iter->second, m_fBidVolumeAtPrice );
          break;
      }

      m_mapOrder.erase( iter );
    }
  }

}

void DoMDispatch::MMAuction_Delete(
  const ou::tf::iqfeed::l2::msg::OrderDelete::decoded& msg,
  fVolumeAtPrice_t& f,
  mapMM_t& mapMM,
  mapAuction_t& mapAuction
) {

  mapMM_t::iterator mapMM_iter = mapMM.find( msg.sMarketMaker );
  if ( mapMM.end() == mapMM_iter ) {
    // this should probably be an assert( false )
  }
  else {
    // remove volume from existing price level
    mapAuction_t::iterator mapAuction_iter = mapAuction.find( mapMM_iter->second.price );
    if ( mapAuction.end() != mapAuction_iter ) {
      mapAuction_iter->second.nQuantity -= mapMM_iter->second.volume; // remove old volume
      if ( f ) f( mapAuction_iter->first, mapAuction_iter->second.nQuantity );
    }
    else assert( false ); // how inconsistent are things?
    mapMM.erase( mapMM_iter ); // TODO: maybe just keep at 0?, no erasure?
  }

}

void DoMDispatch::AuctionAdd(
  const ou::tf::iqfeed::l2::msg::OrderArrival::decoded& msg,
  fVolumeAtPrice_t& f,
  mapAuction_t& map
) {
  mapAuction_t::iterator iterAuction = map.find( msg.dblPrice );
  if ( map.end() == iterAuction ) {
    map.emplace(
      std::pair(
        msg.dblPrice,
        Auction( msg )
      )
    );
  }
  else {
    iterAuction->second.nQuantity += msg.nQuantity;
    iterAuction->second.nOrders++;
  }
  if ( f ) f( msg.dblPrice, iterAuction->second.nQuantity );
}

void DoMDispatch::AuctionUpdate(
  mapAuction_t& map,
  Order& order,
  const ou::tf::iqfeed::l2::msg::OrderArrival::decoded& msg,
  fVolumeAtPrice_t& f
) {

  if ( order.chOrderSide != msg.chOrderSide ) {
    std::cout << "order side change " << order.chOrderSide << " to " << msg.chOrderSide << std::endl;
  }

  mapAuction_t::iterator iterAuction = map.find( order.dblPrice );
  if ( map.end() == iterAuction ) {
    std::cout << "AuctionUpdate price not found: " << order.dblPrice << std::endl;
  }
  else {
    auto& nQuantity( iterAuction->second.nQuantity );
    // assert( nQuantity >= order.nQuantity ); // doesn't work for equities, need to fix for mmid
    //nQuantity += msg.nQuantity;
    nQuantity -= order.nQuantity;
    iterAuction->second.nOrders--;
    if ( f ) f( order.dblPrice, nQuantity );
  }

  if ( order.dblPrice != msg.dblPrice ) {
    //std::cout << "price change " << order.dblPrice << " to " << msg.dblPrice << std::endl;
    AuctionAdd( msg, f, map );
    order.dblPrice = msg.dblPrice;
  }
  else {
    auto& nQuantity( iterAuction->second.nQuantity );
    // assert( nQuantity >= order.nQuantity ); // doesn't work for equities, need to fix for mmid
    nQuantity += msg.nQuantity;
    iterAuction->second.nOrders++;
    //nQuantity -= order.nQuantity;
    if ( f ) f( order.dblPrice, nQuantity );
  }
}

void DoMDispatch::AuctionDel( mapAuction_t& map, const Order& order, fVolumeAtPrice_t& f ) {

  mapAuction_t::iterator iterAuction = map.find( order.dblPrice );
  if ( map.end() == iterAuction ) {
    std::cout << "AuctionDel price not found: " << order.dblPrice << std::endl;
  }
  else {
    iterAuction->second.nQuantity -= order.nQuantity;
    iterAuction->second.nOrders--;
  }
  if ( f ) f( order.dblPrice, iterAuction->second.nQuantity );

}