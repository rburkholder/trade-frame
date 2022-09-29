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

void L2Base::Clear( const ou::tf::Depth& depth ) {
  switch ( depth.Side() ) {
    case 'A':
      m_LevelAggregateAsk.Clear( depth );
      break;
    case 'B':
      m_LevelAggregateBid.Clear( depth );
      break;
    default:
      assert( false );
      break;
  }
}

void L2Base::Add( const ou::tf::Depth& depth ) {
  switch ( depth.Side() ) {
    case 'A':
      m_LevelAggregateAsk.Add( depth );
      break;
    case 'B':
      m_LevelAggregateBid.Add( depth );
      break;
    default:
      assert( false );
      break;
  }
}

void L2Base::Delete( const ou::tf::Depth& depth ) {
  switch ( depth.Side() ) {
    case 'A':
      m_LevelAggregateAsk.Delete( depth );
      break;
    case 'B':
      m_LevelAggregateBid.Delete( depth );
      break;
    default:
      assert( false );
      break;
  }
}

// ==== MarketMaker === for nasdaq equities LII

void MarketMaker::OnMBOUpdate( const msg::OrderArrival::decoded& msg ) {

  //if ( 0 != msg.nPriority ) { // does not appear to be in use for MM style messages
  //  BOOST_LOG_TRIVIAL(info) << "MarketMaker::OnMBOUpdate priority is in use: " << msg.nPriority;
  //}

  ptime dt( ou::TimeSource::GlobalInstance().External() );
  ou::tf::DepthByMM depth( dt, msg.chMsgType, msg.chOrderSide, msg.nQuantity, msg.dblPrice, msg.mmid.id );

  if ( nullptr != m_fMarketDepthByMM ) {
    m_fMarketDepthByMM( depth );
  }
  else {
    DepthByMM_Update( depth );
  }
}

void MarketMaker::OnMBODelete( const msg::OrderDelete::decoded& msg ) {

  ptime dt( ou::TimeSource::GlobalInstance().External() );
  ou::tf::DepthByMM depth( dt, msg.chMsgType, msg.chOrderSide, 0, 0.0, msg.mmid.id );

  if ( nullptr != m_fMarketDepthByMM ) {
    m_fMarketDepthByMM( depth );
  }
  else {
    DepthByMM_Delete( depth );
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
      break;
  }
}

void MarketMaker::DepthByMM_Update( const ou::tf::DepthByMM& depth ) { // TODO: redo this, has redundant test
  switch ( depth.Side() ) {
    case 'A':
      MMLimitOrder_Update( depth, m_mapMMAsk );
      break;
    case 'B':
      MMLimitOrder_Update( depth, m_mapMMBid );
      break;
    default:
      assert( false );
      break;
  }
}

void MarketMaker::MMLimitOrder_Update( const ou::tf::DepthByMM& depth, mapMM_t& mapMM ) {

  price_level pl( depth.Price(), depth.Volume() );

  mapMM_t::iterator mapMM_iter = mapMM.find( depth.MMID() );
  if ( mapMM.end() == mapMM_iter ) {
    auto pair = mapMM.emplace( depth.MMID(), pl );
    assert( pair.second );
    //mapMM_iter = pair.first;
  }
  else {
    // remove volume from existing price level
    ou::tf::Depth depth_( depth.DateTime(), depth.Side(), mapMM_iter->second.price, mapMM_iter->second.volume );
    Delete( depth_ );
    // assign new price level
    mapMM_iter->second = pl;
  }

  // update new price level
  Add( depth );

}

void MarketMaker::DepthByMM_Delete( const ou::tf::DepthByMM& depth ) {
  switch ( depth.Side() ) {
    case 'A':
      MMLimitOrder_Delete( depth, m_mapMMAsk );
      break;
    case 'B':
      MMLimitOrder_Delete( depth, m_mapMMBid );
      break;
    default:
      assert( false );
      break;
  }
}

void MarketMaker::MMLimitOrder_Delete( const ou::tf::DepthByMM& depth, mapMM_t& mapMM ) {

  mapMM_t::iterator mapMM_iter = mapMM.find( depth.MMID() );
  if ( mapMM.end() == mapMM_iter ) {
    // this should probably be an assert( false )
    // but then will reqquire recovery from stream outages
  }
  else {
    // remove volume from existing price level
    ou::tf::Depth depth_( depth.DateTime(), depth.Side(), mapMM_iter->second.price, mapMM_iter->second.volume );
    Delete( depth_ );
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

OrderBased::OrderBased()
: L2Base()
, m_state( EState::Ready )
{}

// Live Messages

void OrderBased::OnMBOClear( const msg::OrderClear::decoded& msg ) {
  ptime dt( ou::TimeSource::GlobalInstance().External() );
  ou::tf::DepthByOrder depth( dt, dt, 0, 0, msg.chMsgType, msg.chOrderSide, 0.0, msg.nQuantity );

  if ( nullptr == m_fMarketDepthByOrder ) {
    LimitOrderClear( depth );
  }
  else {
    m_fMarketDepthByOrder( depth );
  }
}

void OrderBased::OnMBOSummary( const msg::OrderArrival::decoded& msg ) {
  // TODO: will need to trigger order book reset to rebuild
  //   may require a state machine to track once summary messages complete
  OnMBOAdd( msg );
}

void OrderBased::OnMBOAdd( const msg::OrderArrival::decoded& msg ) {
  ptime dt( ou::TimeSource::GlobalInstance().External() );
  ou::tf::DepthByOrder depth( dt, msg.dt(), msg.nOrderId, msg.nPriority, msg.chMsgType, msg.chOrderSide, msg.dblPrice, msg.nQuantity );

  if ( nullptr == m_fMarketDepthByOrder ) {
    LimitOrderAdd( depth );
  }
  else {
    m_fMarketDepthByOrder( depth );
  }
}

void OrderBased::OnMBOUpdate( const msg::OrderArrival::decoded& msg ) {
  // priority is in use, and currently has high numbers, maybe reset at begin of each session?
  //   order 649948133402 priority 12440218202
  //   order 649948113561 priority 12440218206

  ptime dt( ou::TimeSource::GlobalInstance().External() );
  ou::tf::DepthByOrder depth( dt, msg.dt(), msg.nOrderId, msg.nPriority, msg.chMsgType, msg.chOrderSide, msg.dblPrice, msg.nQuantity );

 if ( nullptr == m_fMarketDepthByOrder ) {
    LimitOrderUpdate( depth );
  }
  else {
    m_fMarketDepthByOrder( depth );
  }
}

void OrderBased::OnMBODelete( const msg::OrderDelete::decoded& msg ) {
  ptime dt( ou::TimeSource::GlobalInstance().External() );
  ou::tf::DepthByOrder depth( dt, msg.dt(), msg.nOrderId, 0, msg.chMsgType, msg.chOrderSide );

  if ( nullptr == m_fMarketDepthByOrder ) {
    LimitOrderDelete( depth );
  }
  else {
    m_fMarketDepthByOrder( depth );
  }
}

// Point of dispatch

void OrderBased::MarketDepth( const ou::tf::DepthByOrder& depth ) {

  switch ( depth.MsgType() ) {
    case '4': // Update
      LimitOrderUpdate( depth );
      break;
    case '3': // add
      LimitOrderAdd( depth );
      break;
    case '5':
      LimitOrderDelete( depth );
      break;
    case '6': // Summary - will need to categorize this properly
      LimitOrderAdd( depth );
      break;
    case 'C':
      LimitOrderClear( depth );
      break;
    default:
      assert( false );
  }
}

// Processing

void OrderBased::LimitOrderClear( const ou::tf::DepthByOrder& depth ) {
  m_state = EState::Clear;

  // todo this properly, will need to only clear those entries for the side provided
  m_mapOrder.clear();
  Clear( depth );

  m_state = EState::Ready;
}

void OrderBased::LimitOrderAdd( const ou::tf::DepthByOrder& depth ) {
  m_state = EState::Add;

  Order order( depth );

  mapOrder_t::iterator iter = m_mapOrder.find( depth.OrderID() );
  if ( m_mapOrder.end() != iter ) {
    // TODO: reset the order book, this happens upon a disconnect/reconnect, can this state be found?
    BOOST_LOG_TRIVIAL(warning) << "LimitOrderAdd re-add order skipped: " << depth.OrderID();
  }
  else {
    auto result = m_mapOrder.emplace( std::pair( depth.OrderID(), order ) );
    assert( result.second );
    m_idOrder = depth.OrderID();
    Add( depth );
  }
  m_state = EState::Ready;
}

void OrderBased::LimitOrderUpdate( const ou::tf::DepthByOrder& depth ) {
  m_state = EState::Update;

  mapOrder_t::iterator iter = m_mapOrder.find( depth.OrderID() );
  if ( m_mapOrder.end() == iter ) {
    BOOST_LOG_TRIVIAL(error) << "LimitOrderUpdate order does not exist: " << depth.OrderID();
  }
  else {

    if ( 0 == depth.Volume() ) {
      BOOST_LOG_TRIVIAL(warning) << "LimitOrderUpdate order " << depth.OrderID() << " warning - zero new quantity";
    }

    Order& order( iter->second );
    if ( order.chOrderSide != depth.Side() ) {
      BOOST_LOG_TRIVIAL(error) << "LimitOrderUpdate error - side change " << order.chOrderSide << " to " << depth.Side();
    }
    else {
      m_idOrder = depth.OrderID();
      ou::tf::Depth depth_( depth.DateTime(), depth.Side(), order.dblPrice, order.nQuantity );
      Delete( depth_ ); // old quantities
      order.dblPrice = depth.Price();
      order.nQuantity = depth.Volume();
      Add( depth );
    }
  }
  m_state = EState::Ready;
}

void OrderBased::LimitOrderDelete( const ou::tf::DepthByOrder& depth ) {
  m_state = EState::Delete;

  mapOrder_t::iterator iter = m_mapOrder.find( depth.OrderID() );
  if ( m_mapOrder.end() == iter ) {
    BOOST_LOG_TRIVIAL(error) << "LimitOrderDelete order " << depth.OrderID() << " does not exist";
  }
  else {
    m_idOrder = depth.OrderID();
    const Order& order( iter->second );
    ou::tf::Depth depth_( depth.DateTime(), depth.Side(), order.dblPrice, order.nQuantity );
    Delete(  depth_ );

    m_mapOrder.erase( iter );
  }
  m_state = EState::Ready;
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
void Symbols::OnMBOClear( const msg::OrderClear::decoded& msg ) {

  assert( ( 'C' == msg.chMsgType ) );
  Call( msg, &L2Base::OnMBOClear );
}

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
