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

// The actual strategy, try and get as close to normal strategy as possible,
//  ie, similar to what is in StrategyRunner

// also in OPtimizeStrategy.cpp
#define FUSION_MAX_VECTOR_SIZE 13
//#define FUSION_MAX_SET_SIZE 13

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/fusion/container/set.hpp>
#include <boost/fusion/include/set.hpp>
#include <boost/fusion/container/set/set_fwd.hpp>
#include <boost/fusion/include/set_fwd.hpp>

#include <OUCommon/FastDelegate.h>

#include <TFSimulation/SimulationProvider.h>
#include <TFTrading/Portfolio.h>
#include <TFTrading/Position.h>
#include <TFTrading/Instrument.h>

#include <TFTimeSeries/TimeSeries.h>

#include <TFIndicators/ZigZag.h>
#include <TFIndicators/TSEMA.h>
#include <TFIndicators/TSDifferential.h>

#include <TFGP/NodeTimeSeries.h>
#include <TFGP/TimeSeriesRegistration.h>

class StrategyEquity {
public:

  typedef ou::tf::SimulationProvider::pProvider_t pProviderSim_t;
  typedef ou::tf::Instrument::pInstrument_t pInstrument_t;
  typedef fastdelegate::FastDelegate0<bool> fdEvaluate_t;
  typedef boost::fusion::set<
      ou::gp::TimeSeriesRegistration<ou::tf::Quotes>,
      ou::gp::TimeSeriesRegistration<ou::tf::Trades>,
      ou::gp::TimeSeriesRegistration<ou::tf::Prices>
    > registrations_t;
  typedef boost::fusion::set<
    ou::gp::IndexedNode<ou::gp::NodeTSQuoteBid,0>, // main quotes
    ou::gp::IndexedNode<ou::gp::NodeTSQuoteMid,0>, // main quotes
    ou::gp::IndexedNode<ou::gp::NodeTSQuoteAsk,0>, // main quotes
    ou::gp::IndexedNode<ou::gp::NodeTSTrade,0>, // main trades
    ou::gp::IndexedNode<ou::gp::NodeTSPrice,0>, // ema1
    ou::gp::IndexedNode<ou::gp::NodeTSPrice,1>, // ema2
    ou::gp::IndexedNode<ou::gp::NodeTSPrice,2>, // ema3
    ou::gp::IndexedNode<ou::gp::NodeTSPrice,3>, // ema1
    ou::gp::IndexedNode<ou::gp::NodeTSPrice,4>, // ema2
    ou::gp::IndexedNode<ou::gp::NodeTSPrice,5>, // ema3
    ou::gp::IndexedNode<ou::gp::NodeTSPrice,6>, // ema1
    ou::gp::IndexedNode<ou::gp::NodeTSPrice,7>, // ema2
    ou::gp::IndexedNode<ou::gp::NodeTSPrice,8>  // ema3
  > NodeTypesTimeSeries_t;

  StrategyEquity( pProviderSim_t pProvider, pInstrument_t pInstrument, const boost::gregorian::date& dateStart );
  ~StrategyEquity(void);

  void Init( 
    StrategyEquity::registrations_t& registrations,
    fdEvaluate_t pfnLong, fdEvaluate_t pfnShort );  // for simulation
  double GetPL( std::stringstream& );
  void End( void );

protected:
private:

  typedef ou::tf::Position::pOrder_t pOrder_t;
  typedef ou::tf::Position::pPosition_t pPosition_t;
  typedef ou::tf::Instrument::pInstrument_t pInstrument_t;

  enum enumTimeFrames { EPreOpen, EBellHeard, EPauseForQuotes, EAfterBell, ETrading, ECancelling, EGoingNeutral, EClosing, EAfterHours };
  enum enumTradeState { ENeutral, ELong, EShort };

  enumTimeFrames m_stateTimeFrame;
  enumTradeState m_stateTrading;

  fdEvaluate_t m_pfnLong;
  fdEvaluate_t m_pfnShort;

  ou::tf::Quotes m_quotes;
  ou::tf::Trades m_trades;

  time_duration m_timeOpeningBell, m_timeCancelTrades, m_timeClosePositions, m_timeClosingBell;
  ptime m_dtOpeningBell, m_dtStartTrading, m_dtCancelTrades, m_dtClosePositions, m_dtClosingBell;

  pInstrument_t m_pUnderlying;
  std::string m_sUnderlying;

  pProviderSim_t m_pProvider;

  ou::tf::Portfolio m_portfolio;

  pPosition_t m_pPosition;

  pOrder_t m_pOrder;  // active order

  ou::tf::ZigZag m_zigzagPrice;  // provides a basis for maximizing profitability, crossover is some fraction of previous day's average range

  ou::tf::hf::TSEMA<ou::tf::Quote> m_emaQuotes1;
  ou::tf::hf::TSEMA<ou::tf::Quote> m_emaQuotes2;
  ou::tf::hf::TSEMA<ou::tf::Quote> m_emaQuotes3;

  ou::tf::hf::TSDifferential m_Ema1Dif1;
  ou::tf::hf::TSDifferential m_Ema2Dif1;
  ou::tf::hf::TSDifferential m_Ema3Dif1;

  ou::tf::hf::TSDifferential m_Ema1Dif2;
  ou::tf::hf::TSDifferential m_Ema2Dif2;
  ou::tf::hf::TSDifferential m_Ema3Dif2;

  void Register( registrations_t&, ou::tf::Quotes* series );
  void Register( registrations_t&, ou::tf::Trades* series );
  void Register( registrations_t&, ou::tf::Prices* series );

  void Trade( void );

  void HandleQuote( const ou::tf::Quote& );
  void HandleTrade( const ou::tf::Trade& );

};

