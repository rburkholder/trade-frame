/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

#include <deque>

#include <TFTimeSeries/DatedDatum.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

//
// ObjectAtTime
// Has a time reference, and holds an object related to the time reference
//

template<typename T>
class ObjectAtTime {
public:
  ObjectAtTime<T>( ptime dt, T* object );
  virtual ~ObjectAtTime<T>();
  ptime getDateTime() { return m_dt; }
  T* getObject() { return m_object; }
protected:
  ptime m_dt;
  T* m_object;
private:
};

template<typename T>
ObjectAtTime<T>::ObjectAtTime( boost::posix_time::ptime dt, T* object ) {
  m_dt = dt;
  m_object = object;
}

template<typename T>
ObjectAtTime<T>::~ObjectAtTime() {
  m_object = nullptr;
  // held object is released elsewhere
}

//
// SlidingWindow
// Holds a series of objects based upon minimizing a time window or a count window
//  ie, any excess objects or objects outside of the time window are removed
// Assumes objects are added in forward chronological order
//

template<typename T>
class SlidingWindow {
public:
  // when both are zero, then do no windowing, should actually raise an exception
  SlidingWindow<T>(size_t WindowSizeSeconds = 0, size_t WindowSizeCount = 0);
  virtual ~SlidingWindow<T>(void);

  // Which ever makes the shortest window takes precedence, both can be non-zero simultaneously
  void SetSlidingWindowSeconds( size_t );
  size_t GetSlidingWindowSeconds() { return m_nWindowSizeSeconds; };
  void SetSlidingWindowCount( size_t );
  size_t GetSlidingWindowCount() { return m_nWindowSizeCount; };

  T* Add( ptime t, T* object );
  T* UndoPush();
  virtual T* Remove();  // inheritor needs to ensure destruction of held object
  void UpdateWindow();
  size_t Count();

  T* First();
  T* Next();

protected:
  // put in lock variable, method?

  size_t m_nWindowSizeCount;
  size_t m_nWindowSizeSeconds;
  time_duration m_tdWindowWidth;
  ptime m_dtLast;

  std::deque<ObjectAtTime<T>*> m_qT;
  typename std::deque<ObjectAtTime<T>*>::iterator iter;

private:
};

template<typename T>
SlidingWindow<T>::SlidingWindow( size_t nWindowSizeSeconds, size_t nWindowSizeCount)
: m_nWindowSizeCount( nWindowSizeCount )
, m_nWindowSizeSeconds( nWindowSizeSeconds )
, m_tdWindowWidth( seconds( nWindowSizeSeconds ) )
{
  //if ( ( 0 == nWindowSizeSeconds ) && ( 0 == nWindowSizeCount ) ) {
  //  throw std::runtime_error( "WindowSize (seconds) and WindowSize (count) cannot both be zero" );
  //}  // can't do this as many things construct with 0 window then set parameters later
}

template<typename T>
SlidingWindow<T>::~SlidingWindow() {
  while ( !m_qT.empty() ) {
    //delete m_qT.front();
    //m_qT.pop_front();
    Remove(); // this ensures inheritor has a chance to delete the held object
  }
}

template<typename T>
size_t SlidingWindow<T>::Count() {
  return m_qT.size();
}

template<typename T>
void SlidingWindow<T>::SetSlidingWindowSeconds( size_t nWindowSizeSeconds) {
  m_nWindowSizeSeconds = nWindowSizeSeconds;
  m_tdWindowWidth = seconds(nWindowSizeSeconds);
}

template<typename T>
void SlidingWindow<T>::SetSlidingWindowCount( size_t nWindowSizeCount) {
  m_nWindowSizeCount = nWindowSizeCount;
}

template<typename T>
T* SlidingWindow<T>::First() {
  iter = m_qT.begin();
  return ( m_qT.end() == iter ) ? NULL : (*iter)->getObject();
}

template<typename T>
T* SlidingWindow<T>::Next() {
  iter++;
  return ( m_qT.end() == iter ) ? NULL : (*iter)->getObject();
}

template<typename T>
T* SlidingWindow<T>::Add(boost::posix_time::ptime dt, T *object) {
  m_qT.push_back( new ObjectAtTime<T>( dt, object ) );
  m_dtLast = dt;
  return object;
}

template<typename T>
T* SlidingWindow<T>::UndoPush() {
  ObjectAtTime<T> *oat = NULL;
  T* object = NULL;
  if ( !m_qT.empty() ) {
    oat = m_qT.back();
    object = oat->getObject();
    delete oat;
    m_qT.pop_back();
    //m_dtLast = dt;
  }
  return object;
}

template<typename T>
T* SlidingWindow<T>::Remove() {
  ObjectAtTime<T> *oat = NULL;
  T* object = NULL;
  if ( !m_qT.empty() ) {
    oat = m_qT.front();
    m_qT.pop_front();
    object = oat->getObject();
    delete oat;
  }
  return object;
}

template<typename T>
void SlidingWindow<T>::UpdateWindow() {
  if ( !m_qT.empty() ) {
    // Time Based Decimation
    if ( 0 != m_nWindowSizeSeconds ) {
      ptime dtPurgePrior = m_dtLast - m_tdWindowWidth;
      ptime dtFirst;
      bool bDone = false;

      while ( !bDone ) {
        dtFirst = m_qT.front()->getDateTime();
        if ( dtFirst < dtPurgePrior ) {
          Remove();
          if ( m_qT.empty() ) bDone = true;
        }
        else {
          bDone = true;
        }
      }
    }
    // Size Based Decimation
    if ( 0 != m_nWindowSizeCount ) {
      while ( m_nWindowSizeCount < m_qT.size() ) {
        Remove();
      }
    }
  }
}

// ================ SlidingWindowBars =================

class SlidingWindowBars: public SlidingWindow<Bar> {
public:
  SlidingWindowBars( size_t WindowSizeSeconds = 0, size_t WindowSizeCount = 0 );
  virtual ~SlidingWindowBars();
  virtual Bar* Remove();
protected:
private:
};

} // namespace tf
} // namespace ou
