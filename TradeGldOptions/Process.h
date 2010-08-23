/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

#include <vector>
#include <string>
#include <sstream>
#include <map>

#include <LibTimeSeries/DatedDatum.h>
#include <LibTimeSeries/TimeSeries.h>

#include <LibTrading/PortfolioManager.h>
#include <LibTrading/ProviderManager.h>

#include <LibIQFeed/IQFeedHistoryQuery.h>  // seems to be a header ordering dependancy
#include <LibIQFeed/IQFeedProvider.h>  // includes CPortfolio and CPosition

#include <LibInteractiveBrokers/IBTWS.h>

#include <LibSimulation/SimulationProvider.h>

//
// ==================
//

class CNakedOption
{
public:
  CNakedOption( double dblStrike );
  CNakedOption( const CNakedOption& rhs );
  virtual ~CNakedOption( void ) {};

  CNakedOption& operator=( const CNakedOption& rhs );

  bool operator< ( const CNakedOption& rhs ) const { return m_dblStrike <  rhs.m_dblStrike; };
  bool operator<=( const CNakedOption& rhs ) const { return m_dblStrike <= rhs.m_dblStrike; };

  double Strike( void ) { return m_dblStrike; };
  void Symbol( CIBSymbol::pSymbol_t pSymbol ) { m_pSymbol = pSymbol; };
  CIBSymbol::pSymbol_t Symbol( void ) { return m_pSymbol; };

  void HandleQuote( const CQuote& quote );
  void HandleTrade( const CTrade& trade );
  void HandleGreek( const CGreek& greek );

  double Bid( void ) const { return m_dblBid; };
  double Ask( void ) const { return m_dblAsk; };

  CQuotes* Quotes( void ) { return &m_quotes; };
  CTrades* Trades( void ) { return &m_trades; };
  CGreeks* Greeks( void ) { return &m_greeks; };

protected:

  std::string m_sSide;

  double m_dblBid;
  double m_dblAsk;
  double m_dblTrade;

  double m_dblStrike;
  CGreek m_greek;

  CQuotes m_quotes;
  CTrades m_trades;
  CGreeks m_greeks;

  bool m_bWatching;

  CIBSymbol::pSymbol_t m_pSymbol;

  std::stringstream m_ss;

private:
};

//
// ==================
//

class CNakedCall: public CNakedOption
{
public:
  CNakedCall( double dblStrike );
  virtual ~CNakedCall( void ) {};
protected:
private:
};

//
// ==================
//

class CNakedPut: public CNakedOption
{
public:
  CNakedPut( double dblStrike );
  virtual ~CNakedPut( void ) {};
protected:
private:
};

//
// ==================
//

class CStrikeInfo 
{
public:
  CStrikeInfo( double dblStrike );
  CStrikeInfo( const CStrikeInfo& rhs );
  ~CStrikeInfo( void );

  CStrikeInfo& operator=( const CStrikeInfo& rhs );

  bool operator< ( const CStrikeInfo& rhs ) const { return m_dblStrike <  rhs.m_dblStrike; };
  bool operator<=( const CStrikeInfo& rhs ) const { return m_dblStrike <= rhs.m_dblStrike; };

  double Strike( void ) const { return m_dblStrike; };

  CNakedCall* Call( void ) { return &m_call; };
  CNakedPut* Put( void ) { return &m_put; };

protected:

  CNakedCall m_call;
  CNakedPut m_put;

private:
  std::stringstream m_ss;
  bool m_bWatching;
  double m_dblStrike;
};

//
// ==================
//

class CProcess: 
  public CIQFeedHistoryQuery<CProcess>
{
  friend CIQFeedHistoryQuery<CProcess>;
public:
  CProcess(void);
  ~CProcess(void);

  void SimConnect( void );
  void SimDisconnect( void );

  void IBConnect( void );
  void IBDisconnect( void );

  void IQFeedConnect( void );
  void IQFeedDisconnect( void );

  void StartWatch( void );
  void StopWatch( void );

  void StartTrading( void ) ;
  void StopTrading( void );

  void EmitStats( void );
  void SaveSeries( void );

protected:

  void OnHistoryConnected( void );
  void OnHistorySummaryData( structSummary* pDP );
  void OnHistoryRequestDone( void );
  void OnHistoryDisconnected( void ) {};

private:

  typedef CPortfolio::pPortfolio_t pPortfolio_t;
  typedef CPosition::pPosition_t pPosition_t;

  typedef CProviderInterfaceBase::pProvider_t pProvider_t;

  typedef CIBTWS::pProvider_t pProviderIBTWS_t;
  typedef CIQFeedProvider::pProvider_t pProviderIQFeed_t;
  typedef CSimulationProvider::pProvider_t pProviderSim_t;

  typedef double strike_t;
  enum enumTradingState {  // arranged in chronological order
    ETSFirstPass,     // any state initialization
    ETSPreMarket,     // time frame before Markets open
    ETSMarketOpened,  // initialize anything on market open
    ETSFirstTrade,    // create opening trade at correct time
    ETSTrading,       // allow active trading
    ETSCloseOrders,   // close trades
    ETSAfterMarket    // after market closes
  } m_TradingState;

  std::string m_sSymbolName;
  long m_contractidUnderlying;
  CIBSymbol::pSymbol_t m_pUnderlying;  // need to make share_ptr

  std::string m_sPathForSeries;

  std::stringstream m_ss;

  pProviderIQFeed_t m_iqfeed;
  bool m_bIQFeedConnected;

  pProviderIBTWS_t m_tws;
  bool m_bIBConnected;
  bool m_bWatchingOptions;
  bool m_bTrading;

  pProviderSim_t m_sim;
  bool m_bSimConnected;

  pProvider_t m_pExecutionProvider;
  pProvider_t m_pDataProvider;
  
  enum enumProcessingState {
    EPSSimulation,
    EPSLive
  } m_ProcessingState;

  CQuotes m_quotes;
  CTrades m_trades;

  CBar m_Bar;  // keep pointer for when data arrives

  double m_dblBaseDelta; // keep trades balanced at this level
  double m_dblBaseDeltaIncrement;

  std::vector<double> m_vCrossOverPoints;  // has pivots and strikes in order
  std::vector<CStrikeInfo> m_vStrikes;  // put/call info for each strike
  std::vector<CStrikeInfo>::iterator m_iterStrikes;

  std::vector<double>::iterator m_iterAboveCrossOver;
  std::vector<double>::iterator m_iterBelowCrossOver;

  std::vector<CStrikeInfo>::iterator m_iterOILowestWatch;
  std::vector<CStrikeInfo>::iterator m_iterOIHighestWatch;
  std::vector<CStrikeInfo>::iterator m_iterOILatestGammaSelectCall;
  std::vector<CStrikeInfo>::iterator m_iterOILatestGammaSelectPut;

  Contract m_contract; // re-usable, persistant contract scratchpad

  time_duration m_dtMarketOpen;
  time_duration m_dtMarketOpeningOrder;
  time_duration m_dtMarketClosingOrder;
  time_duration m_dtMarketClose;

  // straddle
  int m_nCalls;
  int m_nPuts;

  // delta neutal long call, long put
  int m_nLongPut;
  int m_nLongUnderlying;

  double m_dblCallPrice;
  double m_dblPutPrice;
  double m_dblUnderlyingPrice;

  pPortfolio_t m_pPortfolio;

  pPosition_t m_posPut;
  pPosition_t m_posUnderlying;

  void HandleOnExecConnected( int );
  void HandleOnExecDisconnected( int );

  void HandleOnDataConnected( int );
  void HandleOnDataDisconnected( int );

  void HandleOnData2Connected( int );
  void HandleOnData2Disconnected( int );

  void HandleStrikeListing1( const ContractDetails& );  // listing of strikes
  void HandleStrikeListing1Done( void );
  void HandleStrikeListing2( const ContractDetails& );  // underlying contract
  void HandleStrikeListing2Done( void );
  void HandleStrikeListing3( const ContractDetails& );  // symbols for Calls
  void HandleStrikeListing3Done( void );
  void HandleStrikeListing4( const ContractDetails& );  // symbols for puts
  void HandleStrikeListing4Done( void );

  void HandleUnderlyingQuote( const CQuote& quote );
  void HandleUnderlyingTrade( const CTrade& trade );  // handles trade state machine

  void HandleTSFirstPass( const CTrade& trade );
  void HandleTSPreMarket( const CTrade& trade );
  void HandleTSMarketOpened( const CTrade& trade );
  void HandleTSOpeningOrder( const CTrade& trade );
  void HandleTSTrading( const CTrade& trade );
  void HandleTSCloseOrders( const CTrade& trade );
  void HandleAfterMarket( const CTrade& trade );

  void OpenPosition( void );
  void ClosePosition( void );
  void PrintGreeks( void );

};
