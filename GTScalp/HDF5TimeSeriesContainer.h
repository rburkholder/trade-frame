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
protected:
private:
};

template<class T> CHDF5TimeSeriesContainer<T>::CHDF5TimeSeriesContainer( const string &sFilename ):
  CHDF5TimeSeriesAccessor<T>( sFilename ) {
}

template<class T> CHDF5TimeSeriesContainer<T>::~CHDF5TimeSeriesContainer(void) {
}