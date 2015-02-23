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
#include <string>

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

class TimeSeriesBase { // used for dynamic_cast capability
public:
  virtual ~TimeSeriesBase( void ) {};
protected:
private:
};

template<typename T> 
class TimeSeries: public TimeSeriesBase {
public:

  typedef T datum_t;

  typedef typename std::vector<T>::size_type size_type;

  typedef typename std::vector<T>::iterator iterator;
  typedef typename std::vector<T>::const_iterator const_iterator;
  typedef typename std::vector<T>::reference reference;
  typedef typename std::vector<T>::const_reference const_reference;

  TimeSeries<T>( void );
  TimeSeries<T>( size_type nSize );
  TimeSeries<T>( const std::string& sName, size_type nSize = 0 );
  TimeSeries<T>( const TimeSeries<T>& );
  virtual ~TimeSeries<T>( void );

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
  const_iterator AtOrAfter( const ptime& time );
  const_iterator After( const ptime& time );

  const_iterator begin() const { return m_vSeries.cbegin(); };
  const_iterator end() const { return m_vSeries.cend(); };
  const_iterator at( size_type ix ) const { 
    assert( ix < m_vSeries.size() );
    return m_vSeries.cbegin() + ix; 
  };

  ou::Delegate<const T&> OnAppend;

  void SetName( const std::string& sName ) { m_sName = sName; };
  const std::string& GetName( void ) { return m_sName; };

  virtual TimeSeries<T>* Subset( const ptime &time ) const; // from At or After to end
  virtual TimeSeries<T>* Subset( const ptime &time, unsigned int n ) const; // from At or After for n T

  H5::DataSpace* DefineDataSpace( H5::DataSpace* pSpace = NULL );

  void Reserve( size_type n ) { m_vSeries.reserve( n ); };

  bool& AppendEnabled( void ) { return m_bAppendToVector; };  // affects Append(...) only

  template<typename Functor>
  typename Functor::return_type ForEach( Functor f ) const {
    return std::for_each( m_vSeries.cbegin(), m_vSeries.cend(), f );
  }

protected:
private:
  bool m_bAppendToVector;  // hf stats use many time series, many not needed, so don't build up vector for those
  std::string m_sName;
  std::vector<T> m_vSeries;
  const_iterator m_vIterator;  // belongs after vector declaration
};

template<typename T> 
TimeSeries<T>::TimeSeries(void)
  : m_vIterator( m_vSeries.end() ), m_bAppendToVector( true ) {
}

template<typename T> 
TimeSeries<T>::TimeSeries( const std::string& sName, size_type nSize )
  : m_vIterator( m_vSeries.end() ), m_sName( sName ), m_bAppendToVector( true ) {
  if ( ( 0 != nSize ) && ( m_vSeries.size() < nSize ) ) m_vSeries.reserve( nSize );
}

template<typename T> 
TimeSeries<T>::TimeSeries( size_type size )
  : m_vIterator( m_vSeries.end() ), m_bAppendToVector( true ) {
  m_vSeries.reserve( size );
}

template<typename T>
TimeSeries<T>::TimeSeries( const TimeSeries<T>& series )
  : m_bAppendToVector( series.m_bAppendToVector ) {
  m_vSeries = series.m_vSeries;
  m_vIterator = m_vSeries.end();
}

template<typename T> 
TimeSeries<T>::~TimeSeries(void) {
  Clear();
}

template<typename T> 
void TimeSeries<T>::Append(const T& datum) {
  if ( m_bAppendToVector ) {
    m_vSeries.push_back( datum );
  }
  else { // provide for .ago(0) capability
    if ( 0 == m_vSeries.size() ) {
      m_vSeries.push_back( datum );
    }
    else {
      m_vSeries.back() = datum;
    }
  }
  OnAppend( datum );
}

template<typename T> 
void TimeSeries<T>::Insert( const ptime& dt, const T& datum ) {
  T key( dt );
  std::pair<iterator, iterator> p;
  p = equal_range( m_vSeries.begin(), m_vSeries.end(), key );
  if ( m_vSeries.end() == p.second ) {
    m_vSeries.push_back( datum );
  }
  else {
    m_vSeries.insert( p.second, datum );
  }
}

template<typename T> 
void TimeSeries<T>::Insert( const T& datum ) {
  std::pair<iterator, iterator> p;
  p = equal_range( m_vSeries.begin(), m_vSeries.end(), datum );
  if ( m_vSeries.end() == p.second ) {
    m_vSeries.push_back( datum );
  }
  else {
    m_vSeries.insert( p.second, datum );
  }
}

template<typename T> 
void TimeSeries<T>::Clear( void ) {
  m_vSeries.clear();
}


template<typename T> 
const T* TimeSeries<T>::First() {
  m_vIterator = m_vSeries.begin();
  if ( m_vSeries.end() == m_vIterator ) {
    return NULL;
  }
  else {
    return &(*m_vIterator);
  }
}

template<typename T> 
const T* TimeSeries<T>::Next() {
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
const T* TimeSeries<T>::Last() {
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
typename TimeSeries<T>::const_reference TimeSeries<T>::Ago( size_type ix ) {
  assert( ix < m_vSeries.size() );
  typename std::vector<T>::const_reverse_iterator iter( m_vSeries.rbegin() );
  iter += ix;
  return *iter;
}

template<typename T> 
typename TimeSeries<T>::const_reference TimeSeries<T>::operator []( size_type ix ) {
  assert( ix < m_vSeries.size() );
  return m_vSeries.at( ix );
}

template<typename T> 
typename TimeSeries<T>::const_reference TimeSeries<T>::At( size_type ix ) {
  assert( ix < m_vSeries.size() );
  return m_vSeries.at( ix );
}

/*
template<typename T> 
typename TimeSeries<T>::const_reference TimeSeries<T>::At( const ptime& dt ) {
  // assumes sorted vector
  // assumes valid access, else undefined
  // TODO: Check that this is correct
  T key( dt );
  std::pair<iterator, iterator> p;
  p = equal_range( m_vSeries.begin(), m_vSeries.end(), key );
//  if ( p.first != p.second ) {
//    m_vIterator = p.first;
//  }
//  return &(*m_vIterator);
  return *p.first;
}
*/

template<typename T> 
typename TimeSeries<T>::const_iterator TimeSeries<T>::AtOrAfter( const ptime &dt ) {
  // assumes sorted vector
  // assumes valid access, else undefined
  // TODO: Check that this is correct
  T key( dt );
  std::pair<const_iterator, const_iterator> p;
  p = equal_range( m_vSeries.begin(), m_vSeries.end(), key );
//  if ( p.first != p.second ) {
//    m_vIterator = p.first;
//  }
//  return &(*m_vIterator);
  return p.first;
}

template<typename T> 
typename TimeSeries<T>::const_iterator TimeSeries<T>::After( const ptime &dt ) {
  // assumes sorted vector
  // assumes valid access, else undefined
  // TODO: Check that this is correct
  T key( dt );
  std::pair<const_iterator, const_iterator> p;
  p = equal_range( m_vSeries.begin(), m_vSeries.end(), key );
  return p.second;
}

template<typename T> 
void TimeSeries<T>::Sort( void ) {
  sort( m_vSeries.begin(), m_vSeries.end() );  // may not keep time series with identical keys in acquired order (may not be an issue, as external clock is written to be monotonically increasing)
}

template<typename T> 
TimeSeries<T>* TimeSeries<T>::Subset( const ptime &dt ) const {
  T datum( dt );
  TimeSeries<T>* series = NULL;
  const_iterator iter;
  iter = lower_bound( m_vSeries.begin(), m_vSeries.end(), datum );
  if ( m_vSeries.end() != iter ) {
    series = new TimeSeries<T>( (unsigned int) (m_vSeries.end() - iter) );
    while ( m_vSeries.end() != iter ) {
      series->Append( *iter );
      ++iter;
    }
  }
  else {
    series = new TimeSeries<T>();
  }
  return series;
}

template<typename T> 
TimeSeries<T>* TimeSeries<T>::Subset( const ptime &dt, unsigned int n ) const { // n is max count
  T datum( dt );
  TimeSeries<T>* series = NULL;
  const_iterator iter;
  iter = lower_bound( m_vSeries.begin(), m_vSeries.end(), datum );
  if ( m_vSeries.end() != iter ) {
    unsigned int todo = std::min<unsigned int>( n, (unsigned int) ( m_vSeries.end() - iter ) );
    series = new TimeSeries<T>( todo );
    while ( 0 < todo ) {
      series->Append( *iter );
      ++iter;
      --todo;
    }
  }
  else {
    series = new TimeSeries<T>();
  }
  return series;
}

template<typename T> 
H5::DataSpace* TimeSeries<T>::DefineDataSpace( H5::DataSpace* pSpace ) {
  if ( NULL == pSpace ) pSpace = new H5::DataSpace( H5S_SIMPLE );
  hsize_t curSize = m_vSeries.size();
  hsize_t maxSize = H5S_UNLIMITED; 
  if ( 0 != curSize ) {
    pSpace->setExtentSimple( 1, &curSize, &maxSize ); 
  }
  else {
    //throw runtime_error( "TimeSeries<T>::DefineDataSpace series is empty" );
    std::cout << "TimeSeries<T>::DefineDataSpace series is empty" << std::endl;
  }
  return pSpace; 
}

// Bars

class Bars: public TimeSeries<Bar> {
public:
  typedef Bar datum_t;
  Bars(void) {};
  Bars( size_type size ): TimeSeries<datum_t>( size ) {};
  virtual ~Bars(void) {};
  Bars* Subset( ptime time ) const { return (Bars*) TimeSeries<datum_t>::Subset( time ); };
  Bars* Subset( ptime time, unsigned int n ) const { return (Bars*) TimeSeries<datum_t>::Subset( time, n ); };
protected:
private:
};

// Trades

class Trades: public TimeSeries<Trade> {
public:
  typedef Trade datum_t;
  Trades( void ) {};
  Trades( size_type size ): TimeSeries<datum_t>( size ) {};
  ~Trades( void ) {};
  Trades* Subset( ptime time ) const { return (Trades*) TimeSeries<datum_t>::Subset( time ); };
  Trades* Subset( ptime time, unsigned int n ) const { return (Trades*) TimeSeries<datum_t>::Subset( time, n ); };
protected:
private:
};

// Quotes

class Quotes: public TimeSeries<Quote> {
public:
  typedef Quote datum_t;
  Quotes( void ) {};
  Quotes( size_type size ): TimeSeries<datum_t>( size ) {};
  ~Quotes( void ) {};
  Quotes* Subset( ptime time ) const { return (Quotes*) TimeSeries<datum_t>::Subset( time ); };
  Quotes* Subset( ptime time, unsigned int n ) const { return (Quotes*) TimeSeries<datum_t>::Subset( time, n ); };
protected:
private:
};

// MarketDepth

class MarketDepths: public TimeSeries<MarketDepth> {
public:
  typedef MarketDepth datum_t;
  MarketDepths( void ) {};
  MarketDepths( size_type size ): TimeSeries<datum_t>( size ) {};
  ~MarketDepths( void ) {};
  MarketDepths* Subset( ptime time ) const { return (MarketDepths*) TimeSeries<datum_t>::Subset( time ); };
  MarketDepths* Subset( ptime time, unsigned int n ) const { return (MarketDepths*) TimeSeries<datum_t>::Subset( time, n ); };
protected:
private:
};

// Greeks

class Greeks: public TimeSeries<Greek> {
public:
  typedef Greek datum_t;
  Greeks( void ) {};
  Greeks( size_type size ): TimeSeries<datum_t>( size ) {};
  ~Greeks( void ) {};
  Greeks* Subset( ptime time ) const { return (Greeks*) TimeSeries<datum_t>::Subset( time ); };
  Greeks* Subset( ptime time, unsigned int n ) const { return (Greeks*) TimeSeries<datum_t>::Subset( time, n ); };
protected:
private:
};

// Prices
// used for holding indicators, returns, ...

class Prices: public TimeSeries<Price> {
public:
  typedef Price datum_t;
  Prices( void ) {};
  Prices( size_type size ): TimeSeries<datum_t>( size ) {};
  ~Prices( void ) {};
  Prices* Subset( ptime time ) const { return (Prices*) TimeSeries<datum_t>::Subset( time ); };
  Prices* Subset( ptime time, unsigned int n ) const { return (Prices*) TimeSeries<datum_t>::Subset( time, n ); };
protected:
private:
};

// PriceIVs

class PriceIVs: public TimeSeries<PriceIV> {
public:
  typedef PriceIV datum_t;
  PriceIVs( void ) {};
  PriceIVs( size_type size ): TimeSeries<datum_t>( size ) {};
  ~PriceIVs( void ) {};
  PriceIVs* Subset( ptime time ) const { return (PriceIVs*) TimeSeries<datum_t>::Subset( time ); };
  PriceIVs* Subset( ptime time, unsigned int n ) const { return (PriceIVs*) TimeSeries<datum_t>::Subset( time, n ); };
protected:
private:
};

} // namespace tf
} // namespace ou
