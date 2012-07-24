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

#include <boost/date_time/posix_time/posix_time.hpp>

#include <OUCommon/FastDelegate.h>

#include <TFSimulation/SimulationProvider.h>
#include <TFTrading/Portfolio.h>
#include <TFTrading/Position.h>
#include <TFTrading/Instrument.h>

#include <TFTimeSeries/TimeSeries.h>

#include <TFIndicators/ZigZag.h>
#include <TFIndicators/TSEMA.h>

#include <TFGP/TimeSeriesRegistration.h>

class StrategyEquity {
public:

  typedef ou::tf::CSimulationProvider::pProvider_t pProviderSim_t;
  typedef ou::tf::CInstrument::pInstrument_t pInstrument_t;
  typedef fastdelegate::FastDelegate0<bool> fdEvaluate_t;

  StrategyEquity( pProviderSim_t pProvider, pInstrument_t pInstrument );
  ~StrategyEquity(void);

  void Set( fdEvaluate_t pfnLong, fdEvaluate_t pfnShort );
  void Start( void );  // for simulation
  double GetPL( void );
  void Stop( void );

protected:
private:

  typedef ou::tf::CPosition::pOrder_t pOrder_t;
  typedef ou::tf::CPosition::pPosition_t pPosition_t;
  typedef ou::tf::CInstrument::pInstrument_t pInstrument_t;

  enum enumTimeFrames { EPreOpen, EBellHeard, EPauseForQuotes, EAfterBell, ETrading, ECancelling, EGoingNeutral, EClosing, EAfterHours };
  enum enumTradeState { ENeutral, ELong, EShort };

  enumTimeFrames m_stateTimeFrame;
  enumTradeState m_stateTrading;

  fdEvaluate_t m_pfnLong;
  fdEvaluate_t m_pfnShort;

  ou::tf::Quotes m_quotes;
  ou::tf::Trades m_trades;

  time_duration m_timeOpeningBell, m_timeCancel, m_timeClose, m_timeClosingBell;

  pInstrument_t m_pUnderlying;
  std::string m_sUnderlying;

  pProviderSim_t m_pProvider;

  ou::tf::CPortfolio m_portfolio;

//  pPosition_t m_pPositionLong;
//  pPosition_t m_pPositionShort;
  pPosition_t m_pPosition;

  pOrder_t m_pOrder;  // active order

  ou::tf::ZigZag m_zigzagPrice;  // provides a basis for maximizing profitability, crossover is some fraction of previous day's average range

  ou::tf::hf::TSEMA<ou::tf::Quote> m_emaQuotes1;
  ou::tf::hf::TSEMA<ou::tf::Quote> m_emaQuotes2;
  ou::tf::hf::TSEMA<ou::tf::Quote> m_emaQuotes3;

  void Register( ou::tf::Prices* series );
  void Register( ou::tf::Quotes* series );
  void Register( ou::tf::Trades* series );

  ou::gp::TimeSeriesRegistration<ou::tf::Prices> m_RegisteredPrices;
  ou::gp::TimeSeriesRegistration<ou::tf::Quotes> m_RegisteredQuotes;
  ou::gp::TimeSeriesRegistration<ou::tf::Trades> m_RegisteredTrades;

  void Trade( void );

  void HandleQuote( const ou::tf::Quote& );
  void HandleTrade( const ou::tf::Trade& );

};

