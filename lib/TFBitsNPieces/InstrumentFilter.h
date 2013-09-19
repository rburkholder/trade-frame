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

// started 2013/09/19

#include <boost/function.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;

#include <TFHDF5TimeSeries/HDF5DataManager.h>
#include <TFHDF5TimeSeries/HDF5IterateGroups.h>
#include <TFHDF5TimeSeries/HDF5TimeSeriesContainer.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

// currently assumes daily bars are being scanned, will need to generalize if other types are being used.

template<typename S, typename TS> // S=shared data structure, TS=time series type to be used
class InstrumentFilter {
public:
  typedef boost::function<bool (S&, const std::string&, const std::string&)> cbUseGroup_t;  // use a particular group in HDF5
  typedef boost::function<bool (S&, const std::string&, TS&)> cbFilter_t; // used for filtering on fields in the Time Series
  typedef boost::function<void (S&, const std::string&, TS&)> cbResult_t;  // send the chosen filtered results back
  InstrumentFilter( const std::string& sPath, ptime dtBegin, ptime dtEnd, typename TS::size_type, 
    cbUseGroup_t, cbFilter_t, cbResult_t );
  ~InstrumentFilter( void ) {};
  void Run( void );
protected:
private:
  bool m_bSendThroughFilter;
  S m_struct;
  typename TS::size_type m_nRequiredDays;
  std::string m_sRootPath;

  ptime m_dtDate1;
  ptime m_dtDate2;

  cbUseGroup_t m_cbUseGroup;
  cbFilter_t m_cbFilter;
  cbResult_t m_cbResult;

  ou::tf::HDF5DataManager m_dm;

  void HandleGroup( const std::string& sPath, const std::string& sObject );
  void HandleObject( const std::string& sPath, const std::string& sObject );
};

template<typename S, typename TS>
InstrumentFilter<S,TS>::InstrumentFilter( 
  const std::string& sPath, ptime dtBegin, ptime dtEnd, typename TS::size_type nRequiredDays, 
  cbUseGroup_t cbUseGroup, cbFilter_t cbFilter, cbResult_t cbResult ) 
  : m_cbUseGroup( cbUseGroup ), m_cbFilter( cbFilter ), m_cbResult( cbResult ), 
    m_dtDate1( dtBegin ), m_dtDate2( dtEnd ),
    m_dm( ou::tf::HDF5DataManager::RO ),
  m_bSendThroughFilter( false ), m_nRequiredDays( nRequiredDays ), m_sRootPath( sPath )
{

  if ( dtBegin >= dtEnd ) {
    throw std::runtime_error( "dtBegin >= dtEnd" );
  }

}

template<typename S, typename TS>
void InstrumentFilter<S,TS>::Run( void ) {
  namespace args = boost::phoenix::placeholders;
  ou::tf::hdf5::IterateGroups ig( 
    m_sRootPath, 
    boost::phoenix::bind( &InstrumentFilter<S,TS>::HandleGroup, this, args::arg1, args::arg2 ), 
    boost::phoenix::bind( &InstrumentFilter<S,TS>::HandleObject, this, args::arg1, args::arg2 ) 
    );
}

template<typename S, typename TS>
void InstrumentFilter<S,TS>::HandleGroup( const std::string& sPath, const std::string& sObject ) {
  m_bSendThroughFilter = m_cbUseGroup( m_struct, sPath, sObject );
}

template<typename S, typename TS>
void InstrumentFilter<S,TS>::HandleObject( const std::string& sPath, const std::string& sObject ) {
  if ( m_bSendThroughFilter ) {
    ou::tf::HDF5TimeSeriesContainer<TS::datum_t> tsRepository( m_dm, sPath );
    ou::tf::HDF5TimeSeriesContainer<TS::datum_t>::iterator begin, end;
    begin = lower_bound( tsRepository.begin(), tsRepository.end(), m_dtDate1 );
    end = lower_bound( begin, tsRepository.end(), m_dtDate2 ); 
    hsize_t cnt = end - begin;
    if ( m_nRequiredDays <= cnt ) {
      TS timeseries;
      timeseries.Resize( cnt );
      tsRepository.Read( begin, end, &timeseries );
      bool b = m_cbFilter( m_struct, sObject, timeseries );
      if ( b ) {
        m_cbResult( m_struct, sObject, timeseries );
      }
    }
  }
}


} // namespace tf
} // namespace ou
