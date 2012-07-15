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

#include <stdexcept>

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

#include <OUCommon/TimeSource.h>

#include "TimeSeries.h"

// Each carrier holds a TimeSeries.  The carrier holds an index to the current DatedDatum in each TimeSeries.
// The current DatedDatum timestamp is maintained for the merge process to figure out which DatedDatum to 
// send into the merge process

namespace ou { // One Unified
namespace tf { // TradeFrame

class MergeCarrierBase {
  friend class MergeDatedDatums;
public:
  typedef FastDelegate1<const DatedDatum &> OnDatumHandler;
  MergeCarrierBase( void ) {};
  virtual ~MergeCarrierBase( void ) {};
  virtual void ProcessDatum( void ) 
    { throw std::runtime_error( "ProcessDatum not defined" ); };
  virtual void Reset( void ) 
    { throw std::runtime_error( "Reset not defined" ); };
  inline const ptime &GetDateTime( void ) { return m_dt; };
  const DatedDatum* GetDatedDatum( void ) const { return m_pDatum; };
  bool operator<( const MergeCarrierBase& other ) const { return m_dt < other.m_dt; };
  bool operator<( const MergeCarrierBase* pOther ) const { return m_dt < pOther->m_dt; };
  static bool lt( MergeCarrierBase* plhs, MergeCarrierBase *prhs ) { return plhs->m_dt < prhs->m_dt; };
protected:
  ptime m_dt;  // datetime of datum to be merged (used in comparison)
  const DatedDatum* m_pDatum;
  OnDatumHandler OnDatum;
private:
};

template<class T> class CMergeCarrier: public MergeCarrierBase {
  // T is a DatedDatum type
  friend class MergeDatedDatums;
public:
  CMergeCarrier<T>( TimeSeries<T> *pSeries, OnDatumHandler function );
  virtual ~CMergeCarrier<T>( void );
  void ProcessDatum( void );
  void Reset( void );
protected:
  TimeSeries<T> *m_pSeries;  // series from which a datum is to be merged to output
private:
};

template<class T> CMergeCarrier<T>::CMergeCarrier( TimeSeries<T> *pSeries, OnDatumHandler function ) : MergeCarrierBase() {
  assert( 0 != pSeries );
  assert( 0 != pSeries->Size() );
  m_pSeries = pSeries;
  OnDatum = function;
  m_pDatum = m_pSeries->First();  // preload with first datum so we have it's time available for comparison
  m_dt = ( NULL == m_pDatum ) 
    ? boost::date_time::special_values::not_a_date_time 
    : m_pDatum->DateTime();
}

template<class T> CMergeCarrier<T>::~CMergeCarrier() {
}

template<class T> void CMergeCarrier<T>::ProcessDatum(void) {
  if ( ou::CTimeSource::Instance().GetSimulationMode() ) {
    ou::CTimeSource::Instance().SetSimulationTime( m_pDatum->DateTime() );
  }
  if ( NULL != OnDatum ) 
    OnDatum( *m_pDatum );
  m_pDatum = m_pSeries->Next();
  m_dt = ( NULL == m_pDatum ) 
    ? boost::date_time::special_values::not_a_date_time 
    : m_pDatum->DateTime();
}

template<class T> void CMergeCarrier<T>::Reset() {
  m_pDatum = m_pSeries->First();  // preload with first datum so we have it's time available for comparison
  m_dt = ( NULL == m_pDatum ) 
    ? boost::date_time::special_values::not_a_date_time 
    : m_pDatum->DateTime();
}

} // namespace tf
} // namespace ou
