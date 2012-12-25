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

//#include "resource.h"

// Started 2012/01/22

#include <TFBitsNPieces/FrameWork01.h>

// may need to inherit and add more functionality to the class:
#include <TFTrading/DBOps.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>

#include "PanelOptionsParameters.h"
#include "StrategyTradeOptions.h"

class AppStrategyRunner:
  public wxApp, public ou::tf::FrameWork01<AppStrategyRunner> {
public:
protected:
private:

  typedef ou::tf::ProviderInterfaceBase::pProvider_t pProvider_t;
  typedef ou::tf::eProviderState_t eProviderState_t;

  typedef ou::tf::IBTWS::pProvider_t pProviderIBTWS_t;
  typedef ou::tf::IQFeedProvider::pProvider_t pProviderIQFeed_t;
  typedef ou::tf::SimulationProvider::pProvider_t pProviderSim_t;

  FrameMain* m_pFrameMain;
  PanelOptionsParameters* m_pPanelOptionsParameters;
  ou::tf::PanelLogging* m_pPanelLogging;

  DBOps m_db;

  StrategyTradeOptions* m_pStrategyTradeOptions;

  std::string m_sTimeSamplingStarted;

  virtual bool OnInit();
  virtual int OnExit();

  void HandlePopulateDatabase( void );
  void HandleBtnStart( void );
  void HandleBtnStop( void );
  void HandleBtnSave( void );

};

// Implements MyApp& wxGetApp()
DECLARE_APP(AppStrategyRunner)

