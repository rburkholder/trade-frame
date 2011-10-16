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

#include <boost/thread.hpp>  // separate thread for asio run processing
#include <boost/asio.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;

#include <TFInteractiveBrokers/IBTWS.h>
#include <TFIQFeed/IQFeedProvider.h>
#include <TFTimeSeries/TimeSeries.h>
#include <TFTrading/InstrumentManager.h>
#include <TFTrading/ProviderManager.h>
#include <TFTrading/OrderManager.h>
#include <TFTrading/MarketStates.h>
#include <TFIndicators/TSSWStochastic.h>
#include <TFIndicators/TSSWStats.h>
#include <TFTrading/Position.h>

struct InstrumentState {

  InstrumentState( void );
  ~InstrumentState( void ) {};

  ou::tf::CQuotes quotes;
  ou::tf::CTrades trades;

  double dblMidQuoteAtOpen;
  double dblOpeningTrade;

  ou::tf::TSSWStochastic stochFast;
  ou::tf::TSSWStochastic stochMed;
  ou::tf::TSSWStochastic stochSlow;

  ou::tf::TSSWStatsMidQuote statsFast;
  ou::tf::TSSWStatsMidQuote statsMed;
  ou::tf::TSSWStatsMidQuote statsSlow;

  time_duration tdMarketOpen;
  time_duration tdMarketOpenIdle;
  //time_duration tdMarketTrading;
  time_duration tdCancelOrders;
  time_duration tdClosePositions;
  time_duration tdAfterMarket;
  time_duration tdMarketClosed;

  ptime dtPreTradingStop;

  ou::tf::CPosition::pPosition_t pPosition;

  bool bMarketHoursCrossMidnight;
  bool bDaySession;
};


class App {
public:
  App(void);
  ~App(void);

  void Run( void );

  struct StateInitialization;
  struct StatePreMarket;
  typedef ou::tf::MachineMarketStates<InstrumentState, StateInitialization> MachineMarketStates;
  MachineMarketStates m_md;  // market data state chart
  
  typedef ou::tf::EvQuote EvQuote;
  typedef ou::tf::EvTrade EvTrade;
  struct StateInitialization: ou::tf::StateInitialization<StateInitialization, MachineMarketStates, StatePreMarket> {};
  struct StatePreMarket: ou::tf::StateBase<MachineMarketStates, StatePreMarket> {
    using ou::tf::StateBase<MachineMarketStates, StatePreMarket>::Handle;
    sc::result Handle( const EvQuote& ); 
    sc::result Handle( const EvTrade& );
  };
  struct StateMarketOpen: ou::tf::StateBase<MachineMarketStates, StateMarketOpen> {
    using ou::tf::StateBase<MachineMarketStates, StateMarketOpen>::Handle;
//    sc::result Handle( const EvQuote& ); 
    sc::result Handle( const EvTrade& );
  };
  struct StatePreTrading: ou::tf::StateBase<MachineMarketStates, StatePreTrading> {
    using ou::tf::StateBase<MachineMarketStates, StatePreTrading>::Handle;
    sc::result Handle( const EvQuote& ); 
  };
  struct StateZeroPosition;
  struct StateTrading: ou::tf::StateBase<MachineMarketStates, StateTrading, StateZeroPosition> {
    using ou::tf::StateBase<MachineMarketStates, StateTrading, StateZeroPosition>::Handle;
//    sc::result Handle( const EvQuote& ); // not called, goes to inner directly
  };
  struct StateCancelOrders: ou::tf::StateBase<MachineMarketStates, StateCancelOrders> {
    using ou::tf::StateBase<MachineMarketStates, StateCancelOrders>::Handle;
    sc::result Handle( const EvQuote& ); 
  };
  struct StateCancelOrdersIdle: ou::tf::StateBase<MachineMarketStates, StateCancelOrdersIdle> {
    using ou::tf::StateBase<MachineMarketStates, StateCancelOrdersIdle>::Handle;
    sc::result Handle( const EvQuote& ); 
  };
  struct StateClosePositions: ou::tf::StateBase<MachineMarketStates, StateClosePositions> {
    using ou::tf::StateBase<MachineMarketStates, StateClosePositions>::Handle;
    sc::result Handle( const EvQuote& ); 
  };
  struct StateClosePositionsIdle: ou::tf::StateBase<MachineMarketStates, StateClosePositionsIdle> {
    using ou::tf::StateBase<MachineMarketStates, StateClosePositionsIdle>::Handle;
    sc::result Handle( const EvQuote& ); 
  };
  struct StateAfterMarket: ou::tf::StateBase<MachineMarketStates, StateAfterMarket> {
    using ou::tf::StateBase<MachineMarketStates, StateAfterMarket>::Handle;
    sc::result Handle( const EvQuote& ); 
  };
  struct StateMarketClosed: ou::tf::StateBase<MachineMarketStates, StateMarketClosed> {
    using ou::tf::StateBase<MachineMarketStates, StateMarketClosed>::Handle;
    sc::result Handle( const EvQuote& ); 
  };

  // these three states determine trading pattern
  struct StateZeroPosition: ou::tf::StateBase<StateTrading, StateZeroPosition> {
    using ou::tf::StateBase<StateTrading, StateZeroPosition>::Handle;
    sc::result Handle( const EvQuote& ); 
  };
  struct StateLong: ou::tf::StateBase<StateTrading, StateLong> {
    using ou::tf::StateBase<StateTrading, StateLong>::Handle;
    sc::result Handle( const EvQuote& ); 
  };
  struct StateShort: ou::tf::StateBase<StateTrading, StateShort> {
    using ou::tf::StateBase<StateTrading, StateShort>::Handle;
    sc::result Handle( const EvQuote& ); 
  };
  
protected:
private:

  typedef ou::tf::CProviderManager::pProvider_t pProvider_t;
  typedef ou::tf::CInstrumentManager::pInstrument_t pInstrument_t;

  ou::tf::CIBTWS::pProvider_t m_ptws;
  ou::tf::CIQFeedProvider::pProvider_t m_piqfeed;

  boost::thread m_asioThread;
  boost::asio::io_service m_io;
  boost::asio::io_service::work* m_pwork;

  ou::tf::CInstrumentManager& m_mgrInstrument;

  ou::tf::CInstrument::pInstrument_t m_pInstrument;

  //ou::tf::CQuotes m_quotes;
  //ou::tf::CTrades m_trades;

  void WorkerThread( void ); // worker thread

  void Connected( int i );
  void DisConnected( int i );

  void StartWatch( void );
  void StopWatch( void );

  // will need to migrate to a container when doing more than one instrument
  void HandleQuote( const ou::tf::CQuote& quote );
  void HandleTrade( const ou::tf::CTrade& trade );
  void HandleOpen( const ou::tf::CTrade& trade );

  void HandleIBContractDetails( const ou::tf::CIBTWS::ContractDetails& details, const pInstrument_t& pInstrument );
  void HandleIBContractDetailsDone( void );
};

