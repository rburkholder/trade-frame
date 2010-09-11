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

#include <boost/shared_ptr.hpp>

#include <LibTimeSeries/DatedDatum.h>
#include <LibTimeSeries/TimeSeries.h>

#include <LibTrading/Instrument.h>
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

  typedef CInstrument::pInstrument_t pInstrument_t;

  CNakedOption( pInstrument_t pInstrument );
  CNakedOption( const CNakedOption& rhs );
  virtual ~CNakedOption( void ) {};

  CNakedOption& operator=( const CNakedOption& rhs );

  bool operator< ( const CNakedOption& rhs ) const { return m_dblStrike <  rhs.m_dblStrike; };
  bool operator<=( const CNakedOption& rhs ) const { return m_dblStrike <= rhs.m_dblStrike; };

  double Strike( void ) { return m_dblStrike; };
  pInstrument_t GetInstrument( void ) { return m_pInstrument; };

  void HandleQuote( const CQuote& quote );
  void HandleTrade( const CTrade& trade );
  void HandleGreek( const CGreek& greek );

  double Bid( void ) const { return m_dblBid; };
  double Ask( void ) const { return m_dblAsk; };

  double ImpliedVolatility( void ) const { return m_greek.ImpliedVolatility(); };
  double Delta( void ) const { return m_greek.Delta(); };
  double Gamma( void ) const { return m_greek.Gamma(); };
  double Theta( void ) const { return m_greek.Theta(); };
  double Vega( void ) const { return m_greek.Vega(); };

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

  pInstrument_t m_pInstrument;

  std::stringstream m_ss;

private:
};

//
// ==================
//

class CNakedCall: public CNakedOption
{
public:
  CNakedCall( pInstrument_t pInstrument );
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
  CNakedPut( pInstrument_t pInstrument );
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

  void AssignCall( CInstrument::pInstrument_t pInstrument ) { assert( 0 == m_call.use_count() ); m_call.reset( new CNakedCall( pInstrument ) ); };
  void AssignPut( CInstrument::pInstrument_t pInstrument )  { assert( 0 == m_put.use_count() );  m_put.reset( new CNakedPut( pInstrument ) ); };

  CNakedCall* Call( void ) { return m_call.get(); };
  CNakedPut*  Put( void )  { return m_put.get(); };

protected:

  boost::shared_ptr<CNakedCall> m_call;
  boost::shared_ptr<CNakedPut>  m_put;

private:
  std::stringstream m_ss;
  bool m_bWatching;  // this needs to be implemented.
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

  enum enumMode {
    EModeSimulation,
    EModeLive
  } m_eMode;

  enum enumDataConnection {
    EDCSim,
    EDCIQFeed,
    EDCIB
  } m_eDataConn;


  CProcess(void);
  ~CProcess(void);

  void SetMode( enumMode );
  void SetDataConnection( enumDataConnection );

  void SimConnect( void );
  void SimStart( void );
  void SimStop( void );
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

  typedef CInstrument::pInstrument_t pInstrument_t;

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
  pInstrument_t m_pUnderlying;

  std::string m_sPathForSeries;
  std::string m_sDesiredSimTradingDay;
  bool m_bProcessSimTradingDayGroup;
  //enum enumTimeSeriesType {
  //  EUnknown, EQuotes, ETrades, EGreeks
  //} m_stateTimeSeries;

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
  
  CQuotes m_quotes;
  CTrades m_trades;

  CBar m_Bar;  // keep pointer for when data arrives

  double m_dblBaseDelta; // keep trades balanced at this level
  double m_dblBaseDeltaIncrement;

  // map used in simulation symbol creation
  typedef std::pair<pInstrument_t,pInstrument_t> option_pair_t;  // call, put
  typedef std::map<double,option_pair_t> strikes_map_t;
  typedef strikes_map_t::iterator strikes_iterator_t;
  strikes_map_t m_mapStrikes;

  typedef std::vector<CStrikeInfo> vStrikeInfo_t;
  typedef vStrikeInfo_t::iterator vStrikeInfo_iter_t;

  std::vector<double> m_vCrossOverPoints;  // has pivots and strikes in order
  vStrikeInfo_t m_vStrikes;  // put/call info for each strike
  vStrikeInfo_iter_t m_iterStrikes;

  std::vector<double>::iterator m_iterAboveCrossOver;
  std::vector<double>::iterator m_iterBelowCrossOver;

  vStrikeInfo_iter_t m_iterOILowestWatch;
  vStrikeInfo_iter_t m_iterOIHighestWatch;
  vStrikeInfo_iter_t m_iterOILatestGammaSelectCall;
  vStrikeInfo_iter_t m_iterOILatestGammaSelectPut;

  Contract m_contract; // re-usable, persistant contract scratchpad

  time_duration m_dtMarketOpen;
  time_duration m_dtMarketOpeningOrder;
  time_duration m_dtMarketClosingOrder;
  time_duration m_dtMarketClose;

  // straddle
  int m_nCalls;
  int m_nPuts;

  // delta neutral long call, long put
  int m_nLongPut;
  int m_nLongUnderlying;

  double m_dblDeltaTotalPut;
  double m_dblDeltaTotalUnderlying;
  bool m_bWaitingForTradeCompletion;

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

  void AcquireSimulationSymbols( void );
  void HandleHDF5Object( const std::string& sPath, const std::string& sName);
  void HandleHDF5Group( const std::string& sPath, const std::string& sName );

  void HandleStrikeListing1( const ContractDetails& );  // underlying
  void HandleStrikeListing1Done( void );
  void HandleStrikeListing2( const ContractDetails& );  // symbols for Calls
  void HandleStrikeListing2Done( void );
  void HandleStrikeListing3( const ContractDetails& );  // symbols for Puts
  void HandleStrikeListing3Done( void );

  void HandleUnderlyingQuote( const CQuote& quote );
  void HandleUnderlyingTrade( const CTrade& trade );  // handles trade state machine

  void HandleTSFirstPass( const CQuote& quote );
  void HandleTSPreMarket( const CQuote& quote );
  void HandleTSMarketOpened( const CQuote& quote );
  void HandleTSOpeningOrder( const CQuote& quote );
  void HandleTSTrading( const CQuote& quote );
  void HandleTSCloseOrders( const CQuote& quote );
  void HandleAfterMarket( const CQuote& quote );

  void HandlePositionExecution( const std::pair<const CPosition*, const CExecution&>& );

  void OpenPosition( void );
  void ClosePosition( void );
  void PrintGreeks( void );

};
