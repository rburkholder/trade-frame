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

class SymbolSelection {
public:

  struct InstrumentInfo {
    std::string sName;
    const ou::tf::Bar barLast; // last bar in series for closing/ pivot calcs
    double dblStop;  // calculated stop price, if any
    InstrumentInfo( const std::string& sName_, const ou::tf::Bar& bar )
      : sName( sName_ ), barLast( bar ), dblStop {} {};
    bool operator<( const InstrumentInfo& rhs ) const { return sName < rhs.sName; };
  };

  using fSelected_t = std::function<void(const InstrumentInfo&)>;

  typedef std::set<InstrumentInfo> setInstrumentInfo_t;

  explicit SymbolSelection( const ptime eod, fSelected_t );
  ~SymbolSelection( void );

  //void Process( setInstrumentInfo_t& selected );

protected:
private:

  ou::tf::Bars::size_type m_nMinPivotBars;

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

  typedef ou::tf::Bars::const_iterator citerBars;
  void CheckForDarvas( citerBars begin, citerBars end, InstrumentInfo&, fSelected_t& );
  void CheckFor10Percent( citerBars begin, citerBars end, const InstrumentInfo& );
  void CheckForVolatility( citerBars begin, citerBars end, const InstrumentInfo& );
  void CheckForPivots( citerBars begin, citerBars end, const InstrumentInfo& );
  void CheckForRange( citerBars begin, citerBars end, const InstrumentInfo& );

  void WrapUp10Percent( setInstrumentInfo_t& selected );
  void WrapUpVolatility( setInstrumentInfo_t& selected );
  void WrapUpPivots( setInstrumentInfo_t& selected );

};

