/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#include "HDF5DataManager.h"
//using namespace H5;

#include "DatedDatum.h"

#include <string>

// inherited by CHDF5TimeSeriesContainer
// called by CHDF5TimeSeriesIterator to access elements
// purpose is to get around the other circular reference of iterator needs to
//  know about the container, and the container issues the iterator

// class T needs to be composed from the CDatedDatum class for access to ptime element
template<class T> class CHDF5TimeSeriesAccessor {
public:
  explicit CHDF5TimeSeriesAccessor<T>( const string &sPathName );
  virtual ~CHDF5TimeSeriesAccessor<T>(void);
  typedef hsize_t size_type;
  size_type size() const { return m_curElementCount; };
  void Read( hsize_t index, T* );
  void Read( hsize_t ixStart, hsize_t count, DataSpace *pMemoryDataSpace, T *pDatedDatum );
  void Write( hsize_t ixStart, size_t count, T * );
protected:
  string m_sPathName;
  CHDF5DataManager dm;
  H5::DataSet *m_pDiskDataSet;
  CompType *m_pDiskCompType;
  size_type m_curElementCount, m_maxElementCount;
  virtual void SetNewSize( size_type size ) {};
  void UpdateElementCount( void );
private:
  CHDF5TimeSeriesAccessor( const CHDF5TimeSeriesAccessor& ); // copy constructor not implemented
  CHDF5TimeSeriesAccessor& operator=( const CHDF5TimeSeriesAccessor& ); // assignment constructor not implemented
};

template<class T> void CHDF5TimeSeriesAccessor<T>::UpdateElementCount( void ) {
  DataSpace *pDiskDataSpace;
  pDiskDataSpace = new DataSpace( m_pDiskDataSet->getSpace() );
  pDiskDataSpace->getSimpleExtentDims( &m_curElementCount, &m_maxElementCount  );  //current, max
  pDiskDataSpace->close();
  delete pDiskDataSpace;
  SetNewSize( m_curElementCount );
}

template<class T> CHDF5TimeSeriesAccessor<T>::CHDF5TimeSeriesAccessor(const std::string &sPathName):

  m_sPathName( sPathName ) {

  try {
    m_pDiskDataSet = new H5::DataSet( dm.GetH5File()->openDataSet( m_sPathName.c_str() ) );
    m_pDiskCompType = new CompType( *m_pDiskDataSet );

    CompType *pMemCompType = T::DefineDataType( NULL );
    if ( ( pMemCompType->getNmembers() != m_pDiskCompType->getNmembers() ) ) { // can't do size as drive datatypes are packed, need instead to check member names
      //|| ( pMemCompType->getSize()     != m_pDiskCompType->getSize() ) ) { // works as Quote, Trade, Bar  have different member count (but MarketDepth has same count as Quote
      throw runtime_error( "CHDF5TimeSeriesAccessor<T>::CHDF5TimeSeriesAccessor CompType doesn't match" );
    }
    pMemCompType->close();
    delete pMemCompType;

    UpdateElementCount();
  }
  catch ( H5::Exception e ) {
    cout << "CHDF5TimeSeriesAccessor<T>::CHDF5TimeSeriesAccessor " << e.getDetailMsg() << endl;
    e.walkErrorStack( H5E_WALK_DOWNWARD, (H5E_walk2_t) &CHDF5DataManager::PrintH5ErrorStackItem, this );
    throw runtime_error( "CHDF5TimeSeriesAccessor<T>::CHDF5TimeSeriesAccessor error 1" );
  }
  catch (...) {
    cout << "CHDF5TimeSeriesAccessor<T>::CHDF5TimeSeriesAccessor unknown error" << endl;
    throw runtime_error( "CHDF5TimeSeriesAccessor<T>::CHDF5TimeSeriesAccessor error 2" );
  }
}

template<class T> CHDF5TimeSeriesAccessor<T>::~CHDF5TimeSeriesAccessor() {
  m_pDiskCompType->close();
  delete m_pDiskCompType;
  //m_pDiskDataSet->flush( H5F_SCOPE_LOCAL );
  m_pDiskDataSet->close();
  delete m_pDiskDataSet;
}

template<class T> void CHDF5TimeSeriesAccessor<T>::Read( hsize_t ixSource, T *pDatedDatum ) {
  // store the retrieved value in pDatedDatum
  assert( ixSource < m_curElementCount );
  try {
    hsize_t dim = 1;
    hsize_t coord1[] = { ixSource };  // index on disk
    hsize_t coord2[] = { 0 };      // only one item in memory
    try {
      CompType *pComp = pDatedDatum->DefineDataType();

      DataSpace MemoryDataspace(1, &dim ); // create one element dataspace to get requested element of dataset
      MemoryDataspace.selectElements( H5S_SELECT_SET, 1, coord2 );

      DataSpace *pDiskDataSpaceSelection = new DataSpace( m_pDiskDataSet->getSpace() );
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
      cout << "CHDF5TimeSeriesAccessor<T>::Retrieve H5::Exception " << e.getDetailMsg() << endl;
      e.walkErrorStack( H5E_WALK_DOWNWARD, (H5E_walk2_t) &CHDF5DataManager::PrintH5ErrorStackItem, this );
    }
  }
  catch (...) {
    cout << "unknown error in CHDF5TimeSeriesAccessor<T>::Retrieve" << endl;
  }
}

template <class T> void CHDF5TimeSeriesAccessor<T>::Read( hsize_t ixStart, hsize_t count, DataSpace *pMemoryDataSpace, T *pDatedDatum ) {
  try {
    hsize_t dim[] = { count };
    try {
      DataSpace *pDiskDataSpaceSelection = new DataSpace( m_pDiskDataSet->getSpace() );
      pDiskDataSpaceSelection->selectHyperslab( H5S_SELECT_SET, &dim[0], &ixStart, 0, 0 );

      DSetMemXferPropList pl;
      bool b = pl.getPreserve();
      pl.setPreserve( true );

      CompType *pComp = pDatedDatum->DefineDataType();

      m_pDiskDataSet->read( pDatedDatum, *pComp, *pMemoryDataSpace, *pDiskDataSpaceSelection, pl );

      pComp->close();
      delete pComp;

      pl.close();

      pDiskDataSpaceSelection->close();
      delete pDiskDataSpaceSelection;
    }
    catch ( H5::Exception e ) {
      cout << "CHDF5TimeSeriesAccessor<T>::Read H5::Exception " << e.getDetailMsg() << endl;
      e.walkErrorStack( H5E_WALK_DOWNWARD, (H5E_walk2_t) &CHDF5DataManager::PrintH5ErrorStackItem, this );
    }
  }
  catch ( ... ) {
    cout << "unknown error in CHDF5TimeSeriesAccessor<T>::Read" << endl;
  }
}

template<class T> void CHDF5TimeSeriesAccessor<T>::Write( hsize_t ixStart, size_t count, T *pDatedDatum ) {
  assert( ixStart <= m_curElementCount );  // at an existing position, or one past the end (sparseness not allowed)
  try {
    hsize_t oldElementCount = m_curElementCount;  // keep for later comparison
    hsize_t dim[] = { count };
    try {
      CompType *pComp = pDatedDatum->DefineDataType();

      DataSpace MemoryDataspace(1, dim ); // rank, dimensions
      MemoryDataspace.selectAll();

      hsize_t newsize[] = { ixStart + count };
      if ( newsize[0] > m_curElementCount ) {
        m_pDiskDataSet->extend( newsize );
        UpdateElementCount();
      }

      DataSpace *pDiskDataSpaceSelection = new DataSpace( m_pDiskDataSet->getSpace() );
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
      cout << "CHDF5TimeSeriesAccessor<T>::Write H5::Exception " << e.getDetailMsg() << endl;
      e.walkErrorStack( H5E_WALK_DOWNWARD, (H5E_walk2_t) &CHDF5DataManager::PrintH5ErrorStackItem, this );
    }
  }
  catch (...) {
    cout << "unknown error in CHDF5TimeSeriesAccessor<T>::Write" << endl;
  }
}

