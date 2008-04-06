#pragma once

#include "DatedDatum.h"

#include "DataManager.h"
using namespace H5;

#include "HDF5TimeSeriesAccessor.h"

#include <iterator>
#include <stdexcept>
//using namespace std;

template<class T> class CHDF5TimeSeriesIterator: 
//  public std::iterator<std::random_access_iterator_tag, const T, hsize_t> {
  public std::iterator<std::random_access_iterator_tag, T, hsize_t> {
public:
  explicit CHDF5TimeSeriesIterator<T>( void ); // end() init
  explicit CHDF5TimeSeriesIterator<T>( CHDF5TimeSeriesAccessor<T> *pAccessor, hsize_t Index );  // begin() or later init
  CHDF5TimeSeriesIterator<T>( const CHDF5TimeSeriesIterator<T>& other );  // copy constructor
  ~CHDF5TimeSeriesIterator<T>( void ) { };
  //typedef CHDF5TimeSeriesIterator self_type;
  //typedef self_type &self_reference;
  CHDF5TimeSeriesIterator<T> &operator=( const CHDF5TimeSeriesIterator<T> &other );
  CHDF5TimeSeriesIterator<T> &operator++(); // pre-increment
  CHDF5TimeSeriesIterator<T>  operator++( int ); // post-increment
  bool operator<( const CHDF5TimeSeriesIterator<T> &other );
  bool operator==( const CHDF5TimeSeriesIterator<T> &other );
  bool operator!=( const CHDF5TimeSeriesIterator<T> &other );
  CHDF5TimeSeriesIterator<T> &operator[]( const hsize_t Index ); 
  reference operator*();
  reference operator->();
protected:
  CHDF5TimeSeriesAccessor<T> *m_pAccessor;
  bool m_bValidIndex;  // m_ItemIndex is valid ie, is something from .begin() to .end()
  hsize_t m_ItemIndex; // index into specific item
  T m_T; // the currently retrieved datum
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
  if ( Index < m_pAccessor->size() ) {
    m_pAccessor->Retrieve( m_ItemIndex, &m_T );
  }
  m_bValidIndex = true;
}

template<class T> CHDF5TimeSeriesIterator<T>::CHDF5TimeSeriesIterator( const CHDF5TimeSeriesIterator<T>& other ) :
  m_pAccessor( other.m_pAccessor ),
  m_bValidIndex( other.m_bValidIndex ), 
  m_ItemIndex( other.m_ItemIndex ),
  m_T( other.m_T ) 
  {
}

template<class T> CHDF5TimeSeriesIterator<T> &CHDF5TimeSeriesIterator<T>::operator=( const CHDF5TimeSeriesIterator<T> &other ) {
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

template<class T> CHDF5TimeSeriesIterator<T> &CHDF5TimeSeriesIterator<T>::operator++() { // pre-increment
  assert( m_bValidIndex );
  assert( m_ItemIndex < m_pAccessor->size() );
  ++m_ItemIndex;
  if ( m_ItemIndex < m_pAccessor->size() ) {
    m_pAccessor->Retrieve( m_ItemIndex, &m_T );  // retrieve at our new location if we can
  }
  return( *this );
}

template<class T> CHDF5TimeSeriesIterator<T> CHDF5TimeSeriesIterator<T>::operator++( int ) { // post-increment
  CHDF5TimeSeriesIterator<T> result( *this );  // make a copy of what is before increment
  assert( m_bValidIndex );
  assert( m_ItemIndex < m_pAccessor->size() );
  ++m_ItemIndex;
  if ( m_ItemIndex < m_pAccessor->size() ) {
    m_pAccessor->Retrieve( m_ItemIndex, &m_T );
  }
  return( result ); 
}

template<class T> CHDF5TimeSeriesIterator<T> &CHDF5TimeSeriesIterator<T>::operator[]( const hsize_t Index ) {
  assert( Index < m_pAccessor->size() );
  m_ItemIndex = Index;
  m_bValidIndex = true;
  m_pAccessor->Retrieve( Index, &m_T );
  return (*this);
}

template<class T> bool CHDF5TimeSeriesIterator<T>::operator<( const CHDF5TimeSeriesIterator<T> &other ) {
  assert( m_pAccessor == other.m_pAccessor );
  return ( m_ItemIndex < other.m_ItemIndex );
}

template<class T> bool CHDF5TimeSeriesIterator<T>::operator==( const CHDF5TimeSeriesIterator<T> &other ) {
  assert( m_pAccessor == other.m_pAccessor );
  return ( m_ItemIndex == other.m_ItemIndex );
}

template<class T> bool CHDF5TimeSeriesIterator<T>::operator!=( const CHDF5TimeSeriesIterator<T> &other ) {
  assert( m_pAccessor == other.m_pAccessor );
  return !( m_ItemIndex == other.m_ItemIndex );
}

template<class T> typename CHDF5TimeSeriesIterator<T>::reference CHDF5TimeSeriesIterator<T>::operator*() {
  return m_T;
}

template<class T> typename CHDF5TimeSeriesIterator<T>::reference CHDF5TimeSeriesIterator<T>::operator->() {
  return m_T;  // not sure yet how this works
}
