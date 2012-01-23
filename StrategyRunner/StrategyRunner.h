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

#include <TFBitsNPieces/FrameWork01.h>

#include "FrameMain.h"

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

  virtual bool OnInit();
  virtual int OnExit();

};

// Implements MyApp& wxGetApp()
DECLARE_APP(AppStrategyRunner)

