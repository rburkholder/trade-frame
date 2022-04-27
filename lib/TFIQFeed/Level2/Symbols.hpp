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
 * File:    Symbols.hpp
 * Author:  raymond@burkholder.net
 * Project: TFIQFeed/Level2
 * Created  April 15, 2022 18:20
 */

#pragma once

#include <memory>

#include <OUCommon/KeyWordMatch.h>

#include <TFTimeSeries/DatedDatum.h>
#include <TFTimeSeries/TimeSeries.h>

#include "Dispatcher.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed
namespace l2 { // level 2 data

using volume_t = ou::tf::Trade::volume_t;

// ==== L2Base

class L2Base {  // TODO: convert to CRTP?
public:

  using pL2Base_t = std::shared_ptr<L2Base>;
  using fVolumeAtPrice_t = std::function<void(double,int,bool)>;
  using fMarketDepth_t = std::function<void(const MarketDepth&)>;

  L2Base();
  virtual ~L2Base() {}

  void Set( fVolumeAtPrice_t&& fBid, fVolumeAtPrice_t&& fAsk ) {
    m_fAskVolumeAtPrice = std::move( fAsk );
    m_fBidVolumeAtPrice = std::move( fBid );
  }
  void Set( fMarketDepth_t&& fMarketDepth ) {
    m_fMarketDepth = std::move( fMarketDepth );
  }

  virtual void OnMBOAdd( const msg::OrderArrival::decoded& ) = 0;
  virtual void OnMBOSummary( const msg::OrderArrival::decoded& ) = 0;
  virtual void OnMBOUpdate( const msg::OrderArrival::decoded& ) = 0;
  virtual void OnMBODelete( const msg::OrderDelete::decoded& ) = 0;

protected:

  fVolumeAtPrice_t m_fBidVolumeAtPrice;
  fVolumeAtPrice_t m_fAskVolumeAtPrice;

  fMarketDepth_t m_fMarketDepth;

  struct LimitOrder {
    // maintain set of orders?
    volume_t nQuantity;
    int nOrders;
    LimitOrder( volume_t nQuantity_ )
    : nQuantity( nQuantity_ ), nOrders( 1 )  {}
    LimitOrder( const msg::OrderArrival::decoded& msg )
    : nQuantity( msg.nQuantity ), nOrders( 1 ) {}
  };

  using mapLimitOrderBook_t = std::map<double,LimitOrder>;  // key is price
  mapLimitOrderBook_t m_mapLimitOrderBookAsk;
  mapLimitOrderBook_t m_mapLimitOrderBookBid;

  void LimitOrderAdd( // used in OrderBased
    const msg::OrderArrival::decoded&,
    fVolumeAtPrice_t&,
    mapLimitOrderBook_t&
  );

private:
};

// ==== MarketMaker (Equities, etc)

class MarketMaker: public L2Base {
public:

  using pMarketMaker_t = std::shared_ptr<MarketMaker>;

  MarketMaker(): L2Base() {}
  virtual ~MarketMaker() {}

  static pMarketMaker_t Factory() { return std::make_shared<MarketMaker>(); }

  virtual void OnMBOAdd( const msg::OrderArrival::decoded& ) { assert( false ); }; // Equity doesn't have this message
  virtual void OnMBOSummary( const msg::OrderArrival::decoded& msg ) { OnMBOUpdate( msg ); }
  virtual void OnMBOUpdate( const msg::OrderArrival::decoded& );
  virtual void OnMBODelete( const msg::OrderDelete::decoded& );

  void MarketDepth( const ou::tf::MarketDepth& ); // offline message submission

  void EmitMarketMakerMaps();

protected:
private:

  struct price_level {

    double price;
    volume_t volume;

    price_level(): price {}, volume {} {}
    price_level( const msg::OrderArrival::decoded& msg )
    : price( msg.dblPrice ), volume( msg.nQuantity ) {}
    price_level( double price_, volume_t volume_ )
    : price( price_ ), volume( volume_ ) {}
  };

  using mapMM_t = std::map<std::string,price_level>; // key=mm, value=price,volume
  mapMM_t m_mapMMAsk;
  mapMM_t m_mapMMBid;

  void BidOrAsk_Update( const ou::tf::MarketDepth& );
  void BidOrAsk_Delete( const ou::tf::MarketDepth& );

  void MMLimitOrder_Update_Live(
    const msg::OrderArrival::decoded&,
    fVolumeAtPrice_t&,
    mapMM_t&, mapLimitOrderBook_t& );

  void MMLimitOrder_Update(
    const std::string& sMarketMaker,
    double price, volume_t volume,
    fVolumeAtPrice_t&,
    mapMM_t&, mapLimitOrderBook_t& );

  void MMLimitOrder_Delete_Live(
    const msg::OrderDelete::decoded&,
    fVolumeAtPrice_t&,
    mapMM_t&, mapLimitOrderBook_t& );

  void MMLimitOrder_Delete(
    const std::string& sMarketMaker,
    fVolumeAtPrice_t&,
    mapMM_t&, mapLimitOrderBook_t& );
};

// ==== OrderBased (Futures, etc)

class OrderBased: public L2Base {
public:

  using pOrderBased_t = std::shared_ptr<OrderBased>;

  OrderBased(): L2Base() {}
  virtual ~OrderBased() {}

  static pOrderBased_t Factory() { return std::make_shared<OrderBased>(); }

  virtual void OnMBOAdd( const msg::OrderArrival::decoded& msg );
  virtual void OnMBOSummary( const msg::OrderArrival::decoded& msg ) {
    OnMBOAdd( msg );
  } // will this work as expected?
  virtual void OnMBOUpdate( const msg::OrderArrival::decoded& msg );
  virtual void OnMBODelete( const msg::OrderDelete::decoded& msg );

protected:
private:

  struct Order {
    char chOrderSide;
    double dblPrice;
    volume_t nQuantity;
    uint64_t nPriority;
    uint8_t nPrecision;
    // ptime, if needed
    // note: there is no MarketMaker in messages with an order ID
    Order( const msg::OrderArrival::decoded& msg )
    : chOrderSide( msg.chOrderSide ),
      dblPrice( msg.dblPrice ), nQuantity( msg.nQuantity ),
      nPriority( msg.nPriority ), nPrecision( msg.nPrecision )
    { assert( 0 == msg.sMarketMaker.size() ); }
  };

  using mapOrder_t = std::map<uint64_t,Order>; // key is order id
  mapOrder_t m_mapOrder;

  void LimitOrderUpdate(
    mapLimitOrderBook_t& map,
    Order& order,
    const msg::OrderArrival::decoded& ,
    fVolumeAtPrice_t&
    );
  void LimitOrderDel( mapLimitOrderBook_t& map, const Order&, fVolumeAtPrice_t& );
};

// ==== Carrier for symbol lookup

struct Carrier {

  using pL2Base_t = L2Base*;
  pL2Base_t pL2Base;

  Carrier(): pL2Base {} {}
  Carrier( pL2Base_t p ): pL2Base( p ) {}
  Carrier( const Carrier& rhs ) {
    pL2Base = rhs.pL2Base;
  }
  Carrier& operator=( const Carrier& rhs ) {
    if ( *this != rhs ) {
      pL2Base = rhs.pL2Base;
    }
    return *this;
  }
  Carrier& operator=( pL2Base_t p ) {
    pL2Base = p;
    return *this;
  }
  bool operator!=( const Carrier& rhs ) { return pL2Base != rhs.pL2Base; }
  bool IsNull() { return nullptr == pL2Base; }
};

// ==== Symbols

class Symbols
: public Dispatcher<Symbols>
{
  friend Dispatcher<Symbols>;
public:

  using fConnected_t = std::function<void()>;

  Symbols( fConnected_t&& );
  virtual ~Symbols();

  void Connect();
  void Disconnect();

  using fVolumeAtPrice_t = L2Base::fVolumeAtPrice_t;

  void WatchAdd( const std::string&, fVolumeAtPrice_t&& fBid, fVolumeAtPrice_t&& fAsk );
  void WatchAdd( const std::string&, L2Base::fMarketDepth_t&& );
  void WatchDel( const std::string& );

  void Single( bool );

protected:

  // called by Network via CRTP
  void OnNetworkConnected();
  //void OnNetworkDisconnected();
  //void OnNetworkError( size_t e );
  void OnL2Initialized();

  void OnMBOAdd( const msg::OrderArrival::decoded& );
  void OnMBOSummary( const msg::OrderArrival::decoded& );
  void OnMBOUpdate( const msg::OrderArrival::decoded& );
  void OnMBODelete( const msg::OrderDelete::decoded& );

private:

  bool m_bSingle;  // don't use m_luSymbol, dedicated to single symbol
  Carrier m_single; // carrier for single symbol

  fConnected_t m_fConnected;

  ou::KeyWordMatch<Carrier> m_luSymbol; // contains the carrier as destination for inbound records

  struct VolumeAtPriceFunctions {

    fVolumeAtPrice_t fBid;
    fVolumeAtPrice_t fAsk;

    VolumeAtPriceFunctions(
      fVolumeAtPrice_t&& fBid_,
      fVolumeAtPrice_t&& fAsk_
    ): fBid( std::move( fBid_ ) ), fAsk( std::move( fAsk_ ) )
    {}

    VolumeAtPriceFunctions( VolumeAtPriceFunctions&& rhs )
    : fBid( std::move( rhs.fBid ) ), fAsk( std::move( rhs.fAsk ) ) {}
  }; // struct VolumeAtPriceFunctions

  using mapVolumeAtPriceFunctions_t = std::map<std::string,VolumeAtPriceFunctions>;
  mapVolumeAtPriceFunctions_t m_mapVolumeAtPriceFunctions; // temporary entries till symbol encountered & assigned to a carrier

  using mapMarketDepthFunction_t = std::map<std::string, L2Base::fMarketDepth_t>;
  mapMarketDepthFunction_t m_mapMarketDepthFunction; // temporary entries till symbol encountered & assigned to carrier

  using pL2Base_t = std::shared_ptr<L2Base>;
  using mapL2Base_t = std::map<std::string,pL2Base_t>; // symbol name, L2Processing
  mapL2Base_t m_mapL2Base; //used for batch operations

  template<typename Msg>
  void SetCarrier( Carrier& carrier, const Msg& msg ) {

    pL2Base_t pL2Base;
    if ( 0 != msg.nOrderId ) {
      assert( 0 == msg.sMarketMaker.size() );
      pL2Base = OrderBased::Factory();
    }
    else {
      assert( 4 == msg.sMarketMaker.size() );
      pL2Base = MarketMaker::Factory();
    }
    m_mapL2Base.emplace( msg.sSymbolName, pL2Base );
    carrier = pL2Base.get();

    // may need mutex on this, vs foreground
    mapVolumeAtPriceFunctions_t::iterator iter = m_mapVolumeAtPriceFunctions.find( msg.sSymbolName );
    if ( m_mapVolumeAtPriceFunctions.end() != iter ) {
      carrier.pL2Base->Set( std::move( iter->second.fBid ), std::move( iter->second.fAsk ) );
      m_mapVolumeAtPriceFunctions.erase( iter );
    }

    mapMarketDepthFunction_t::iterator iterDelegate = m_mapMarketDepthFunction.find( msg.sSymbolName );
    if ( m_mapMarketDepthFunction.end() != iterDelegate ) {
      carrier.pL2Base->Set( std::move( iterDelegate->second ) );
      m_mapMarketDepthFunction.erase( iterDelegate );
    }
  }

  template<typename Msg, typename Function>
  void Call( const Msg& msg, Function f ) {

    if ( m_bSingle ) {
      if ( m_single.IsNull() ) {
        SetCarrier( m_single, msg );
      }
      (m_single.pL2Base->*f)( msg );
    }
    else {
      Carrier carrier = m_luSymbol.FindMatch( msg.sSymbolName );
      if ( carrier.IsNull() ) {
        SetCarrier( carrier, msg );
        m_luSymbol.AddPattern( msg.sSymbolName, carrier );
      }
      (carrier.pL2Base->*f)( msg );
    }
  }

};

} // namespace l2
} // namesapce iqfeed
} // namespace tf
} // namespace ou
