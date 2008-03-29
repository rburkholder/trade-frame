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
protected:
  string m_sFilename;
  CDataManager dm;
  DataSet *m_pDiskDataSet;
  DataSpace *m_pDiskDataSpace, *m_pDiskDataSpaceSelection;
  hsize_t m_curElementCount, m_maxElementCount;
private:
};

template<class T> CHDF5TimeSeriesAccessor<T>::CHDF5TimeSeriesAccessor(const std::string &sFileName):
  m_sFilename( sFilename ) {

  m_pDiskDataSet = new DataSet( dm.GetH5File()->openDataSet( sFilename.c_str() ) );
  m_pDiskDataSpace = new DataSpace( m_pDiskDataSet->getSpace() );
  m_pDiskDataSpaceSelection = new DataSpace( m_pDiskDataSet->getSpace() );

  m_pDiskDataSpace->getSimpleExtentDims( &m_curElementCount, &m_maxElementCount  );  //current, max
}

template<class T> CHDF5TimeSeriesAccessor<T>::~CHDF5TimeSeriesAccessor() {
  m_pDiskDataSpaceSelection->close();
  m_pDiskDataSpace->close();
  m_pDiskDataSet->close();
}