/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#include "Option.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

class Strike {
public:

  typedef ou::tf::ProviderInterfaceBase::pProvider_t pProvider_t;

  Strike();  // for construction in std::Map
  Strike( double dblStrike );
  Strike( const Strike& rhs );
  ~Strike();

  Strike& operator=( const Strike& rhs );

  bool operator< ( const Strike& rhs ) const { return m_dblStrike <  rhs.m_dblStrike; };
  bool operator<=( const Strike& rhs ) const { return m_dblStrike <= rhs.m_dblStrike; };

  double GetStrike() const { return m_dblStrike; };

  void AssignCall( Instrument::pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider );
  void AssignPut( Instrument::pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider );

  void AssignCall( Instrument::pInstrument_t pInstrument, pProvider_t pDataProvider );
  void AssignPut( Instrument::pInstrument_t pInstrument, pProvider_t pDataProvider );

  ou::tf::option::Call* Call() { return m_call.get(); };
  ou::tf::option::Put*  Put()  { return m_put.get(); };

  void SetWatchableOn();  // watchable defaults to off at time of construction
  void SetWatchableOff();

  void WatchStart(); // not started if watchable is off
  void WatchStop();
  bool IsWatching( void ) const { return 0 != m_nWatching; };

  void SaveSeries( const std::string& sPrefix );
  void EmitValues();

protected:

  boost::shared_ptr<ou::tf::option::Call> m_call;
  boost::shared_ptr<ou::tf::option::Put>  m_put;

private:

  bool m_bWatchable;  // for when large number of strikes, but only a few to watch and collect
  unsigned int m_nWatching;  // 0 = no watch, > 0 watching, < 0 illegal
  double m_dblStrike;
};

} // namespace option
} // namespace tf
} // namespace ou

