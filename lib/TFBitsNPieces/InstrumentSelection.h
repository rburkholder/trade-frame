/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

// 2013/08/24, started in Phi2, moved here, can be further expaned with code 
//  located in other projects, generalized with additional criteria,
//  perhaps use boost::fusion characteristics to use additional selection criteria

#include <set>
#include <map>

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;

#include <TFTimeSeries/TimeSeries.h>
#include <TFHDF5TimeSeries/HDF5DataManager.h>

class InstrumentSelection {
public:

  struct Info {
    std::string sName;
    ou::tf::Bar barLast; // last bar in series for closing/ pivot calcs
    Info( const std::string& sName_, const ou::tf::Bar& bar )
      : sName( sName_ ), barLast( bar ) {};
    bool operator<( const Info& rhs ) { return sName < rhs.sName; };
  };
  struct InfoCompare {
    bool operator() ( const Info& lhs, const Info& rhs ) { return lhs.sName < rhs.sName; };
  };
  typedef std::set<Info> setInfo_t;

  explicit InstrumentSelection(void);
  ~InstrumentSelection(void);

  void Process( const ptime& eod, setInfo_t& selected );

protected:
private:

  static const unsigned int m_nDaysToAverage = 14;

  ou::tf::HDF5DataManager m_dm;

  ptime m_dtDate1;
  ptime m_dtDate2;

  setInfo_t* m_psetInstruments;

  typedef std::multimap<ou::tf::Bar::volume_t, Info> mapInfoRankedByVolume_t;
  typedef std::pair<ou::tf::Bar::volume_t, Info> pairInfoRankedByVolume_t;

  mapInfoRankedByVolume_t m_mapInfoRankedByVolume;

  void ProcessGroupItem( const std::string& sObjectPath, const std::string& sObjectName );

};

