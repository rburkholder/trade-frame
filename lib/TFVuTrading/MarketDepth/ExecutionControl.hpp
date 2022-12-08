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
 * File:    ExecutionControl.hpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFVuTrading/MarketDepth
 * Created: 2022/11/21 14:59:32
 */

#pragma once

// overall controller for handling interface events and distributing requests to the models
// initiates orders and updates

// TODO: add stop orders

#include <map>

#include <TFTrading/Order.h>
#include <TFTrading/Position.h>

#include "PriceLevelOrder.hpp"

namespace ou {
namespace tf {
namespace l2 {

class PanelTrade;

class ExecutionControl {
public:

  using pOrder_t = ou::tf::Order::pOrder_t;
  using pPosition_t = ou::tf::Position::pPosition_t;

  ExecutionControl( pPosition_t, unsigned int nDefaultOrder );
  ~ExecutionControl();

  void Set( ou::tf::l2::PanelTrade* );

protected:
private:

  pPosition_t m_pPosition;

  ou::tf::l2::PanelTrade* m_pPanelTrade;

  unsigned int m_sizeDefaultOrder;

  // TODO: allow multiple orders per level
  using mapOrders_t = std::map<double,PriceLevelOrder>;
  // note: the exchange will complain if there are orders on both sides
  mapOrders_t m_mapAskOrders;
  mapOrders_t m_mapBidOrders;

  PriceLevelOrder m_KillPriceLevelOrder; // temporary for unrolling lambda call

  int m_nActiveOrders;
  double m_dblAveragePrice; // the zero point for the ladder

  using pWatch_t = ou::tf::Watch::pWatch_t;

  struct TrackStop {

    using fSubmit_t = std::function<void( ou::tf::OrderSide::EOrderSide )>;

    double m_dblStop;
    pWatch_t m_pWatch;
    fSubmit_t m_fSubmit; // create order & send
    ou::tf::OrderSide::EOrderSide m_side;

    bool m_bWatching;

    TrackStop()
    : m_dblStop {}, m_side( ou::tf::OrderSide::Unknown )
    , m_bWatching( false )
    , m_fSubmit( nullptr )
    {}

    // simple static stop
    TrackStop( ou::tf::OrderSide::EOrderSide side, double dblStop, pWatch_t pWatch, fSubmit_t&& fSubmit )
    : m_bWatching( false )
    , m_side( side ), m_dblStop( dblStop )
    , m_pWatch( std::move( pWatch ) )
    , m_fSubmit( std::move( fSubmit ) )
    {
      StartWatch();
    }

    TrackStop( TrackStop&& rhs )
    : m_side( rhs.m_side)
    , m_dblStop( rhs.m_dblStop )
    {
      m_bWatching = rhs.m_bWatching; // NOTE: needs to be prior to the StopWatch
      rhs.StopWatch();
      m_pWatch = std::move( rhs.m_pWatch ); // this comes after the stop
      m_fSubmit = std::move( rhs.m_fSubmit );
      if ( m_bWatching ) { // as previously set
        m_bWatching = false; // force a StartWatch
        StartWatch();
      }
    }

    ~TrackStop() {
      StopWatch();
      m_pWatch.reset();
      m_fSubmit = nullptr;
    }

    TrackStop& operator=( TrackStop&& rhs ) {
      if ( this != &rhs ) {
        m_side = rhs.m_side;
        m_dblStop = rhs.m_dblStop;
        m_bWatching = rhs.m_bWatching; // needs to be prior to the stop
        rhs.StopWatch();
        m_pWatch = std::move( rhs.m_pWatch ); // this is after the stop
        m_fSubmit = std::move( rhs.m_fSubmit );
        if ( m_bWatching ) { // as previously set
          m_bWatching = false; // force a StartWatch
          StartWatch();
        }
      }
      return *this;
    }

    void StartWatch() {
      if ( !m_bWatching ) {
        m_bWatching = true;
        m_pWatch->OnQuote.Add( MakeDelegate( this, &TrackStop::HandleQuote ) );
      }
    }

    void StopWatch() {
      if ( m_bWatching ) {
        m_bWatching = false;
        m_pWatch->OnQuote.Remove( MakeDelegate( this, &TrackStop::HandleQuote ) );
      }
    }

    // TODO: HandleQuoteForBidSide, HandleQuoteForAskSide
    void HandleQuote( const ou::tf::Quote& quote ) {
      switch ( m_side ) {
        case ou::tf::OrderSide::Sell: // bid side
          if ( m_dblStop >= quote.Ask() ) {
            StopWatch();
            if ( m_fSubmit ) m_fSubmit( m_side );
            // don't make additional changes, this object has been moved
          }
          break;
        case ou::tf::OrderSide::Buy: // ask side
          if ( m_dblStop <= quote.Bid() ) {
            StopWatch();
            if (m_fSubmit ) m_fSubmit( m_side );
            // don't make additional changes, this object has been moved
          }
          break;
        default:
          assert( false );
      }
    }
  };

  using mapTrackStop_t = std::map<double,TrackStop>;
  mapTrackStop_t m_mapAskTrackStop;
  mapTrackStop_t m_mapBidTrackStop;

  TrackStop m_KillTrackStop; // auto destroy outside of fSubmit_t

  void AskLimit( double );
  void AskStop( double );
  void AskCancel( double );

  void BidLimit( double );
  void BidStop( double );
  void BidCancel( double );

  void Cancel( double, mapOrders_t&, mapTrackStop_t& );

  void HandleExecution( const ou::tf::Execution& );
  void HandlePositionChanged( const ou::tf::Position& );

};

} // market depth
} // namespace tf
} // namespace ou
