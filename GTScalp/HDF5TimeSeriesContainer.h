#pragma once

#include "HDF5TimeSeriesIterator.h"
#include "HDF5TimeSeriesAccessor.h"

#include "DataManager.h"
using namespace H5;

#include <string>
using namespace std;

template<class T> class CHDF5TimeSeriesContainer: public CHDF5TimeSeriesAccessor<T> {
public:
  CHDF5TimeSeriesContainer<T>( const string &sFilename );
  virtual ~CHDF5TimeSeriesContainer<T>( void );
  //typedef CHDF5TimeSeriesIterator<T> const_iterator;
  typedef CHDF5TimeSeriesIterator<T> iterator;
  iterator begin();
  iterator end();
protected:
private:
};

template<class T> CHDF5TimeSeriesContainer<T>::CHDF5TimeSeriesContainer( const string &sFilename ):
  CHDF5TimeSeriesAccessor<T>( sFilename ) {
}

template<class T> CHDF5TimeSeriesContainer<T>::~CHDF5TimeSeriesContainer(void) {
}

template<class T> typename CHDF5TimeSeriesContainer<T>::iterator CHDF5TimeSeriesContainer<T>::begin() {
//template<class T> CHDF5TimeSeriesIterator<T> CHDF5TimeSeriesContainer<T>::begin() {
  iterator result( this, 0 );
  return result;
}

//template<class T> typename CHDF5TimeSeriesContainer<T>::const_iterator CHDF5TimeSeriesContainer<T>::end() const {
//  const_iterator result( this, m_curElementCount );
//  return result;
//}

template<class T> typename CHDF5TimeSeriesContainer<T>::iterator CHDF5TimeSeriesContainer<T>::end() {
  iterator result( this, size() );
  return result;
}

