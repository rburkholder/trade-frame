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

// Started 2012/10/15

#include <string>

#include <TFBitsNPieces/FrameWork01.h>

// may need to inherit and add more functionality to the class:
#include <TFTrading/DBOps.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>

#include "Worker.h"
#include "ManagePortfolio.h"

class WorkerDoneEvent: public wxEvent {
public:
  WorkerDoneEvent( wxEventType eventType ): wxEvent( 0, eventType ) {};
  WorkerDoneEvent( const WorkerDoneEvent& event): wxEvent( event ) {};
  ~WorkerDoneEvent( void ) {};
  WorkerDoneEvent* Clone( void ) const { return new WorkerDoneEvent( *this ); };
};

wxDECLARE_EVENT( EVT_WorkerDone, WorkerDoneEvent );

class AppBasketTrading:
  public wxApp, public ou::tf::FrameWork01<AppBasketTrading> {
public:
protected:
private:

  FrameMain* m_pFrameMain;
//  PanelOptionsParameters* m_pPanelOptionsParameters;
  ou::tf::PanelLogging* m_pPanelLogging;

  Worker* m_pWorker;

  DBOps m_db;

  ManagePortfolio m_portfolio;

  virtual bool OnInit();
  virtual int OnExit();

  void HandlePopulateDatabase( void );

  void HandleRegisterTables( ou::db::Session& session );
  void HandleRegisterRows( ou::db::Session& session );

  void HandleWorkerCompletion0( void ); // for direct execution by worker thread
  void HandleWorkerCompletion1( wxEvent& event ); // cross thread migration

};

// Implements MyApp& wxGetApp()
DECLARE_APP(AppBasketTrading)

