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

// wraps strategy to allow multiple self contained instances for optimization by Genetic Programming 
// contains instance of simulator, strategy and related wrapper stuff
// rewrite sometime to form basis of generalized optimization tool

#include <TFSimulation/SimulationProvider.h>
#include <TFTrading/Instrument.h>

#include "StrategyEquity.h"

class StrategyWrapper {
public:

  typedef ou::tf::CInstrument::pInstrument_t pInstrument_t;
  typedef StrategyEquity::fdEvaluate_t fdEvaluate_t;

  StrategyWrapper(void);
  ~StrategyWrapper(void);

  void Init( 
    StrategyEquity::registrations_t& registrations,
    pInstrument_t pInstrument, 
    const boost::gregorian::date& dateStart, 
    const std::string& sSourcePath, 
    fdEvaluate_t pfnLong, fdEvaluate_t pfnShort );
  void Start( void );
  double GetPL( void );
//  void Stop( void );

protected:
private:

  typedef ou::tf::CSimulationProvider::pProvider_t pProviderSim_t;

  date m_dtStart;

  bool m_bRunning;

  pProviderSim_t m_pProvider;
  pInstrument_t m_pInstrument;

  StrategyEquity* m_pStrategy;

  void HandleProviderConnected( int );
  void HandleProviderDisconnected( int );

  void HandleSimulationComplete( void );
};

