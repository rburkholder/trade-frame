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
  size_type size() const;
  void Retrieve( hsize_t index, T* );
protected:
  string m_sFilename;
  CDataManager dm;
  DataSet *m_pDiskDataSet;
  DataSpace *m_pDiskDataSpace;
  DataSpace *m_pDiskDataSpaceSelection;
  size_type m_curElementCount, m_maxElementCount;
private:
  CHDF5TimeSeriesAccessor( const CHDF5TimeSeriesAccessor& ); // not implemented
  CHDF5TimeSeriesAccessor& operator=( const CHDF5TimeSeriesAccessor& ); // not implemented
};

template<class T> CHDF5TimeSeriesAccessor<T>::CHDF5TimeSeriesAccessor(const std::string &sFileName):
  // Todo:  need to validate that requested dataset matches specified class T
  m_sFilename( sFileName ) {

  m_pDiskDataSet = new DataSet( dm.GetH5File()->openDataSet( m_sFilename.c_str() ) );
  m_pDiskDataSpace = new DataSpace( m_pDiskDataSet->getSpace() );
  m_pDiskDataSpaceSelection = new DataSpace( m_pDiskDataSet->getSpace() );

  m_pDiskDataSpace->getSimpleExtentDims( &m_curElementCount, &m_maxElementCount  );  //current, max
}

template<class T> CHDF5TimeSeriesAccessor<T>::~CHDF5TimeSeriesAccessor() {
  m_pDiskDataSpaceSelection->close();
  delete m_pDiskDataSpaceSelection;
  m_pDiskDataSpace->close();
  delete m_pDiskDataSpace;
  m_pDiskDataSet->close();
  delete m_pDiskDataSet;
}

template<class T> hsize_t CHDF5TimeSeriesAccessor<T>::size() const {
  return m_curElementCount;
}

template<class T> void CHDF5TimeSeriesAccessor<T>::Retrieve( hsize_t index, T *pDatedDatum ) {
  // store the retrieved value in pDatedDatum
  try {
    hsize_t dim = 1;
    assert( index < m_curElementCount );
    hsize_t coord1[] = { index };  // index on disk
    hsize_t coord2[] = { 0 };      // only one item in memory
    DataType *pdtype = T::DefineDataType();
    DataSpace MemoryDataspace(1, &dim ); // create one element dataspace to get requested element of dataset
    try {
      m_pDiskDataSpaceSelection->selectElements( H5S_SELECT_SET, 1, reinterpret_cast<const hsize_t **>(coord1) );
      MemoryDataspace.selectElements( H5S_SELECT_SET, 1, reinterpret_cast<const hsize_t **>(coord2) );
      m_pDiskDataSet->read( pDatedDatum, *pdtype, MemoryDataspace, *m_pDiskDataSpaceSelection );
    }
    catch ( H5::Exception e ) {
      cout << "CHDF5TimeSeriesAccessor<T>::Retrieve H5::Exception " << e.getDetailMsg() << endl;
      e.walkErrorStack( H5E_WALK_DOWNWARD, (H5E_walk2_t) &CDataManager::PrintH5ErrorStackItem, this );
    }
    catch (...) {
      cout << "unknown error" << endl;
    }
    pdtype->close();
    delete pdtype;
  }
  catch (...) {
    cout << "unknown error in CHDF5TimeSeriesAccessor<T>::Retrieve" << endl;
  }
}