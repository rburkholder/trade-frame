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

#include <stdexcept>
#include <string>

#include <TFTimeSeries/DatedDatum.h>

#include "HDF5DataManager.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

// inherited by CHDF5TimeSeriesContainer
// called by CHDF5TimeSeriesIterator to access elements
// purpose is to get around the other circular reference of iterator needs to
//  know about the container, and the container issues the iterator

// class DD needs to be composed from the CDatedDatum class for access to ptime element
template<class DD> class HDF5TimeSeriesAccessor {
public:
  explicit HDF5TimeSeriesAccessor<DD>( HDF5DataManager& dm, const std::string &sPathName );
  virtual ~HDF5TimeSeriesAccessor<DD>( void );
  typedef hsize_t size_type;
  size_type size() const { return m_curElementCount; };
  void Read( hsize_t index, DD* );
  void Read( hsize_t ixStart, hsize_t count, H5::DataSpace *pMemoryDataSpace, DD* pDatedDatum );
  void Write( hsize_t ixStart, size_t count, const DD* );
protected:
  std::string m_sPathName;
  H5::DataSet* m_pDiskDataSet;
  H5::CompType* m_pDiskCompType;
  size_type m_curElementCount, m_maxElementCount;
  virtual void SetNewSize( size_type size ) {};
  void UpdateElementCount( void );
private:
  HDF5DataManager& m_dm;
  HDF5TimeSeriesAccessor( const HDF5TimeSeriesAccessor& ); // copy constructor not implemented
  HDF5TimeSeriesAccessor& operator=( const HDF5TimeSeriesAccessor& ); // assignment constructor not implemented
};

template<class DD> void HDF5TimeSeriesAccessor<DD>::UpdateElementCount( void ) {
  H5::DataSpace *pDiskDataSpace;
  pDiskDataSpace = new H5::DataSpace( m_pDiskDataSet->getSpace() );
  pDiskDataSpace->getSimpleExtentDims( &m_curElementCount, &m_maxElementCount  );  //current, max
  pDiskDataSpace->close();
  delete pDiskDataSpace;
  SetNewSize( m_curElementCount );
}

template<class DD> HDF5TimeSeriesAccessor<DD>::HDF5TimeSeriesAccessor( HDF5DataManager& dm, const std::string &sPathName)
: m_dm( dm )
, m_sPathName( sPathName )
{

  try {
    m_pDiskDataSet = new H5::DataSet( m_dm.GetH5File()->openDataSet( m_sPathName.c_str() ) );
    m_pDiskCompType = new H5::CompType( *m_pDiskDataSet );

    H5::CompType *pMemCompType = DD::DefineDataType( NULL );
    if ( ( pMemCompType->getNmembers() != m_pDiskCompType->getNmembers() ) ) { // can't do size as drive datatypes are packed, need instead to check member names
      //|| ( pMemCompType->getSize()     != m_pDiskCompType->getSize() ) ) { // works as Quote, Trade, Bar  have different member count (but MarketDepth has same count as Quote
      throw std::runtime_error( "CompType doesn't match" );
    }
    pMemCompType->close();
    delete pMemCompType;

    UpdateElementCount();
  }
  catch ( H5::AttributeIException& e ) {
    std::cout << "HDF5TimeSeriesAccessor<DD>::HDF5TimeSeriesAccessor AttributeIException (" << m_sPathName << ") " << e.getDetailMsg() << std::endl;
    e.walkErrorStack( H5E_WALK_DOWNWARD, (H5E_walk2_t) &HDF5DataManager::PrintH5ErrorStackItem, this );
    throw std::runtime_error( "HDF5TimeSeriesAccessor<DD>::HDF5TimeSeriesAccessor error 0" );
  }
  catch ( H5::Exception& e ) {
    if ( !m_dm.GetH5File()->nameExists( m_sPathName.c_str() ) ) {
      std::cout << "HDF5TimeSeriesAccessor<DD>::HDF5TimeSeriesAccessor " << m_sPathName << " does not exist" << std::endl;
      throw std::runtime_error( "HDF5TimeSeriesAccessor<DD>::HDF5TimeSeriesAccessor non existence" );
    }
    else {
      std::cout << "HDF5TimeSeriesAccessor<DD>::HDF5TimeSeriesAccessor H " << m_sPathName << ' ' << e.getDetailMsg() << std::endl;
      e.walkErrorStack( H5E_WALK_DOWNWARD, (H5E_walk2_t) &HDF5DataManager::PrintH5ErrorStackItem, this );
      throw std::runtime_error( "HDF5TimeSeriesAccessor<DD>::HDF5TimeSeriesAccessor error 1" );
    }
  }
  catch ( std::runtime_error& e ) {
    std::cout << "HDF5TimeSeriesAccessor<DD>::HDF5TimeSeriesAccessor R " << m_sPathName << ' ' << e.what() << std::endl;
    throw std::runtime_error( "HDF5TimeSeriesAccessor<DD>::HDF5TimeSeriesAccessor error 2" );
  }
  catch (...) {
    std::cout << "HDF5TimeSeriesAccessor<DD>::HDF5TimeSeriesAccessor unknown error (" << m_sPathName << ')' << std::endl;
    throw std::runtime_error( "HDF5TimeSeriesAccessor<DD>::HDF5TimeSeriesAccessor error 3" );
  }
}

template<class DD> HDF5TimeSeriesAccessor<DD>::~HDF5TimeSeriesAccessor() {
  m_pDiskCompType->close();
  delete m_pDiskCompType;
  //m_pDiskDataSet->flush( H5F_SCOPE_LOCAL );
  m_pDiskDataSet->close();
  delete m_pDiskDataSet;
}

template<class DD> void HDF5TimeSeriesAccessor<DD>::Read( hsize_t ixSource, DD* pDatedDatum ) {
  // store the retrieved value in pDatedDatum
  assert( ixSource < m_curElementCount );
  try {
    hsize_t dim = 1;
    hsize_t coord1[] = { ixSource };  // index on disk
    hsize_t coord2[] = { 0 };      // only one item in memory
    try {
      H5::CompType *pComp = pDatedDatum->DefineDataType();

      H5::DataSpace MemoryDataspace(1, &dim ); // create one element dataspace to get requested element of dataset
      MemoryDataspace.selectElements( H5S_SELECT_SET, 1, coord2 );

      H5::DataSpace *pDiskDataSpaceSelection = new H5::DataSpace( m_pDiskDataSet->getSpace() );
      pDiskDataSpaceSelection->selectElements( H5S_SELECT_SET, 1, coord1 );

      m_pDiskDataSet->read( pDatedDatum, *pComp, MemoryDataspace, *pDiskDataSpaceSelection );

      pDiskDataSpaceSelection->close();
      delete pDiskDataSpaceSelection;

      MemoryDataspace.close();

      pComp->close();
      delete pComp;

      //cout << "read from index " << ixSource << endl;
    }
    catch ( H5::Exception e ) {
      std::cout << "HDF5TimeSeriesAccessor<DD>::Retrieve H5::Exception " << e.getDetailMsg() << std::endl;
      e.walkErrorStack( H5E_WALK_DOWNWARD, (H5E_walk2_t) &HDF5DataManager::PrintH5ErrorStackItem, this );
    }
  }
  catch (...) {
    std::cout << "unknown error in HDF5TimeSeriesAccessor<DD>::Retrieve" << std::endl;
  }
}

template <class DD> void HDF5TimeSeriesAccessor<DD>::Read( hsize_t ixStart, hsize_t count, H5::DataSpace *pMemoryDataSpace, DD *pDatedDatum ) {
  try {
    hsize_t dim[] = { count };
    try {
      H5::DataSpace *pDiskDataSpaceSelection = new H5::DataSpace( m_pDiskDataSet->getSpace() );
      pDiskDataSpaceSelection->selectHyperslab( H5S_SELECT_SET, &dim[0], &ixStart, 0, 0 );

      H5::DSetMemXferPropList pl;
      bool b = pl.getPreserve();
      pl.setPreserve( true );

      H5::CompType *pComp = pDatedDatum->DefineDataType();

      m_pDiskDataSet->read( pDatedDatum, *pComp, *pMemoryDataSpace, *pDiskDataSpaceSelection, pl );

      pComp->close();
      delete pComp;

      pl.close();

      pDiskDataSpaceSelection->close();
      delete pDiskDataSpaceSelection;
    }
    catch ( H5::Exception e ) {
      std::cout << "HDF5TimeSeriesAccessor<DD>::Read H5::Exception " << e.getDetailMsg() << std::endl;
      e.walkErrorStack( H5E_WALK_DOWNWARD, (H5E_walk2_t) &HDF5DataManager::PrintH5ErrorStackItem, this );
    }
  }
  catch ( ... ) {
    std::cout << "unknown error in HDF5TimeSeriesAccessor<DD>::Read" << std::endl;
  }
}

template<class DD> void HDF5TimeSeriesAccessor<DD>::Write( hsize_t ixStart, size_t count, const DD* pDatedDatum ) {
  assert( ixStart <= m_curElementCount );  // at an existing position, or one past the end (sparseness not allowed)
  try {
    hsize_t oldElementCount = m_curElementCount;  // keep for later comparison
    hsize_t dim[] = { count };
    try {
      H5::CompType *pComp = pDatedDatum->DefineDataType();

      H5::DataSpace MemoryDataspace(1, dim ); // rank, dimensions
      MemoryDataspace.selectAll();

      hsize_t newsize[] = { ixStart + count };
      if ( newsize[0] > m_curElementCount ) {
        m_pDiskDataSet->extend( newsize );
        UpdateElementCount();
      }

      H5::DataSpace *pDiskDataSpaceSelection = new H5::DataSpace( m_pDiskDataSet->getSpace() );
      pDiskDataSpaceSelection->selectHyperslab( H5S_SELECT_SET, &dim[0], &ixStart, 0, 0 );

      m_pDiskDataSet->write( pDatedDatum, *pComp, MemoryDataspace, *pDiskDataSpaceSelection );

      pDiskDataSpaceSelection->close();
      delete pDiskDataSpaceSelection;

      MemoryDataspace.close();

      pComp->close();
      delete pComp;

      if ( m_curElementCount == oldElementCount ) {
        //cout << "Dataset did not expand" << endl;
      }
      //cout << "Wrote " << count << ", total " << m_curElementCount << endl;
    }
    catch ( H5::Exception e ) {
      std::cout << "HDF5TimeSeriesAccessor<DD>::Write H5::Exception " << e.getDetailMsg() << std::endl;
      e.walkErrorStack( H5E_WALK_DOWNWARD, (H5E_walk2_t) &HDF5DataManager::PrintH5ErrorStackItem, this );
    }
  }
  catch (...) {
    std::cout << "unknown error in HDF5TimeSeriesAccessor<DD>::Write" << std::endl;
  }
}

} // namespace tf
} // namespace ou
