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

#include <TFTrading/NoRiskInterestRateSeries.h>
#include <TFOptions/Binomial.h>
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
  typedef Watch::pWatch_t pWatch_t;

  Bundle(void);
  ~Bundle(void);

  void SetUnderlying( pInstrument_t pInstrument, pProvider_t pProvider );
  void SetCall( pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider );
  void SetPut( pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider );

  Call* GetCall( double dblStrike );
  Put* GetPut( double dblStrike );

  void AdjacentStrikes( double dblValue, double& dblLower, double& dblUpper );  // uses <= and >= logic around dblStrike, therefore possibility of dblLower = dblUpper

  void UpdateATMWatch( double dblValue );

  pWatch_t GetUnderlying( void ) { return m_pwatchUnderlying; };

  void SetWatchableOn( double dblStrike );  // each strike is not watcheable by default
  void SetWatchableOff( double dblStrike );

  //void SetWatchUnderlyingOn( void );
  //void SetWatchUnderlyingOff( void );

  void SetWatchOn( double dblStrike, bool bForce = false ); // watch only selected call/put at strike, force watchable on
  void SetWatchOff( double dblStrike, bool bForce = false ); // forces watchable off when true

  void SetWatchOn( void ); // watch underlying plus all watcheable options
  void SetWatchOff( void ); 

  void SaveSeries( const std::string& sPrefix );
  void EmitValues( void );

  void SetExpiry( ptime dt ); // utc

  void CalcGreeks( double dblUnderlying, double dblVolHistorical, ptime now, ou::tf::LiborFromIQFeed& libor );

protected:
private:

  typedef std::map<double,Strike> mapStrikes_t;
  typedef mapStrikes_t::iterator mapStrikes_iter_t;

  enum EOptionWatchState { EOWSNoWatch, EOWSWatching } m_stateOptionWatch;

  bool m_bWatching;  // single threadable only

  ptime m_dtExpiry;  // eg, 4pm EST third Fri of month for normal US equity options, in utc

  pWatch_t m_pwatchUnderlying;
  mapStrikes_t m_mapStrikes;

  mapStrikes_iter_t m_iterUpper;
  mapStrikes_iter_t m_iterMid;
  mapStrikes_iter_t m_iterLower;

  double m_dblUpperTrigger;
  double m_dblLowerTrigger;

  mapStrikes_t::iterator FindStrike( double strike );
  mapStrikes_t::iterator FindStrikeAuto( double strike ); // Auto insert new strike

  void RecalcATMWatch( double dblValue );
  void CalcGreeksAtStrike( ptime now, mapStrikes_iter_t iter, ou::tf::option::binomial::structInput& input );
  void CalcGreekForOption( 
    double dblPrice, 
    ou::tf::option::binomial::structInput& input, 
    ou::tf::option::binomial::structOutput& output );

};

} // namespace option
} // namespace tf
} // namespace ou

