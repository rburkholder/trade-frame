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

#include <OUCharting/ChartMaster.h>

#include <TFBitsNPieces/FrameWork01.h>

#include "FrameMain.h"
#include "PanelSimulationControl.h"
#include "PanelFinancialChart.h"

#include "Strategy.h"

class AppStrategy1:
  public wxApp, public ou::tf::FrameWork01<AppStrategy1>
{
public:
protected:
private:

  typedef ou::tf::CProviderInterfaceBase::pProvider_t pProvider_t;
  typedef ou::tf::eProviderState_t eProviderState_t;

  typedef ou::tf::CIBTWS::pProvider_t pProviderIBTWS_t;
  typedef ou::tf::CIQFeedProvider::pProvider_t pProviderIQFeed_t;
  typedef ou::tf::CSimulationProvider::pProvider_t pProviderSim_t;

  Strategy* m_pStrategy;

  FrameMain* m_pFrameMain;
  PanelSimulationControl* m_pPanelSimulationControl;
  PanelFinancialChart* m_pPanelFinancialChart;

  wxWindow* m_winChart;

  ou::ChartMaster m_chart;

  bool m_bReadyToDrawChart;

  std::string m_sTSDataStreamOpened;

  virtual bool OnInit();
  virtual int OnExit();

  void HandleBtnSimulationStart( void );
  void HandleBtnDrawChart( void );

  void HandleDrawChart( const MemBlock& );
  void HandlePaint( wxPaintEvent& event );
  void HandleSize( wxSizeEvent& event );

  void HandleFrameMainSize( wxSizeEvent& event );
  void HandleFrameMainMove( wxMoveEvent& event );

  };

// Implements MyApp& wxGetApp()
DECLARE_APP(AppStrategy1)

