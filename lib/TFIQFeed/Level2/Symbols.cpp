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

L2Base::L2Base()
: m_fMarketDepthByMM( nullptr )
, m_fMarketDepthByOrder( nullptr )
{}

void L2Base::Add( char chSide, price_t price, volume_t volume ) {
  switch ( chSide ) {
    case 'A':
      m_LevelAggregateAsk.Add( price, volume );
      break;
    case 'B':
      m_LevelAggregateBid.Add( price, volume );
      break;
  }
}

void L2Base::Update( char chSide, price_t oldp, volume_t oldv, price_t newp, volume_t newv ) {
  switch ( chSide ) {
    case 'A':
      m_LevelAggregateAsk.Update( oldp, oldv, newp, newv );
      break;
    case 'B':
      m_LevelAggregateBid.Update( oldp, oldv, newp, newv );
      break;
  }
}

void L2Base::Delete( char chSide, price_t price, volume_t volume ) {
  switch ( chSide ) {
    case 'A':
      m_LevelAggregateAsk.Delete( price, volume );
      break;
    case 'B':
      m_LevelAggregateBid.Delete( price, volume );
      break;
  }
}

// ==== MarketMaker === for nasdaq equities LII

void MarketMaker::OnMBOUpdate( const msg::OrderArrival::decoded& msg ) {

  if ( nullptr != m_fMarketDepthByMM ) {
    ptime dt( ou::TimeSource::Instance().External() );
    ou::tf::DepthByMM md( dt, msg.chMsgType, msg.chOrderSide, msg.nQuantity, msg.dblPrice, msg.mmid.id );
    m_fMarketDepthByMM( md );
  }
  else {
    MMLimitOrder_Update_Live( msg );
  }
}

void MarketMaker::OnMBODelete( const msg::OrderDelete::decoded& msg ) {

  if ( nullptr != m_fMarketDepthByMM ) {
    ptime dt( ou::TimeSource::Instance().External() );
    ou::tf::DepthByMM md( dt, msg.chMsgType, msg.chOrderSide, 0, 0.0, msg.mmid.id );
    m_fMarketDepthByMM( md );
  }
  else {
    MMLimitOrder_Delete_Live( msg );
  }
}

void MarketMaker::MarketDepth( const ou::tf::DepthByMM& depth ) {
  switch ( depth.MsgType() ) {
    //case 3:  doesn't have add
    case '4': // Update
    case '6': // Summary
      DepthByMM_Update( depth );
      break;
    case '5':
      DepthByMM_Delete( depth );
      break;
    default:
      assert( false );
  }
}

void MarketMaker::DepthByMM_Update( const ou::tf::DepthByMM& depth ) { // TODO: redo this, has redundant test
  switch ( depth.Side() ) {
    case 'A':
      MMLimitOrder_Update( depth.Side(), depth.MMID(), depth.Price(), depth.Volume(),  m_mapMMAsk );
      break;
    case 'B':
      MMLimitOrder_Update( depth.Side(), depth.MMID(), depth.Price(), depth.Volume(), m_mapMMBid );
      break;
  }
}

void MarketMaker::MMLimitOrder_Update_Live(
  const msg::OrderArrival::decoded& msg
) {
  switch ( msg.chOrderSide ) {
    case 'A':
      MMLimitOrder_Update( msg.chOrderSide, msg.mmid.id, msg.dblPrice, msg.nQuantity, m_mapMMAsk );
      break;
    case 'B':
      MMLimitOrder_Update( msg.chOrderSide, msg.mmid.id, msg.dblPrice, msg.nQuantity, m_mapMMBid );
      break;
  }
}

void MarketMaker::MMLimitOrder_Update(
  char chSide,
  DepthByMM::MMID_t mmid,
  double price, volume_t volume,
  mapMM_t& mapMM
) {

  price_level pl( price, volume );

  mapMM_t::iterator mapMM_iter = mapMM.find( mmid );
  if ( mapMM.end() == mapMM_iter ) {
    auto pair = mapMM.emplace( mmid, pl );
    assert( pair.second );
    //mapMM_iter = pair.first;
  }
  else {
    // remove volume from existing price level
    Delete( chSide, mapMM_iter->second.price, mapMM_iter->second.volume );
    // assign new price level
    mapMM_iter->second = pl;
  }

  // update new price level
  Add( chSide, price, volume );

}

void MarketMaker::MMLimitOrder_Delete_Live(
  const msg::OrderDelete::decoded& msg
) {
  switch ( msg.chOrderSide ) {
    case 'A':
      MMLimitOrder_Delete( msg.chOrderSide, msg.mmid.id, m_mapMMAsk );
      break;
    case 'B':
      MMLimitOrder_Delete( msg.chOrderSide, msg.mmid.id, m_mapMMBid );
      break;
  }
}

void MarketMaker::DepthByMM_Delete( const ou::tf::DepthByMM& depth ) {
  switch ( depth.Side() ) {
    case 'A':
      MMLimitOrder_Delete( depth.Side(), depth.MMID(), m_mapMMAsk );
      break;
    case 'B':
      MMLimitOrder_Delete( depth.Side(), depth.MMID(), m_mapMMBid );
      break;
  }
}

void MarketMaker::MMLimitOrder_Delete(
  char chSide,
  DepthByMM::MMID_t mmid, // MMID
  mapMM_t& mapMM
) {

  mapMM_t::iterator mapMM_iter = mapMM.find( mmid );
  if ( mapMM.end() == mapMM_iter ) {
    // this should probably be an assert( false )
    // but then will reqquire recovery from stream outages
  }
  else {
    // remove volume from existing price level
    Delete( chSide, mapMM_iter->second.price, mapMM_iter->second.volume );
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
    price_mm pmm( vt.second.price, ou::tf::DepthByMM::Cast( vt.first ) );
    mapPriceMMAsk.emplace( pmm, vt.second.volume );

    //std::cout
    //  << "ask "
    //  << vt.first
    //  << ": " << vt.second.volume
    //  << "@" << vt.second.price
    //  << std::endl;
  }

  for ( const mapMM_t::value_type& vt: m_mapMMBid ) {
    price_mm pmm( vt.second.price, ou::tf::DepthByMM::Cast( vt.first ) );
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

// Live Messages

void OrderBased::OnMBOSummary( const msg::OrderArrival::decoded& msg ) { // TODO: this may require special processing
  OnMBOAdd( msg );
}

void OrderBased::OnMBOAdd( const msg::OrderArrival::decoded& msg ) {
  if ( nullptr == m_fMarketDepthByOrder ) {
    LimitOrderAdd( msg.nOrderId, Order( msg ) );
  }
  else {
    ptime dt( ou::TimeSource::Instance().External() );
    ou::tf::DepthByOrder md( dt, msg.dt(), msg.nOrderId, msg.nPriority, msg.chMsgType, msg.chOrderSide, msg.dblPrice, msg.nQuantity );
    m_fMarketDepthByOrder( md );
  }
}

void OrderBased::OnMBOUpdate( const msg::OrderArrival::decoded& msg ) {
  if ( nullptr == m_fMarketDepthByOrder ) {
    LimitOrderUpdate( msg.nOrderId, msg.chOrderSide, msg.dblPrice, msg.nQuantity );
  }
  else {
    ptime dt( ou::TimeSource::Instance().External() );
    ou::tf::DepthByOrder md( dt, msg.dt(), msg.nOrderId, msg.nPriority, msg.chMsgType, msg.chOrderSide, msg.dblPrice, msg.nQuantity );
    m_fMarketDepthByOrder( md );
  }
}

void OrderBased::OnMBODelete( const msg::OrderDelete::decoded& msg ) {
  if ( nullptr == m_fMarketDepthByOrder ) {
    LimitOrderDelete( msg.nOrderId );
  }
  else {
    ptime dt( ou::TimeSource::Instance().External() );
    ou::tf::DepthByOrder md( dt, msg.dt(), msg.nOrderId, 0, msg.chMsgType, msg.chOrderSide );
    m_fMarketDepthByOrder( md );
  }
}

// Replay from Simulation

void OrderBased::MarketDepth( const ou::tf::DepthByOrder& depth ) {
  switch ( depth.MsgType() ) {
    case '4': // Update
      LimitOrderUpdate( depth.OrderID(), depth.Side(), depth.Price(), depth.Volume() );
      break;
    case '6': // Summary - will need to categorize this properly
      LimitOrderAdd( depth.OrderID(), Order( depth ) );
      break;
    case '3': // add
      LimitOrderAdd( depth.OrderID(), Order( depth ) );
      break;
    case '5':
      LimitOrderDelete( depth.OrderID() );
      break;
    default:
      assert( false );
  }
}

// Processing

void OrderBased::LimitOrderAdd( uint64_t nOrderId, const Order& order ) {

  mapOrder_t::iterator iter = m_mapOrder.find( nOrderId );
  if ( m_mapOrder.end() != iter ) {
    //    std::cout << "map add order already exists: " << msg.nOrderId << std::endl;
  }
  else {
    m_mapOrder.emplace( std::pair( nOrderId, order ) );
  }

  Add( order.chOrderSide, order.dblPrice, order.nQuantity );

}

void OrderBased::LimitOrderUpdate( uint64_t nOrderId, char chOrderSide, double dblPriceNew, volume_t nQuantityNew ) {

  mapOrder_t::iterator iter = m_mapOrder.find( nOrderId );
  if ( m_mapOrder.end() == iter ) {
    BOOST_LOG_TRIVIAL(error) << "LimitOrderUpdate order does not exist: " << nOrderId;
  }
  else {

    if ( 0 == nQuantityNew ) {
      BOOST_LOG_TRIVIAL(warning) << "LimitOrderUpdate order " << nOrderId << " warning - zero new quantity";
    }

    Order& order( iter->second );
    if ( order.chOrderSide != chOrderSide ) {
      BOOST_LOG_TRIVIAL(error) << "LimitOrderUpdate error - side change " << order.chOrderSide << " to " << chOrderSide;
    }
    else {
      LimitOrderUpdate( order, dblPriceNew, nQuantityNew );
    }
  }
}

void OrderBased::LimitOrderUpdate(
  Order& order,
  double dblPriceNew,
  volume_t nQuantityNew
) {
  // TODO: can this be refactored with caller to minimize the call overhead?
  Delete( order.chOrderSide, order.dblPrice, order.nQuantity );
  order.dblPrice = dblPriceNew;
  order.nQuantity = nQuantityNew;
  Add( order.chOrderSide, dblPriceNew, nQuantityNew );

}

void OrderBased::LimitOrderDelete( uint64_t nOrderId ) {

  mapOrder_t::iterator iter = m_mapOrder.find( nOrderId );
  if ( m_mapOrder.end() == iter ) {
    BOOST_LOG_TRIVIAL(error) << "LimitOrderDelete order " << nOrderId << " does not exist";
  }
  else {
    const Order& order( iter->second );
    Delete( order.chOrderSide, order.dblPrice, order.nQuantity );

    m_mapOrder.erase( iter );
  }
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

void Symbols::WatchAdd( const std::string& sSymbol, fBookChanges_t&& fBid, fBookChanges_t&& fAsk ) {
  mapL2Base_t::iterator iter = m_mapL2Base.find( sSymbol );
  assert( m_mapL2Base.end() == iter );

  m_mapBookChangeFunctions.emplace( sSymbol, BookChangeFunctions( std::move( fBid ), std::move( fAsk) ) );
  StartMarketByOrder( sSymbol );
  // don't add pattern here as Equity/Future is unknown
}

void Symbols::WatchAdd( const std::string& sSymbol, fVolumeAtPrice_t&& fBid, fVolumeAtPrice_t&& fAsk ) {
  mapL2Base_t::iterator iter = m_mapL2Base.find( sSymbol );
  assert( m_mapL2Base.end() == iter );

  m_mapVolumeAtPriceFunctions.emplace( sSymbol, VolumeAtPriceFunctions( std::move( fBid ), std::move( fAsk) ) );
  StartMarketByOrder( sSymbol );
  // don't add pattern here as Equity/Future is unknown
}

void Symbols::WatchAdd( const std::string& sSymbol, L2Base::fMarketDepthByMM_t&& fMarketDepth ) {
  mapL2Base_t::iterator iter = m_mapL2Base.find( sSymbol );
  assert( m_mapL2Base.end() == iter );

  m_mapMarketDepthFunctionByMM.emplace( sSymbol, std::move( fMarketDepth ) );
  StartMarketByOrder( sSymbol );
}

void Symbols::WatchAdd( const std::string& sSymbol, L2Base::fMarketDepthByOrder_t&& fMarketDepth ) {
  mapL2Base_t::iterator iter = m_mapL2Base.find( sSymbol );
  assert( m_mapL2Base.end() == iter );

  m_mapMarketDepthFunctionByOrder.emplace( sSymbol, std::move( fMarketDepth ) );
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
