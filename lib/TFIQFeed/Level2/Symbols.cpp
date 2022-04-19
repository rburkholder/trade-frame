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
 * File:    Symbols.cpp
 * Author:  raymond@burkholder.net
 * Project: TFIQFeed/Level2
 * Created  April 15, 2022 18:20
 */

 // Next Step:  use this to replace Summary.cpp

 #include "Symbols.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed
namespace l2 { // level 2 data

// ==== L2Base

void L2Base::LimitOrderAdd(
  const ou::tf::iqfeed::l2::msg::OrderArrival::decoded& msg,
  fVolumeAtPrice_t& f,
  mapLimitOrderBook_t& map
) {
  mapLimitOrderBook_t::iterator iterLimitOrderBook = map.find( msg.dblPrice );
  if ( map.end() == iterLimitOrderBook ) {
    map.emplace(
      std::pair(
        msg.dblPrice,
        LimitOrder( msg )
      )
    );
  }
  else {
    iterLimitOrderBook->second.nQuantity += msg.nQuantity;
    iterLimitOrderBook->second.nOrders++;
  }
  if ( f ) f( msg.dblPrice, iterLimitOrderBook->second.nQuantity );
}

// ==== MarketMaker

// for nasdaq LII
void MarketMaker::OnMBOOrderArrival( const ou::tf::iqfeed::l2::msg::OrderArrival::decoded& msg ) {

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
      MMLimitOrder_Update( msg, m_fAskVolumeAtPrice, m_mapMMAsk, m_mapLimitOrderBookAsk );
      break;
    case 'B':
      MMLimitOrder_Update( msg, m_fBidVolumeAtPrice, m_mapMMBid, m_mapLimitOrderBookBid );
      break;
  }
}

void MarketMaker::OnMBODelete( const msg::OrderDelete::decoded& msg ) {

  switch ( msg.chOrderSide ) {
    case 'A':
      MMLimitOrder_Delete( msg, m_fAskVolumeAtPrice, m_mapMMAsk, m_mapLimitOrderBookAsk );
      break;
    case 'B':
      MMLimitOrder_Delete( msg, m_fBidVolumeAtPrice, m_mapMMBid, m_mapLimitOrderBookBid );
      break;
  }

}

void MarketMaker::MMLimitOrder_Update(
  const ou::tf::iqfeed::l2::msg::OrderArrival::decoded& msg,
  fVolumeAtPrice_t& f,
  mapMM_t& mapMM,
  mapLimitOrderBook_t& mapLimitOrderBook
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
    mapLimitOrderBook_t::iterator mapLimitOrderBook_iter = mapLimitOrderBook.find( mapMM_iter->second.price );
    if ( mapLimitOrderBook.end() != mapLimitOrderBook_iter ) {
      mapLimitOrderBook_iter->second.nQuantity -= mapMM_iter->second.volume; // remove old volume
      if ( f ) f( mapLimitOrderBook_iter->first, mapLimitOrderBook_iter->second.nQuantity );
    }
    else assert( false ); // how inconsistent are things?
    // assign new price level
    mapMM_iter->second = pl;
  }

  // update new price level
  bool bResult;
  mapLimitOrderBook_t::iterator mapLimitOrderBook_iter = mapLimitOrderBook.find( msg.dblPrice );
  if ( mapLimitOrderBook.end() == mapLimitOrderBook_iter ) {
    auto pair = mapLimitOrderBook.emplace( msg.dblPrice, LimitOrder( msg.nQuantity ) ); // provide new price_level
    assert( pair.second );
    mapLimitOrderBook_iter = pair.first;
  }
  else {
    mapLimitOrderBook_iter->second.nQuantity += mapMM_iter->second.volume; // add new volume
  }

  if ( f ) f( mapLimitOrderBook_iter->first, mapLimitOrderBook_iter->second.nQuantity );

}

void MarketMaker::MMLimitOrder_Delete(
  const ou::tf::iqfeed::l2::msg::OrderDelete::decoded& msg,
  fVolumeAtPrice_t& f,
  mapMM_t& mapMM,
  mapLimitOrderBook_t& mapLimitOrderBook
) {

  mapMM_t::iterator mapMM_iter = mapMM.find( msg.sMarketMaker );
  if ( mapMM.end() == mapMM_iter ) {
    // this should probably be an assert( false )
  }
  else {
    // remove volume from existing price level
    mapLimitOrderBook_t::iterator mapLimitOrderBook_iter = mapLimitOrderBook.find( mapMM_iter->second.price );
    if ( mapLimitOrderBook.end() != mapLimitOrderBook_iter ) {
      mapLimitOrderBook_iter->second.nQuantity -= mapMM_iter->second.volume; // remove old volume
      if ( f ) f( mapLimitOrderBook_iter->first, mapLimitOrderBook_iter->second.nQuantity );
    }
    else assert( false ); // how inconsistent are things?
    mapMM.erase( mapMM_iter ); // TODO: maybe just keep at 0?, no erasure?
  }

}

void MarketMaker::EmitMarketMakerMaps() {
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

// ==== OrderBased

void OrderBased::OnMBOAdd( const msg::OrderArrival::decoded& msg ) {

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
      LimitOrderAdd( msg, m_fAskVolumeAtPrice, m_mapLimitOrderBookAsk );
      break;
    case 'B':
      LimitOrderAdd( msg, m_fBidVolumeAtPrice, m_mapLimitOrderBookBid );
      break;
  }
}

void OrderBased::OnMBOUpdate( const msg::OrderArrival::decoded& msg ) {

  mapOrder_t::iterator iter = m_mapOrder.find( msg.nOrderId );
  if ( m_mapOrder.end() == iter ) {
    std::cout << "map update order does not exist: " << msg.nOrderId << std::endl;
  }
  else {
    switch ( msg.chOrderSide ) {
      case 'A':
        LimitOrderUpdate( m_mapLimitOrderBookAsk, iter->second, msg, m_fAskVolumeAtPrice );
        break;
      case 'B':
        LimitOrderUpdate( m_mapLimitOrderBookBid, iter->second, msg, m_fBidVolumeAtPrice );
        break;
    }

    iter->second.nQuantity = msg.nQuantity;
  }
}

void OrderBased::OnMBODelete( const msg::OrderDelete::decoded& msg ) {

  mapOrder_t::iterator iter = m_mapOrder.find( msg.nOrderId );
  if ( m_mapOrder.end() == iter ) {
    std::cout << "map order delete does not exist: " << msg.nOrderId << std::endl;
  }
  else {

    switch ( msg.chOrderSide ) {
      case 'A':
        LimitOrderDel( m_mapLimitOrderBookAsk, iter->second, m_fAskVolumeAtPrice );
        break;
      case 'B':
        LimitOrderDel( m_mapLimitOrderBookBid, iter->second, m_fBidVolumeAtPrice );
        break;
    }

    m_mapOrder.erase( iter );
  }
}

void OrderBased::LimitOrderUpdate(
  mapLimitOrderBook_t& map,
  Order& order,
  const ou::tf::iqfeed::l2::msg::OrderArrival::decoded& msg,
  fVolumeAtPrice_t& f
) {

  if ( order.chOrderSide != msg.chOrderSide ) {
    std::cout << "order side change " << order.chOrderSide << " to " << msg.chOrderSide << std::endl;
  }

  mapLimitOrderBook_t::iterator iterLimitOrderBook = map.find( order.dblPrice );
  if ( map.end() == iterLimitOrderBook ) {
    std::cout << "LimitOrderUpdate price not found: " << order.dblPrice << std::endl;
  }
  else {
    auto& nQuantity( iterLimitOrderBook->second.nQuantity );
    // assert( nQuantity >= order.nQuantity ); // doesn't work for equities, need to fix for mmid
    //nQuantity += msg.nQuantity;
    nQuantity -= order.nQuantity;
    iterLimitOrderBook->second.nOrders--;
    if ( f ) f( order.dblPrice, nQuantity );
  }

  if ( order.dblPrice != msg.dblPrice ) {
    //std::cout << "price change " << order.dblPrice << " to " << msg.dblPrice << std::endl;
    LimitOrderAdd( msg, f, map );
    order.dblPrice = msg.dblPrice;
  }
  else {
    auto& nQuantity( iterLimitOrderBook->second.nQuantity );
    // assert( nQuantity >= order.nQuantity ); // doesn't work for equities, need to fix for mmid
    nQuantity += msg.nQuantity;
    iterLimitOrderBook->second.nOrders++;
    //nQuantity -= order.nQuantity;
    if ( f ) f( order.dblPrice, nQuantity );
  }
}

void OrderBased::LimitOrderDel( mapLimitOrderBook_t& map, const Order& order, fVolumeAtPrice_t& f ) {

  mapLimitOrderBook_t::iterator iterLimitOrderBook = map.find( order.dblPrice );
  if ( map.end() == iterLimitOrderBook ) {
    std::cout << "LimitOrderDel price not found: " << order.dblPrice << std::endl;
  }
  else {
    iterLimitOrderBook->second.nQuantity -= order.nQuantity;
    iterLimitOrderBook->second.nOrders--;
  }
  if ( f ) f( order.dblPrice, iterLimitOrderBook->second.nQuantity );

}

// ==== Symbols

using inherited_t = Dispatcher<Symbols>;

Symbols::Symbols( fConnected_t&& fConnected )
: inherited_t()
, m_bSingle( false )
, m_fConnected( std::move( fConnected ) )
, m_luSymbol( Carrier(), 20 )
{}

Symbols::~Symbols() {
}

void Symbols::Single( bool bSingle ) {
  if ( bSingle ) {
    assert( 1 >= m_luSymbol.GetNodeCount() );
  }
  else {
    assert( m_single.IsNull() );
  }
  m_bSingle = bSingle;
}

void Symbols::Connect() {
  inherited_t::Connect();
}

void Symbols::Disconnect() {
  inherited_t::Disconnect();
}

void Symbols::OnNetworkConnected() {
}

void Symbols::OnL2Initialized() {
  if ( m_fConnected ) m_fConnected();
}

void Symbols::WatchAdd( const std::string& sSymbol, fVolumeAtPrice_t&& fBid, fVolumeAtPrice_t&& fAsk ) {
  m_mapVolumeAtPriceFunctions.emplace( sSymbol, VolumeAtPriceFunctions( std::move( fBid ), std::move( fAsk) ) );
  StartMarketByOrder( sSymbol );
  // don't add pattern here as Equity/Future is unknown
}

void Symbols::WatchDel( const std::string& sSymbol ) {
  StopMarketByOrder( sSymbol );
}

  // for nasdaq l2 on equities, there is no orderid, so will need to lookup by mmid
  // so need a way to distinquish between futures and nasdaq and price levels

  // MBO Futures        WOR WPL L2O|
  // non-MBO Futures    --- WPL ---|
  // Equity Level2      WOR --- ---| has no orderid, use mmid for key?

  // will need to different maps, or skip the lookup in to m_mapOrder

  // equity, nasdaq l2:  '4,SPY,,NSDQ,B,451.4400,300,,4,11:33:27.030724,2022-04-01,'

// used with futures, not equities
void Symbols::OnMBOAdd( const msg::OrderArrival::decoded& msg ) {

  assert( ( '3' == msg.chMsgType ) || ( '6' == msg.chMsgType ) );
  Call( msg, &L2Base::OnMBOAdd );
}

void Symbols::OnMBOSummary( const msg::OrderArrival::decoded& msg ) {

  assert( '6' == msg.chMsgType );
  Call( msg, &L2Base::OnMBOSummary );
}

void Symbols::OnMBOUpdate( const msg::OrderArrival::decoded& msg ) {

  assert( '4' == msg.chMsgType );
  Call( msg, &L2Base::OnMBOUpdate );
}

void Symbols::OnMBODelete( const msg::OrderDelete::decoded& msg ) {

  assert( '5' == msg.chMsgType );
  Call( msg, &L2Base::OnMBODelete );
}

} // namespace l2
} // namesapce iqfeed
} // namespace tf
} // namespace ou
