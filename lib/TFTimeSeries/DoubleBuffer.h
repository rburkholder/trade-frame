/************************************************************************
 * Copyright(c) 2017, One Unified. All rights reserved.                 *
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

// Started 2013/07/26

#ifndef DOUBLEBUFFER_H
#define DOUBLEBUFFER_H

#include <vector>
#include <queue>

#include <boost/thread/mutex.hpp>
//#include <boost/thread/lock_types.hpp>
#include <boost/thread/locks.hpp>

namespace ou { // One Unified
namespace tf { // TradeFrame

template<typename TS>
class DoubleBufferRef {
public:
  typedef typename TS::datum_t datum_t;
  DoubleBufferRef( TS& tsBackground, TS& tsForeground );
  virtual ~DoubleBufferRef();
  void Append( const datum_t& );
  void Sync( void );
protected:
private:
  boost::mutex m_mutex;
  TS& m_tsInbound; // inbound time series via background thread
  TS& m_tsBatched; // syncs to inbound when needed and used in other foreground threads
};

template<typename TS>
DoubleBufferRef<TS>::DoubleBufferRef( TS& tsBackground, TS& tsForeground )
: m_tsInbound( tsBackground ), m_tsBatched( tsForeground )
{
}

template<typename TS>
void DoubleBufferRef<TS>::Append( const datum_t& datum ) {
  boost::lock_guard<boost::mutex> guard(m_mutex);
  m_tsInbound.Append( datum );
}

template<typename TS>
void DoubleBufferRef<TS>::Sync( void ) {
  boost::lock_guard<boost::mutex> guard(m_mutex);
  while ( m_tsInbound.Size() > m_tsBatched.Size() ) {
    m_tsBatched.Append( m_tsInbound[ m_tsBatched.Size() ] );
  }
}

//
// =================
//

template<typename datum_t>
class DoubleBuffer {
public:
  typedef std::vector<datum_t> vDatum_t;
  typedef typename vDatum_t::size_type size_type;
  DoubleBuffer( void );
  virtual ~DoubleBuffer() {}
  
  void Append( const datum_t& ); // sync'd
  size_type Sync( void ); // uses lock
  void Reserve( size_type nSize ); // sync'd
  void Clear( void ); // sync'd
  size_type Size( void ); // sync'd
  
  const datum_t* GetRef( void ) const;  // not sync'd
  const datum_t* operator[]( size_type ix ) const; // not sync'd
  const vDatum_t& GetVector( void ) const { return m_tsBatched; } // not sync'd
protected:
private:
  boost::mutex m_mutex;
  vDatum_t m_tsInbound; // inbound time series via background thread
  vDatum_t m_tsBatched; // syncs to inbound when needed and used in other foreground threads
};

template<typename datum_t>
DoubleBuffer<datum_t>::DoubleBuffer( void ) {}

template<typename datum_t>
void DoubleBuffer<datum_t>::Append( const datum_t& datum ) {
  boost::lock_guard<boost::mutex> guard(m_mutex);
  m_tsInbound.push_back( datum );
}

template<typename datum_t>
void DoubleBuffer<datum_t>::Clear( void ) {
  boost::lock_guard<boost::mutex> guard(m_mutex);
  m_tsInbound.clear();
  m_tsBatched.clear();
}

template<typename datum_t>
typename DoubleBuffer<datum_t>::size_type DoubleBuffer<datum_t>::Sync( void ) {
  boost::lock_guard<boost::mutex> guard(m_mutex);
  while ( m_tsInbound.size() > m_tsBatched.size() ) {
    m_tsBatched.push_back( m_tsInbound[ m_tsBatched.size() ] );
  }
  return m_tsBatched.size();
}

template<typename datum_t>
typename DoubleBuffer<datum_t>::size_type DoubleBuffer<datum_t>::Size( void ) {
  return Sync();
}

template<typename datum_t>
const datum_t* DoubleBuffer<datum_t>::GetRef( void ) const {
  //Sync();
  return &m_tsBatched[0];
}

template<typename datum_t>
const datum_t* DoubleBuffer<datum_t>::operator[]( size_type ix ) const {
  //Sync();
  return &m_tsBatched[ ix ];
}

template<typename datum_t>
void DoubleBuffer<datum_t>::Reserve( size_type nSize ) {
  boost::lock_guard<boost::mutex> guard(m_mutex);
  assert( nSize >= m_tsInbound.size() );
  assert( nSize >= m_tsBatched.size() );
  m_tsInbound.reserve( nSize );
  m_tsBatched.reserve( nSize );
}

//
// =================
//

template<typename datum_t>
class Queue {
  typedef std::queue<datum_t> qDatum_t;
public:
  Queue( void ) {}
  virtual ~Queue() {}
  
  void Append( const datum_t& datum ) {
    boost::lock_guard<boost::mutex> guard(m_mutex);
    m_qDatum.push( datum );
  }
  
  template<typename Function>
  void Sync( Function f ) {
    boost::lock_guard<boost::mutex> guard(m_mutex);
    f( m_qDatum.front() );
    m_qDatum.pop();
  }
  
protected:
private:
  boost::mutex m_mutex;
  qDatum_t m_qDatum;
};

} // namespace tf
} // namespace ou

#endif /* DOUBLEBUFFER_H */

