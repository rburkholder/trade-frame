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
#pragma warning( disable: 4482 )

#include <vector>
#include <algorithm>

#include <OUCommon/Delegate.h>

#include "DatedDatum.h"

// 2012/04/01 use Intel Thread Building Blocks to use concurrent_vector?
// not sure:  the time series here are typically just used for batch mode processing into and out of hdf5 files
//   time series operators usually work with most current value, 
//   not many, if any, algorithms scan the time series,
//   there fore time series can be extended by back ground threads, so long as no access by other threads
//   bottom line:  current implementation is not thread safe

//#include <boost/serialization/vector.hpp>
// http://www.boost.org/libs/serialization/doc/traits.html

namespace ou { // One Unified
namespace tf { // TradeFrame

template<typename T> 
class CTimeSeries {
public:

  typedef typename T datum_t;

  typedef typename std::vector<T>::iterator iterator;
  typedef typename std::vector<T>::const_iterator const_iterator;
  typedef typename std::vector<T>::size_type size_type;
  typedef typename std::vector<T>::reference reference;
  typedef typename std::vector<T>::const_reference const_reference;

  CTimeSeries<T>( void );
  CTimeSeries<T>( size_type Size );
  virtual ~CTimeSeries<T>( void );

  size_type Size() const { return m_vSeries.size(); };

  void Clear( void );
  void Append( const T& datum );
  void Insert( const ptime& time, const T& datum );  // time overrides datum.time?
  void Insert( const T& datum );
  void Resize( size_type Size ) { m_vSeries.resize( Size );  }; 

  void Sort( void ); // use when loaded from external data
  void Flip( void ) { reverse( m_vSeries.begin(), m_vSeries.end() ); };

  // these three methods update m_vIterator, used mostly with MergeDatedDatumCarrier
  const T* First();
  const T* Next();
  const T* Last();

  const_reference Ago( size_type ix );
  const_reference operator[]( size_type ix );
  const_reference At( size_type ix );
  //const_reference At( const ptime& time );
  const_reference AtOrAfter( const ptime& time );
  const_reference After( const ptime& time );

  const_iterator begin() const { return m_vSeries.cbegin(); };
  const_iterator end() const { return m_vSeries.cend(); };
  const_iterator at( size_type ix ) const { 
    assert( ix < m_vSeries.size() );
    return m_vSeries.cbegin() + ix; 
  };

  ou::Delegate<const T&> OnAppend;

  virtual CTimeSeries<T>* Subset( const ptime &time ) const; // from At or After to end
  virtual CTimeSeries<T>* Subset( const ptime &time, unsigned int n ) const; // from At or After for n T

  H5::DataSpace* DefineDataSpace( H5::DataSpace *pSpace = NULL );

protected:
private:
  std::vector<T> m_vSeries;
  const_iterator m_vIterator;
};

template<typename T> 
CTimeSeries<T>::CTimeSeries(void): m_vIterator( m_vSeries.end() ) {
}

template<typename T> 
CTimeSeries<T>::CTimeSeries( size_type size ) {
  m_vSeries.reserve( size );
}

template<typename T> 
CTimeSeries<T>::~CTimeSeries(void) {
  Clear();
}

template<typename T> 
void CTimeSeries<T>::Append(const T& datum) {
  m_vSeries.push_back( datum );
  OnAppend( datum );
}

template<typename T> 
void CTimeSeries<T>::Insert( const ptime& dt, const T& datum ) {
  T key( dt );
  std::pair<std::vector<T>::iterator, std::vector<T>::iterator> p;
  p = equal_range( m_vSeries.begin(), m_vSeries.end(), key );
  if ( m_vSeries.end() == p.second ) {
    m_vSeries.push_back( datum );
  }
  else {
    m_vSeries.insert( p.second, datum );
  }
}

template<typename T> 
void CTimeSeries<T>::Insert( const T& datum ) {
  std::pair<std::vector<T>::iterator, std::vector<T>::iterator> p;
  p = equal_range( m_vSeries.begin(), m_vSeries.end(), datum );
  if ( m_vSeries.end() == p.second ) {
    m_vSeries.push_back( datum );
  }
  else {
    m_vSeries.insert( p.second, datum );
  }
}

template<typename T> 
void CTimeSeries<T>::Clear( void ) {
  m_vSeries.clear();
}


template<typename T> 
const T* CTimeSeries<T>::First() {
  m_vIterator = m_vSeries.begin();
  if ( m_vSeries.end() == m_vIterator ) {
    return NULL;
  }
  else {
    return &(*m_vIterator);
  }
}

template<typename T> 
const T* CTimeSeries<T>::Next() {
  if ( m_vSeries.end() == m_vIterator ) {
    return NULL;
  }
  else {
    m_vIterator++;
    if ( m_vSeries.end() == m_vIterator ) {
      return NULL;
    }
    else {
      return &(*m_vIterator);
    }
  }
}

template<typename T> 
const T* CTimeSeries<T>::Last() {
  m_vIterator = m_vSeries.end();
  if ( 0 == m_vSeries.size() ) {
    return NULL;
  }
  else {
    --m_vIterator;
    return &(*m_vIterator);
  }
}

template<typename T> 
typename CTimeSeries<T>::const_reference CTimeSeries<T>::Ago( size_type ix ) {
  assert( ix < m_vSeries.size() );
  typename std::vector<T>::const_reverse_iterator iter( m_vSeries.rbegin() );
  iter += ix;
  return *iter;
}

template<typename T> 
typename CTimeSeries<T>::const_reference CTimeSeries<T>::operator []( size_type ix ) {
  assert( ix < m_vSeries.size() );
  return m_vSeries.at( ix );
}

template<typename T> 
typename CTimeSeries<T>::const_reference CTimeSeries<T>::At( size_type ix ) {
  assert( ix < m_vSeries.size() );
  return m_vSeries.at( ix );
}

/*
template<typename T> 
typename CTimeSeries<T>::const_reference CTimeSeries<T>::At( const ptime& dt ) {
  // assumes sorted vector
  // assumes valid access, else undefined
  // TODO: Check that this is correct
  T key( dt );
  std::pair<std::vector<T>::iterator, std::vector<T>::iterator> p;
  p = equal_range( m_vSeries.begin(), m_vSeries.end(), key );
//  if ( p.first != p.second ) {
//    m_vIterator = p.first;
//  }
//  return &(*m_vIterator);
  return *p.first;
}
*/

template<typename T> 
typename CTimeSeries<T>::const_reference CTimeSeries<T>::AtOrAfter( const ptime &dt ) {
  // assumes sorted vector
  // assumes valid access, else undefined
  // TODO: Check that this is correct
  T key( dt );
  std::pair<std::vector<T>::iterator, std::vector<T>::iterator> p;
  p = equal_range( m_vSeries.begin(), m_vSeries.end(), key );
//  if ( p.first != p.second ) {
//    m_vIterator = p.first;
//  }
//  return &(*m_vIterator);
  return *p.first;
}

template<typename T> 
typename CTimeSeries<T>::const_reference CTimeSeries<T>::After( const ptime &dt ) {
  // assumes sorted vector
  // assumes valid access, else undefined
  // TODO: Check that this is correct
  T key( dt );
  std::pair<std::vector<T>::iterator, std::vector<T>::iterator> p;
  p = equal_range( m_vSeries.begin(), m_vSeries.end(), key );
  return *p.second;
}

template<typename T> 
void CTimeSeries<T>::Sort( void ) {
  sort( m_vSeries.begin(), m_vSeries.end() );  // may not keep time series with identical keys in acquired order (may not be an issue, as external clock is written to be monotonically increasing)
}

template<typename T> 
CTimeSeries<T>* CTimeSeries<T>::Subset( const ptime &dt ) const {
  T datum( dt );
  CTimeSeries<T> *series = NULL;
  const_iterator iter;
  iter = lower_bound( m_vSeries.begin(), m_vSeries.end(), datum );
  if ( m_vSeries.end() != iter ) {
    series = new CTimeSeries<T>( (unsigned int) (m_vSeries.end() - iter) );
    while ( m_vSeries.end() != iter ) {
      series->Append( *iter );
      ++iter;
    }
  }
  else {
    series = new CTimeSeries<T>();
  }
  return series;
}

template<typename T> 
CTimeSeries<T>* CTimeSeries<T>::Subset( const ptime &dt, unsigned int n ) const { // n is max count
  T datum( dt );
  CTimeSeries<T> *series = NULL;
  const_iterator iter;
  iter = lower_bound( m_vSeries.begin(), m_vSeries.end(), datum );
  if ( m_vSeries.end() != iter ) {
    unsigned int todo = std::min<unsigned int>( n, (unsigned int) ( m_vSeries.end() - iter ) );
    series = new CTimeSeries<T>( todo );
    while ( 0 < todo ) {
      series->Append( *iter );
      ++iter;
      --todo;
    }
  }
  else {
    series = new CTimeSeries<T>();
  }
  return series;
}

template<typename T> 
H5::DataSpace* CTimeSeries<T>::DefineDataSpace( H5::DataSpace *pSpace ) {
  if ( NULL == pSpace ) pSpace = new H5::DataSpace( H5S_SIMPLE );
  hsize_t curSize = m_vSeries.size();
  hsize_t maxSize = H5S_UNLIMITED; 
  if ( 0 != curSize ) {
    pSpace->setExtentSimple( 1, &curSize, &maxSize ); 
  }
  else {
    //throw runtime_error( "CTimeSeries<T>::DefineDataSpace series is empty" );
    std::cout << "CTimeSeries<T>::DefineDataSpace series is empty" << std::endl;
  }
  return pSpace; 
}

// Bars

class CBars: public CTimeSeries<CBar> {
public:
  CBars(void);
  CBars( size_type );
  virtual ~CBars(void);
  CBars* Subset( ptime time ) const { return (CBars*) CTimeSeries<CBar>::Subset( time ); };
  CBars* Subset( ptime time, unsigned int n ) const { return (CBars*) CTimeSeries<CBar>::Subset( time, n ); };
protected:
private:
};

// Trades

class CTrades: public CTimeSeries<CTrade> {
public:
  CTrades( void );
  CTrades( size_type );
  ~CTrades( void );
  CTrades* Subset( ptime time ) const { return (CTrades*) CTimeSeries<CTrade>::Subset( time ); };
  CTrades* Subset( ptime time, unsigned int n ) const { return (CTrades*) CTimeSeries<CTrade>::Subset( time, n ); };
protected:
private:
};

// Quotes

class CQuotes: public CTimeSeries<CQuote> {
public:
  CQuotes( void );
  CQuotes( size_type );
  ~CQuotes( void );
  CQuotes* Subset( ptime time ) const { return (CQuotes*) CTimeSeries<CQuote>::Subset( time ); };
  CQuotes* Subset( ptime time, unsigned int n ) const { return (CQuotes*) CTimeSeries<CQuote>::Subset( time, n ); };
protected:
private:
};

// MarketDepth

class CMarketDepths: public CTimeSeries<CMarketDepth> {
public:
  CMarketDepths( void );
  CMarketDepths( size_type );
  ~CMarketDepths( void );
  CMarketDepths* Subset( ptime time ) const { return (CMarketDepths*) CTimeSeries<CMarketDepth>::Subset( time ); };
  CMarketDepths* Subset( ptime time, unsigned int n ) const { return (CMarketDepths*) CTimeSeries<CMarketDepth>::Subset( time, n ); };
protected:
private:
};

// Greeks

class CGreeks: public CTimeSeries<CGreek> {
public:
  CGreeks( void );
  CGreeks( size_type );
  ~CGreeks( void );
  CGreeks* Subset( ptime time ) const { return (CGreeks*) CTimeSeries<CGreek>::Subset( time ); };
  CGreeks* Subset( ptime time, unsigned int n ) const { return (CGreeks *) CTimeSeries<CGreek>::Subset( time, n ); };
protected:
private:
};

// Prices
// used for holding indicators, returns, ...

class CPrices: public CTimeSeries<CPrice> {
public:
  CPrices( void );
  CPrices( size_type );
  ~CPrices( void );
  CPrices* Subset( ptime time ) const { return (CPrices*) CTimeSeries<CPrice>::Subset( time ); };
  CPrices* Subset( ptime time, unsigned int n ) const { return (CPrices*) CTimeSeries<CPrice>::Subset( time, n ); };
protected:
private:
};



} // namespace tf
} // namespace ou
