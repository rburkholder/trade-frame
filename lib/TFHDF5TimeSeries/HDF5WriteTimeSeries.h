/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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
#include <stdexcept>

#include "HDF5TimeSeriesContainer.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

// TS: TimeSeries
template<class TS> class HDF5WriteTimeSeries {
public:

  typedef typename TS::datum_t DD;  // type for inherited type with base of CDatedDatum

  HDF5WriteTimeSeries<TS>( HDF5DataManager& dm );  // dm needs to be read/write
  HDF5WriteTimeSeries<TS>( HDF5DataManager& dm, bool bDeflatable, bool bExpandable, int nDeflate = 5, hsize_t nChunkSize = 1024 );
  virtual ~HDF5WriteTimeSeries<TS>( void );
  void Write( const std::string &sPathName, TS* timeseries );

protected:
private:
  HDF5DataManager& m_dm;
  bool m_bDeflatable;
  int m_nDeflate;
  bool m_bExpandable;
  hsize_t m_nChunkSize;
};

template<class TS> HDF5WriteTimeSeries<TS>::HDF5WriteTimeSeries( HDF5DataManager& dm )
: m_dm( dm ), m_bDeflatable( false ), m_bExpandable( false ), m_nDeflate( 0 ), m_nChunkSize( 0 )
{
}

template<class TS> HDF5WriteTimeSeries<TS>::HDF5WriteTimeSeries( HDF5DataManager& dm, bool bDeflatable, bool bExpandable, int nDeflate, hsize_t nChunkSize )
: m_dm( dm ), m_bDeflatable( bDeflatable ), m_bExpandable( bExpandable ), m_nDeflate( nDeflate ), m_nChunkSize( nChunkSize )
{
  if ( bDeflatable ) assert( 0 < nDeflate );
  if ( bExpandable ) assert( 0 < nChunkSize );
}

template<class TS> HDF5WriteTimeSeries<TS>::~HDF5WriteTimeSeries() {
}

template<class TS> void HDF5WriteTimeSeries<TS>::Write(const std::string &sPathName, TS* timeseries) {

  if ( 0 == timeseries->Size() ) {
    throw std::invalid_argument( "zero length time series found" );
  }

  H5::DataSet *dataset;
  bool bNeedToCreateDataSet = false;
  //HDF5DataManager dm( HDF5DataManager::RDWR );

  // ensure that appropriate group has been created in the file
  m_dm.AddGroup( sPathName );  // needs to be read/write

  try { // check if dataset exists (for overwrite)
    dataset = new H5::DataSet( m_dm.GetH5File()->openDataSet( sPathName ) );
    dataset->close();
    delete dataset;
  }
  catch ( H5::FileIException e ) {
    bNeedToCreateDataSet = true;
    //dataset->close();
    //delete dataset;
  }

  try {
    if ( bNeedToCreateDataSet ) {

      H5::CompType *pdt = DD::DefineDataType();
      pdt->pack();

      hsize_t curSize = 0;
      hsize_t maxSize = H5S_UNLIMITED;
      H5::DataSpace *pds = new H5::DataSpace( 1, &curSize, &maxSize );
      //H5::DataSpace *pds = new H5::DataSpace( H5S_SIMPLE );
//      hsize_t maxSize = 4000000000;  // probably will get us into trouble, but good enough till we get a good number for above.
//      pds->setExtentSimple( 1, &curSize, &maxSize );

      H5::DSetCreatPropList pl;
      //hsize_t sizeChunk = HDF5DataManager::H5ChunkSize();
      if ( m_bExpandable ) {
        pl.setChunk( 1, &m_nChunkSize );
      }
      if ( m_bDeflatable ) {
        pl.setShuffle();
        pl.setDeflate(m_nDeflate);
      }

      dataset = new H5::DataSet( m_dm.GetH5File()->createDataSet( sPathName, *pdt, *pds, pl ) );
      dataset->close();
      pds->close();
      pdt->close();
      delete pds;
      delete pdt;
      delete dataset;
    }
    else {
      //dataset->close();  // from successful open
      //cout << "Code is needed to write over existing dataset for " << m_sSymbol << endl;
    }
  }
  catch ( H5::FileIException e ) {
    std::cout << "H5::FileIException " << e.getDetailMsg() << std::endl;
    e.walkErrorStack( H5E_WALK_DOWNWARD, (H5E_walk2_t) &HDF5DataManager::PrintH5ErrorStackItem, this );
  }

  try {
    HDF5TimeSeriesContainer<DD> repository( m_dm, sPathName );
    repository.Write( timeseries->First(), timeseries->Last() + 1 );
    //dm.AddGroupForSymbol( m_sSymbol );
    //dm.GetH5File()->link( H5L_type_t::H5L_TYPE_HARD, sFileName1, "/symbol/" + m_sSymbol + "/bar.86400" );
  }
  catch ( H5::FileIException e ) {
    std::cout << "H5::FileIException " << e.getDetailMsg() << std::endl;
    e.walkErrorStack( H5E_WALK_DOWNWARD, (H5E_walk2_t) &HDF5DataManager::PrintH5ErrorStackItem, this );
  }
  catch ( ... ) {
    std::cout << "HDF5WriteTimeSeries::Write:  unknown error 2" << std::endl;
  }
}



/*
      }
      catch (  H5::Exception e ) {
        cout << "CHistoryCollectorDaily::WriteData Exception " << e.getDetailMsg() << endl;
        e.walkErrorStack( H5E_WALK_DOWNWARD, (H5E_walk2_t) &HDF5DataManager::PrintH5ErrorStackItem, this );
        throw e;
      }
*/

} // namespace tf
} // namespace ou
