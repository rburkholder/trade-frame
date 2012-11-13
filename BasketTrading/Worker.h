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

#include <string>

#include <boost/thread/thread.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;

#include <TFHDF5TimeSeries/HDF5DataManager.h>

class Worker {
public:
  Worker(void);
  ~Worker(void);
  void operator()( void );
  void Join( void ) { m_pThread->join(); };
protected:
private:

  boost::thread* m_pThread;

  ou::tf::HDF5DataManager m_dm;

  // provided:
  ptime m_dtLast;  // last available eod
  ptime m_dtDarvasTrigger;
  // calculated:
  ptime m_dtEnd;  // dtLast + 1
  ptime m_dtOneYearAgo;
  ptime m_dt26WeeksAgo;

  struct MaxNegativesCompare {
    bool operator() ( double dbl1, double dbl2 ) {
      return dbl2 < dbl1; // reverse form of operator so most negative at end of list
    }
  };

  static const unsigned short m_nMaxInList = 10;  // maximum of 10 items in each list
  std::multimap<double, std::string> m_mapMaxPositives;
  std::multimap<double, std::string, MaxNegativesCompare> m_mapMaxNegatives;

  std::multimap<double, std::string> m_mapMaxVolatility;
  
  void ProcessGroupItem( const std::string& sObjectPath, const std::string& sObjectName );

  typedef ou::tf::Bars::const_iterator citer;
  void CheckForDarvas( const std::string& sSymbol, citer begin, citer end );
  void CheckFor10Percent( const std::string& sSymbol, citer begin, citer end );
  void CheckForVolatility( const std::string& sSymbol, citer begin, citer end );

  void WrapUp10Percent( void );
  void WrapUpVolatility( void );

};

