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
#include <TFTrading/DBOps.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>

#include <TFIQFeed/ValidateMktSymbolLine.h>

class AppCollectAndView:
  public wxApp, public ou::tf::FrameWork01<AppCollectAndView> {
public:
protected:
private:

  typedef ou::tf::iqfeed::MarketSymbol::TableRowDef trd_t;

  FrameMain* m_pFrameMain;
//  PanelOptionsParameters* m_pPanelOptionsParameters;
  ou::tf::PanelLogging* m_pPanelLogging;

  DBOps m_db;
  trd_t m_trd;  // needs to be changed so that new structure can be used each time with prepared statement
  ou::db::QueryFields<ou::tf::iqfeed::MarketSymbol::TableRowDef>::pQueryFields_t pInsertIQFeedSymbol;

  virtual bool OnInit();
  virtual int OnExit();

  void HandlePopulateDatabase( void );

  void HandleParsedStructure( trd_t& trd );
  void HandleUpdateHasOption( const std::string& );

  void HandleRegisterTables( ou::db::Session& session );
  void HandleRegisterRows( ou::db::Session& session );

};

// Implements MyApp& wxGetApp()
DECLARE_APP(AppCollectAndView)

