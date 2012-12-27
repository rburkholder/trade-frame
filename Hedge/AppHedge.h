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

#include <TFBitsNPieces/FrameWork01.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>

#include "ThreadMain.h"

class AppHedge : public wxApp, public ou::tf::FrameWork01<AppHedge> {
  friend ou::tf::FrameWork01<AppHedge>;
public:
  virtual bool OnInit();
  virtual int OnExit();
protected:
private:

  FrameMain* m_pFrameMain;
//  PanelOptionsParameters* m_pPanelOptionsParameters;
  ou::tf::PanelLogging* m_pPanelLogging;

  ThreadMain* m_pThreadMain;
};
 
// Implements MyApp& wxGetApp()
DECLARE_APP(AppHedge)