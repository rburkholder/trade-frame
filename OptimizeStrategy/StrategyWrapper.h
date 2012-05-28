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

#include <TFSimulation/SimulationProvider.h>
#include <TFTrading/Instrument.h>

#include "StrategyEquity.h"

class StrategyWrapper {
public:

  typedef ou::tf::CInstrument::pInstrument_t pInstrument_t;

  StrategyWrapper(void);
  ~StrategyWrapper(void);

  void Start( pInstrument_t pInstrument, const std::string& sSourcePath );
  void Stop( void );

protected:
private:

  typedef ou::tf::CSimulationProvider::pProvider_t pProviderSim_t;

  bool m_bRunning;

  pProviderSim_t m_pProvider;

  StrategyEquity* m_pStrategy;

  void HandleProviderConnected( int );
  void HandleProviderDisconnected( int );

  void HandleSimulationComplete( void );
};

