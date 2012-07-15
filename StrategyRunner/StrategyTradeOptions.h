/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

#include <map>
#include <vector>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/shared_ptr.hpp>

#include <TFTrading/PortfolioManager.h>
#include <TFTrading/ProviderManager.h>
//#include <TFTrading/InstrumentData.h>
#include <TFInteractiveBrokers/IBTWS.h>
#include <TFIQFeed/IQFeedProvider.h>
#include <TFOptions/Option.h>
#include <TFTimeSeries/TimeSeries.h>
#include <TFIndicators/TSSWStochastic.h>

class StrategyTradeOptions {
public:

  typedef ou::tf::CProviderInterfaceBase::pProvider_t pProvider_t;

  StrategyTradeOptions( pProvider_t pExecutionProvider, pProvider_t pData1Provider, pProvider_t pData2Provider );
  ~StrategyTradeOptions(void);

//  void Start( void ); // for real time
//  void Start( const std::string& sSymbolPath );  // for simulation
  void Start( const std::string& sUnderlying, boost::gregorian::date dtOptionNearDate, boost::gregorian::date dtOptionFarDate );
  void Stop( void );

  void Save( const std::string& sPath );

protected:
private:

  typedef ou::tf::CInstrument::pInstrument_t pInstrument_t;
  typedef ou::tf::CPortfolioManager::pPortfolio_t pPortfolio_t;
  typedef ou::tf::CPortfolioManager::pPosition_t pPosition_t;

  enum enumTradeStates { EPreOpen, EBellHeard, EPauseForQuotes, EAfterBell, ETrading, ECancelling, EGoingNeutral, EClosing, EAfterHours };

  typedef ou::tf::Greek::greeks_t greeks_t;

  struct statsOptions {
    unsigned int nLong;
    double sumStrikeLong; // add nLong * strike so to find average strike price determining spread sizes
    unsigned int nShort;
    double sumStrikeShort; // add nShort * strike so to find average strike price determining spread sizes
    statsOptions( void ) : nLong( 0 ), nShort( 0 ), sumStrikeLong( 0.0 ), sumStrikeShort( 0.0 ) {};
  };

  template<typename Side> 
  struct option_t { // individual option state plus position state
    typedef boost::shared_ptr<Side> pOption_t;
    pOption_t pOption;  // individual option state (naming is a bit confusing, but is functional )
    pPosition_t pPosition;
    option_t( void ) {};
    option_t( pOption_t pOption_ ) : pOption( pOption_t ) {};
    option_t( pOption_t pOption_, pPosition_t pPosition_ ) : pOption( pOption_ ), pPosition( pPosition_ ) {};
  };

  typedef option_t<ou::tf::option::Call> call_t;
  typedef option_t<ou::tf::option::Put> put_t;

  typedef std::vector<call_t> vCalls_t;
  typedef std::vector<put_t> vPuts_t;

  struct options_t  {

    double strike;

    call_t optionNearDateCall;
    put_t  optionNearDatePut;

    call_t optionFarDateCall;
    put_t  optionFarDatePut;

    vCalls_t vOtherCalls;  // still active calls not in current near or far category
    vPuts_t vOtherPuts;    // still active puts  not in current near or far category

    options_t( void ): strike( 0.0 ) {};
    options_t( double strike_ ) : strike( strike_ ) {};
    ~options_t( void ) {};

    template<class Visitor>
    void ScanCallOptions( greeks_t& greeks, statsOptions& stats, Visitor f ) {
      f( greeks, stats, optionNearDateCall );
      f( greeks, stats, optionFarDateCall );
      for ( vCalls_t::iterator iter = vOtherCalls.begin(); vOtherCalls.end() != iter; ++iter ) {
        f( greeks, stats, *iter );
      }
    }

    template<class Visitor>
    void ScanPutOptions( greeks_t& greeks, statsOptions& stats, Visitor f ) {
      f( greeks, stats, optionNearDatePut );
      f( greeks, stats, optionFarDatePut );
      for ( vPuts_t::iterator iter = vOtherPuts.begin(); vOtherPuts.end() != iter; ++iter ) {
        f( greeks, stats, *iter );
      }
    }

    void StartWatch( void ) {
      if ( 0 != optionNearDateCall.pOption.get() ) optionNearDateCall.pOption->StartWatch();
      if ( 0 != optionNearDatePut.pOption.get() ) optionNearDatePut.pOption->StartWatch();
      if ( 0 != optionFarDateCall.pOption.get() ) optionFarDateCall.pOption->StartWatch();
      if ( 0 != optionFarDatePut.pOption.get() ) optionFarDatePut.pOption->StartWatch();
    }
    void StopWatch( void ) {
      if ( 0 != optionNearDateCall.pOption.get() ) optionNearDateCall.pOption->StopWatch();
      if ( 0 != optionNearDatePut.pOption.get() ) optionNearDatePut.pOption->StopWatch();
      if ( 0 != optionFarDateCall.pOption.get() ) optionFarDateCall.pOption->StopWatch();
      if ( 0 != optionFarDatePut.pOption.get() ) optionFarDatePut.pOption->StopWatch();
    }

    void Save( const std::string& sPrefix ) {
      if ( 0 != optionNearDateCall.pOption.get() ) optionNearDateCall.pOption->SaveSeries( sPrefix );
      if ( 0 != optionNearDatePut.pOption.get() ) optionNearDatePut.pOption->SaveSeries( sPrefix );
      if ( 0 != optionFarDateCall.pOption.get() ) optionFarDateCall.pOption->SaveSeries( sPrefix );
      if ( 0 != optionFarDatePut.pOption.get() ) optionFarDatePut.pOption->SaveSeries( sPrefix );
      for ( vCalls_t::iterator iter = vOtherCalls.begin(); vOtherCalls.end() != iter; ++iter ) {
        iter->pOption->SaveSeries( sPrefix );
      }
      for ( vPuts_t::iterator iter = vOtherPuts.begin(); vOtherPuts.end() != iter; ++iter ) {
        iter->pOption->SaveSeries( sPrefix );
      }
    }
  };

  struct stochastic_t {
    enum EActiveSide { ESideOut, ESideTop, ESideBottom };
    ou::tf::TSSWStochastic ts;
    EActiveSide sideCall, sidePut;  // near call is out, short at top, exit at bottom; put is out, short at bottom, exit at top
    stochastic_t( ou::tf::Quotes& quotes, time_duration tdWindowWidth ): ts( quotes, tdWindowWidth ), sideCall( ESideOut ), sidePut( ESideOut ) {};
  };

  typedef std::map<double,options_t> mapOptions_t;
  typedef mapOptions_t::iterator mapOptions_iter_t;
  typedef std::pair<double,options_t> mapOptions_pair_t;
  mapOptions_t m_mapOptions;

  ou::tf::Quotes m_quotes;
  ou::tf::Trades m_trades;

  mapOptions_iter_t m_iterMapOptionsAbove2;
  mapOptions_iter_t m_iterMapOptionsAbove1;
  mapOptions_iter_t m_iterMapOptionsMiddle;
  mapOptions_iter_t m_iterMapOptionsBelow1; 
  mapOptions_iter_t m_iterMapOptionsBelow2; 

  vCalls_t m_vCalls;  // loaded with HandlePositionsLoad:  near, far, other, used for calc delta
  vPuts_t m_vPuts;    // loaded with HandlePositionsLoad:  near, far, other, used for calc delta

  enumTradeStates m_TradeStates;

  time_duration m_timeOpeningBell, m_timeCancel, m_timeClose, m_timeClosingBell;
  time_duration m_timePauseForQuotes;

  std::string m_sUnderlying;

  std::string m_sTimeStampWatchStarted;

  double m_paramWorkingDelta;

  pPortfolio_t m_pPortfolio;
  pInstrument_t m_pUnderlying;

  // these need to come after m_quotes
  stochastic_t m_stoch1;
  stochastic_t m_stoch2;
  stochastic_t m_stoch3;
  stochastic_t m_stoch4;

  struct bundle_t {
    pInstrument_t pInstrument;
    ou::tf::Quotes quotes;
    ou::tf::Trades trades;
    void HandleTrade( const ou::tf::Trade& trade ) { trades.Append( trade ); };
    void HandleQuote( const ou::tf::Quote& quote ) { quotes.Append( quote ); };
  };

  //bundle_t bundleUnderlying;
  bundle_t bundle10YrTreasury;

  pProvider_t m_pExecutionProvider;
  pProvider_t m_pData1Provider;
  pProvider_t m_pData2Provider;

  ou::tf::CIQFeedProvider::pProvider_t m_pData1ProviderIQFeed;
  ou::tf::CIBTWS::pProvider_t m_pData2ProviderIB;
  ou::tf::CIBTWS::pProvider_t m_pExecutionProviderIB;

  boost::gregorian::date m_dateOptionNearDate;
  boost::gregorian::date m_dateOptionFarDate;

  template<class Option>
  void ProcessOptions( greeks_t& greeks, statsOptions& stats, Option& option );

  void AdjustTheOptions( const ou::tf::Quote& quote );
  bool SetPointersFirstTime( const ou::tf::Quote& quote );
  bool AdjustThePointers( const ou::tf::Quote& quote );

  void LoadExistingInstrumentsAndPortfolios( const std::string& sUnderlying );

  void ProcessCallOptions( call_t& call );
  void ProcessPutOptions( put_t& put );

  void HandlePositionsLoad( pPosition_t pPosition );

  void HandleNearOptionsLoad( pInstrument_t pInstrument );
  void HandleFarOptionsLoad( pInstrument_t pInstrument );

  void HandleUnderlyingContractDetails( const ou::tf::CIBTWS::ContractDetails&, ou::tf::CIBTWS::pInstrument_t& );
  void HandleUnderlyingContractDetailsDone( void );

  void HandleNearDateContractDetails( const ou::tf::CIBTWS::ContractDetails&, ou::tf::CIBTWS::pInstrument_t& );
  void HandleNearDateContractDetailsDone( void );

  void HandleFarDateContractDetails( const ou::tf::CIBTWS::ContractDetails&, ou::tf::CIBTWS::pInstrument_t& );
  void HandleFarDateContractDetailsDone( void );

  void HandleQuote( const ou::tf::Quote& );
  void HandleTrade( const ou::tf::Trade& );
};

template<class Option>
void StrategyTradeOptions::ProcessOptions( greeks_t& greeks, statsOptions& stats, Option& option ) {
  if ( 0 != option.pPosition.get() ) {
    if ( 0 != option.pPosition->GetRow().nPositionActive ) {
      const ou::tf::CPosition::TableRowDef& row( option.pPosition->GetRow() );
      if ( ( m_iterMapOptionsMiddle->first == option.pOption->GetStrike() )  // don't sell what we'll just buy back
        || ( 0 >= ( option.pPosition->GetUnRealizedPL() - ( 1.00 /*minprofit*/ + ( 2.0 * 0.75 /*commission*/ * row.nPositionActive ) ) ) ) // some basic profit minimum plus in + out * average option contract price
        ) {
          // determine remaining delta for balancing purposes
          greeks.delta += row.nPositionActive * option.pOption->Delta();
          greeks.gamma += row.nPositionActive * option.pOption->Gamma();
          greeks.theta += row.nPositionActive * option.pOption->Theta();
          greeks.vega  += row.nPositionActive * option.pOption->Vega();
//          greeks.rho += option.pPosition->GetRow().nPositionActive * option.pOption->Rho();
          switch ( row.eOrderSideActive ) {
          case ou::tf::OrderSide::Buy:
            stats.nLong += row.nPositionActive;
            stats.sumStrikeLong += row.nPositionActive * option.pOption->GetStrike();
            break;
          case ou::tf::OrderSide::Sell:
            stats.nShort += row.nPositionActive;
            stats.sumStrikeShort += row.nPositionActive * option.pOption->GetStrike();
            break;
          }
          
      }
      else {
        // close out profitable positions
        option.pPosition->ClosePosition();
        std::cout << ou::CTimeSource::Instance().Internal() << " closing option " << option.pOption->GetInstrument()->GetInstrumentName() << std::endl;
      }
    }
  }
}
