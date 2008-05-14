#pragma once

//#include <queue>
#include <deque>
#include <stdexcept>
using namespace std;

#include "boost/date_time/posix_time/posix_time.hpp"
using namespace boost::posix_time;
using namespace boost::gregorian;

#include "DatedDatum.h"

//
// CObjectAtTime
// Has a time reference, and holds an object related to the time reference
//

template<class T> class CObjectAtTime {
public:
  CObjectAtTime<T>( ptime dt, T *object );
  virtual ~CObjectAtTime<T>(void);
  ptime getDateTime(void) { return m_dt; };
  T *getObject(void) { return m_object; };
protected:
  ptime m_dt;
  T *m_object;
private:
};

template<class T> CObjectAtTime<T>::CObjectAtTime(boost::posix_time::ptime dt, T *object) {
  m_dt = dt;
  m_object = object;
}

template<class T> CObjectAtTime<T>::~CObjectAtTime(void) {
  m_object = NULL;
  // held object is released elsewhere
}

//
// CSlidingWindow
// Holds a series of objects based upon minimizing a time window or a count window
//  ie, any excess objects or objects outside of the time window are removed
// Assumes objects are added in forward chronological order
//

template<class T> class CSlidingWindow {
public:
  // when both are zero, then do no windowing, should actually raise an exception
  CSlidingWindow<T>(long WindowSizeSeconds = 0, long WindowSizeCount = 0);
  virtual ~CSlidingWindow<T>(void);

  // Which ever makes the shortest window takes precedence, both can be non-zero simultaneously
  void SetSlidingWindowSeconds( long );
  long GetSlidingWindowSeconds( void ) { return m_nWindowSizeSeconds; };
  void SetSlidingWindowCount( long );
  long GetSlidingWindowCount( void ) { return m_nWindowSizeCount; };

  T *Add( ptime t, T *object );
  T *UndoPush( void );
  virtual T *Remove( void );  // inheritor needs to ensure destruction of held object
  void UpdateWindow();
  size_t Count();

  T *First();
  T *Next();

protected:
  // put in lock variable, method?

  long m_nWindowSizeCount;
  long m_nWindowSizeSeconds;
  time_duration m_tdWindowWidth;
  ptime m_dtLast;

  //queue<CObjectAtTime<T> *> m_qT;
  deque<CObjectAtTime<T> *> m_qT;
  typename deque<CObjectAtTime<T> *>::iterator iter;

private:
};

template<class T> CSlidingWindow<T>::CSlidingWindow(long nWindowSizeSeconds, long nWindowSizeCount) {
  //if ( ( 0 == nWindowSizeSeconds ) && ( 0 == nWindowSizeCount ) ) {
  //  throw std::runtime_error( "WindowSize (seconds) and WindowSize (count) cannot both be zero" );
  //}  // can't do this as many things construct with 0 window then set parameters later
  m_nWindowSizeCount = nWindowSizeCount;
  m_nWindowSizeSeconds = nWindowSizeSeconds;
  m_tdWindowWidth = seconds(nWindowSizeSeconds);
}

template<class T> CSlidingWindow<T>::~CSlidingWindow(void) {
  while ( !m_qT.empty() ) {
    //delete m_qT.front();
    //m_qT.pop_front();
    Remove(); // this ensures inheritor has a chance to delete the held object
  }
}

template<class T> size_t CSlidingWindow<T>::Count() {
  return m_qT.size();
}

template<class T> void CSlidingWindow<T>::SetSlidingWindowSeconds(long nWindowSizeSeconds) {
  m_nWindowSizeSeconds = nWindowSizeSeconds;
  m_tdWindowWidth = seconds(nWindowSizeSeconds);
}

template<class T> void CSlidingWindow<T>::SetSlidingWindowCount(long nWindowSizeCount) {
  m_nWindowSizeCount = nWindowSizeCount;
}

template<class T> T *CSlidingWindow<T>::First() {
  iter = m_qT.begin();
  return ( m_qT.end() == iter ) ? NULL : (*iter)->getObject();
}

template<class T> T *CSlidingWindow<T>::Next() {
  iter++;
  return ( m_qT.end() == iter ) ? NULL : (*iter)->getObject();
}

template<class T> T *CSlidingWindow<T>::Add(boost::posix_time::ptime dt, T *object) {
  m_qT.push_back( new CObjectAtTime<T>( dt, object ) );
  m_dtLast = dt;
  return object;
}

template<class T> T *CSlidingWindow<T>::UndoPush(void) {
  CObjectAtTime<T> *oat = NULL;
  T *object = NULL;
  if ( !m_qT.empty() ) {
    oat = m_qT.back();
    object = oat->getObject();
    delete oat;
    m_qT.pop_back();
    //m_dtLast = dt;
  }
  return object;
}

template<class T> T *CSlidingWindow<T>::Remove() {
  CObjectAtTime<T> *oat = NULL;
  T *object = NULL;
  if ( !m_qT.empty() ) {
    oat = m_qT.front();
    m_qT.pop_front();
    object = oat->getObject();
    delete oat;
  }
  return object;
}

template<class T> void CSlidingWindow<T>::UpdateWindow() {
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
      while ( m_nWindowSizeCount < (long) m_qT.size() ) {
        Remove();
      }
    }
  }
}


class CSlidingWindowBars: public CSlidingWindow<CBar> {
public:
  CSlidingWindowBars(unsigned int WindowSizeSeconds = 0, unsigned int WindowSizeCount = 0);
  virtual ~CSlidingWindowBars();
  virtual CBar *Remove( void );
protected:
private:
};