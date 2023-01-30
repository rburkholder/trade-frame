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

#include <TFTimeSeries/TimeSeries.h>

// Each carrier holds a TimeSeries.  The carrier holds an index to the current DatedDatum in each TimeSeries.
// The current DatedDatum timestamp is maintained for the merge process to figure out which DatedDatum to
// send into the merge process

namespace ou { // One Unified
namespace tf { // TradeFrame

// MergeCarrierBase

class MergeCarrierBase {
  friend class MergeDatedDatums;
public:
  using OnDatumHandler = FastDelegate1<const DatedDatum &>;
  MergeCarrierBase() {};
  virtual ~MergeCarrierBase() {};
  virtual void ProcessDatum()
    { throw std::runtime_error( "ProcessDatum not defined" ); };
  virtual void Reset()
    { throw std::runtime_error( "Reset not defined" ); };
  inline const ptime &GetDateTime() { return m_dt; };
  const DatedDatum* GetDatedDatum() const { return m_pDatum; };
  bool operator<( const MergeCarrierBase& other ) const { return m_dt < other.m_dt; };
  bool operator<( const MergeCarrierBase* pOther ) const { return m_dt < pOther->m_dt; };
  static bool lt( MergeCarrierBase* plhs, MergeCarrierBase *prhs ) { return plhs->m_dt < prhs->m_dt; };
protected:
  ptime m_dt;  // datetime of datum to be merged (used in comparison)
  const DatedDatum* m_pDatum;
  OnDatumHandler OnDatum;
private:
};

// MergeCarrier

template<class T> // T is a DatedDatum type
class MergeCarrier: public MergeCarrierBase {
  friend class MergeDatedDatums;
public:
  MergeCarrier<T>( TimeSeries<T>& series, OnDatumHandler function );
  virtual ~MergeCarrier<T>();
  void ProcessDatum();
  void Reset();
protected:
  TimeSeries<T>& m_series;  // series from which a datum is to be merged to output
private:
};

template<class T>
MergeCarrier<T>::MergeCarrier( TimeSeries<T>& series, OnDatumHandler function )
  : MergeCarrierBase(), m_series( series )
{
  assert( 0 != m_series.Size() );
  OnDatum = function;
  m_pDatum = m_series.First();  // preload with first datum so we have it's time available for comparison
  m_dt = ( 0 == m_pDatum )
    ? boost::date_time::special_values::not_a_date_time
    : m_pDatum->DateTime();
}

template<class T>
MergeCarrier<T>::~MergeCarrier() {
}

template<class T>
void MergeCarrier<T>::ProcessDatum() {
  if ( ou::TimeSource::LocalCommonInstance().GetSimulationMode() ) {
    ou::TimeSource::LocalCommonInstance().SetSimulationTime( m_pDatum->DateTime() );
  }
  if ( nullptr != OnDatum )
    OnDatum( *m_pDatum );
  m_pDatum = m_series.Next();
  m_dt = ( nullptr == m_pDatum )
    ? boost::date_time::special_values::not_a_date_time
    : m_pDatum->DateTime();
}

template<class T>
void MergeCarrier<T>::Reset() {
  m_pDatum = m_series.First();  // preload with first datum so we have it's time available for comparison
  m_dt = ( 0 == m_pDatum )
    ? boost::date_time::special_values::not_a_date_time
    : m_pDatum->DateTime();
}

} // namespace tf
} // namespace ou
