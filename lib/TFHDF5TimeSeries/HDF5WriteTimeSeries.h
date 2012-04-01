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

// TS: TimeSeries, DD: DatedDatum, the type that TS uses
// todo:  need to implement compile time type-matching for allowable TS/DD conbinations
template<class TS, class DD> class CHDF5WriteTimeSeries {
public:
  CHDF5WriteTimeSeries<TS,DD>( void );
  virtual ~CHDF5WriteTimeSeries<TS,DD>( void );
  void Write( const std::string &sPathName, TS* timeseries );
protected:
private:
};

template<class TS, class DD> CHDF5WriteTimeSeries<TS,DD>::CHDF5WriteTimeSeries() {
}

template<class TS, class DD> CHDF5WriteTimeSeries<TS,DD>::~CHDF5WriteTimeSeries() {
}

template<class TS, class DD> void CHDF5WriteTimeSeries<TS,DD>::Write(const std::string &sPathName, TS *timeseries) {

  if ( 0 == timeseries->Size() ) {
    throw std::invalid_argument( "zero length time series found" );
  }

  H5::DataSet *dataset;
  bool bNeedToCreateDataSet = false;
  CHDF5DataManager dm;

  // ensure that appropriate group has been created in the file
  dm.AddGroup( sPathName );

  try { // check if dataset exists (for overwrite)
    dataset = new H5::DataSet( dm.GetH5File()->openDataSet( sPathName ) );
    dataset->close();
    delete dataset;
  }
  catch ( H5::FileIException e ) {
    bNeedToCreateDataSet = true;
    //dataset->close();
    //delete dataset;
  }

  if ( bNeedToCreateDataSet ) {

    H5::CompType *pdt = DD::DefineDataType();
    pdt->pack();

    H5::DataSpace *pds = new H5::DataSpace( H5S_SIMPLE );
    hsize_t curSize = 0;
    hsize_t maxSize = H5S_UNLIMITED; 
    pds->setExtentSimple( 1, &curSize, &maxSize ); 

    H5::DSetCreatPropList pl;
    hsize_t sizeChunk = CHDF5DataManager::H5ChunkSize();
    pl.setChunk( 1, &sizeChunk );
    pl.setShuffle();
    pl.setDeflate(5);

    dataset = new H5::DataSet( dm.GetH5File()->createDataSet( sPathName, *pdt, *pds, pl ) );
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

  try {
    CHDF5TimeSeriesContainer<DD> repository( sPathName );
    repository.Write( timeseries->First(), timeseries->Last() + 1 );
    //dm.AddGroupForSymbol( m_sSymbol );
    //dm.GetH5File()->link( H5L_type_t::H5L_TYPE_HARD, sFileName1, "/symbol/" + m_sSymbol + "/bar.86400" );
  }
  catch (  H5::FileIException e ) {
    std::cout << "H5::FileIException " << e.getDetailMsg() << std::endl;
    e.walkErrorStack( H5E_WALK_DOWNWARD, (H5E_walk2_t) &CHDF5DataManager::PrintH5ErrorStackItem, this );
  }
  catch ( ... ) {
    std::cout << "CHistoryCollectorDaily::WriteData:  unknown error 2" << std::endl;
  }
}



/*
      }
      catch (  H5::Exception e ) {
        cout << "CHistoryCollectorDaily::WriteData Exception " << e.getDetailMsg() << endl;
        e.walkErrorStack( H5E_WALK_DOWNWARD, (H5E_walk2_t) &CHDF5DataManager::PrintH5ErrorStackItem, this );
        throw e;
      }
*/

} // namespace tf
} // namespace ou
