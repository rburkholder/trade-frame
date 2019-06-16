/************************************************************************
 * Copyright(c) 2018, One Unified. All rights reserved.                 *
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

/*
 * File:   AtmIv.h
 * Author: raymond@burkholder.net
 *
 * Created on September 10, 2018, 10:43 PM
 */

#ifndef ATMIV_H
#define ATMIV_H

#include <map>
#include <tuple>
#include <string>
#include <functional>

#include <TFOptions/Option.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

// TODO: re-factor into two classes:
//   1) mostly static lookups
//   2) dynamic strike following
// TODO: also, re-factor the strike classifiers

// this code is deprecating some code in Bundle


class IvAtm {
public:

  using pInstrument_t = Option::pInstrument_t;
  using pWatch_t = Option::pWatch_t;
  using pOption_t = Option::pOption_t;

  using PriceIV = ou::tf::PriceIV;

  using fConstructedOption_t = std::function<void(pOption_t)>;
  using fConstructOption_t = std::function<void(const std::string&,pInstrument_t,fConstructedOption_t)>; // construct pOption_t from IQFeed Symbol name, with or without IB contract

  using fStartCalc_t = std::function<void(pOption_t,pWatch_t)>; // option, underlying
  using fStopCalc_t =  std::function<void(pOption_t,pWatch_t)>; // option, underlying

  struct exception_strike_not_found: public std::runtime_error {
    exception_strike_not_found( const char* ch ): std::runtime_error( ch ) {}
  };
  struct exception_at_start_of_chain: public exception_strike_not_found {
    exception_at_start_of_chain( const char* ch ): exception_strike_not_found( ch ) {}
  };
  struct exception_at_end_of_chain: public exception_strike_not_found {
    exception_at_end_of_chain( const char* ch ): exception_strike_not_found( ch ) {}
  };

  IvAtm( pWatch_t pWatchUnderlying, fConstructOption_t, fStartCalc_t, fStopCalc_t );
  IvAtm( IvAtm&& rhs );
  virtual ~IvAtm( );

  using fOnPriceIV_t = std::function<void(const PriceIV&)>;

  void CalcIvAtm( ptime dtNow, fOnPriceIV_t& );

  void EmitValues( void );
  void SaveSeries( const std::string& sPrefix60sec, const std::string& sPrefix86400sec );

  void SetIQFeedNameCall( double dblStrike, const std::string& sIQFeedSymbolName );
  void SetIQFeedNamePut( double dblStrike, const std::string& sIQFeedSymbolName );

  const std::string GetIQFeedNameCall( double dblStrike );
  const std::string GetIQFeedNamePut( double dblStrike );

  double Put_Itm( double );
  double Put_ItmAtm( double );
  double Put_Atm( double );
  double Put_OtmAtm( double );
  double Put_Otm( double );

  double Call_Itm( double );
  double Call_ItmAtm( double );
  double Call_Atm( double );
  double Call_OtmAtm( double );
  double Call_Otm( double );

  // returns 0, 1, 2 strikes found
  // needs exact match on strikeSource
  int AdjacentStrikes( double strikeSource, double& strikeLower, double& strikeUpper );

protected:
private:

  struct OptionsAtStrike {
    typedef ou::tf::option::Option::pOption_t pOption_t;
    std::string sCall;
    pOption_t pCall;
    std::string sPut;
    pOption_t pPut;
    bool bStarted;
    OptionsAtStrike(): bStarted( false ) {}
    OptionsAtStrike( OptionsAtStrike&& rhs )
    : sCall( std::move( rhs.sCall ) ),
      pCall( std::move( rhs.pCall ) ),
      sPut( std::move( rhs.sPut ) ),
      pPut( std::move( rhs.pPut ) ),
      bStarted( rhs.bStarted )
    { }

    void Start( fStartCalc_t& fStart, pWatch_t pWatchUnderlying, fConstructOption_t& fConstruct ) {
      assert( !bStarted );

      pInstrument_t pInstrumentUnderlying = pWatchUnderlying->GetInstrument();

      if ( nullptr == pCall.get() ) {
        fConstruct( sCall, pInstrumentUnderlying, [this,pWatchUnderlying,fStart](pOption_t pOption){
          pCall = pOption;
          fStart( pCall, pWatchUnderlying );
        } );
      }

      if ( nullptr == pPut.get() ) {
        fConstruct( sPut, pInstrumentUnderlying, [this,pWatchUnderlying,fStart](pOption_t pOption){
          pPut = pOption;
         fStart( pPut, pWatchUnderlying );
        } );
      }

      bStarted = true;
    }
    void Stop( fStopCalc_t& fStop, pWatch_t pUnderlying ) {
      assert( bStarted );
      fStop( pCall, pUnderlying );
      fStop( pPut, pUnderlying );
      bStarted = false;
    }
    void SaveSeries( const std::string& sPrefix ) {
      if ( nullptr != pCall.get() ) pCall->SaveSeries( sPrefix );
      if ( nullptr != pPut.get() ) pPut->SaveSeries( sPrefix );
    }
  };

  using mapChain_t = std::map<double, OptionsAtStrike>;

  mapChain_t::iterator m_iterUpper;
  mapChain_t::iterator m_iterMid;
  mapChain_t::iterator m_iterLower;

  mapChain_t m_mapChain;

  double m_dblUpperTrigger;
  double m_dblLowerTrigger;

  pWatch_t m_pWatchUnderlying;
  fConstructOption_t m_fConstructOption;

  fStartCalc_t m_fStartCalc;
  fStopCalc_t m_fStopCalc;

  enum EOptionWatchState { EOWSNoWatch, EOWSWatching };
  EOptionWatchState m_stateOptionWatch;

  static const size_t StrikeUpper = 1;
  static const size_t StrikeLower = 0;
  using tupleAdjacentStrikes_t = std::tuple<double,double>;

  ou::tf::PriceIVs m_tsIvAtm;

  double CurrentUnderlying() const { return m_pWatchUnderlying->LastQuote().Midpoint(); }

  mapChain_t::const_iterator FindStrike( const double strike ) const;
  mapChain_t::iterator FindStrike( const double strike );
  tupleAdjacentStrikes_t FindAdjacentStrikes() const;
  void RecalcATMWatch( double dblUnderlying );
  void UpdateATMWatch( double dblUnderlying );

  void SaveIvAtm( const std::string& sPrefix, const std::string& sPrefix86400sec );

};

} // namespace option
} // namespace tf
} // namespace ou


#endif /* ATMIV_H */

