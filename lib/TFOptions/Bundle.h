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

#include <OUCommon/Delegate.h>

#include <TFTrading/PortfolioManager.h>
#include <TFTrading/Watch.h>

#include <TFTimeSeries/BarFactory.h>

#include <TFIQFeed/MarketSymbol.h>

#include "NoRiskInterestRateSeries.h"
#include "Binomial.h"
#include "Strike.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

class ExpiryBundle {
  friend std::ostream& operator<<( std::ostream& os, const ExpiryBundle& );
public:

  typedef Instrument::pInstrument_t pInstrument_t;
  typedef ou::tf::ProviderInterfaceBase::pProvider_t pProvider_t;
  typedef Watch::pWatch_t pWatch_t;

  ExpiryBundle(void);
  virtual ~ExpiryBundle(void);

  typedef FastDelegate1<ou::tf::option::Strike&> OnStrikeWatch_t; // used from the MultiBundle level
  ou::Delegate<ou::tf::option::Strike&> OnStrikeWatchOn;
  ou::Delegate<ou::tf::option::Strike&> OnStrikeWatchOff;

  typedef FastDelegate1<const ou::tf::PriceIVExpiry&> OnAtmIvCalc_t;
  ou::Delegate<const ou::tf::PriceIVExpiry&> OnAtmIvCalc;

//  void SetUnderlying( pInstrument_t pInstrument, pProvider_t pProvider );
  void SetCall( pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider );
  void SetPut( pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider );

  void SetCall( pInstrument_t pInstrument, pProvider_t pDataProvider );
  void SetPut( pInstrument_t pInstrument, pProvider_t pDataProvider );

  Call* GetCall( double dblStrike );
  Put* GetPut( double dblStrike );

  void FindAdjacentStrikes( double dblValue, double& dblLower, double& dblUpper );  // uses <= and >= logic around dblStrike, therefore possibility of dblLower = dblUpper

  void UpdateATMWatch( double dblValue );

  void SetWatchableOn( double dblStrike );  // each strike is not watchable by default
  void SetWatchableOff( double dblStrike );

  void SetWatchOn( double dblStrike, bool bForce = false ); // watch only selected call/put at strike, force watchable on
  void SetWatchOff( double dblStrike, bool bForce = false ); // forces watchable off when true

  void StartWatch( void ); // watch underlying plus all watchable options
  void StopWatch( void );

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

  ou::tf::PriceIVExpirys m_tsAtmIv;
  ou::tf::BarFactory m_bfIVUnderlyingCall;
  ou::tf::BarFactory m_bfIVUnderlyingPut;

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

std::ostream& operator<<( std::ostream& os, const ExpiryBundle& );

// =======================================================

class ExpiryBundleWithUnderlying: public ExpiryBundle {
public:

  ExpiryBundleWithUnderlying( void );
  ~ExpiryBundleWithUnderlying( void );

  typedef Instrument::pInstrument_t pInstrument_t;
  typedef ou::tf::ProviderInterfaceBase::pProvider_t pProvider_t;
  typedef Watch::pWatch_t pWatch_t;

  void SetUnderlying( pInstrument_t pInstrument, pProvider_t pProvider );

  void StartWatch( void );
  void StopWatch( void );
  void EmitValues( void );

  void SaveSeries( const std::string& sPrefix60sec, const std::string& sPrefix86400sec );

protected:
private:
  pWatch_t m_pwatchUnderlying;
};

// =======================================================

class MultiExpiryBundle {
  friend std::ostream& operator<<( std::ostream& os, const MultiExpiryBundle& );
public:

  typedef ou::tf::ProviderInterfaceBase::pProvider_t pProvider_t;
  typedef ou::tf::Portfolio::pPortfolio_t pPortfolio_t;
  typedef ou::tf::Position::pPosition_t pPosition_t;
  typedef ou::tf::Instrument::pInstrument_t pInstrument_t;
  typedef Watch::pWatch_t pWatch_t;

  MultiExpiryBundle( const std::string& sName ): m_sName( sName ) {};
  virtual ~MultiExpiryBundle( void );

  const std::string& Name( void ) { return m_sName; };

  pPortfolio_t& Portfolio( void ) { return m_pPortfolio; };

  void SetWatchUnderlying( pInstrument_t& pInstrument, pProvider_t& pProvider );
  pWatch_t GetWatchUnderlying( void ) { return m_pWatchUnderlying; };

  // the references are void when the map has insertions or deletions
  bool ExpiryBundleExists( boost::gregorian::date );
  ExpiryBundle& GetExpiryBundle( boost::gregorian::date );
  //ExpiryBundle& CreateExpiryBundle( boost::gregorian::date );
  ExpiryBundle& CreateExpiryBundle( boost::posix_time::ptime dt ); // date portion used for map, full ptime for expiry calcs

  void StartWatch( void );
  void StopWatch( void );
  void CalcIV( ptime dtNow /*utc*/, ou::tf::LiborFromIQFeed& libor );
  void SaveData( const std::string& sPrefixSession, const std::string& sPrefix86400sec );
  void AssignOption( pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider );

  void AddOnStrikeWatchOn( ExpiryBundle::OnStrikeWatch_t );
  void RemoveOnStrikeWatchOn( ExpiryBundle::OnStrikeWatch_t );
  void AddOnStrikeWatchOff( ExpiryBundle::OnStrikeWatch_t );
  void RemoveOnStrikeWatchOff( ExpiryBundle::OnStrikeWatch_t );

  void AddOnAtmIv( ExpiryBundle::OnAtmIvCalc_t );
  void RemoveOnAtmIv( ExpiryBundle::OnAtmIvCalc_t );

protected:

private:

  typedef std::map<boost::gregorian::date,ExpiryBundle> mapExpiryBundles_t;

  std::string m_sName;

  pPortfolio_t m_pPortfolio;  // summary portfolio for all things related to symbols in this Bundle

  pWatch_t m_pWatchUnderlying;

  mapExpiryBundles_t m_mapExpiryBundles;

  void HandleUnderlyingQuote( const ou::tf::Quote& quote );
  void HandleUnderlyingTrade( const ou::tf::Trade& trade ) {};

};

std::ostream& operator<<( std::ostream& os, const MultiExpiryBundle& );

// PopulateMultiExpiryBundle used like:
// m_listIQFeedSymbols.SelectOptionsByUnderlying( sName, PopulateMultiExpiryBundle( *m_pBundle, m_pData1Provider, pNull ) );

struct PopulateMultiExpiryBundle {

  typedef ou::tf::Instrument::pInstrument_t pInstrument_t;
  typedef ou::tf::ProviderInterfaceBase::pProvider_t pProvider_t;

  PopulateMultiExpiryBundle(
    ou::tf::option::MultiExpiryBundle& bundle, pProvider_t pDataProvider_, pProvider_t pGreekProvider_ )
    : meb( bundle ), pDataProvider( pDataProvider_ ), pGreekProvider( pGreekProvider_ )
  {}

  void operator()( const ou::tf::iqfeed::MarketSymbol::TableRowDef& trd ) {
    //assert( trd.sUnderlying == meb.Name() );
    assert( ( ou::tf::iqfeed::MarketSymbol::ESecurityType::IEOption == trd.sc )
         || ( ou::tf::iqfeed::MarketSymbol::ESecurityType::FOption == trd.sc )
      );

    // to trade, when is the IB contract number acquired?
    // and when does the symbol get registered, so it can be traded?

    //boost::gregorian::date dateTrdExpiry( trd.nYear, trd.nMonth, trd.nDay - 1 );  // IQFeed dates are on Saturday
    boost::gregorian::date dateTrdExpiry( trd.nYear, trd.nMonth, trd.nDay ); // fix IQFeed dates elsewhere

    if ( meb.ExpiryBundleExists( dateTrdExpiry ) ) {
      pInstrument_t pInstrument;
      std::string side;
      side = trd.eOptionSide;
      std::stringstream ss;
      ss << trd.sUnderlying << " " << dateTrdExpiry << " " << side << " " << trd.dblStrike;
      switch ( trd.sc ) {
      case ou::tf::iqfeed::MarketSymbol::ESecurityType::IEOption:
        pInstrument.reset(
          new ou::tf::Instrument(
            ss.str(), ou::tf::InstrumentType::Option, "SMART",
            dateTrdExpiry.year(), dateTrdExpiry.month(), dateTrdExpiry.day(),
//            meb.GetWatchUnderlying()->GetInstrument(),
	    trd.eOptionSide, trd.dblStrike ) );
        break;
      case ou::tf::iqfeed::MarketSymbol::ESecurityType::FOption:
        pInstrument.reset(
          new ou::tf::Instrument(
            ss.str(), ou::tf::InstrumentType::FuturesOption, "SMART",
            dateTrdExpiry.year(), dateTrdExpiry.month(), dateTrdExpiry.day(),
//            meb.GetWatchUnderlying()->GetInstrument(),
	    trd.eOptionSide, trd.dblStrike ) );
        break;
      }
      pInstrument->SetAlternateName( ou::tf::Instrument::eidProvider_t::EProviderIQF, trd.sSymbol );
      meb.AssignOption( pInstrument, pDataProvider, pGreekProvider );
    }
  }

  ou::tf::option::MultiExpiryBundle& meb;
  pProvider_t pDataProvider;
  pProvider_t pGreekProvider;
};



} // namespace option
} // namespace tf
} // namespace ou

