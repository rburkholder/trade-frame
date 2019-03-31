/*** *********************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

#pragma once

// Project: BasketTrading

#include <map>
#include <set>
#include <functional>

#include <TFTimeSeries/TimeSeries.h>

struct InstrumentInfo {
  const std::string sName;
  const ou::tf::Bar bar; // last bar in series for closing/ pivot calcs

  InstrumentInfo( const std::string& sName_, const ou::tf::Bar& bar_ )
    : sName( sName_ ), bar( bar_ )
    {}
  InstrumentInfo( const InstrumentInfo& rhs )
    : sName( rhs.sName ), bar( rhs.bar )
    {}
  InstrumentInfo( const InstrumentInfo&& rhs )
    : sName( std::move( rhs.sName ) ), bar( rhs.bar )
    {}
  bool operator<( const InstrumentInfo& rhs ) const { return sName < rhs.sName; };
};

// ** IIDarvas

struct IIDarvas: InstrumentInfo {
  double dblStop;  // calculated stop price, if any
  IIDarvas( const std::string& sName, const ou::tf::Bar& bar )
    : InstrumentInfo( sName, bar ), dblStop{}
  {}
};

// ** IIPivot

struct IIPivot: InstrumentInfo {
  enum class PState { PV, AbovePV, BelowPV, BtwnPVR1, BtwnPVS1 };
  enum class Direction { Unknown, Up, Down };

  using Pair = std::pair<double,Direction>;

  double dblR1;
  double dblPV;
  double dblS1;
  double dblProbabilityAboveAndUp;
  double dblProbabilityAboveAndDown;
  double dblProbabilityBelowAndUp;
  double dblProbabilityBelowAndDown;
  IIPivot( const std::string& sName, const ou::tf::Bar& bar )
    : InstrumentInfo( sName, bar ),
      dblR1 {}, dblPV {}, dblS1 {},
      dblProbabilityAboveAndUp {}, dblProbabilityAboveAndDown {},
      dblProbabilityBelowAndUp {}, dblProbabilityBelowAndDown {}
  {}
  IIPivot( const IIPivot& rhs )
    : InstrumentInfo( rhs ),
      dblR1( rhs.dblR1 ), dblPV( rhs.dblPV ), dblS1( rhs.dblS1 ),
      dblProbabilityAboveAndUp( rhs.dblProbabilityAboveAndUp ),
      dblProbabilityAboveAndDown( rhs.dblProbabilityAboveAndDown ),
      dblProbabilityBelowAndUp( rhs.dblProbabilityBelowAndUp ),
      dblProbabilityBelowAndDown( rhs.dblProbabilityBelowAndDown )
  {}
  IIPivot( const IIPivot&& rhs )
    : InstrumentInfo( std::move( rhs ) ),
      dblR1( rhs.dblR1 ), dblPV( rhs.dblPV ), dblS1( rhs.dblS1 ),
      dblProbabilityAboveAndUp( rhs.dblProbabilityAboveAndUp ),
      dblProbabilityAboveAndDown( rhs.dblProbabilityAboveAndDown ),
      dblProbabilityBelowAndUp( rhs.dblProbabilityBelowAndUp ),
      dblProbabilityBelowAndDown( rhs.dblProbabilityBelowAndDown )
  {}

  Pair Test( double price ) const {
    Pair pair;
    pair.first = 0.0;
    pair.second = Direction::Unknown;
    PState state = PState::PV;
    Direction direction = Direction::Unknown;
    if ( dblPV < price ) {
      state = PState::AbovePV;
      if ( dblR1 > price ) {
        state = PState::BtwnPVR1;
        if ( dblProbabilityAboveAndUp >= dblProbabilityAboveAndDown ) {
          pair.first = dblProbabilityAboveAndUp;
          pair.second = Direction::Up;
        }
        else {
          pair.first = dblProbabilityAboveAndDown;
          pair.second = Direction::Down;
        }
      }
    }
    else {
      if ( dblPV > price ) {
        state = PState::BelowPV;
        if ( dblS1 < price ) {
          state = PState::BtwnPVS1;
          if ( dblProbabilityBelowAndDown >= dblProbabilityBelowAndUp ) {
            pair.first = dblProbabilityBelowAndDown;
            pair.second = Direction::Down;
          }
          else {
            pair.first = dblProbabilityBelowAndUp;
            pair.second = Direction::Up;
          }
        }
      }
    }
    return pair;
  }

};

// ** SymbolSelection

class SymbolSelection {
public:

  using fSelectedDarvas_t = std::function<void(const IIDarvas&)>;
  using fSelectedPivot_t  = std::function<void(const IIPivot&)>;

  using setInstrumentInfo_t = std::set<InstrumentInfo>;

  SymbolSelection( const ptime dtLast );
  SymbolSelection( const ptime dtLast, fSelectedDarvas_t );
  SymbolSelection( const ptime dtLast, fSelectedPivot_t );
  ~SymbolSelection( void );

protected:
private:

  ou::tf::Bars::size_type m_nMinBars;

  ptime m_dtDarvasTrigger;
  ptime m_dtLast;  // last available eod
  // calculated:
  ptime m_dtOneYearAgo;
  ptime m_dt26WeeksAgo;
  ptime m_dtDateOfFirstBar;

  using mapPivotRanking_t = std::multimap<boost::uint32_t,InstrumentInfo>;
  mapPivotRanking_t m_mapPivotRanking;

  using mapRangeRanking_t = std::multimap<int,InstrumentInfo>;
  mapRangeRanking_t m_mapRangeRanking;

  struct MaxNegativesCompare {
    bool operator() ( double dbl1, double dbl2 ) const {
      return dbl2 < dbl1; // reverse form of operator so most negative at end of list
    }
  };

  using mapRankingPos_t = std::multimap<double, InstrumentInfo>;
  using mapRankingNeg_t = std::multimap<double, InstrumentInfo, MaxNegativesCompare>;
  using pairRanking_t = std::pair<double,InstrumentInfo>;

  static const unsigned short m_nMaxInList = 10;  // maximum of 10 items in each list
  mapRankingPos_t m_mapMaxPositives;
  mapRankingNeg_t m_mapMaxNegatives;

  mapRankingPos_t m_mapMaxVolatility;

  using citerBars = ou::tf::Bars::const_iterator;

  void CheckForDarvas( const ou::tf::Bars&, IIDarvas&, fSelectedDarvas_t& );
  void CheckFor10Percent( citerBars begin, citerBars end, const InstrumentInfo& );
  void CheckForVolatility( citerBars begin, citerBars end, const InstrumentInfo& );
  void CheckForPivot( citerBars begin, citerBars end, const InstrumentInfo& );
  void CheckForRange( citerBars begin, citerBars end, const InstrumentInfo& );

  void WrapUp10Percent( setInstrumentInfo_t& selected );
  void WrapUpVolatility( setInstrumentInfo_t& selected );
  void WrapUpPivots( setInstrumentInfo_t& selected );

};

