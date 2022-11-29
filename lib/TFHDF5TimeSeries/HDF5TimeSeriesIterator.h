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

#include <iterator>
#include <stdexcept>

#include <TFTimeSeries/DatedDatum.h>

#include "HDF5DataManager.h"

#include "HDF5TimeSeriesAccessor.h"
#include "HDF5TimeSeriesContainer.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

// DD is expecting type derived from DatedDatum
template<class DD> class HDF5TimeSeriesContainer;

template<class DD> class HDF5TimeSeriesIterator{
  friend class HDF5TimeSeriesContainer<DD>;
public:

  //typedef HDF5TimeSeriesIterator self_type;
  //typedef self_type& self_reference;
  using iterator_category = std::random_access_iterator_tag;
  using value_type = DD;
  using difference_type = hsize_t;
  using pointer = DD*;
  using reference = DD&;

  HDF5TimeSeriesIterator<DD>();
  HDF5TimeSeriesIterator<DD>( HDF5TimeSeriesAccessor<DD> *pAccessor, hsize_t Index );  // begin() or later init
  HDF5TimeSeriesIterator<DD>( const HDF5TimeSeriesIterator<DD>& other );  // copy constructor
  ~HDF5TimeSeriesIterator<DD>() {};
  HDF5TimeSeriesIterator<DD>& operator=( const HDF5TimeSeriesIterator<DD>& other );
  HDF5TimeSeriesIterator<DD>& operator++(); // pre-increment
  HDF5TimeSeriesIterator<DD>  operator++( int ); // post-increment
  HDF5TimeSeriesIterator<DD>& operator--(); // pre-decrement
  HDF5TimeSeriesIterator<DD>  operator--( int ); // post-decrement
  HDF5TimeSeriesIterator<DD>& operator+=( const hsize_t inc );
  HDF5TimeSeriesIterator<DD>& operator-=( const hsize_t inc );
  HDF5TimeSeriesIterator<DD>  operator-( const hsize_t val );
  hsize_t operator-( const HDF5TimeSeriesIterator<DD>& other ) const;
  bool operator<( const HDF5TimeSeriesIterator<DD>& other ) const;
  bool operator==( const HDF5TimeSeriesIterator<DD>& other ) const;
  bool operator!=( const HDF5TimeSeriesIterator<DD>& other ) const;
  //HDF5TimeSeriesIterator<T> &operator[]( const hsize_t Index );
  reference operator*();
  //reference operator->();
protected:
  bool m_bValidIndex;  // m_ItemIndex is valid ie, is something from .begin() to .end()
  HDF5TimeSeriesAccessor<DD>* m_pAccessor;
  hsize_t m_ItemIndex; // index into specific item
  DD m_DD; // the currently retrieved datum, could keep and track two values in order to optimize lower_bound speed somewhat
private:
};

template<class DD>
HDF5TimeSeriesIterator<DD>::HDF5TimeSeriesIterator():
  m_pAccessor( NULL ),
  m_ItemIndex( 0 ),
  m_bValidIndex( false )
   {
}

template<class DD>
HDF5TimeSeriesIterator<DD>::HDF5TimeSeriesIterator( HDF5TimeSeriesAccessor<DD> *pAccessor, hsize_t Index ):
  m_pAccessor( pAccessor ),
  m_ItemIndex( Index ),
  m_bValidIndex( false ) {

  if ( Index > m_pAccessor->size() ) throw std::runtime_error( "Index out of range on construction" );
  // Index == m_pAccessor->size() is same as end();
  //if ( Index < m_pAccessor->size() ) {
  //  m_pAccessor->ReadItem( m_ItemIndex, &m_T );  // don't preload
  //}
  m_bValidIndex = true;
}

template<class DD>
HDF5TimeSeriesIterator<DD>::HDF5TimeSeriesIterator( const HDF5TimeSeriesIterator<DD>& other ) :
  m_pAccessor( other.m_pAccessor ),
  m_bValidIndex( other.m_bValidIndex ),
  m_ItemIndex( other.m_ItemIndex ),
  m_DD( other.m_DD )
  {
}

template<class DD>
HDF5TimeSeriesIterator<DD>& HDF5TimeSeriesIterator<DD>::operator=( const HDF5TimeSeriesIterator<DD> &other ) {
  // has two choices:  first assigned from, second being assigned to
  if ( this == &other ) {
    // a) being assigned from, so just return reference
  }
  else {
    // b) being assigned to
    m_pAccessor = other.m_pAccessor;
    m_bValidIndex = other.m_bValidIndex;
    m_ItemIndex = other.m_ItemIndex;
    m_DD = other.m_DD;
  }
  return( *this );
}

template<class DD>
HDF5TimeSeriesIterator<DD>& HDF5TimeSeriesIterator<DD>::operator++() { // pre-increment
  assert( m_bValidIndex );
  assert( m_ItemIndex < m_pAccessor->size() );
  ++m_ItemIndex;
  //if ( m_ItemIndex < m_pAccessor->size() ) {
  //  m_pAccessor->ReadItem( m_ItemIndex, &m_T );  // retrieve at our new location if we can
  //}
  return( *this );
}

template<class DD>
HDF5TimeSeriesIterator<DD> HDF5TimeSeriesIterator<DD>::operator++( int ) { // post-increment
  HDF5TimeSeriesIterator<DD> result( *this );  // make a copy of what is before increment
  assert( m_bValidIndex );
  assert( m_ItemIndex < m_pAccessor->size() );
  ++m_ItemIndex;
  //if ( m_ItemIndex < m_pAccessor->size() ) {
  //  m_pAccessor->Retrieve( m_ItemIndex, &m_T );
  //}
  return( result );
}

template<class DD>
HDF5TimeSeriesIterator<DD>& HDF5TimeSeriesIterator<DD>::operator--() { // pre-decrement
  assert( m_bValidIndex );
  assert( 0 < m_ItemIndex );
  --m_ItemIndex;
  //if ( m_ItemIndex < m_pAccessor->size() ) {
  //  m_pAccessor->ReadItem( m_ItemIndex, &m_T );  // retrieve at our new location if we can
  //}
  return( *this );
}

template<class DD>
HDF5TimeSeriesIterator<DD> HDF5TimeSeriesIterator<DD>::operator--( int ) { // post-decrement
  HDF5TimeSeriesIterator<DD> result( *this );  // make a copy of what is before decrement
  assert( m_bValidIndex );
  assert( 0 < m_ItemIndex );
  --m_ItemIndex;
  //if ( m_ItemIndex < m_pAccessor->size() ) {
  //  m_pAccessor->Retrieve( m_ItemIndex, &m_T );
  //}
  return( result );
}

template<class DD>
HDF5TimeSeriesIterator<DD>& HDF5TimeSeriesIterator<DD>::operator+=( const hsize_t inc ) { // plus assignment
  assert( m_bValidIndex );
  m_ItemIndex += inc;
  assert( m_ItemIndex <= m_pAccessor->size() );
  //if ( m_ItemIndex < m_pAccessor->size() ) {
  //  m_pAccessor->ReadItem( m_ItemIndex, &m_T );  // retrieve at our new location if we can
  //}
  return( *this );
}

template<class DD>
HDF5TimeSeriesIterator<DD>& HDF5TimeSeriesIterator<DD>::operator-=( const hsize_t inc ) { // plus assignment
  assert( m_bValidIndex );
  assert( inc <= m_ItemIndex );
  m_ItemIndex -= inc;
  //if ( m_ItemIndex < m_pAccessor->size() ) {
  //  m_pAccessor->ReadItem( m_ItemIndex, &m_T );  // retrieve at our new location if we can
  //}
  return( *this );
}

template<class DD>
HDF5TimeSeriesIterator<DD> HDF5TimeSeriesIterator<DD>::operator-( const hsize_t val ) { // subtraction
  assert( val <= m_ItemIndex );
  HDF5TimeSeriesIterator<DD> result( *this );  // make a copy of this
  result.m_ItemIndex -= val;
  return result;
}

template<class DD>
hsize_t HDF5TimeSeriesIterator<DD>::operator-( const HDF5TimeSeriesIterator<DD>& other ) const { // subtraction
  return m_ItemIndex - other.m_ItemIndex;
}

template<class DD>
bool HDF5TimeSeriesIterator<DD>::operator<( const HDF5TimeSeriesIterator<DD>& other ) const {
  assert( m_pAccessor == other.m_pAccessor );
  return ( m_ItemIndex < other.m_ItemIndex );
}

template<class DD>
bool HDF5TimeSeriesIterator<DD>::operator==( const HDF5TimeSeriesIterator<DD>& other ) const {
  assert( m_pAccessor == other.m_pAccessor );
  return ( m_ItemIndex == other.m_ItemIndex );
}

template<class DD>
bool HDF5TimeSeriesIterator<DD>::operator!=( const HDF5TimeSeriesIterator<DD>& other ) const {
  assert( m_pAccessor == other.m_pAccessor );
  return !( m_ItemIndex == other.m_ItemIndex );
}

template<class DD>
typename HDF5TimeSeriesIterator<DD>::reference HDF5TimeSeriesIterator<DD>::operator*() {
  assert( m_bValidIndex );
  assert( m_ItemIndex < m_pAccessor->size() );
  m_pAccessor->Read( m_ItemIndex, &m_DD );
  return m_DD;
}

//template<class T> typename HDF5TimeSeriesIterator<T>::reference HDF5TimeSeriesIterator<T>::operator->() {
//  m_pAccessor->ReadItem( m_ItemIndex, &m_T );
//  return m_T;  // not sure yet how this works
//}

// does operator[] make sense for iterator?  ... look in requirements for a random interator
//template<class T> HDF5TimeSeriesIterator<T> &HDF5TimeSeriesIterator<T>::operator[]( const hsize_t Index ) {
//  assert( Index < m_pAccessor->size() );
//  m_ItemIndex = Index;
//  m_bValidIndex = true;
//  m_pAccessor->Retrieve( Index, &m_T );
//  return (*this);
//}

} // namespace tf
} // namespace ou
