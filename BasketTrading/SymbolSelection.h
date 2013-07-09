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

#include <map>
#include <set>

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;

#include <TFTimeSeries/TimeSeries.h>
#include <TFHDF5TimeSeries/HDF5DataManager.h>

class SymbolSelection {
public:

  struct InstrumentInfo {
    std::string sName;
    ou::tf::Bar barLast; // last bar in series for closing/ pivot calcs
    InstrumentInfo( const std::string& sName_, const ou::tf::Bar& bar )
      : sName( sName_ ), barLast( bar ) {};
    bool operator<( const InstrumentInfo& rhs ) { return sName < rhs.sName; };
  };
  struct InstrumentInfoCompare {
    bool operator() ( const InstrumentInfo& lhs, const InstrumentInfo& rhs ) { return lhs.sName < rhs.sName; };
  };
  typedef std::set<InstrumentInfo, InstrumentInfoCompare> setInstrumentInfo_t;

  explicit SymbolSelection( ptime eod );
  ~SymbolSelection( void );

  void Process( setInstrumentInfo_t& selected );

protected:
private:
  ou::tf::HDF5DataManager m_dm;

  ptime m_dtLast;  // last available eod

  ptime m_dtDarvasTrigger;
  // calculated:
  ptime m_dtEnd;  // dtLast + 1
  ptime m_dtOneYearAgo;
  ptime m_dt26WeeksAgo;
  ptime m_dtDateOfFirstBar;

  setInstrumentInfo_t* m_psetSymbols;

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
  
  void ProcessGroupItem( const std::string& sObjectPath, const std::string& sObjectName );

  typedef ou::tf::Bars::const_iterator citer;
  void CheckForDarvas( const InstrumentInfo& sSymbol, citer begin, citer end );
  void CheckFor10Percent( const InstrumentInfo& sSymbol, citer begin, citer end );
  void CheckForVolatility( const InstrumentInfo& sSymbol, citer begin, citer end );

  void WrapUp10Percent( setInstrumentInfo_t& selected );
  void WrapUpVolatility( setInstrumentInfo_t& selected );

};

