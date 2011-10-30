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

#include <string>

#include <TFInteractiveBrokers/IBTWS.h>
// history comes before provider
#include <TFIQFeed/IQFeedHistoryQuery.h>
#include <TFIQFeed/IQFeedProvider.h>
#include <TFTrading/InstrumentManager.h>
#include <TFTrading/MarketStates.h>

#include "InstrumentState.h"

class Operation {
public:

  struct structSymbolInfo {
    std::string sName;
    double S3, S2, S1, PV, R1, R2, R3;
    double dblClose;
  };

  Operation( const structSymbolInfo& si, ou::tf::CIQFeedProvider::pProvider_t, ou::tf::CIBTWS::pProvider_t );
  ~Operation(void);

  unsigned int CalcShareCount( double dblFunds );
  bool& ToBeTraded( void ) { return m_bToBeTraded; };
  void Start( double dblAmountToTrade );
  void SaveSeries( const std::string& sPrefix );
  void Stop( void );

  // ===================== State Chart
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
  // ===================== State Chart End
  
protected:
private:

  typedef ou::tf::CIBTWS::pInstrument_t pInstrument_t;

  structSymbolInfo m_si;
  ou::tf::CIBTWS::pProvider_t m_ptws;
  ou::tf::CIQFeedProvider::pProvider_t m_piqfeed;

  ou::tf::CInstrument::pInstrument_t m_pInstrument;

  bool m_bToBeTraded;

  void StartWatch( void );
  void StopWatch( void );

  void HandleIBContractDetails( const ou::tf::CIBTWS::ContractDetails& details, const pInstrument_t& pInstrument );
  void HandleIBContractDetailsDone( void );

  // will need to migrate to a container when doing more than one instrument
  void HandleQuote( const ou::tf::CQuote& quote );
  void HandleTrade( const ou::tf::CTrade& trade );
  void HandleOpen( const ou::tf::CTrade& trade );
};

