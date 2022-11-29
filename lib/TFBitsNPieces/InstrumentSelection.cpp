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

#include "stdafx.h"

#include <TFHDF5TimeSeries/HDF5IterateGroups.h>
#include <TFHDF5TimeSeries/HDF5TimeSeriesContainer.h>

#include "InstrumentSelection.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

InstrumentSelection::InstrumentSelection(void): m_dm( ou::tf::HDF5DataManager::RO ) {
}

InstrumentSelection::~InstrumentSelection(void) {
}

void InstrumentSelection::Process( const ptime& eod, setInfo_t& selected ) {

  m_psetInstruments = &selected;

  m_dtDate1 = eod - date_duration( m_nDaysToAverage );  // average two weeks of volume
  m_dtDate2 = eod;

  std::cout << "Running" << std::endl;

  ou::tf::HDF5IterateGroups groups;
  groups.SetOnHandleObject( MakeDelegate( this, &InstrumentSelection::ProcessGroupItem ) );
  try {
    int result = groups.Start( "/bar/86400/" );
  }
  catch (...) {
    std::cout << "ouch" << std::endl;
  }

  std::cout << "History Scanned." << std::endl;

  std::cout << "Map size: " << m_mapInfoRankedByVolume.size() << std::endl;
  mapInfoRankedByVolume_t::const_iterator begin, end;
  begin = m_mapInfoRankedByVolume.begin();
  for ( mapInfoRankedByVolume_t::const_iterator citer = m_mapInfoRankedByVolume.begin(); citer != m_mapInfoRankedByVolume.end(); ++citer ) {
    std::cout << citer->second.sName << ": " << citer->first << std::endl;
  }

}

struct AverageVolume {
private:
  ou::tf::Bar::volume_t m_nTotalVolume;
  unsigned long m_nNumberOfValues;
protected:
public:
  AverageVolume() : m_nTotalVolume( 0 ), m_nNumberOfValues( 0 ) {};
  void operator() ( const ou::tf::Bar& bar ) {
    m_nTotalVolume += bar.Volume();
    ++m_nNumberOfValues;
  }
  operator ou::tf::Bar::volume_t() { return m_nTotalVolume / m_nNumberOfValues; };
};

void InstrumentSelection::ProcessGroupItem( const std::string& sObjectPath, const std::string& sObjectName ) {
  ou::tf::HDF5TimeSeriesContainer<ou::tf::Bar> barRepository( m_dm, sObjectPath );
  ou::tf::HDF5TimeSeriesContainer<ou::tf::Bar>::iterator begin, end;
  begin = std::lower_bound( barRepository.begin(), barRepository.end(), m_dtDate1 );
  end = std::lower_bound( begin, barRepository.end(), m_dtDate2 );
  hsize_t cnt = end - begin;
  if ( 8 < cnt ) {
//    ptime dttmp = (*(end-1)).DateTime();
//    std::cout << sObjectName << m_dtLast << ", " << dttmp << std::endl;
    ou::tf::Bars bars;
    bars.Resize( cnt );
    barRepository.Read( begin, end, &bars );
    ou::tf::Bars::const_iterator iterVolume = bars.begin();
    ou::tf::Bar::volume_t volAverage = std::for_each( iterVolume, bars.end(), AverageVolume() );
    if ( ( 1000000 < volAverage )
      && ( 12.0 <= bars.Last()->Close() )
      && ( 80.0 >= bars.Last()->Close() ) ) {
        Info info( sObjectName, *bars.Last() );
        m_mapInfoRankedByVolume.insert( pairInfoRankedByVolume_t( volAverage, info ) );
    }
  }
}

} // namespace tf
} // namespace ou
