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

#include <functional>

#include <boost/date_time/posix_time/posix_time.hpp>
namespace pt = boost::posix_time;
namespace gregorian = boost::gregorian;

#include <TFHDF5TimeSeries/HDF5DataManager.h>
#include <TFHDF5TimeSeries/HDF5IterateGroups.h>
#include <TFHDF5TimeSeries/HDF5TimeSeriesContainer.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

// currently assumes daily bars are being scanned, will need to generalize if other types are being used.

template<typename S, typename TS> // S=shared data structure, TS=time series type to be used
class InstrumentFilter {
public:

  using cbUseGroup_t = std::function<bool (S&, const std::string&, const std::string&)>;  // use a particular group in HDF5
  using cbFilter_t   = std::function<bool (S&, const std::string&, const TS&)>; // used for filtering on fields in the Time Series
  using cbResult_t   = std::function<void (S&, const std::string&, const std::string&, const TS&)>;  // send the chosen filtered results back: structure, path, name, timeseries

  InstrumentFilter(
    const std::string& sPath,
    pt::ptime dtBegin, pt::ptime dtEnd,
    typename TS::size_type,
    S&,
    cbUseGroup_t, cbFilter_t, cbResult_t );
  ~InstrumentFilter( void ) {};

protected:
private:
  bool m_bSendThroughFilter;
  S& m_struct;
  typename TS::size_type m_nRequiredDays;
  std::string m_sRootPath;

  pt::ptime m_dtDate1;
  pt::ptime m_dtDate2;

  cbUseGroup_t m_cbUseGroup;
  cbFilter_t m_cbFilter;
  cbResult_t m_cbResult;

  ou::tf::HDF5DataManager m_dm;

  void HandleGroup( const std::string& sPath, const std::string& sObject );
  void HandleObject( const std::string& sPath, const std::string& sObject );
};

template<typename S, typename TS>
InstrumentFilter<S,TS>::InstrumentFilter(
  const std::string& sPath, pt::ptime dtBegin, pt::ptime dtEnd,
  typename TS::size_type nRequiredDays, S& struct_,
  cbUseGroup_t cbUseGroup, cbFilter_t cbFilter, cbResult_t cbResult )
  : m_cbUseGroup( cbUseGroup ), m_cbFilter( cbFilter ), m_cbResult( cbResult ),
    m_dtDate1( dtBegin ), m_dtDate2( dtEnd ),
    m_struct( struct_ ),
    m_dm( ou::tf::HDF5DataManager::RO ),
    m_bSendThroughFilter( false ), m_nRequiredDays( nRequiredDays ), m_sRootPath( sPath )
{
  if ( dtBegin >= dtEnd ) {
    throw std::runtime_error( "dtBegin >= dtEnd" );
  }

  namespace ph = std::placeholders;
  ou::tf::hdf5::IterateGroups ig(
    m_dm, m_sRootPath,
    std::bind( &InstrumentFilter<S,TS>::HandleGroup, this, ph::_1, ph::_2 ),
    std::bind( &InstrumentFilter<S,TS>::HandleObject, this, ph::_1, ph::_2 )
    );
}

template<typename S, typename TS>
void InstrumentFilter<S,TS>::HandleGroup( const std::string& sPath, const std::string& sObjectName ) {
  m_bSendThroughFilter = m_cbUseGroup( m_struct, sPath, sObjectName );
}

template<typename S, typename TS>
void InstrumentFilter<S,TS>::HandleObject( const std::string& sPath, const std::string& sObjectName ) {
  if ( m_bSendThroughFilter ) {
    typename ou::tf::HDF5TimeSeriesContainer<typename TS::datum_t> tsRepository( m_dm, sPath );
    typename ou::tf::HDF5TimeSeriesContainer<typename TS::datum_t>::iterator begin, end;
    begin = std::lower_bound( tsRepository.begin(), tsRepository.end(), m_dtDate1 );
    end   = std::lower_bound( begin, tsRepository.end(), m_dtDate2 );
    hsize_t cnt = end - begin;
    if ( m_nRequiredDays <= cnt ) {
      TS timeseries;
      timeseries.Resize( cnt );
      tsRepository.Read( begin, end, &timeseries );
      bool b = m_cbFilter( m_struct, sObjectName, timeseries );
      if ( b ) {
        m_cbResult( m_struct, sPath, sObjectName, timeseries );
      }
    }
  }
}

} // namespace tf
} // namespace ou
