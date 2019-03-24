/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

// Started 2013/09/22

#include <thread>

#include <TFBitsNPieces/FrameWork01.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>

#include "SignalGenerator.h"

class AppWeeklies:
  public wxApp, public ou::tf::FrameWork01<AppWeeklies> {
    friend ou::tf::FrameWork01<AppWeeklies>;
public:
protected:
private:

  std::thread m_worker;

  SignalGenerator m_sg;

  FrameMain* m_pFrameMain;
  ou::tf::PanelLogging* m_pPanelLogging;

  virtual bool OnInit();
  virtual int OnExit();
  void OnClose( wxCloseEvent& event );

  void HandleMenuActionRunScan( void );

};

DECLARE_APP(AppWeeklies)

