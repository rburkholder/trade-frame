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

#include <iterator>
#include <stdexcept>

#include "LibTimeSeries/DatedDatum.h"

#include "HDF5DataManager.h"

#include "HDF5TimeSeriesAccessor.h"
#include "HDF5TimeSeriesContainer.h"

template<class T> class CHDF5TimeSeriesContainer;

template<class T> class CHDF5TimeSeriesIterator: 
  public std::iterator<std::random_access_iterator_tag, T, hsize_t> {
    friend class CHDF5TimeSeriesContainer<T>;
public:
  explicit CHDF5TimeSeriesIterator<T>( void );
  explicit CHDF5TimeSeriesIterator<T>( CHDF5TimeSeriesAccessor<T> *pAccessor, hsize_t Index );  // begin() or later init
  typedef std::random_access_iterator_tag iterator_category;
  CHDF5TimeSeriesIterator<T>( const CHDF5TimeSeriesIterator<T>& other );  // copy constructor
  ~CHDF5TimeSeriesIterator<T>( void ) { };
  //typedef CHDF5TimeSeriesIterator self_type;
  //typedef self_type &self_reference;
  CHDF5TimeSeriesIterator<T>& operator=( const CHDF5TimeSeriesIterator<T> &other );
  CHDF5TimeSeriesIterator<T>& operator++(); // pre-increment
  CHDF5TimeSeriesIterator<T>  operator++( int ); // post-increment
  CHDF5TimeSeriesIterator<T>& operator+=( const hsize_t inc );
  CHDF5TimeSeriesIterator<T>& operator-=( const hsize_t inc );
  CHDF5TimeSeriesIterator<T>  operator-( const hsize_t val );
  hsize_t operator-( const CHDF5TimeSeriesIterator<T> &other ) const;
  bool operator<( const CHDF5TimeSeriesIterator<T> &other ) const;
  bool operator==( const CHDF5TimeSeriesIterator<T> &other ) const;
  bool operator!=( const CHDF5TimeSeriesIterator<T> &other ) const;
  //CHDF5TimeSeriesIterator<T> &operator[]( const hsize_t Index ); 
  reference operator*();
  //reference operator->();
protected:
  bool m_bValidIndex;  // m_ItemIndex is valid ie, is something from .begin() to .end()
  CHDF5TimeSeriesAccessor<T> *m_pAccessor;
  hsize_t m_ItemIndex; // index into specific item
  T m_T; // the currently retrieved datum, could keep and track two values in order to optimize lower_bound speed somewhat
private:
};

template<class T> CHDF5TimeSeriesIterator<T>::CHDF5TimeSeriesIterator( void ):
  m_pAccessor( NULL ), 
  m_ItemIndex( 0 ),
  m_bValidIndex( false )
   {
}

template<class T> CHDF5TimeSeriesIterator<T>::CHDF5TimeSeriesIterator( CHDF5TimeSeriesAccessor<T> *pAccessor, hsize_t Index ):
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

template<class T> CHDF5TimeSeriesIterator<T>::CHDF5TimeSeriesIterator( const CHDF5TimeSeriesIterator<T>& other ) :
  m_pAccessor( other.m_pAccessor ),
  m_bValidIndex( other.m_bValidIndex ), 
  m_ItemIndex( other.m_ItemIndex ),
  m_T( other.m_T ) 
  {
}

template<class T> CHDF5TimeSeriesIterator<T>& CHDF5TimeSeriesIterator<T>::operator=( const CHDF5TimeSeriesIterator<T> &other ) {
  // has two choices:  first assigned from, second being assigned to
  if ( this == &other ) {
    // a) being assigned from, so just return reference
  }
  else {
    // b) being assigned to
    m_pAccessor = other.m_pAccessor;
    m_bValidIndex = other.m_bValidIndex;
    m_ItemIndex = other.m_ItemIndex;
    m_T = other.m_T;
  }
  return( *this );
}

template<class T> CHDF5TimeSeriesIterator<T>& CHDF5TimeSeriesIterator<T>::operator++() { // pre-increment
  assert( m_bValidIndex );
  assert( m_ItemIndex < m_pAccessor->size() );
  ++m_ItemIndex;
  //if ( m_ItemIndex < m_pAccessor->size() ) {
  //  m_pAccessor->ReadItem( m_ItemIndex, &m_T );  // retrieve at our new location if we can
  //}
  return( *this );
}

template<class T> CHDF5TimeSeriesIterator<T> CHDF5TimeSeriesIterator<T>::operator++( int ) { // post-increment
  CHDF5TimeSeriesIterator<T> result( *this );  // make a copy of what is before increment
  assert( m_bValidIndex );
  assert( m_ItemIndex < m_pAccessor->size() );
  ++m_ItemIndex;
  //if ( m_ItemIndex < m_pAccessor->size() ) {
  //  m_pAccessor->Retrieve( m_ItemIndex, &m_T );
  //}
  return( result ); 
}

template<class T> CHDF5TimeSeriesIterator<T>& CHDF5TimeSeriesIterator<T>::operator+=( const hsize_t inc ) { // plus assignment
  assert( m_bValidIndex );
  m_ItemIndex += inc;
  assert( m_ItemIndex <= m_pAccessor->size() );
  //if ( m_ItemIndex < m_pAccessor->size() ) {
  //  m_pAccessor->ReadItem( m_ItemIndex, &m_T );  // retrieve at our new location if we can
  //}
  return( *this );
}

template<class T> CHDF5TimeSeriesIterator<T>& CHDF5TimeSeriesIterator<T>::operator-=( const hsize_t inc ) { // plus assignment
  assert( m_bValidIndex );
  assert( inc <= m_ItemIndex );
  m_ItemIndex -= inc;
  //if ( m_ItemIndex < m_pAccessor->size() ) {
  //  m_pAccessor->ReadItem( m_ItemIndex, &m_T );  // retrieve at our new location if we can
  //}
  return( *this );
}

template<class T> CHDF5TimeSeriesIterator<T> CHDF5TimeSeriesIterator<T>::operator-( const hsize_t val ) { // subtraction
  assert( val <= m_ItemIndex );
  CHDF5TimeSeriesIterator<T> result( *this );  // make a copy of this
  result.m_ItemIndex -= val;
  return result;
}

template<class T> hsize_t CHDF5TimeSeriesIterator<T>::operator-( const CHDF5TimeSeriesIterator<T> &other ) const { // subtraction
  return m_ItemIndex - other.m_ItemIndex;
}

template<class T> bool CHDF5TimeSeriesIterator<T>::operator<( const CHDF5TimeSeriesIterator<T> &other ) const {
  assert( m_pAccessor == other.m_pAccessor );
  return ( m_ItemIndex < other.m_ItemIndex );
}

template<class T> bool CHDF5TimeSeriesIterator<T>::operator==( const CHDF5TimeSeriesIterator<T> &other ) const {
  assert( m_pAccessor == other.m_pAccessor );
  return ( m_ItemIndex == other.m_ItemIndex );
}

template<class T> bool CHDF5TimeSeriesIterator<T>::operator!=( const CHDF5TimeSeriesIterator<T> &other ) const {
  assert( m_pAccessor == other.m_pAccessor );
  return !( m_ItemIndex == other.m_ItemIndex );
}

template<class T> typename CHDF5TimeSeriesIterator<T>::reference CHDF5TimeSeriesIterator<T>::operator*() {
  assert( m_bValidIndex );
  assert( m_ItemIndex < m_pAccessor->size() );
  m_pAccessor->Read( m_ItemIndex, &m_T );
  return m_T;
}

//template<class T> typename CHDF5TimeSeriesIterator<T>::reference CHDF5TimeSeriesIterator<T>::operator->() {
//  m_pAccessor->ReadItem( m_ItemIndex, &m_T );
//  return m_T;  // not sure yet how this works
//}

// does operator[] make sense for iterator?  ... look in requirements for a random interator
//template<class T> CHDF5TimeSeriesIterator<T> &CHDF5TimeSeriesIterator<T>::operator[]( const hsize_t Index ) {
//  assert( Index < m_pAccessor->size() );
//  m_ItemIndex = Index;
//  m_bValidIndex = true;
//  m_pAccessor->Retrieve( Index, &m_T );
//  return (*this);
//}

