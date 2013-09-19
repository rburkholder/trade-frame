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

// Started 2013/09/18

#include <TFBitsNPieces/FrameWork01.h>

#include <OUCommon/Worker.h>

#include <TFTrading/DBOps.h>

#include <TFTimeSeries/DatedDatum.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>

class AppScanner:
  public wxApp, public ou::tf::FrameWork01<AppScanner> {
    friend ou::tf::FrameWork01<AppScanner>;
public:
  void HandleHdf5Group( const std::string&, const std::string& ) {};
  void HandleHdf5Object( const std::string&, const std::string& );
protected:
private:
  FrameMain* m_pFrameMain;
  ou::tf::PanelLogging* m_pPanelLogging;
  ou::tf::DBOps m_db;

  ou::action::Worker m_worker;

  virtual bool OnInit();
  virtual int OnExit();
  void OnClose( wxCloseEvent& event );

  void OnData1Connected( int );
  void OnData2Connected( int );
  void OnExecConnected( int );
  void OnData1Disconnected( int );
  void OnData2Disconnected( int );
  void OnExecDisconnected( int );

  struct s_t {
    ou::tf::Bar::volume_t nAverageVolume;
    size_t nEnteredFilter;
    size_t nPassedFilter;
    s_t( void ): nAverageVolume( 0 ), nEnteredFilter( 0 ), nPassedFilter( 0 ) {};
  };

  void HandleMenuActionScan( void );
  void ScanBars( void );
  bool HandleCallBackUseGroup( s_t&, const std::string& sPath, const std::string& sGroup );
  bool HandleCallBackFilter( s_t&, const std::string& sObject, ou::tf::Bars& bars );
  void HandleCallBackResults( s_t&, const std::string& sObject, ou::tf::Bars& bars );

};

// Implements MyApp& wxGetApp()
DECLARE_APP(AppScanner)

