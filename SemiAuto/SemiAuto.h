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

#include <wx/wx.h>

#include <TFTrading/PortfolioManager.h>
#include <TFTrading/ProviderManager.h>

#include <TFIQFeed/IQFeedHistoryQuery.h>  // seems to be a header ordering dependancy
#include <TFIQFeed/IQFeedProvider.h>  // includes CPortfolio and CPosition

#include <TFInteractiveBrokers/IBTWS.h>

#include <TFSimulation/SimulationProvider.h>

// may need to inherit and add more functionality to the class:
#include <TFTrading/DBOps.h>

//#include "ThreadMain.h"
#include "FrameMain.h"
#include "FrameProviderControl.h"

using namespace ou::tf;

//wxFrame (wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=wxDEFAULT_FRAME_STYLE, const wxString &name=wxFrameNameStr)

class AppSemiAuto : public wxApp {

  enum enumMode {
    EModeSimulation,
    EModeLive
  } m_eMode;

  virtual bool OnInit();
  virtual int OnExit();
protected:
private:
  FrameMain* m_FrameMain;
  FrameProviderControl* m_FrameProviderControl;

  DBOps m_db;
//  ThreadMain* m_pThreadMain;

  typedef CPortfolio::pPortfolio_t pPortfolio_t;
  typedef CPosition::pPosition_t pPosition_t;

  typedef ou::tf::keytypes::idInstrument_t idInstrument_t;
  typedef CInstrument::pInstrument_t pInstrument_t;

  typedef CProviderInterfaceBase::pProvider_t pProvider_t;

  typedef CIBTWS::pProvider_t pProviderIBTWS_t;
  typedef CIQFeedProvider::pProvider_t pProviderIQFeed_t;
  typedef CSimulationProvider::pProvider_t pProviderSim_t;

  pProviderIQFeed_t m_iqfeed;
  bool m_bIQFeedConnected;

  pProviderIBTWS_t m_tws;
  bool m_bIBConnected;
  bool m_bWatchingOptions;
  bool m_bTrading;

  pProviderSim_t m_sim;
  bool m_bSimConnected;

  pProvider_t m_pExecutionProvider;
  pProvider_t m_pDataProvider;

  bool m_bExecConnected;
  bool m_bDataConnected;
  bool m_bData2Connected;
  bool m_bConnectDone;


};
 
// Implements MyApp& wxGetApp()
DECLARE_APP(AppSemiAuto)