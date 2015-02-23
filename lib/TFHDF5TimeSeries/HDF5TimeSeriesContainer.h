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
template<class DD> class HDF5TimeSeriesContainer: public HDF5TimeSeriesAccessor<DD> {
public:
  HDF5TimeSeriesContainer<DD>( HDF5DataManager& dm, const std::string& sPathName );
  virtual ~HDF5TimeSeriesContainer<DD>( void );
  //typedef HDF5TimeSeriesIterator<T> const_iterator;
  typedef HDF5TimeSeriesIterator<DD> iterator;
  typedef typename HDF5TimeSeriesAccessor<DD>::size_type size_type;
  iterator begin();
  const iterator &end();
  //void Read( const iterator &_begin, const iterator &_end, T* _dest ); 
  void Read( iterator &_begin, iterator &_end, typename ou::tf::TimeSeries<DD>* _dest ); 
  void Write( const DD* _begin, const DD* _end );
protected:
  iterator* m_end;
  virtual void SetNewSize( size_type newsize );
private:
};

template<class DD> HDF5TimeSeriesContainer<DD>::HDF5TimeSeriesContainer( HDF5DataManager& dm, const std::string& sPathName ):
  HDF5TimeSeriesAccessor<DD>( dm, sPathName ) {
    m_end = new iterator( this, this->size() );
}

template<class DD> HDF5TimeSeriesContainer<DD>::~HDF5TimeSeriesContainer(void) {
  delete m_end;
}

template<class DD> typename HDF5TimeSeriesContainer<DD>::iterator HDF5TimeSeriesContainer<DD>::begin() {
  iterator result( this, 0 );
  return result;
}

//template<class T> typename HDF5TimeSeriesContainer<T>::const_iterator HDF5TimeSeriesContainer<T>::end() const {
//  const_iterator result( this, m_curElementCount );
//  return result;
//}

template<class DD> const typename HDF5TimeSeriesContainer<DD>::iterator &HDF5TimeSeriesContainer<DD>::end() {
  return* m_end;
}

template<class DD> void HDF5TimeSeriesContainer<DD>::SetNewSize( size_type newsize ) {
  delete m_end;
  m_end = new iterator( this, newsize );
}

template<class DD> void HDF5TimeSeriesContainer<DD>::Read( iterator& _begin, iterator& _end, typename ou::tf::TimeSeries<DD>* _dest ) {
  hsize_t cnt = _end - _begin;
  H5::DataSpace* pDs = _dest->DefineDataSpace();
  if ( cnt > 0 ) {
    HDF5TimeSeriesAccessor<DD>::Read( _begin.m_ItemIndex, cnt, pDs, const_cast<DD*>( &(*_dest->First()) ) );
  }
  pDs->close();
  delete pDs;
}

template<class DD> void HDF5TimeSeriesContainer<DD>::Write( const DD* _begin, const DD* _end ) {
  size_t cnt = _end - _begin;
  if ( cnt > 0 ) {
    std::pair<HDF5TimeSeriesContainer<DD>::iterator, HDF5TimeSeriesContainer<DD>::iterator> p;
    p = equal_range( begin(), end(), *_begin );
    // whether we found something or not, p.first is insertion point
    HDF5TimeSeriesAccessor<DD>::Write( p.first.m_ItemIndex, cnt, _begin );
  }
}

} // namespace tf
} // namespace ou
