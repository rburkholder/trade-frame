/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#include <TFOptions/CalcExpiry.h>
#include <TFOptions/Option.h>
#include <TFOptions/Strike.h>

#include <TFTimeSeries/DatedDatum.h>
#include <TFTimeSeries/TimeSeries.h>

#include <TFTrading/PortfolioManager.h>
#include <TFTrading/ProviderManager.h>

#include <TFIQFeed/IQFeedHistoryQuery.h>  // seems to be a header ordering dependancy
#include <TFIQFeed/IQFeedProvider.h>  // includes CPortfolio and CPosition

#include <TFInteractiveBrokers/IBTWS.h>

#include <TFSimulation/SimulationProvider.h>

#include "HedgeCommon.h"
#include "DB.h"

using namespace ou::tf;

//
// ==================
//

class CProcess: 
  public HistoryQuery<CProcess>
{
  friend HistoryQuery<CProcess>;
public:

  enum enumDataConnection {
    EDCSim,
    EDCIQFeed,
    EDCIB
  } m_eDataConn;


  CProcess(enumMode eMode, DB& db);
  ~CProcess(void);

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

  void EnterTrade( const std::string& sSymbol );
  void PauseTrade( void );
  void ExitTrade( void );

protected:

  void OnHistoryConnected( void );
  void OnHistorySummaryData( structSummary* pDP );
  void OnHistoryRequestDone( void );
  void OnHistoryDisconnected( void ) {};

private:

  typedef CPortfolio::pPortfolio_t pPortfolio_t;
  typedef CPosition::pPosition_t pPosition_t;

  typedef ou::tf::keytypes::idInstrument_t idInstrument_t;
  typedef Instrument::pInstrument_t pInstrument_t;

  typedef ProviderInterfaceBase::pProvider_t pProvider_t;

  typedef IBTWS::pProvider_t pProviderIBTWS_t;
  typedef IQFeedProvider::pProvider_t pProviderIQFeed_t;
  typedef SimulationProvider::pProvider_t pProviderSim_t;

  typedef ou::tf::option::Strike CStrikeInfo;

  typedef double strike_t;

  enumMode m_eMode;

  enum enumTradingState {  // arranged in chronological order
    ETSFirstPass,     // any state initialization
    ETSPreMarket,     // time frame before Markets open
    ETSMarketOpened,  // initialize anything on market open
    ETSFirstTrade,    // create opening trade at correct time
    ETSTrading,       // allow active trading
    ETSCloseOrders,   // close trades
    ETSAfterMarket    // after market closes
  } m_TradingState;

  boost::gregorian::date m_dExpiry;

  idInstrument_t m_sSymbolName;
  long m_contractidUnderlying;
  pInstrument_t m_pUnderlying;

  std::string m_sPathForSeries;
  std::string m_sDesiredSimTradingDay;
  bool m_bProcessSimTradingDayGroup;

  std::stringstream m_ss;

  // db stuff

  DB& m_db;

  // end db stuff

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

  bool m_bExecConnected;
  bool m_bDataConnected;
  bool m_bData2Connected;
  bool m_bConnectDone;

  bool m_bPositionsOpened;
  
  Quotes m_quotes;
  Trades m_trades;

  Bar m_Bar;  // keep pointer for when data arrives

  double m_dblBaseDelta; // keep trades balanced at this level
  double m_dblBaseDeltaIncrement;

  // map used in simulation symbol creation
  typedef std::pair<pInstrument_t,pInstrument_t> option_pair_t;  // call, put
  typedef std::map<double,option_pair_t> strikes_map_t;
  typedef strikes_map_t::iterator strikes_iterator_t;
  strikes_map_t m_mapStrikes;

  std::vector<double> m_vCrossOverPoints;  // has combination of pivots and strikes in order
  std::vector<double>::iterator m_iterAboveCrossOver;
  std::vector<double>::iterator m_iterBelowCrossOver;

  typedef std::map<double,CStrikeInfo> mapStrikeInfo_t;
  typedef std::pair<double,CStrikeInfo> mapStrikeinfo_pair_t;
  typedef mapStrikeInfo_t::iterator mapStrikeInfo_iter_t;
  mapStrikeInfo_t m_mapStrikeInfo;

  mapStrikeInfo_iter_t m_iterOILowestWatch;  // 15 watches, balance low and high option watch around opening of underlying
  mapStrikeInfo_iter_t m_iterOIHighestWatch;
  mapStrikeInfo_iter_t m_iterOILatestGammaSelectCall;
  mapStrikeInfo_iter_t m_iterOILatestGammaSelectPut;

  Contract m_contract; // re-usable, persistant contract scratchpad

  time_duration m_dtMarketOpen;
  time_duration m_dtMarketOpeningOrder;
  time_duration m_dtMarketClosingOrder;
  time_duration m_dtMarketClose;

//  double m_dblCallPrice;
  double m_dblPutPrice;
  double m_dblUnderlyingPrice;

  ou::tf::keytypes::idPortfolio_t m_idPortfolio;
  pPortfolio_t m_pPortfolio;

  pPosition_t m_posPut;
  pPosition_t m_posUnderlying;

  void HandleOnExecConnected( int );
  void HandleOnExecDisconnected( int );

  void HandleOnDataConnected( int );
  void HandleOnDataDisconnected( int );

  void HandleOnConnected( int );

  void HandleOnData2Connected( int );
  void HandleOnData2Disconnected( int );

  void AcquireSimulationSymbols( void );
  void HandleHDF5Object( const std::string& sPath, const std::string& sName);
  void HandleHDF5Group( const std::string& sPath, const std::string& sName );

  void HandleUnderlyingListing( const ContractDetails&, const pInstrument_t& );  // underlying
  void HandleUnderlyingListingDone( void );
  void HandleStrikeFromIB( const ContractDetails& details, const pInstrument_t& );  // symbols for options
  void HandleStrikeFromDb( pInstrument_t );
  void HandleStrikeListingDone( void );

  void HandleUnderlyingQuote( const Quote& quote );
  void HandleUnderlyingTrade( const Trade& trade );  // handles trade state machine

  void HandleTSFirstPass( const Quote& quote );
  void HandleTSPreMarket( const Quote& quote );
  void HandleTSMarketOpened( const Quote& quote );
  void HandleTSActiveMarketStart( const Quote& quote );
  void HandleTSTrading( const Quote& quote );
  void HandleTSCloseOrders( const Quote& quote );
  void HandleAfterMarket( const Quote& quote );

  void HandlePositionExecution( CPosition::execution_delegate_t pair );

  void HandlePopulateDatabase( void ) ;

  void AddOptionToStrikeInfo( pInstrument_t );
  mapStrikeInfo_iter_t LocateOptionStrikeInfo( const pInstrument_t& pInstrument );

  void CalculateHighGammaOption( void );
  void OpenPositions( void );
  void ClosePosition( void );
  void PrintGreeks( void );

};
