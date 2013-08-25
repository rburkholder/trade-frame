/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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
// Started 2012/07/10

#pragma once

#include <map>

#include <boost/smart_ptr.hpp>

#include <TFTrading/Watch.h>
#include "Strike.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

// don't assign underlying if more than one bundle used for the underlying, 
//   eg when using one bundle per expiry, then use separate watch for underlying

class Bundle {
public:

  typedef Instrument::pInstrument_t pInstrument_t;
  typedef ou::tf::ProviderInterfaceBase::pProvider_t pProvider_t;
  //typedef ou::tf::Watch* pWatch_t;
  typedef Watch::pWatch_t pWatch_t;

  Bundle(void);
  ~Bundle(void);

  void SetUnderlying( pInstrument_t pInstrument, pProvider_t pProvider );
  void SetCall( pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider );
  void SetPut( pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider );

  Call* GetCall( double dblStrike );
  Put* GetPut( double dblStrike );

  void AdjacentStrikes( double dblStrike, double& dblLower, double& dblUpper );

  pWatch_t GetUnderlying( void ) { return m_pwatchUnderlying; };

  void SetWatchableOn( double dblStrike );
  void SetWatchableOff( double dblStrike );

  void SetWatchOn( void ); // watch underlying plus all options
  void SetWatchOff( void ); 

  void SetWatchOn( double dblStrike ); // watch only selected call/put at strike
  void SetWatchOff( double dblStrike );

  void SaveSeries( const std::string& sPrefix );
  void EmitValues( void );

protected:
private:

  typedef std::map<double,Strike> mapStrikes_t;

  bool m_bWatching;  // single threadable only

  pWatch_t m_pwatchUnderlying;
  mapStrikes_t m_mapStrikes;

  mapStrikes_t::iterator FindStrike( double strike );
  mapStrikes_t::iterator FindStrikeAuto( double strike ); // Auto insert new strike

};

} // namespace option
} // namespace tf
} // namespace ou

