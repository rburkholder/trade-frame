#pragma once

#include "DatedDatum.h"

#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;

//#include <boost/serialization/vector.hpp>

// http://www.boost.org/libs/serialization/doc/traits.html

template<class T> class CTimeSeries {
public:
  CTimeSeries<T>(void);
  CTimeSeries<T>( unsigned int Size );
  virtual ~CTimeSeries<T>(void);

  size_t Count();
  void AppendDatum( const T &datum );
  void InsertDatum( const ptime &time, const T &datum );
  T *First();
  T *Next();
  T *Last();
  T *Ago( unsigned int ix );
  T *operator[]( unsigned int ix );
  T *At( unsigned int ix );
  T *At( const ptime &time );
  T *AtOrAfter( const ptime &time );
  T *After( const ptime &time );
  virtual CTimeSeries<T> *Subset( const ptime &time ); // from At or After to end
  virtual CTimeSeries<T> *Subset( const ptime &time, unsigned int n ); // from At or After for n T
  void Sort( void ); // use when loaded from external data
  void Flip( void ) {reverse( m_vSeries.begin(), m_vSeries.end() ); };
  void Clear( void );
  H5::DataSpace *DefineDataSpace( H5::DataSpace *pSpace = NULL );
protected:
  vector<T> m_vSeries;
  typename vector<T>::iterator m_vIterator;
private:
};

template<class T> CTimeSeries<T>::CTimeSeries(void) {
}

template<class T> CTimeSeries<T>::CTimeSeries( unsigned int size ) {
  m_vSeries.reserve( size );
}

template<class T> CTimeSeries<T>::~CTimeSeries(void) {
}

template<class T> void CTimeSeries<T>::AppendDatum(const T &datum) { // changed name 'cause VC++ doesn't do symbol results properly
  m_vSeries.push_back( datum );
  m_vIterator = m_vSeries.end() - 1;
}

template<class T> void CTimeSeries<T>::InsertDatum( const ptime &dt, const T &datum ) {
  T key( dt );
  // T *datum = NULL;  // is this needed?
  pair<vector<T>::iterator, vector<T>::iterator> p;
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

template<class T> size_t CTimeSeries<T>::Count() {
  return m_vSeries.size();
}

template<class T> T *CTimeSeries<T>::First() {
  m_vIterator = m_vSeries.begin();
  if ( m_vSeries.end() == m_vIterator ) {
    return NULL;
  }
  else {
    return &(*m_vIterator);
  }
}

template<class T> T *CTimeSeries<T>::Next() {
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

template<class T> T *CTimeSeries<T>::Last() {
  // TODO:  Can we use .back on this?
  return m_vSeries.empty() ? NULL : &m_vSeries[ m_vSeries.size() - 1 ];
}

template<class T> T *CTimeSeries<T>::Ago( unsigned int ix ) {
  T *datum = NULL;
  if ( 0 != m_vSeries.size() ) {
    if ( ix < m_vSeries.size() ) {
      m_vIterator = m_vSeries.end() - ix;
      //datum = &m_vSeries[ m_vSeries.size() - ix - 1 ];
      datum = &(*m_vIterator);
    }
  }
  return datum;
}

template<class T> T *CTimeSeries<T>::operator []( unsigned int ix ) {
  T *datum = NULL;
  if ( ix < m_vSeries.size() ) {
    m_vIterator = m_vSeries.begin() + ix;
    datum = &(*m_vIterator);
  }
  return datum;
}

template<class T> T *CTimeSeries<T>::At( unsigned int ix ) {
  T *datum = NULL;
  if ( ix < m_vSeries.size() ) {
    m_vIterator = m_vSeries.begin() + ix;
    datum = &(*m_vIterator);
  }
  return datum;
}

template<class T> T *CTimeSeries<T>::At( const ptime &dt ) {
  // assumes sorted vector
  // TODO: Check that this is correct
  T key( dt );
  T *datum = NULL;
  pair<vector<T>::iterator, vector<T>::iterator> p;
  p = equal_range( m_vSeries.begin(), m_vSeries.end(), key );
  if ( p.first != p.second ) {
    m_vIterator = p.first;
    datum = &(*m_vIterator);
  }
  return datum;
}

template<class T> T *CTimeSeries<T>::AtOrAfter( const ptime &dt ) {
  // TODO:  Check that this is correct
  T key( dt );
  T *datum = NULL;
  pair<vector<T>::iterator, vector<T>::iterator> p;
  p = equal_range( m_vSeries.begin(), m_vSeries.end(), key );
  if ( p.first == p.second ) {
    if ( m_vSeries.end() != p.first ) {
      m_vIterator = p.first;
      datum = &(*m_vIterator);
    }
  }
  return datum;
}

template<class T> T *CTimeSeries<T>::After( const ptime &dt ) {
  // assumes sorted vector
  // TODO: Check that this is correct
  T key( dt );
  T *datum = NULL;
  pair<vector<T>::iterator, vector<T>::iterator> p;
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

//template<class T> void CTimeSeries<T>::Flip() {
  //std::vector<T>::iterator iterBegin = m_vSeries.begin();
  //std::vector<T>::iterator iterEnd = m_vSeries.end();
  //iterEnd--;

  //T datum;
  //while ( iterBegin < iterEnd ) {
  //  datum = *iterBegin;
  //  *iterBegin = *iterEnd;
  //  *iterEnd = datum;
  //  --iterEnd;
  //  ++iterBegin;
  //}

  //reverse( m_vSeries.begin(), m_vSeries.end() );
//}

template<class T> CTimeSeries<T> * CTimeSeries<T>::Subset( const ptime &dt ) {
  T datum( dt );
  CTimeSeries<T> *series = NULL;
  vector<T>::iterator iter;
  iter = lower_bound( m_vSeries.begin(), m_vSeries.end(), datum );
  if ( m_vSeries.end() != iter ) {
    series = new CTimeSeries<T>( (unsigned int) (m_vSeries.end() - iter) );
    while ( m_vSeries.end() != iter ) {
      series->AppendDatum( *iter );
      ++iter;
    }
  }
  else {
    series = new CTimeSeries<T>();
  }
  return series;
}

template<class T> CTimeSeries<T> * CTimeSeries<T>::Subset( const ptime &dt, unsigned int n ) { // n is max count
  T datum( dt );
  CTimeSeries<T> *series = NULL;
  vector<T>::iterator iter;
  iter = lower_bound( m_vSeries.begin(), m_vSeries.end(), datum );
  if ( m_vSeries.end() != iter ) {
    unsigned int todo = min( n, (unsigned int) ( m_vSeries.end() - iter ) );
    series = new CTimeSeries<T>( todo );
    while ( 0 < todo ) {
      series->AppendDatum( *iter );
      ++iter;
      --todo;
    }
  }
  else {
    series = new CTimeSeries<T>();
  }
  return series;
}

template<class T> H5::DataSpace *CTimeSeries<T>::DefineDataSpace( H5::DataSpace *pSpace ) {
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
  CBars( unsigned int );
  virtual ~CBars(void);
  CBars *Subset( ptime time ) { return (CBars *) CTimeSeries<CBar>::Subset( time ); };
  CBars *Subset( ptime time, unsigned int n ) { return (CBars *) CTimeSeries<CBar>::Subset( time, n ); };
protected:
private:
};

// Trades

class CTrades: public CTimeSeries<CTrade> {
public:
  CTrades(void);
  CTrades( unsigned int );
  virtual ~CTrades(void);
  CTrades *Subset( ptime time ) { return (CTrades *) CTimeSeries<CTrade>::Subset( time ); };
  CTrades *Subset( ptime time, unsigned int n ) { return (CTrades *) CTimeSeries<CTrade>::Subset( time, n ); };
protected:
private:
};

// Quotes

class CQuotes: public CTimeSeries<CQuote> {
public:
  CQuotes(void);
  CQuotes( unsigned int );
  virtual ~CQuotes(void);
  CQuotes *Subset( ptime time ) { return (CQuotes *) CTimeSeries<CQuote>::Subset( time ); };
  CQuotes *Subset( ptime time, unsigned int n ) { return (CQuotes *) CTimeSeries<CQuote>::Subset( time, n ); };
protected:
private:
};

// MarketDepth

class CMarketDepths: public CTimeSeries<CMarketDepth> {
public:
  CMarketDepths(void);
  CMarketDepths( unsigned int );
  virtual ~CMarketDepths(void);
  CMarketDepths *Subset( ptime time ) { return (CMarketDepths *) CTimeSeries<CMarketDepth>::Subset( time ); };
  CMarketDepths *Subset( ptime time, unsigned int n ) { return (CMarketDepths *) CTimeSeries<CMarketDepth>::Subset( time, n ); };
protected:
private:
};

