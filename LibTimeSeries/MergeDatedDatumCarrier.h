#pragma once

#include "TimeSeries.h"
#include "TimeSource.h"

#include "FastDelegate.h"
using namespace fastdelegate;

#include <stdexcept>

class CMergeCarrierBase {
  friend class CMergeDatedDatums;
public:
  typedef FastDelegate1<const CDatedDatum &> OnDatumHandler;
  CMergeCarrierBase( void ) {};
  virtual ~CMergeCarrierBase( void ) {};
  virtual void ProcessDatum( void ) { throw std::runtime_error( "ProcessDatum not instantiated" ); };
  virtual void Reset( void ) { throw std::runtime_error( "Reset not instantiated" ); };
  const ptime &GetDateTime( void ) { return m_dt; };
  CDatedDatum *GetDatedDatum( void ) { return m_pDatum; };
protected:
  ptime m_dt;  // datetime of datum to be merged (used in comparison)
  CDatedDatum *m_pDatum;
  OnDatumHandler OnDatum;
  CTimeSource m_timesource;
private:
};

template<class T> class CMergeCarrier: public CMergeCarrierBase {
  // T is a DatedDatum type
  friend class CMergeDatedDatums;
public:
  CMergeCarrier<T>( CTimeSeries<T> *pSeries, OnDatumHandler function );
  virtual ~CMergeCarrier<T>( void );
  virtual void ProcessDatum( void );
  virtual void Reset( void );
protected:
  CTimeSeries<T> *m_pSeries;  // series from which a datum is to be merged to output
private:
};

template<class T> CMergeCarrier<T>::CMergeCarrier( CTimeSeries<T> *pSeries, OnDatumHandler function ) : CMergeCarrierBase() {
  
  m_pSeries = pSeries;
  OnDatum = function;
  m_pDatum = m_pSeries->First();  // preload with first datum so we have it's time available for comparison
  m_dt = ( NULL == m_pDatum ) 
    ? boost::date_time::special_values::not_a_date_time 
    : m_pDatum->m_dt;
}

template<class T> CMergeCarrier<T>::~CMergeCarrier() {
}

template<class T> void CMergeCarrier<T>::ProcessDatum(void) {
  //static int i = 1000;
  //if ( i > 0 ) {
  //  std::cout << "#" << m_pDatum->m_dt << std::endl;
  //  --i;
  //}
  if ( CTimeSource::GetSimulationMode() ) {
    CTimeSource::SetSimulationTime( m_pDatum->m_dt );
  }
  if ( NULL != OnDatum ) OnDatum( *m_pDatum );
  m_pDatum = m_pSeries->Next();
  m_dt = ( NULL == m_pDatum ) 
    ? boost::date_time::special_values::not_a_date_time 
    : m_pDatum->m_dt;
}

template<class T> void CMergeCarrier<T>::Reset() {
  m_pDatum = m_pSeries->First();  // preload with first datum so we have it's time available for comparison
  m_dt = ( NULL == m_pDatum ) 
    ? boost::date_time::special_values::not_a_date_time 
    : m_pDatum->m_dt;
}