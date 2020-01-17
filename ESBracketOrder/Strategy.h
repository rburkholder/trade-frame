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

#include "TFTimeSeries/DatedDatum.h"
#include <map>

#include <OUCharting/ChartDVBasics.h>

#include <TFTimeSeries/BarFactory.h>

#include <TFTrading/Watch.h>
#include <TFTrading/Order.h>
#include <TFTrading/Position.h>
#include <TFTrading/Portfolio.h>

#include <TFInteractiveBrokers/IBTWS.h>

class Strategy:
  public ou::ChartDVBasics
{
public:

  using pWatch_t = ou::tf::Watch::pWatch_t;

  Strategy( pWatch_t );
  virtual ~Strategy();

  void HandleButtonUpdate();
  void HandleButtonSend( ou::tf::OrderSide::enumOrderSide );
  //void HandleButtonSend();
  void HandleButtonCancel();

protected:
private:

  using pIB_t = ou::tf::IBTWS::pProvider_t;
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
        if ( x < y ) return -1;
        else return 1;
      }
    }
    void Compare( const ou::tf::Bar& bar1, const ou::tf::Bar& bar2 ) {
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

  struct Results {
    unsigned int cntOrders; // should match cntWins + cntLosses
    unsigned int cntWins;
    unsigned int cntLosses;
    Results(): cntOrders {}, cntWins {}, cntLosses {} {}
  };

  struct OrderResults {
    unsigned int cntBars; // count of bar transitions
    Results longs;
    Results shorts;
    OrderResults(): cntBars {} {}
  };

  using mapMatching_t = std::map<BarMatching,OrderResults>;
  using mapMatching_pair_t = std::pair<mapMatching_t::iterator, bool>;
  mapMatching_t m_mapMatching;

  size_t m_cntBars;
  ou::tf::Bar m_barLast;

  pIB_t m_pIB;
  pWatch_t m_pWatch;

  ou::tf::Trade m_tradeLast;

  double m_dblAverageBarSize;
  ou::tf::BarFactory m_bfBar;

  pOrder_t m_pOrderEntry;
  pOrder_t m_pOrderProfit;
  pOrder_t m_pOrderStop;

  pPosition_t m_pPosition;

  BarMatching m_BarMatching;

  void HandleQuote( const ou::tf::Quote& );
  void HandleTrade( const ou::tf::Trade& );

  void HandleBarComplete( const ou::tf::Bar& );

  void HandleUnRealizedPL( const ou::tf::Position::PositionDelta_delegate_t& );  // unrealized p/l
  void HandleExecution( const ou::tf::Position::PositionDelta_delegate_t& );  // realized p/l
};
