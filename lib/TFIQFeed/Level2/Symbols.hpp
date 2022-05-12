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

#include <boost/log/trivial.hpp>

#include <OUCommon/KeyWordMatch.h>

#include <TFTimeSeries/DatedDatum.h>
#include <TFTimeSeries/TimeSeries.h>

#include "Dispatcher.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed
namespace l2 { // level 2 data

class Symbols;

using price_t = ou::tf::Trade::price_t;
using volume_t = ou::tf::Trade::volume_t;

enum class EOp { Insert, Update, Delete };

using fBookChanges_t = std::function<void(EOp,unsigned int,const ou::tf::Depth&)>; // operation, level, attributes
using fVolumeAtPrice_t = std::function<void(double,int,bool)>; // price, volume, add

template<typename Compare>  // ask is std::less<key>, bid is std::greater<key>, where key is currently double
class MapLevelAggregate {
  friend class Symbols;
private:

  struct LevelAggregate { // aggregates limit orders at each level

    unsigned int ixLevel; // maitain index of first n levels of order book, starts at 1, zero is dead
    volume_t nQuantity;
    int nOrders;  // currently used in OrderBased only

    // TODO: maintain set of order ids? will require vector/map update on each change
    //   may need multi-key map:  price/datetime or price/priority
    //   may need to adjust persisted message to incorporate priority/time/date
    //   but this may best be maintained in OrderBased

    LevelAggregate( volume_t nQuantity_ )
    : ixLevel {}, nQuantity( nQuantity_ ), nOrders( 1 ) {}
    LevelAggregate( const msg::OrderArrival::decoded& msg )
    : ixLevel {}, nQuantity( msg.nQuantity ), nOrders( 1 ) {}
    LevelAggregate( const LevelAggregate& rhs )
    : ixLevel{ rhs.ixLevel }, nQuantity( rhs.nQuantity ), nOrders( rhs.nOrders ) {}
  };

  using mapLevelAggregate_t = std::map<double,LevelAggregate,Compare>;

public:

  static const unsigned int max_ix = 10;

  MapLevelAggregate()
  : m_fVolumeAtPrice( nullptr )
  {}

  void Set( fVolumeAtPrice_t&& fVolumeAtPrice ) { // simple callback
    m_fVolumeAtPrice = std::move( fVolumeAtPrice );
  }

  void Set( fBookChanges_t&& fBookChanges ) {
    m_fBookChanges = std::move( fBookChanges );
  }

  void Add( const ou::tf::Depth& depth ) {

    price_t price( depth.Price() );
    volume_t volume( depth.Volume() );

    unsigned int ix {};

    typename mapLevelAggregate_t::iterator iterLevelAggregate = m_mapLevelAggregate.find( price );
    if ( m_mapLevelAggregate.end() == iterLevelAggregate ) {

      auto pair = m_mapLevelAggregate.emplace( std::pair( price, LevelAggregate( volume ) ) );
      assert( pair.second );
      iterLevelAggregate = pair.first;

      if ( m_fBookChanges ) { // this chunk needed for fBookChanges only

        // determine ix for inserted entry
        if ( m_mapLevelAggregate.begin() == iterLevelAggregate ) {
          ix = 1;
        }
        else { // pickup ix from predecessor
          typename mapLevelAggregate_t::iterator iterIx = iterLevelAggregate;
          iterIx--;
          ix = iterIx->second.ixLevel;
          if ( 0 == ix ) {} // outside of range
          else {
            if ( max_ix == ix ) {
              ix = 0;   // reached max_ix already, so outside of range
            }
            else {
              ix++;
            }
          }
        }

        // insert entry
        //iterLevelAggregate->second.ixLevel = ix;
        m_fBookChanges( EOp::Insert, ix, depth );
        for ( // renumber within range
          typename mapLevelAggregate_t::iterator iterIx = iterLevelAggregate;
          ( max_ix >= ix ) && ( m_mapLevelAggregate.end() != iterIx );
          iterIx++, ix++
        ) {
          iterIx->second.ixLevel = ix;
        }
      }
    }
    else { // exising level
      iterLevelAggregate->second.nQuantity += volume;
      iterLevelAggregate->second.nOrders++;
      if ( m_fBookChanges ) {
        ix = iterLevelAggregate->second.ixLevel;
        ou::tf::Depth depth_( depth.DateTime(), price, iterLevelAggregate->second.nQuantity );
        m_fBookChanges( EOp::Update, ix, depth_ );
      }
    }

    if ( m_fVolumeAtPrice ) m_fVolumeAtPrice( price, iterLevelAggregate->second.nQuantity, true );
  }

  void Delete( const ou::tf::Depth& depth ) {

    price_t price( depth.Price() );
    volume_t volume( depth.Volume() );

    typename mapLevelAggregate_t::iterator iterLevelAggregate = m_mapLevelAggregate.find( price );
    if ( m_mapLevelAggregate.end() == iterLevelAggregate ) {
      BOOST_LOG_TRIVIAL(error) << "MapLevelAggregate::Delete price not found: " << price;
    }
    else {
      assert( volume <= iterLevelAggregate->second.nQuantity ); // ensure no wrap around
      iterLevelAggregate->second.nQuantity -= volume;
      iterLevelAggregate->second.nOrders--;

      if ( m_fVolumeAtPrice ) m_fVolumeAtPrice( price, iterLevelAggregate->second.nQuantity, false );

      unsigned int ix { iterLevelAggregate->second.ixLevel };

      if ( 0 == iterLevelAggregate->second.nQuantity ) { // level to be removed
        assert( 0 == iterLevelAggregate->second.nOrders );

        if ( m_fBookChanges ) {

          ou::tf::Depth depth_( depth.DateTime(), price, 0 );
          m_fBookChanges( EOp::Delete, ix, depth_ );

          auto iterIx = iterLevelAggregate;
          iterIx++;
          while ( ( max_ix >= ix ) && ( m_mapLevelAggregate.end() != iterIx ) ) {
            iterIx->second.ixLevel = ix;
            ix++;
            iterIx++;
          }
          if ( ( max_ix < ix ) && ( m_mapLevelAggregate.end() != iterIx ) ) {
            iterIx->second.ixLevel = 0;
          }
        }

        m_mapLevelAggregate.erase( iterLevelAggregate );
      }
      else { // level changes but is not removed
        if ( m_fBookChanges ) {
          // need to pass in deletion message type so can match against ticks? or performed elsewhere?
          ou::tf::Depth depth_( depth.DateTime(), price, iterLevelAggregate->second.nQuantity );
          m_fBookChanges( EOp::Update, ix, depth_ );
        }
      }
    }
  }

protected:

  mapLevelAggregate_t m_mapLevelAggregate;

private:
  fBookChanges_t m_fBookChanges;
  fVolumeAtPrice_t m_fVolumeAtPrice;
};

// ==== L2Base

class L2Base {  // TODO: convert to CRTP?
  friend class Symbols;
public:

  L2Base();
  virtual ~L2Base() {}

  using fMarketDepthByMM_t = std::function<void(const DepthByMM&)>;
  using fMarketDepthByOrder_t = std::function<void(const DepthByOrder&)>;

  void Set( fBookChanges_t&& fBid, fBookChanges_t&& fAsk ) {
    m_LevelAggregateAsk.Set( std::move( fAsk ) );
    m_LevelAggregateBid.Set( std::move( fBid ) );
  }
  void Set( fVolumeAtPrice_t&& fBid, fVolumeAtPrice_t&& fAsk ) {
    m_LevelAggregateAsk.Set( std::move( fAsk ) );
    m_LevelAggregateBid.Set( std::move( fBid ) );
  }
  void Set( fMarketDepthByMM_t&& fMarketDepth ) {  // callback for mm structure
    m_fMarketDepthByMM = std::move( fMarketDepth );
  }
  void Set( fMarketDepthByOrder_t&& fMarketDepth ) { // callback for limit order based structure
    m_fMarketDepthByOrder = std::move( fMarketDepth );
  }

protected:

  using MapLevelAggregateAsk_t = MapLevelAggregate<std::less<double> >;
  using MapLevelAggregateBid_t = MapLevelAggregate<std::greater<double> >;

  MapLevelAggregateAsk_t m_LevelAggregateAsk;
  MapLevelAggregateBid_t m_LevelAggregateBid;

  fMarketDepthByMM_t m_fMarketDepthByMM;
  fMarketDepthByOrder_t m_fMarketDepthByOrder;

  // called from Symbols
  virtual void OnMBOAdd( const msg::OrderArrival::decoded& ) = 0;
  virtual void OnMBOSummary( const msg::OrderArrival::decoded& ) = 0;
  virtual void OnMBOUpdate( const msg::OrderArrival::decoded& ) = 0;
  virtual void OnMBODelete( const msg::OrderDelete::decoded& ) = 0;

  // local bid / ask dispatch into proper book
  void Add( const ou::tf::Depth& );
  void Update( const ou::tf::Depth&, price_t oldp, volume_t oldv );
  void Delete( const ou::tf::Depth& );

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

  void MarketDepth( const ou::tf::DepthByMM& ); // offline message submission

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

  using mapMM_t = std::map<DepthByMM::MMID_t,price_level>; // key=mm, value=price,volume
  mapMM_t m_mapMMAsk;
  mapMM_t m_mapMMBid;

  void DepthByMM_Update( const ou::tf::DepthByMM& );

  void MMLimitOrder_Update(
    const ou::tf::DepthByMM&,
    mapMM_t& );

  void DepthByMM_Delete( const ou::tf::DepthByMM& );

  void MMLimitOrder_Delete(
    const ou::tf::DepthByMM&,
    mapMM_t& );
};

// ==== OrderBased (Futures, etc)

class OrderBased: public L2Base {
public:

  using pOrderBased_t = std::shared_ptr<OrderBased>;

  OrderBased(): L2Base() {}
  virtual ~OrderBased() {}

  static pOrderBased_t Factory() { return std::make_shared<OrderBased>(); }

  virtual void OnMBOSummary( const msg::OrderArrival::decoded& msg );
  virtual void OnMBOAdd( const msg::OrderArrival::decoded& msg );
  virtual void OnMBOUpdate( const msg::OrderArrival::decoded& msg );
  virtual void OnMBODelete( const msg::OrderDelete::decoded& msg );

  void MarketDepth( const ou::tf::DepthByOrder& ); // offline message submission

protected:
private:

  struct Order {

    double dblPrice;
    uint64_t nPriority;
    volume_t nQuantity;
    char chOrderSide;
    uint8_t nPrecision;
    // ptime, if needed

    Order( const msg::OrderArrival::decoded& msg )
    : dblPrice( msg.dblPrice )
    , nQuantity( msg.nQuantity )
    , chOrderSide( msg.chOrderSide )
    , nPriority( msg.nPriority )
    , nPrecision( msg.nPrecision )
    {
      assert( 0 == msg.mmid.rch[0] ); // note: there is no MarketMaker in messages with an order ID
    }

    Order( const ou::tf::DepthByOrder& depth )
    : dblPrice( depth.Price() )
    , nQuantity( depth.Volume() )
    , chOrderSide( depth.Side() )
    , nPriority( depth.Priority() )
    , nPrecision( 0 )
    {}
  };

  using mapOrder_t = std::map<uint64_t,Order>; // key is order id
  mapOrder_t m_mapOrder;

   // interface for msg/depth
  void LimitOrderAdd( const ou::tf::DepthByOrder& );
  void LimitOrderUpdate( const ou::tf::DepthByOrder& );
  void LimitOrderDelete( const ou::tf::DepthByOrder& );

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

  void WatchAdd( const std::string&, fBookChanges_t&& fBid, fBookChanges_t&& fAsk );
  void WatchAdd( const std::string&, fVolumeAtPrice_t&& fBid, fVolumeAtPrice_t&& fAsk );
  void WatchAdd( const std::string&, L2Base::fMarketDepthByMM_t&& );     // compose MarketDepth & ship outside
  void WatchAdd( const std::string&, L2Base::fMarketDepthByOrder_t&& );  // compose MarketDepth & ship outside
  void WatchDel( const std::string& );

  void Single( bool ); // optimize for a single symbol stream

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

  struct BookChangeFunctions {

    fBookChanges_t fBid;
    fBookChanges_t fAsk;

    BookChangeFunctions(
      fBookChanges_t&& fBid_,
      fBookChanges_t&& fAsk_
    ): fBid( std::move( fBid_ ) ), fAsk( std::move( fAsk_ ) )
    {}

    BookChangeFunctions( BookChangeFunctions&& rhs )
    : fBid( std::move( rhs.fBid ) ), fAsk( std::move( rhs.fAsk ) ) {}
  }; // struct BookChangeFunctions

  using mapBookChangeFunctions_t = std::map<std::string,BookChangeFunctions>;
  mapBookChangeFunctions_t m_mapBookChangeFunctions; // temporary entries till symbol encountered & assigned to a carrier

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

  using mapMarketDepthFunctionByMM_t = std::map<std::string, L2Base::fMarketDepthByMM_t>;
  mapMarketDepthFunctionByMM_t m_mapMarketDepthFunctionByMM; // temporary entries till symbol encountered & assigned to carrier

  using mapMarketDepthFunctionByOrder_t = std::map<std::string, L2Base::fMarketDepthByOrder_t>;
  mapMarketDepthFunctionByOrder_t m_mapMarketDepthFunctionByOrder; // temporary entries till symbol encountered & assigned to carrier

  using pL2Base_t = std::shared_ptr<L2Base>;
  using mapL2Base_t = std::map<std::string,pL2Base_t>; // symbol name, L2Processing
  mapL2Base_t m_mapL2Base; //used for batch operations

  template<typename Msg>
  void SetCarrier( Carrier& carrier, const Msg& msg ) {

    pL2Base_t pL2Base;
    if ( 0 != msg.nOrderId ) {
      assert( 0 == msg.mmid.rch[0] );
      pL2Base = OrderBased::Factory();
    }
    else {
      //assert( 4 == msg.sMarketMaker.size() ); // TODO: check each character is non-zero
      pL2Base = MarketMaker::Factory();
    }
    m_mapL2Base.emplace( msg.sSymbolName, pL2Base );
    carrier = pL2Base.get();

    {
      // may need mutex on this, vs foreground
      mapBookChangeFunctions_t::iterator iter = m_mapBookChangeFunctions.find( msg.sSymbolName );
      if ( m_mapBookChangeFunctions.end() != iter ) {
        carrier.pL2Base->Set( std::move( iter->second.fBid ), std::move( iter->second.fAsk ) );
        m_mapBookChangeFunctions.erase( iter );
      }
    }

    {
      // may need mutex on this, vs foreground
      mapVolumeAtPriceFunctions_t::iterator iter = m_mapVolumeAtPriceFunctions.find( msg.sSymbolName );
      if ( m_mapVolumeAtPriceFunctions.end() != iter ) {
        carrier.pL2Base->Set( std::move( iter->second.fBid ), std::move( iter->second.fAsk ) );
        m_mapVolumeAtPriceFunctions.erase( iter );
      }
    }

    {
      mapMarketDepthFunctionByMM_t::iterator iterDelegate = m_mapMarketDepthFunctionByMM.find( msg.sSymbolName );
      if ( m_mapMarketDepthFunctionByMM.end() != iterDelegate ) {
        carrier.pL2Base->Set( std::move( iterDelegate->second ) );
        m_mapMarketDepthFunctionByMM.erase( iterDelegate );
      }
    }

    {
      mapMarketDepthFunctionByOrder_t::iterator iterDelegate = m_mapMarketDepthFunctionByOrder.find( msg.sSymbolName );
      if ( m_mapMarketDepthFunctionByOrder.end() != iterDelegate ) {
        carrier.pL2Base->Set( std::move( iterDelegate->second ) );
        m_mapMarketDepthFunctionByOrder.erase( iterDelegate );
      }
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
