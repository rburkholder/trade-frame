/************************************************************************
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

//#include <boost/date_time/posix_time/posix_time.hpp>
//using namespace boost::posix_time;
//using namespace boost::gregorian;

//#include <TFTimeSeries/TimeSeries.h>

struct InstrumentInfo {
  const std::string sName;
  const ou::tf::Bar barLast; // last bar in series for closing/ pivot calcs

  InstrumentInfo( const std::string& sName_, const ou::tf::Bar& bar )
    : sName( sName_ ), barLast( bar )
    {};
  bool operator<( const InstrumentInfo& rhs ) const { return sName < rhs.sName; };
};

struct IIDarvas: InstrumentInfo {
  double dblStop;  // calculated stop price, if any
  IIDarvas( const std::string& sName, const ou::tf::Bar& bar )
    : InstrumentInfo( sName, bar ), dblStop{}
  {}
};

struct IIPivot: InstrumentInfo {
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
};

class SymbolSelection {
public:

  using fSelectedDarvas_t = std::function<void(const IIDarvas&)>;
  using fSelectedPivot_t  = std::function<void(const IIPivot&)>;

  using setInstrumentInfo_t = std::set<InstrumentInfo>;
  //using setIIDarvas_t       = std::set<IIDarvas>;
  //using setIIPivot_t        = std::set<IIPivot>;

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

  typedef std::multimap<boost::uint32_t,InstrumentInfo> mapPivotRanking_t;
  mapPivotRanking_t m_mapPivotRanking;

  typedef std::multimap<int,InstrumentInfo> mapRangeRanking_t;
  mapRangeRanking_t m_mapRangeRanking;

  struct MaxNegativesCompare {
    bool operator() ( double dbl1, double dbl2 ) {
      return dbl2 < dbl1; // reverse form of operator so most negative at end of list
    }
  };

  typedef std::multimap<double, InstrumentInfo> mapRankingPos_t;
  typedef std::multimap<double, InstrumentInfo, MaxNegativesCompare> mapRankingNeg_t;
  typedef std::pair<double,InstrumentInfo> pairRanking_t;

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

