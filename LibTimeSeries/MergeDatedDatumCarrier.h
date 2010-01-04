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

#include <stdexcept>

#include "LibCommon/FastDelegate.h"
using namespace fastdelegate;

#include "LibCommon/TimeSource.h"

#include "TimeSeries.h"

// Each carrier holds a CTimeSeries.  The carrier holds an index to the current DatedDatum in each CTimeSeries.
// The current DatedDatum timestamp is maintained for the merge process to figure out which DatedDatum to 
// send into the merge process

class CMergeCarrierBase {
  friend class CMergeDatedDatums;
public:
  typedef FastDelegate1<const CDatedDatum &> OnDatumHandler;
  CMergeCarrierBase( void ) {};
  virtual ~CMergeCarrierBase( void ) {};
  virtual void ProcessDatum( void ) { throw std::runtime_error( "ProcessDatum not instantiated" ); };
  virtual void Reset( void ) { throw std::runtime_error( "Reset not instantiated" ); };
  inline const ptime &GetDateTime( void ) { return m_dt; };
  CDatedDatum *GetDatedDatum( void ) { return m_pDatum; };
  bool operator<( const CMergeCarrierBase &other ) const { return m_dt < other.m_dt; };
  bool operator<( const CMergeCarrierBase *pOther ) const { return m_dt < pOther->m_dt; };
  static bool lt( CMergeCarrierBase *plhs, CMergeCarrierBase *prhs ) { return plhs->m_dt < prhs->m_dt; };
protected:
  ptime m_dt;  // datetime of datum to be merged (used in comparison)
  CDatedDatum *m_pDatum;
  OnDatumHandler OnDatum;
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
