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

// Started 2012/09/03

#include <string>

#include <TFBitsNPieces/FrameWork01.h>

// may need to inherit and add more functionality to the class:
//#include <TFTrading/DBOps.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>

//#include <TFIQFeed/LoadMktSymbols.h>
#include <TFBitsNPieces/IQFeedSymbolListOps.h>

#include "Worker.h"

class AppIQFeedGetHistory:
  public wxApp,
  public ou::tf::FrameWork01<AppIQFeedGetHistory>
{
  friend ou::tf::FrameWork01<AppIQFeedGetHistory>;
public:
protected:
private:

  FrameMain* m_pFrameMain;
//  PanelOptionsParameters* m_pPanelOptionsParameters;
  ou::tf::PanelLogging* m_pPanelLogging;

  ou::tf::iqfeed::InMemoryMktSymbolList m_listIQFeedSymbols;
  ou::tf::IQFeedSymbolListOps* m_pIQFeedSymbolListOps;

  wxMenu* m_pMenuLoadDays;

  Worker* m_pWorker;

  virtual bool OnInit();
  virtual int OnExit();

  void HandleNewSymbolListRemote( void );
  void HandleNewSymbolListLocal( void );
  void HandleLocalBinarySymbolList( void );

  void HandleMenuActionDays10( void );
  void HandleMenuActionDays30( void );
  void HandleMenuActionDays100( void );
  void HandleMenuActionDays150( void );
  void HandleMenuActionDays200( void );
  void HandleMenuActionDays0( void );

  void StartWorker( const std::string& s, size_t nDatums );

  void EnableMenuActionDays();
  void DisableMenuActionDays();

  void OnData1Connected( int );
  void OnData1Disconnected( int );


};

// Implements MyApp& wxGetApp()
DECLARE_APP(AppIQFeedGetHistory)

