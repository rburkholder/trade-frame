#pragma once

#include "DataManager.h"
using namespace H5;

#include "DatedDatum.h"

#include <string>
using namespace std;

// inherited by CHDF5TimeSeriesContainer
// called by CHDF5TimeSeriesIterator to access elements
// purpose is to get around the other circular reference of iterator needs to
//  know about the container, and the container issues the iterator

// class T needs to be composed from the CDatedDatum class for access to ptime element
template<class T> class CHDF5TimeSeriesAccessor {
public:
  explicit CHDF5TimeSeriesAccessor<T>( const string &sFileName );
  virtual ~CHDF5TimeSeriesAccessor<T>(void);
  typedef hsize_t size_type;
  size_type size() const { return m_curElementCount; };
  void Read( hsize_t index, T* );
  void Write( hsize_t index, const T & );
  void Write( hsize_t index, size_t count, T *  );
protected:
  string m_sFilename;
  CDataManager dm;
  DataSet *m_pDiskDataSet;
  DataSpace *m_pDiskDataSpace;
  DataSpace *m_pDiskDataSpaceSelection;
  size_type m_curElementCount, m_maxElementCount;
  CompType *m_pDiskCompType;
  virtual void SetNewSize( size_type size ) = 0;
private:
  CHDF5TimeSeriesAccessor( const CHDF5TimeSeriesAccessor& ); // copy constructor not implemented
  CHDF5TimeSeriesAccessor& operator=( const CHDF5TimeSeriesAccessor& ); // assignment constructor not implemented
};

template<class T> CHDF5TimeSeriesAccessor<T>::CHDF5TimeSeriesAccessor(const std::string &sFileName):
  // Todo:  need to validate that requested dataset matches specified class T

  m_sFilename( sFileName ) {

  try {
    m_pDiskDataSet = new DataSet( dm.GetH5File()->openDataSet( m_sFilename.c_str() ) );

    m_pDiskDataSpace = new DataSpace( m_pDiskDataSet->getSpace() );
    m_pDiskDataSpace->getSimpleExtentDims( &m_curElementCount, &m_maxElementCount  );  //current, max
    m_pDiskCompType = new CompType( *m_pDiskDataSet );

    CompType *pMemCompType = T::DefineDataType( NULL );
    if ( ( pMemCompType->getNmembers() != m_pDiskCompType->getNmembers() ) 
      || ( pMemCompType->getSize()     != m_pDiskCompType->getSize() ) ) { // works as Quote, Trade, Bar  have different member count (but MarketDepth has same count as Quote
      throw runtime_error( "CHDF5TimeSeriesAccessor<T>::CHDF5TimeSeriesAccessor CompType doesn't match" );
    }
    pMemCompType->close();
    delete pMemCompType;

    m_pDiskDataSpaceSelection = new DataSpace( m_pDiskDataSet->getSpace() );

  }
  catch ( H5::Exception e ) {
    cout << "CHDF5TimeSeriesAccessor<T>::CHDF5TimeSeriesAccessor " << e.getDetailMsg() << endl;
    e.walkErrorStack( H5E_WALK_DOWNWARD, (H5E_walk2_t) &CDataManager::PrintH5ErrorStackItem, this );
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
  m_pDiskDataSpaceSelection->close();
  delete m_pDiskDataSpaceSelection;
  m_pDiskDataSpace->close();
  delete m_pDiskDataSpace;
  m_pDiskDataSet->close();
  delete m_pDiskDataSet;
}

template<class T> void CHDF5TimeSeriesAccessor<T>::Read( hsize_t ixSource, T *pDatedDatum ) {
  // store the retrieved value in pDatedDatum
  try {
    hsize_t dim = 1;
    assert( ixSource < m_curElementCount );
    hsize_t coord1[] = { ixSource };  // index on disk
    hsize_t coord2[] = { 0 };      // only one item in memory
    DataSpace MemoryDataspace(1, &dim ); // create one element dataspace to get requested element of dataset
    try {
      m_pDiskDataSpaceSelection->selectElements( H5S_SELECT_SET, 1, reinterpret_cast<const hsize_t **>(coord1) );
      MemoryDataspace.selectElements( H5S_SELECT_SET, 1, reinterpret_cast<const hsize_t **>(coord2) );
      m_pDiskDataSet->read( pDatedDatum, *m_pDiskCompType, MemoryDataspace, *m_pDiskDataSpaceSelection );
      cout << "read from index " << ixSource << endl;
    }
    catch ( H5::Exception e ) {
      cout << "CHDF5TimeSeriesAccessor<T>::Retrieve H5::Exception " << e.getDetailMsg() << endl;
      e.walkErrorStack( H5E_WALK_DOWNWARD, (H5E_walk2_t) &CDataManager::PrintH5ErrorStackItem, this );
    }
  }
  catch (...) {
    cout << "unknown error in CHDF5TimeSeriesAccessor<T>::Retrieve" << endl;
  }
}

template<class T> void CHDF5TimeSeriesAccessor<T>::Write( hsize_t ixDest, const T &DatedDatum ) {
  try {
    hsize_t dim = 1;
    assert( ixDest <= m_curElementCount );  // at an existing position, or one past the end (sparseness not allowed)
    hsize_t coord1[] = { ixDest };  // index on disk
    hsize_t coord2[] = { 0 };      // only one item in memory
    DataSpace MemoryDataspace(1, &dim ); // represents one element memory based dataspace
    try {
      m_pDiskDataSpaceSelection->selectElements( H5S_SELECT_SET, 1, reinterpret_cast<const hsize_t **>(coord1) );
      MemoryDataspace.selectElements( H5S_SELECT_SET, 1, reinterpret_cast<const hsize_t **>(coord2) );
      m_pDiskDataSet->write( &DatedDatum, *m_pDiskCompType, MemoryDataspace, *m_pDiskDataSpaceSelection );
      if ( ixDest == m_curElementCount ) {
        m_pDiskDataSpace->getSimpleExtentDims( &m_curElementCount, &m_maxElementCount  );  // update current, max
        SetNewSize( m_curElementCount );
      }
    }
    catch ( H5::Exception e ) {
      cout << "CHDF5TimeSeriesAccessor<T>::Retrieve H5::Exception " << e.getDetailMsg() << endl;
      e.walkErrorStack( H5E_WALK_DOWNWARD, (H5E_walk2_t) &CDataManager::PrintH5ErrorStackItem, this );
    }
  }
  catch (...) {
    cout << "unknown error in CHDF5TimeSeriesAccessor<T>::Retrieve" << endl;
  }
}

template<class T> void CHDF5TimeSeriesAccessor<T>::Write( hsize_t ixStart, size_t count, T *pDatedDatum ) {
  try {
    assert( ixStart <= m_curElementCount );  // at an existing position, or one past the end (sparseness not allowed)
    hsize_t oldElementCount = m_curElementCount;
    hsize_t dim[] = { count };
    DataSpace MemoryDataspace(1, dim ); // rank, dimensions
    hsize_t coord1[][2] = { 0 };      // starting memory element index
    hsize_t coord2[][2] = { ixStart };  // starting index on disk
    try {
      MemoryDataspace.selectElements( H5S_SELECT_SET, count, reinterpret_cast<const hsize_t **>(coord1) );
      m_pDiskDataSpaceSelection->selectElements( H5S_SELECT_SET, count, reinterpret_cast<const hsize_t **>(coord2) );
      //m_pDiskDataSpaceSelection->selectElements( H5S_SELECT_SET, count, coord1 );
      m_pDiskDataSet->write( pDatedDatum, *m_pDiskCompType, MemoryDataspace, *m_pDiskDataSpaceSelection );
      //if ( ixDest == m_curElementCount ) {
        m_pDiskDataSpace->getSimpleExtentDims( &m_curElementCount, &m_maxElementCount  );  // update current, max
        SetNewSize( m_curElementCount );
      //}
        if ( m_curElementCount == oldElementCount ) {
          cout << "Dataset did not expand" << endl;
        }
    }
    catch ( H5::Exception e ) {
      cout << "CHDF5TimeSeriesAccessor<T>::Retrieve H5::Exception " << e.getDetailMsg() << endl;
      e.walkErrorStack( H5E_WALK_DOWNWARD, (H5E_walk2_t) &CDataManager::PrintH5ErrorStackItem, this );
    }
  }
  catch (...) {
    cout << "unknown error in CHDF5TimeSeriesAccessor<T>::Retrieve" << endl;
  }
}
