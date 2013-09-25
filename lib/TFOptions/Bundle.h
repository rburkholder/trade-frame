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

#include <TFTrading/PortfolioManager.h>
#include <TFTrading/NoRiskInterestRateSeries.h>
#include <TFOptions/Binomial.h>
#include <TFTrading/Watch.h>
#include <TFTimeSeries/BarFactory.h>

#include "Strike.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

class ExpiryBundle {
public:

  typedef Instrument::pInstrument_t pInstrument_t;
  typedef ou::tf::ProviderInterfaceBase::pProvider_t pProvider_t;
  typedef Watch::pWatch_t pWatch_t;

  ExpiryBundle(void);
  virtual ~ExpiryBundle(void);

  void SetUnderlying( pInstrument_t pInstrument, pProvider_t pProvider );
  void SetCall( pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider );
  void SetPut( pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider );

  Call* GetCall( double dblStrike );
  Put* GetPut( double dblStrike );

  void FindAdjacentStrikes( double dblValue, double& dblLower, double& dblUpper );  // uses <= and >= logic around dblStrike, therefore possibility of dblLower = dblUpper

  void UpdateATMWatch( double dblValue );

  void SetWatchableOn( double dblStrike );  // each strike is not watcheable by default
  void SetWatchableOff( double dblStrike );

  //void SetWatchUnderlyingOn( void );
  //void SetWatchUnderlyingOff( void );

  void SetWatchOn( double dblStrike, bool bForce = false ); // watch only selected call/put at strike, force watchable on
  void SetWatchOff( double dblStrike, bool bForce = false ); // forces watchable off when true

  void SetWatchOn( void ); // watch underlying plus all watcheable options
  void SetWatchOff( void ); 

  void SaveSeries( const std::string& sPrefix60sec, const std::string& sPrefix86400sec );
  void EmitValues( void );

  void SetExpiry( ptime dt ); // utc

  void CalcGreeks( double dblUnderlying, double dblVolHistorical, ptime now, ou::tf::LiborFromIQFeed& libor );

protected:
private:

  typedef std::map<double,Strike> mapStrikes_t;
  typedef mapStrikes_t::iterator mapStrikes_iter_t;

  enum EOptionWatchState { EOWSNoWatch, EOWSWatching } m_stateOptionWatch;

  ptime m_dtExpiry;  // eg, 4pm EST third Fri of month for normal US equity options, in utc

  double m_dblUpperTrigger;
  double m_dblLowerTrigger;

  ou::tf::PriceIVs m_tsAtmIv; // composite of all expiries, or only front expiry?
  ou::tf::BarFactory m_bfIVUnderlyingCall; // ditto
  ou::tf::BarFactory m_bfIVUnderlyingPut;  // ditto

  mapStrikes_iter_t m_iterUpper;
  mapStrikes_iter_t m_iterMid;
  mapStrikes_iter_t m_iterLower;

  mapStrikes_t m_mapStrikes;

  mapStrikes_iter_t FindStrike( double strike );
  mapStrikes_iter_t FindStrikeAuto( double strike ); // Auto insert new strike

  void RecalcATMWatch( double dblValue );  
  void CalcGreeksAtStrike( ptime now, mapStrikes_iter_t iter, ou::tf::option::binomial::structInput& input );

  void SaveAtmIv( const std::string& sPrefix, const std::string& sPrefix86400Min );
};

// =======================================================

class ExpiryBundleWithUnderlying: public ExpiryBundle {
public:

  typedef Instrument::pInstrument_t pInstrument_t;
  typedef ou::tf::ProviderInterfaceBase::pProvider_t pProvider_t;
  typedef Watch::pWatch_t pWatch_t;

  void SetUnderlying( pInstrument_t pInstrument, pProvider_t pProvider );

  void SetWatchOn( void );
  void SetWatchOff( void );
  void EmitValues( void );

  void SaveSeries( const std::string& sPrefix60sec, const std::string& sPrefix86400sec );

protected:
private:
  pWatch_t m_pwatchUnderlying;
};

// =======================================================

class MultiExpiryBundle {
public:

  typedef ou::tf::ProviderInterfaceBase::pProvider_t pProvider_t;
  typedef ou::tf::Portfolio::pPortfolio_t pPortfolio_t;
  typedef ou::tf::Position::pPosition_t pPosition_t;
  typedef ou::tf::Instrument::pInstrument_t pInstrument_t;
  typedef Watch::pWatch_t pWatch_t;

  explicit MultiExpiryBundle( const std::string& sName ): m_sName( sName ) {};
  virtual ~MultiExpiryBundle( void );

  const std::string& Name( void ) { return m_sName; };

  void SetWatchUnderlying( pInstrument_t& pInstrument, pProvider_t& pProvider );
  pWatch_t GetWatchUnderlying( void ) { return m_pWatchUnderlying; };

  // the references are void when the map has insertions or deletions
  bool ExpiryBundleExists( boost::gregorian::date );
  ExpiryBundle& GetExpiryBundle( boost::gregorian::date );
  ExpiryBundle& CreateExpiryBundle( boost::gregorian::date );

  void SetWatchOn( void );
  void SetWatchOff( void );
  void CalcIV( ptime dtNow /*utc*/, ou::tf::LiborFromIQFeed& libor );
  void SaveData( const std::string& sPrefixSession, const std::string& sPrefix86400sec );
  void AssignOption( pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider );

protected:

private:

  typedef std::map<boost::gregorian::date,ExpiryBundle> mapExpiryBundles_t;

  std::string m_sName;

  pPortfolio_t pPortfolio;  // summary portfolio for all things related to symbols in this Bundle

  pWatch_t m_pWatchUnderlying;
  pPosition_t pPositionUnderlying;

  mapExpiryBundles_t m_mapExpiryBundles;

  void HandleUnderlyingQuote( const ou::tf::Quote& quote );
  void HandleUnderlyingTrade( const ou::tf::Trade& trade ) {};

};

} // namespace option
} // namespace tf
} // namespace ou

