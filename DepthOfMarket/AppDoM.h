/************************************************************************
 * Copyright(c) 2021, One Unified. All rights reserved.                 *
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

/*
 * File:    AppDoM.h
 * Author:  raymond@burkholder.net
 * Project: App Depth of Market
 * Created on October 12, 2021, 23:04
 */

#include <memory>

#include <wx/app.h>

#include <TFBitsNPieces/FrameWork01.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>

#include <TFVuTrading/MarketDepth/PanelTrade.h>

#include "DoMDispatch.h"

class AppDoM:
  public wxApp,
  public ou::tf::FrameWork01<AppDoM>
{
  friend ou::tf::FrameWork01<AppDoM>;
public:
protected:
private:

  FrameMain* m_pFrameMain;
  ou::tf::PanelLogging* m_pPanelLogging;

  ou::tf::l2::PanelTrade* m_pPanelTrade;

  //wxMenu* m_pMenuLoadDays;

  std::unique_ptr<DoMDispatch> m_pDispatch;  // one per symbol

  virtual bool OnInit();
  void OnClose( wxCloseEvent& event );
  virtual int OnExit();

  void OnData1Connected( int );
  void OnData1Disconnecting( int );
  void OnData1Disconnected( int );

};

// Implements MyApp& wxGetApp()
DECLARE_APP(AppDoM)
