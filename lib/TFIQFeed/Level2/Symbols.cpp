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

#include <OUCommon/TimeSource.h>

#include <TFTrading/KeyTypes.h>

#include "Symbols.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed
namespace l2 { // level 2 data

// ==== L2Base

L2Base::L2Base(){}

void L2Base::LimitOrderAdd(
  const msg::OrderArrival::decoded& msg,
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
  if ( f ) f( msg.dblPrice, iterLimitOrderBook->second.nQuantity, true );
}

// ==== MarketMaker

// for nasdaq LII
void MarketMaker::OnMBOUpdate( const msg::OrderArrival::decoded& msg ) {

  if ( nullptr != m_fMarketDepth ) {
    ptime dt( ou::TimeSource::Instance().External() );
    ou::tf::MarketDepth md( dt, msg.chMsgType, msg.chOrderSide, msg.nQuantity, msg.dblPrice, msg.mmid.id );
    m_fMarketDepth( md );
  }
  else {
    switch ( msg.chOrderSide ) {
      case 'A':
        MMLimitOrder_Update_Live( msg, m_fAskVolumeAtPrice, m_mapMMAsk, m_mapLimitOrderBookAsk );
        break;
      case 'B':
        MMLimitOrder_Update_Live( msg, m_fBidVolumeAtPrice, m_mapMMBid, m_mapLimitOrderBookBid );
        break;
    }
  }
}

void MarketMaker::OnMBODelete( const msg::OrderDelete::decoded& msg ) {

  if ( nullptr != m_fMarketDepth ) {
    ptime dt( ou::TimeSource::Instance().External() );
    ou::tf::MarketDepth md( dt, msg.chMsgType, msg.chOrderSide, 0, 0.0, msg.mmid.id );
    m_fMarketDepth( md );
  }
  else {
    switch ( msg.chOrderSide ) {
      case 'A':
        MMLimitOrder_Delete_Live( msg, m_fAskVolumeAtPrice, m_mapMMAsk, m_mapLimitOrderBookAsk );
        break;
      case 'B':
        MMLimitOrder_Delete_Live( msg, m_fBidVolumeAtPrice, m_mapMMBid, m_mapLimitOrderBookBid );
        break;
    }
  }
}

void MarketMaker::MarketDepth( const ou::tf::MarketDepth& depth ) {
  switch ( depth.MsgType() ) {
    //case 3:  doesn't have add
    case '4': // Update
    case '6': // Summary
      BidOrAsk_Update( depth );
      break;
    case '5':
      BidOrAsk_Delete( depth );
      break;
    default:
      assert( false );
  }
}

void MarketMaker::BidOrAsk_Update( const ou::tf::MarketDepth& depth ) {
  switch ( depth.Side() ) {
    case 'A':
      MMLimitOrder_Update( depth.MMID(), depth.Price(), depth.Volume(),  m_fAskVolumeAtPrice, m_mapMMAsk, m_mapLimitOrderBookAsk );
      break;
    case 'B':
      MMLimitOrder_Update( depth.MMID(), depth.Price(), depth.Volume(), m_fBidVolumeAtPrice, m_mapMMBid, m_mapLimitOrderBookBid );
      break;
  }
}

void MarketMaker::BidOrAsk_Delete( const ou::tf::MarketDepth& depth ) {
  switch ( depth.Side() ) {
    case 'A':
      MMLimitOrder_Delete( depth.MMID(), m_fAskVolumeAtPrice, m_mapMMAsk, m_mapLimitOrderBookAsk );
      break;
    case 'B':
      MMLimitOrder_Delete( depth.MMID(), m_fBidVolumeAtPrice, m_mapMMBid, m_mapLimitOrderBookBid );
      break;
  }
}

void MarketMaker::MMLimitOrder_Update_Live(
  const msg::OrderArrival::decoded& msg,
  fVolumeAtPrice_t& f,
  mapMM_t& mapMM, mapLimitOrderBook_t& mapLimitOrderBook
) {
  MMLimitOrder_Update( msg.mmid.id, msg.dblPrice, msg.nQuantity, f, mapMM, mapLimitOrderBook );
}

void MarketMaker::MMLimitOrder_Update(
  MarketDepth::MMID_t mmid,
  double price, volume_t volume,
  fVolumeAtPrice_t& f,
  mapMM_t& mapMM,
  mapLimitOrderBook_t& mapLimitOrderBook
) {

  price_level pl( price, volume );

  mapMM_t::iterator mapMM_iter = mapMM.find( mmid );
  if ( mapMM.end() == mapMM_iter ) {
    auto pair = mapMM.emplace( mmid, pl );
    assert( pair.second );
    mapMM_iter = pair.first;
  }
  else {
    // remove volume from existing price level
    mapLimitOrderBook_t::iterator mapLimitOrderBook_iter = mapLimitOrderBook.find( mapMM_iter->second.price );
    if ( mapLimitOrderBook.end() != mapLimitOrderBook_iter ) {
      mapLimitOrderBook_iter->second.nQuantity -= mapMM_iter->second.volume; // remove old volume
      if ( f ) f( mapLimitOrderBook_iter->first, mapLimitOrderBook_iter->second.nQuantity, false );
    }
    else assert( false ); // how inconsistent are things?
    // assign new price level
    mapMM_iter->second = pl;
  }

  // update new price level
  mapLimitOrderBook_t::iterator mapLimitOrderBook_iter = mapLimitOrderBook.find( price );
  if ( mapLimitOrderBook.end() == mapLimitOrderBook_iter ) {
    auto pair = mapLimitOrderBook.emplace( price, LimitOrder( volume ) ); // provide new price_level
    assert( pair.second );
    mapLimitOrderBook_iter = pair.first;
  }
  else {
    mapLimitOrderBook_iter->second.nQuantity += mapMM_iter->second.volume; // add new volume
  }

  if ( f ) f( mapLimitOrderBook_iter->first, mapLimitOrderBook_iter->second.nQuantity, true );

}

void MarketMaker::MMLimitOrder_Delete_Live(
  const msg::OrderDelete::decoded& msg,
  fVolumeAtPrice_t& f,
  mapMM_t& mapMM,
  mapLimitOrderBook_t& mapLimitOrderBook
) {
  MMLimitOrder_Delete( msg.mmid.id, f, mapMM, mapLimitOrderBook );
}

void MarketMaker::MMLimitOrder_Delete(
  MarketDepth::MMID_t mmid, // MMID
  fVolumeAtPrice_t& f,
  mapMM_t& mapMM,
  mapLimitOrderBook_t& mapLimitOrderBook
) {

  mapMM_t::iterator mapMM_iter = mapMM.find( mmid );
  if ( mapMM.end() == mapMM_iter ) {
    // this should probably be an assert( false )
    // but then will reqquire recovery from stream outages
  }
  else {
    // remove volume from existing price level
    mapLimitOrderBook_t::iterator mapLimitOrderBook_iter = mapLimitOrderBook.find( mapMM_iter->second.price );
    if ( mapLimitOrderBook.end() != mapLimitOrderBook_iter ) {
      mapLimitOrderBook_iter->second.nQuantity -= mapMM_iter->second.volume; // remove old volume
      if ( f ) f( mapLimitOrderBook_iter->first, mapLimitOrderBook_iter->second.nQuantity, false );
    }
    else assert( false ); // how inconsistent are things?
    mapMM.erase( mapMM_iter );
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
    price_mm pmm( vt.second.price, ou::tf::MarketDepth::Cast( vt.first ) );
    mapPriceMMAsk.emplace( pmm, vt.second.volume );

    //std::cout
    //  << "ask "
    //  << vt.first
    //  << ": " << vt.second.volume
    //  << "@" << vt.second.price
    //  << std::endl;
  }

  for ( const mapMM_t::value_type& vt: m_mapMMBid ) {
    price_mm pmm( vt.second.price, ou::tf::MarketDepth::Cast( vt.first ) );
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
  const msg::OrderArrival::decoded& msg,
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
    if ( f ) f( order.dblPrice, nQuantity, false );
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
    if ( f ) f( order.dblPrice, nQuantity, true );
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
  if ( f ) f( order.dblPrice, iterLimitOrderBook->second.nQuantity, false );

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
  mapL2Base_t::iterator iter = m_mapL2Base.find( sSymbol );
  assert( m_mapL2Base.end() == iter );

  m_mapVolumeAtPriceFunctions.emplace( sSymbol, VolumeAtPriceFunctions( std::move( fBid ), std::move( fAsk) ) );
  StartMarketByOrder( sSymbol );
  // don't add pattern here as Equity/Future is unknown
}

void Symbols::WatchAdd( const std::string& sSymbol, L2Base::fMarketDepth_t&& fMarketDepth ) {
  mapL2Base_t::iterator iter = m_mapL2Base.find( sSymbol );
  assert( m_mapL2Base.end() == iter );

  m_mapMarketDepthFunction.emplace( sSymbol, std::move( fMarketDepth ) );
  StartMarketByOrder( sSymbol );
}

void Symbols::WatchDel( const std::string& sSymbol ) {
  StopMarketByOrder( sSymbol );
  mapL2Base_t::iterator iter = m_mapL2Base.find( sSymbol );
  //m_mapL2Base.erase( iter );
  // TODO: need to update m_luSymbol/m_single as well
  // TODO: may need some sort of sync if values come in during the meantime
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
