/************************************************************************
 * Copyright(c) 2020, One Unified. All rights reserved.                 *
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
 * File:    Strategy.h
 * Author:  raymond@burkholder.net
 * Project: ESBracketOrder
 * Created: January 6, 2020, 11:41 AM
 */

#pragma once

#include <map>
#include <array>

#include <OUCharting/ChartDVBasics.h>

#include <TFTimeSeries/BarFactory.h>

#include <TFTrading/Watch.h>
#include <TFTrading/Order.h>
#include <TFTrading/Position.h>
#include <TFTrading/Portfolio.h>
#include <TFTrading/DailyTradeTimeFrames.h>

#include <TFInteractiveBrokers/IBTWS.h>

class Strategy:
  public ou::ChartDVBasics,
  public ou::tf::DailyTradeTimeFrame<Strategy>
{
  friend class ou::tf::DailyTradeTimeFrame<Strategy>;
public:

  using pWatch_t = ou::tf::Watch::pWatch_t;

  Strategy( pWatch_t );
  virtual ~Strategy();

  void HandleButtonUpdate();
  void HandleButtonSend( ou::tf::OrderSide::enumOrderSide );
  //void HandleButtonSend();
  void HandleButtonCancel();

  void EmitBarSummary();

protected:
private:

  using pIB_t = ou::tf::ib::TWS::pProvider_t;
  using pOrder_t = ou::tf::Order::pOrder_t;
  using pPosition_t = ou::tf::Position::pPosition_t;
  using pPortfolio_t = ou::tf::Portfolio::pPortfolio_t;

  struct BarMatching {
    int high;   // -1 lower,   0 same, 1 higher
    int low;    // -1 lower,   0 same, 1 higher
    int close;  // -1 lower,   0 same, 1 higher
    int volume; // -1 lower,   0 same, 1 higher
    BarMatching(): high {}, low {}, close {}, volume {} {}
    BarMatching( int high_, int low_, int close_, int volume_ )
    : high( high_ ), low( low_ ), close( close_ ), volume( volume_ ) {}

    template<typename pod>
    int ComparePod( pod x, pod y ) const {
      if ( x == y ) return 0;
      else {
        if ( x < y ) return 1;
        else return -1;
      }
    }

    void Set( const ou::tf::Bar& bar1, const ou::tf::Bar& bar2 ) {
      high   = ComparePod( bar1.High(),   bar2.High() );
      low    = ComparePod( bar1.Low(),    bar2.Low() );
      close  = ComparePod( bar1.Close(),  bar2.Close() );
      volume = ComparePod( bar1.Volume(), bar2.Volume() );
    }

    bool operator<( const BarMatching& bm ) const {
      if ( high != bm.high ) {
        return high < bm.high;
      }
      else {
        if ( low != bm.low ) {
          return low < bm.low;
        }
        else {
          if ( close != bm.close ) {
            return close < bm.close;
          }
          else {
            if ( volume != bm.volume ) {
              return volume < bm.volume;
            }
            else return false;
          }
        }
      }
    }
  };

  enum class Tri { down=-1, zero=0, up=1 };
  using vTri_t = std::vector<Tri>;

  vTri_t m_vTriEmaLatest; // latest relative ema
  vTri_t m_vTriCrossing;  // indicates crossings

  struct Match {

    vTri_t m_vTri;

    void Reset() { m_vTri.clear(); }

    void Reset( vTri_t::size_type size ) {
      m_vTri.clear();
      m_vTri.assign( size, Tri::zero );
    }

    template<typename Pod>
    Tri Compare( Pod x, Pod y ) const {
      if ( x == y ) return Tri::zero;
      else {
        if ( x < y ) return Tri::up;
        else return Tri::down;
      }
    }

    template<typename Pod>
    void Append( Pod A, Pod B ) {
      Tri tri = Compare( A, B );
      m_vTri.emplace_back( tri );
    }

    bool operator<( const vTri_t& vTri ) const {
      assert( 0 < m_vTri.size() );
      assert( m_vTri.size() == vTri.size() );
      for ( vTri_t::size_type ix = 0; ix < m_vTri.size(); ix++ ) {
        Tri triA = m_vTri[ ix ];
        Tri triB =   vTri[ ix ];
        if ( triA != triB ) {
          return triA < triB;
        }
      }
      return false;
    }

  };

  //Match m_matchCrossing;

  using rMerrill_t = std::array<double,5>;
  using vMerrill_t = std::vector<rMerrill_t>;
  vMerrill_t m_vMerrill;

  ou::ChartEntryShape m_ceMerrill;

  struct Results {
    unsigned int cntOrders; // should match cntWins + cntLosses
    unsigned int cntWins;
    unsigned int cntLosses;
    double dblProfit;
    double dblLoss;
    Results(): cntOrders {}, cntWins {}, cntLosses {}, dblProfit {}, dblLoss {} {}
  };

  struct OrderResults {
    unsigned int cntInstances; // count of bar transitions
    Results longs;
    Results shorts;
    OrderResults(): cntInstances {} {}
  };

  using mapMatching_t = std::map<BarMatching,OrderResults>;
  using mapMatching_pair_t = std::pair<mapMatching_t::iterator, bool>;
  mapMatching_t m_mapMatching;

  size_t m_cntBars;
  ou::tf::Bar m_barLast;

  pIB_t m_pIB;
  pWatch_t m_pWatch;

  ou::tf::Trade m_tradeLast;
  ou::tf::Quote m_quoteLast;

  double m_dblAverageBarSize;
  ou::tf::BarFactory m_bfBar;

  pOrder_t m_pOrderEntry;
  pOrder_t m_pOrderProfit;
  pOrder_t m_pOrderStop;

  pPosition_t m_pPosition;

  struct StateInfo {
    size_t nBarDuration; // use bars to determine waiting period to entry, or cancel
    BarMatching barMatching;
    ou::tf::OrderSide::enumOrderSide sideEntry;
    double dblEntryPrice;
    StateInfo(): dblEntryPrice {} {};
  };

  StateInfo m_stateInfo;

  enum class EState { initial, entry_wait, entry_filling, entry_cancelling, exit_filling, cancel_wait, quiesce };
  EState m_state;

  using mapEntry_t = std::map<BarMatching,ou::tf::OrderSide::enumOrderSide>;
  mapEntry_t m_mapEntry;

  void HandleQuote( const ou::tf::Quote& );
  void HandleTrade( const ou::tf::Trade& );

  void HandleBarComplete( const ou::tf::Bar& );

  void CancelOrders();

  TimeFrame m_tfLatest;

  void HandleRHTrading( const ou::tf::Bar& );
  void HandleCancelling( const ou::tf::Bar& );
  void HandleGoingNeutral( const ou::tf::Bar& );

  void HandleOrderCancelled( const ou::tf::Order& );
  void HandleOrderFilled( const ou::tf::Order& );

  void HandleUnRealizedPL( const ou::tf::Position::PositionDelta_delegate_t& );  // unrealized p/l
  void HandleExecution( const ou::tf::Position::PositionDelta_delegate_t& );  // realized p/l
};
