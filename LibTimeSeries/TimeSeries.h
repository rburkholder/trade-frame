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

#include <vector>
#include <algorithm>
#include <iostream>

#include "DatedDatum.h"

//#include <boost/serialization/vector.hpp>
// http://www.boost.org/libs/serialization/doc/traits.html

template<class T> class CTimeSeries {
public:

  typedef typename std::vector<T>::iterator iterator;

  CTimeSeries<T>( void );
  CTimeSeries<T>( size_t Size );
  virtual ~CTimeSeries<T>( void );

  size_t Size() const { return m_vSeries.size(); };
  void Clear( void );
  void Append( const T& datum );
  void Insert( const ptime& time, const T& datum );
  void Resize( size_t Size ) { 
    m_vSeries.resize( Size );  
    void *p = &m_vSeries; 
  }; 

  void Sort( void ); // use when loaded from external data
  void Flip( void ) { reverse( m_vSeries.begin(), m_vSeries.end() ); };

  T* First();
  T* Next();
  T* Last();
  T* Ago( size_t ix );
  T* operator[]( size_t ix );
  T* At( size_t ix );
  T* At( const ptime &time );
  T* AtOrAfter( const ptime &time );
  T* After( const ptime &time );

  iterator iterAt( const ptime &time );
  iterator iterAtOrAfter( const ptime &time );

  iterator begin() const { return m_vSeries.begin(); };
  iterator end() const { return m_vSeries.end(); };

  virtual CTimeSeries<T> *Subset( const ptime &time ); // from At or After to end
  virtual CTimeSeries<T> *Subset( const ptime &time, unsigned int n ); // from At or After for n T

  H5::DataSpace *DefineDataSpace( H5::DataSpace *pSpace = NULL );
protected:
  std::vector<T> m_vSeries;
  iterator m_vIterator;
private:
};

template<class T> CTimeSeries<T>::CTimeSeries(void) {
}

template<class T> CTimeSeries<T>::CTimeSeries( size_t size ) {
  m_vSeries.reserve( size );
}

template<class T> CTimeSeries<T>::~CTimeSeries(void) {
  Clear();
}

template<class T> void CTimeSeries<T>::Append(const T &datum) { // changed name 'cause VC++ doesn't do symbol results properly
  m_vSeries.push_back( datum );
  m_vIterator = m_vSeries.end() - 1;
}

template<class T> void CTimeSeries<T>::Insert( const ptime &dt, const T &datum ) {
  T key( dt );
  // T *datum = NULL;  // is this needed?
  std::pair<std::vector<T>::iterator, std::vector<T>::iterator> p;
  p = equal_range( m_vSeries.begin(), m_vSeries.end(), key );
  if ( m_vSeries.end() == p.second ) {
    m_vIterator = m_vSeries.push_back( datum );
  }
  else {
    m_vIterator = m_vSeries.insert( p.second, datum );
  }
}

template<class T> void CTimeSeries<T>::Clear( void ) {
  m_vSeries.clear();
  m_vIterator = m_vSeries.end();
}

template<class T> T* CTimeSeries<T>::First() {
  m_vIterator = m_vSeries.begin();
  if ( m_vSeries.end() == m_vIterator ) {
    return NULL;
  }
  else {
    return &(*m_vIterator);
  }
}

template<class T> T* CTimeSeries<T>::Next() {
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

template<class T> T* CTimeSeries<T>::Last() {
  // TODO:  Can we use .back on this?
  return m_vSeries.empty() ? NULL : &m_vSeries[ m_vSeries.size() - 1 ];
}

template<class T> T* CTimeSeries<T>::Ago( size_t ix ) {
  T* datum = NULL;
  if ( 0 != m_vSeries.size() ) {
    if ( ix < m_vSeries.size() ) {
      m_vIterator = m_vSeries.end() - ix;
      //datum = &m_vSeries[ m_vSeries.size() - ix - 1 ];
      datum = &(*m_vIterator);
    }
  }
  return datum;
}

template<class T> T* CTimeSeries<T>::operator []( size_t ix ) {
  T* datum = NULL;
  if ( ix < m_vSeries.size() ) {
    m_vIterator = m_vSeries.begin() + ix;
    datum = &(*m_vIterator);
  }
  return datum;
}

template<class T> T* CTimeSeries<T>::At( size_t ix ) {
  T* datum = NULL;
  if ( ix < m_vSeries.size() ) {
    m_vIterator = m_vSeries.begin() + ix;
    datum = &(*m_vIterator);
  }
  return datum;
}

template<class T> T* CTimeSeries<T>::At( const ptime &dt ) {
  // assumes sorted vector
  // TODO: Check that this is correct
  T key( dt );
  T* datum = NULL;
  std::pair<std::vector<T>::iterator, std::vector<T>::iterator> p;
  p = equal_range( m_vSeries.begin(), m_vSeries.end(), key );
  if ( p.first != p.second ) {
    m_vIterator = p.first;
    datum = &(*m_vIterator);
  }
  return datum;
}

template<class T> typename std::vector<T>::iterator CTimeSeries<T>::iterAt( const ptime &time ) {
  T key( time );
  std::pair<std::vector<T>::iterator, std::vector<T>::iterator> p;
  p = equal_range( m_vSeries.begin(), m_vSeries.end(), key );
  return ( p.first != p.second ) ? *(p.first) : m_vSeries.end();
}

template<class T> T* CTimeSeries<T>::AtOrAfter( const ptime &dt ) {
  // TODO:  Check that this is correct
  T key( dt );
  T *datum = NULL;
  std::pair<std::vector<T>::iterator, std::vector<T>::iterator> p;
  p = equal_range( m_vSeries.begin(), m_vSeries.end(), key );
  if ( p.first == p.second ) {
    if ( m_vSeries.end() != p.first ) {
      m_vIterator = p.first;
      datum = &(*m_vIterator);
    }
  }
  return datum;
}

template<class T> typename std::vector<T>::iterator CTimeSeries<T>::iterAtOrAfter( const ptime &time ) {
  T key( time );
  std::pair<std::vector<T>::iterator, std::vector<T>::iterator> p;
  p = std::equal_range( m_vSeries.begin(), m_vSeries.end(), key );
  return p.first;
}

template<class T> T *CTimeSeries<T>::After( const ptime &dt ) {
  // assumes sorted vector
  // TODO: Check that this is correct
  T key( dt );
  T *datum = NULL;
  std::pair<std::vector<T>::iterator, std::vector<T>::iterator> p;
  p = equal_range( m_vSeries.begin(), m_vSeries.end(), key );
  if ( m_vSeries.end() != p.second ) {
    m_vIterator = p.second;
    datum = &(*m_vIterator);
  }
  return datum;
}

template<class T> void CTimeSeries<T>::Sort( void ) {
  sort( m_vSeries.begin(), m_vSeries.end() );  // may not keep time series with identical keys in acquired order
}

template<class T> CTimeSeries<T>* CTimeSeries<T>::Subset( const ptime &dt ) {
  T datum( dt );
  CTimeSeries<T> *series = NULL;
  std::vector<T>::iterator iter;
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

template<class T> CTimeSeries<T>* CTimeSeries<T>::Subset( const ptime &dt, unsigned int n ) { // n is max count
  T datum( dt );
  CTimeSeries<T> *series = NULL;
  std::vector<T>::iterator iter;
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

template<class T> H5::DataSpace* CTimeSeries<T>::DefineDataSpace( H5::DataSpace *pSpace ) {
  if ( NULL == pSpace ) pSpace = new H5::DataSpace( H5S_SIMPLE );
  hsize_t curSize = m_vSeries.size();
  hsize_t maxSize = H5S_UNLIMITED; 
  if ( 0 != curSize ) {
    pSpace->setExtentSimple( 1, &curSize, &maxSize ); 
  }
  else {
    //throw runtime_error( "CTimeSeries<T>::DefineDataSpace series is empty" );
    cout << "CTimeSeries<T>::DefineDataSpace series is empty" << endl;
  }
  return pSpace; 
}

// Bars

class CBars: public CTimeSeries<CBar> {
public:
  CBars(void);
  CBars( size_t );
  virtual ~CBars(void);
  CBars *Subset( ptime time ) { return (CBars *) CTimeSeries<CBar>::Subset( time ); };
  CBars *Subset( ptime time, unsigned int n ) { return (CBars *) CTimeSeries<CBar>::Subset( time, n ); };
protected:
private:
};

// Trades

class CTrades: public CTimeSeries<CTrade> {
public:
  CTrades( void );
  CTrades( size_t );
  ~CTrades( void );
  CTrades *Subset( ptime time ) { return (CTrades *) CTimeSeries<CTrade>::Subset( time ); };
  CTrades *Subset( ptime time, unsigned int n ) { return (CTrades *) CTimeSeries<CTrade>::Subset( time, n ); };
protected:
private:
};

// Quotes

class CQuotes: public CTimeSeries<CQuote> {
public:
  CQuotes( void );
  CQuotes( size_t );
  ~CQuotes( void );
  CQuotes *Subset( ptime time ) { return (CQuotes *) CTimeSeries<CQuote>::Subset( time ); };
  CQuotes *Subset( ptime time, unsigned int n ) { return (CQuotes *) CTimeSeries<CQuote>::Subset( time, n ); };
protected:
private:
};

// MarketDepth

class CMarketDepths: public CTimeSeries<CMarketDepth> {
public:
  CMarketDepths( void );
  CMarketDepths( size_t );
  ~CMarketDepths( void );
  CMarketDepths *Subset( ptime time ) { return (CMarketDepths *) CTimeSeries<CMarketDepth>::Subset( time ); };
  CMarketDepths *Subset( ptime time, unsigned int n ) { return (CMarketDepths *) CTimeSeries<CMarketDepth>::Subset( time, n ); };
protected:
private:
};

