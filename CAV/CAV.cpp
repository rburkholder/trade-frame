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

// CAV.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

#include "CAV.h"
IMPLEMENT_APP(AppCollectAndView)

#include <TFTrading/InstrumentManager.h>
#include <TFTrading/AccountManager.h>
#include <TFTrading/OrderManager.h>
#include <TFTrading/PortfolioManager.h>

#include <TFIQFeed/ParseMktSymbolDiskFile.h>

bool AppCollectAndView::OnInit() {

  m_pFrameMain = new FrameMain( 0, wxID_ANY, "Collect And View" );
  wxWindowID idFrameMain = m_pFrameMain->GetId();
  //m_pFrameMain->Bind( wxEVT_SIZE, &AppStrategy1::HandleFrameMainSize, this, idFrameMain );
  //m_pFrameMain->Bind( wxEVT_MOVE, &AppStrategy1::HandleFrameMainMove, this, idFrameMain );
  //m_pFrameMain->Center();
//  m_pFrameMain->Move( -2500, 50 );
  m_pFrameMain->SetSize( 500, 600 );
  SetTopWindow( m_pFrameMain );

  wxBoxSizer* m_sizerMain;
  m_sizerMain = new wxBoxSizer(wxVERTICAL);
  m_pFrameMain->SetSizer(m_sizerMain);

  wxBoxSizer* m_sizerControls;
  m_sizerControls = new wxBoxSizer( wxHORIZONTAL );
  m_sizerMain->Add( m_sizerControls, 0, wxLEFT|wxTOP|wxRIGHT, 5 );

  m_pPanelProviderControl = new ou::tf::PanelProviderControl( m_pFrameMain, wxID_ANY );
  m_sizerControls->Add( m_pPanelProviderControl, 1, wxEXPAND|wxALIGN_LEFT|wxRIGHT, 5);
  m_pPanelProviderControl->Show( true );

  LinkToPanelProviderControl();
/*
  m_pPanelOptionsParameters = new PanelOptionsParameters( m_pFrameMain, wxID_ANY );
  m_sizerControls->Add( m_pPanelOptionsParameters, 1, wxEXPAND|wxALIGN_LEFT, 0);
  m_pPanelOptionsParameters->Show( true );
  m_pPanelOptionsParameters->SetOnStart( MakeDelegate( this, &AppStrategyRunner::HandleBtnStart ) );
  m_pPanelOptionsParameters->SetOnStop( MakeDelegate( this, &AppStrategyRunner::HandleBtnStop ) );
  m_pPanelOptionsParameters->SetOnSave( MakeDelegate( this, &AppStrategyRunner::HandleBtnSave ) );
  m_pPanelOptionsParameters->SetOptionNearDate( boost::gregorian::date( 2012, 4, 20 ) );
  m_pPanelOptionsParameters->SetOptionFarDate( boost::gregorian::date( 2012, 6, 15 ) );
*/
  wxBoxSizer* m_sizerStatus = new wxBoxSizer( wxHORIZONTAL );
  m_sizerMain->Add( m_sizerStatus, 1, wxEXPAND|wxALL, 5 );

  m_pPanelLogging = new ou::tf::PanelLogging( m_pFrameMain, wxID_ANY );
  m_sizerStatus->Add( m_pPanelLogging, 1, wxALL | wxEXPAND|wxALIGN_LEFT|wxALIGN_RIGHT|wxALIGN_TOP|wxALIGN_BOTTOM, 0);
  m_pPanelLogging->Show( true );

  m_pFrameMain->Show( true );

  m_db.OnRegisterTables.Add( MakeDelegate( this, &AppCollectAndView::HandleRegisterTables ) );
  m_db.OnRegisterRows.Add( MakeDelegate( this, &AppCollectAndView::HandleRegisterRows ) );
  m_db.SetOnPopulateDatabaseHandler( MakeDelegate( this, &AppCollectAndView::HandlePopulateDatabase ) );

  m_db.Open( "cav.db" );

  typedef ou::tf::iqfeed::ParseMktSymbolDiskFile diskfile_t;
  diskfile_t diskfile;
  ou::tf::iqfeed::ValidateMktSymbolLine validator;
  diskfile.SetOnProcessLine( MakeDelegate( &validator, &ou::tf::iqfeed::ValidateMktSymbolLine::Parse<diskfile_t::iterator_t> ) );
  validator.SetOnProcessLine( MakeDelegate( this, &AppCollectAndView::HandleParsedStructure ) );

  pInsertIQFeedSymbol = m_db.Insert<ou::tf::iqfeed::MarketSymbol::TableRowDef>( m_trd ).NoExecute();

  diskfile.Run();

  validator.SetOnProcessHasOption( MakeDelegate( this, &AppCollectAndView::HandleUpdateHasOption ) );
  validator.PostProcess();
  validator.Summary();

  // maybe set scenario with database and with in memory data structure

  return 1;

}

int AppCollectAndView::OnExit() {

//  DelinkFromPanelProviderControl();  generates stack errors

  if ( m_db.IsOpen() ) m_db.Close();

  return 0;
}

void AppCollectAndView::HandleParsedStructure( trd_t& trd ) {
  m_trd = trd;
  int i;
  if ( "A" == trd.sSymbol ) {
    i = 0;
  }
  m_db.Reset( pInsertIQFeedSymbol );
  m_db.Bind<ou::tf::iqfeed::MarketSymbol::TableRowDef>( pInsertIQFeedSymbol );
  m_db.Execute( pInsertIQFeedSymbol );
//  ou::db::QueryFields<ou::tf::iqfeed::MarketSymbol::TableRowDef>::pQueryFields_t pInsertIQFeedSymbol 
//    = m_db.Insert<ou::tf::iqfeed::MarketSymbol::TableRowDef>( trd );
}

void AppCollectAndView::HandleUpdateHasOption( const std::string& ) {
}


void AppCollectAndView::HandleRegisterTables(  ou::db::Session& session ) {
  session.RegisterTable<ou::tf::iqfeed::MarketSymbol::TableCreateDef>( "iqfeedsymbols" );
}

void AppCollectAndView::HandleRegisterRows(  ou::db::Session& session ) {
  session.MapRowDefToTableName<ou::tf::iqfeed::MarketSymbol::TableRowDef>( "iqfeedsymbols" );
}

void AppCollectAndView::HandlePopulateDatabase( void ) {

  ou::tf::CAccountManager::pAccountAdvisor_t pAccountAdvisor 
    = ou::tf::CAccountManager::Instance().ConstructAccountAdvisor( "aaRay", "Raymond Burkholder", "One Unified" );

  ou::tf::CAccountManager::pAccountOwner_t pAccountOwner
    = ou::tf::CAccountManager::Instance().ConstructAccountOwner( "aoRay", "aaRay", "Raymond", "Burkholder" );

  ou::tf::CAccountManager::pAccount_t pAccountIB
    = ou::tf::CAccountManager::Instance().ConstructAccount( "ib01", "aoRay", "Raymond Burkholder", ou::tf::keytypes::EProviderIB, "Interactive Brokers", "acctid", "login", "password" );

  ou::tf::CAccountManager::pAccount_t pAccountIQFeed
    = ou::tf::CAccountManager::Instance().ConstructAccount( "iq01", "aoRay", "Raymond Burkholder", ou::tf::keytypes::EProviderIQF, "IQFeed", "acctid", "login", "password" );

  ou::tf::CAccountManager::pAccount_t pAccountSimulator
    = ou::tf::CAccountManager::Instance().ConstructAccount( "sim01", "aoRay", "Raymond Burkholder", ou::tf::keytypes::EProviderSimulator, "Sim", "acctid", "login", "password" );

//  ou::tf::CPortfolioManager::pPortfolio_t pPortfolio
//    //= ou::tf::CPortfolioManager::Instance().ConstructPortfolio( m_idPortfolio, "aoRay", "SemiAuto" );
//    = ou::tf::CPortfolioManager::Instance().ConstructPortfolio( "pflioOptions", "aoRay", "options" );

}

