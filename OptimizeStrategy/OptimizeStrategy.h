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

// Started 2012/05/27

// Deals with user interface and population control
// Separate population control out into an optimizer at some point int time

//#include <TFBitsNPieces/FrameWork01.h>

#include <TFTrading/ProviderManager.h>
#include <TFTrading/InstrumentManager.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>

#include "StrategyWrapper.h"

class AppOptimizeStrategy: public wxApp {
public:
protected:
private:

  typedef ou::tf::CInstrument::pInstrument_t pInstrument_t;
  typedef ou::tf::CProviderInterfaceBase::pProvider_t pProvider_t;

  FrameMain* m_pFrameMain;
  ou::tf::PanelLogging* m_pPanelLogging;

  pInstrument_t m_pInstrument;

  StrategyWrapper* m_swStrategy;  // will ultimately need one per simultaneous individual running from population

  virtual bool OnInit();
  virtual int OnExit();

  void HandlePopulateDatabase( void );

  void HandleBtnStart( void );
  void HandleBtnStop( void );

};

// Implements MyApp& wxGetApp()
DECLARE_APP(AppOptimizeStrategy)

