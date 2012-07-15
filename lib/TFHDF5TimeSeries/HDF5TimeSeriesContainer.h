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

#include <OUCommon/Delegate.h>

#include <TFTimeSeries/TimeSeries.h>

#include "HDF5DataManager.h"

#include "HDF5TimeSeriesIterator.h"
#include "HDF5TimeSeriesAccessor.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

// DD is expecting type derived from DatedDatum
template<class DD> class CHDF5TimeSeriesContainer: public CHDF5TimeSeriesAccessor<DD> {
public:
  CHDF5TimeSeriesContainer<DD>( const std::string& sPathName );
  virtual ~CHDF5TimeSeriesContainer<DD>( void );
  //typedef CHDF5TimeSeriesIterator<T> const_iterator;
  typedef CHDF5TimeSeriesIterator<DD> iterator;
  iterator begin();
  const iterator &end();
  //void Read( const iterator &_begin, const iterator &_end, T* _dest ); 
  void Read( iterator &_begin, iterator &_end, typename TimeSeries<DD>* _dest ); 
  void Write( const DD* _begin, const DD* _end );
protected:
  iterator* m_end;
  virtual void SetNewSize( size_type newsize );
private:
};

template<class DD> CHDF5TimeSeriesContainer<DD>::CHDF5TimeSeriesContainer( const std::string& sPathName ):
  CHDF5TimeSeriesAccessor<DD>( sPathName ) {
    m_end = new iterator( this, size() );
}

template<class DD> CHDF5TimeSeriesContainer<DD>::~CHDF5TimeSeriesContainer(void) {
  delete m_end;
}

template<class DD> typename CHDF5TimeSeriesContainer<DD>::iterator CHDF5TimeSeriesContainer<DD>::begin() {
  iterator result( this, 0 );
  return result;
}

//template<class T> typename CHDF5TimeSeriesContainer<T>::const_iterator CHDF5TimeSeriesContainer<T>::end() const {
//  const_iterator result( this, m_curElementCount );
//  return result;
//}

template<class DD> const typename CHDF5TimeSeriesContainer<DD>::iterator &CHDF5TimeSeriesContainer<DD>::end() {
  return* m_end;
}

template<class DD> void CHDF5TimeSeriesContainer<DD>::SetNewSize( size_type newsize ) {
  delete m_end;
  m_end = new iterator( this, newsize );
}

template<class DD> void CHDF5TimeSeriesContainer<DD>::Read( iterator& _begin, iterator& _end, typename TimeSeries<DD>* _dest ) {
  hsize_t cnt = _end - _begin;
  H5::DataSpace* pDs = _dest->DefineDataSpace();
  if ( cnt > 0 ) {
    CHDF5TimeSeriesAccessor<DD>::Read( _begin.m_ItemIndex, cnt, pDs, const_cast<DD*>( &(*_dest->First()) ) );
  }
  pDs->close();
  delete pDs;
}

template<class DD> void CHDF5TimeSeriesContainer<DD>::Write( const DD* _begin, const DD* _end ) {
  size_t cnt = _end - _begin;
  if ( cnt > 0 ) {
    std::pair<CHDF5TimeSeriesContainer<DD>::iterator, CHDF5TimeSeriesContainer<DD>::iterator> p;
    p = equal_range( begin(), end(), *_begin );
    // whether we found something or not, p.first is insertion point
    CHDF5TimeSeriesAccessor<DD>::Write( p.first.m_ItemIndex, cnt, _begin );
  }
}

} // namespace tf
} // namespace ou
