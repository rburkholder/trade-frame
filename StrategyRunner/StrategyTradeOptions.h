/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

#include <map>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <TFTrading/ProviderManager.h>
//#include <TFTrading/InstrumentData.h>
#include <TFInteractiveBrokers/IBTWS.h>
#include <TFIQFeed/IQFeedProvider.h>
#include <TFOptions/Option.h>
#include <TFTimeSeries/TimeSeries.h>

class StrategyTradeOptions {
public:

  typedef ou::tf::CProviderInterfaceBase::pProvider_t pProvider_t;

  StrategyTradeOptions( pProvider_t pExecutionProvider, pProvider_t pData1Provider, pProvider_t pData2Provider );
  ~StrategyTradeOptions(void);

//  void Start( void ); // for real time
//  void Start( const std::string& sSymbolPath );  // for simulation
  void Start( const std::string& sUnderlying, boost::gregorian::date dtOptionNearDate, boost::gregorian::date dtOptionFarDate );
  void Stop( void );

protected:
private:

  typedef ou::tf::CInstrument::pInstrument_t pInstrument_t;

  enum enumStates { EPreOpen, EAfterBell, ETrading, ECancelling, EClosing, EAfterHours };

  struct options_t  {
    double strike;
    ou::tf::option::Call* pNearDateCall;
    ou::tf::option::Put*  pNearDatePut;
    ou::tf::option::Call* pFarDateCall;
    ou::tf::option::Put* pFarDatePut;
    options_t( void ) {};
    options_t( double strike_ ) : strike( strike_ ), 
      pNearDateCall( 0 ), pNearDatePut( 0 ), pFarDateCall( 0 ), pFarDatePut( 0 ) {};
    ~options_t( void ) {
      if ( 0 == pNearDateCall ) delete pNearDateCall;
      if ( 0 == pNearDatePut ) delete pNearDatePut;
      if ( 0 == pFarDateCall ) delete pFarDateCall;
      if ( 0 == pFarDatePut ) delete pFarDatePut;
    }
  };

  typedef std::map<double,options_t> mapOptions_t;
  mapOptions_t m_mapOptions;

  std::string m_sUnderlying;

  ou::tf::CQuotes m_quotes;
  ou::tf::CTrades m_trades;

  pProvider_t m_pExecutionProvider;
  pProvider_t m_pData1Provider;
  pProvider_t m_pData2Provider;

  ou::tf::CIQFeedProvider::pProvider_t m_pData1ProviderIQFeed;
  ou::tf::CIBTWS::pProvider_t m_pData2ProviderIB;
  ou::tf::CIBTWS::pProvider_t m_pExecutionProviderIB;

  pInstrument_t m_pUnderlying;

  boost::gregorian::date m_dateOptionNearDate;
  boost::gregorian::date m_dateOptionFarDate;

  void LoadExistingInstruments( const std::string& sUnderlying );

  void HandleNearOptionsLoad( pInstrument_t pInstrument );
  void HandleFarOptionsLoad( pInstrument_t pInstrument );

  void HandleUnderlyingContractDetails( const ou::tf::CIBTWS::ContractDetails&, ou::tf::CIBTWS::pInstrument_t& );
  void HandleUnderlyingContractDetailsDone( void );

  void HandleNearDateContractDetails( const ou::tf::CIBTWS::ContractDetails&, ou::tf::CIBTWS::pInstrument_t& );
  void HandleNearDateContractDetailsDone( void );

  void HandleFarDateContractDetails( const ou::tf::CIBTWS::ContractDetails&, ou::tf::CIBTWS::pInstrument_t& );
  void HandleFarDateContractDetailsDone( void );

  void HandleQuote( const ou::tf::CQuote& );
  void HandleTrade( const ou::tf::CTrade& );
};

