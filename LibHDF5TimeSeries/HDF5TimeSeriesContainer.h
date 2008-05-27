#pragma once

#include "TimeSeries.h"

#include "HDF5TimeSeriesIterator.h"
#include "HDF5TimeSeriesAccessor.h"

#include "HDF5DataManager.h"
using namespace H5;

#include <string>
using namespace std;

#include "Delegate.h"

template<class T> class CHDF5TimeSeriesContainer: public CHDF5TimeSeriesAccessor<T> {
public:
  CHDF5TimeSeriesContainer<T>( const string &sFilename );
  virtual ~CHDF5TimeSeriesContainer<T>( void );
  //typedef CHDF5TimeSeriesIterator<T> const_iterator;
  typedef CHDF5TimeSeriesIterator<T> iterator;
  iterator begin();
  const iterator &end();
  //void Read( const iterator &_begin, const iterator &_end, T *_dest ); 
  void Read( iterator &_begin, iterator &_end, typename CTimeSeries<T> *_dest ); 
  void Write( T *_begin, T *_end );
protected:
  iterator *m_end;
  virtual void SetNewSize( size_type newsize );
private:
};

template<class T> CHDF5TimeSeriesContainer<T>::CHDF5TimeSeriesContainer( const string &sFilename ):
  CHDF5TimeSeriesAccessor<T>( sFilename ) {
    m_end = new iterator( this, size() );
}

template<class T> CHDF5TimeSeriesContainer<T>::~CHDF5TimeSeriesContainer(void) {
  delete m_end;
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

template<class T> const typename CHDF5TimeSeriesContainer<T>::iterator &CHDF5TimeSeriesContainer<T>::end() {
  return *m_end;
}

template<class T> void CHDF5TimeSeriesContainer<T>::SetNewSize( size_type newsize ) {
  delete m_end;
  m_end = new iterator( this, newsize );
}

//template<class T> void CHDF5TimeSeriesContainer<T>::Read( const iterator &_begin, const iterator &_end, T *_dest ) {
template<class T> void CHDF5TimeSeriesContainer<T>::Read( iterator &_begin, iterator &_end, typename CTimeSeries<T> *_dest ) {
  hsize_t cnt = _end - _begin;
  DataSpace *pDs = _dest->DefineDataSpace();
  if ( cnt > 0 ) {
    CHDF5TimeSeriesAccessor<T>::Read( _begin.m_ItemIndex, cnt, pDs, _dest->First() );
  }
  pDs->close();
  delete pDs;
}

template<class T> void CHDF5TimeSeriesContainer<T>::Write( T *_begin, T *_end ) {
  size_t cnt = _end - _begin;
  if ( cnt > 0 ) {
    pair<CHDF5TimeSeriesContainer<T>::iterator, CHDF5TimeSeriesContainer<T>::iterator> p;
    p = equal_range( begin(), end(), (*_begin).m_dt );
    //p = lower_bound( begin(), end(), (*_begin).m_dt );
    // whether we found something or not, p.first is insertion point
    CHDF5TimeSeriesAccessor<T>::Write( p.first.m_ItemIndex, cnt, _begin );
  }
}