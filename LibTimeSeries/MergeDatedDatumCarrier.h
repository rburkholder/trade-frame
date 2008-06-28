#pragma once

#include "TimeSeries.h"

#include "FastDelegate.h"
using namespace fastdelegate;

class CMergeCarrierBase {
};

template<class T> class CMergeCarrier: public class CMergeCarrierBase {
  // T is a DatedDatum type
  friend class CMergeDatedDatums;
public:
  typedef FastDelegate1<const T &> OnDatumHandler;

  CMergeCarrier<T>( CTimeSeries<T> *pSeries, OnDatumHandler function );
  virtual ~CMergeCarrier<T>( void );

  void ProcessDatum( void );
protected:
  ptime m_dt;  // datetime of datum to be merged (used in comparison)
  CTimeSeries<T> *m_pSeries;  // series from which a datum is to be merged to output
  T *m_pDatum;
  OnDatumHandler OnDatum;
private:
};


//
// CMergeCarrier
//

template<class T> CMergeCarrier<T>::CMergeCarrier( CTimeSeries<T> *pSeries, OnDatumHandler function ) {
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
  if ( NULL != OnDatum ) OnDatum( *m_pDatum );
  m_pDatum = m_pSeries->Next();
  m_dt = ( NULL == m_pDatum ) 
    ? boost::date_time::special_values::not_a_date_time 
    : m_pDatum->m_dt;
}
