/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

#include "FrameMain.h"
#include "PanelOptionsParameters.h"
#include "StrategyTradeOptions.h"

class AppStrategyRunner:
  public wxApp, public ou::tf::FrameWork01<AppStrategyRunner> {
public:
protected:
private:

  typedef ou::tf::CProviderInterfaceBase::pProvider_t pProvider_t;
  typedef ou::tf::eProviderState_t eProviderState_t;

  typedef ou::tf::CIBTWS::pProvider_t pProviderIBTWS_t;
  typedef ou::tf::CIQFeedProvider::pProvider_t pProviderIQFeed_t;
  typedef ou::tf::CSimulationProvider::pProvider_t pProviderSim_t;

  FrameMain* m_pFrameMain;
  PanelOptionsParameters* m_pPanelOptionsParameters;

  DBOps m_db;

  StrategyTradeOptions* m_pStrategyTradeOptions;

  virtual bool OnInit();
  virtual int OnExit();

  void HandlePopulateDatabase( void );
  void HandleBtnStart( void );

};

// Implements MyApp& wxGetApp()
DECLARE_APP(AppStrategyRunner)

