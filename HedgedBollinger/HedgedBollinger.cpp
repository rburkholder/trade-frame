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

#include "stdafx.h"

#include <iostream>

#include <math.h>

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;

#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/timer/timer.hpp>

#include <boost/foreach.hpp>

#include <OUCommon/TimeSource.h>

#include <TFTrading/InstrumentManager.h>
#include <TFTrading/AccountManager.h>
#include <TFTrading/OrderManager.h>


#include "HedgedBollinger.h"

IMPLEMENT_APP(AppHedgedBollinger)

size_t atm = 125;

bool AppHedgedBollinger::OnInit() {

  m_pFrameMain = new FrameMain( 0, wxID_ANY, "Weeklies" );
  wxWindowID idFrameMain = m_pFrameMain->GetId();
  //m_pFrameMain->Bind( wxEVT_SIZE, &AppStrategy1::HandleFrameMainSize, this, idFrameMain );
  //m_pFrameMain->Bind( wxEVT_MOVE, &AppStrategy1::HandleFrameMainMove, this, idFrameMain );
  //m_pFrameMain->Center();
//  m_pFrameMain->Move( -2500, 50 );
  m_pFrameMain->SetSize( 800, 500 );
  SetTopWindow( m_pFrameMain );

  wxBoxSizer* m_sizerMain;
  m_sizerMain = new wxBoxSizer(wxVERTICAL);
  m_pFrameMain->SetSizer(m_sizerMain);

  wxBoxSizer* m_sizerControls;
  m_sizerControls = new wxBoxSizer( wxHORIZONTAL );
  m_sizerMain->Add( m_sizerControls, 0, wxLEFT|wxTOP|wxRIGHT, 5 );

  // populate variable in FrameWork01
  m_pPanelProviderControl = new ou::tf::PanelProviderControl( m_pFrameMain, wxID_ANY );
  m_sizerControls->Add( m_pPanelProviderControl, 0, wxEXPAND|wxALIGN_LEFT|wxRIGHT, 5);
  m_pPanelProviderControl->Show( true );

  LinkToPanelProviderControl();

//  m_pPanelManualOrder = new ou::tf::PanelManualOrder( m_pFrameMain, wxID_ANY );
//  m_sizerControls->Add( m_pPanelManualOrder, 0, wxEXPAND|wxALIGN_LEFT|wxRIGHT, 5);
//  m_pPanelManualOrder->Show( true );

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

  m_db.OnRegisterTables.Add( MakeDelegate( this, &AppHedgedBollinger::HandleRegisterTables ) );
  m_db.OnRegisterRows.Add( MakeDelegate( this, &AppHedgedBollinger::HandleRegisterRows ) );
  m_db.SetOnPopulateDatabaseHandler( MakeDelegate( this, &AppHedgedBollinger::HandlePopulateDatabase ) );

  m_bData1Connected = false;
  m_bData2Connected = false;
  m_bExecConnected = false;

//  m_timerGuiRefresh.SetOwner( this );

//  Bind( wxEVT_TIMER, &AppPhi::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppHedgedBollinger::OnClose, this );  // start close of windows and controls

//  m_pPanelManualOrder->SetOnNewOrderHandler( MakeDelegate( this, &AppPhi::HandlePanelNewOrder ) );
//  m_pPanelManualOrder->SetOnSymbolTextUpdated( MakeDelegate( this, &AppPhi::HandlePanelSymbolText ) );
//  m_pPanelManualOrder->SetOnFocusPropogate( MakeDelegate( this, &AppPhi::HandlePanelFocusPropogate ) );

  // maybe set scenario with database and with in memory data structure
  //m_idPortfolio = boost::gregorian::to_iso_string( boost::gregorian::day_clock::local_day() ) + "phi";
//  m_idPortfolio = "weeklies";  // makes it easy for swing trading

  std::string sTimeZoneSpec( "date_time_zonespec.csv" );
  if ( !boost::filesystem::exists( sTimeZoneSpec ) ) {
    std::cout << "Required file does not exist:  " << sTimeZoneSpec << std::endl;
  }
  
  std::string sDbName( "hedgedbollinger.db" );
  if ( boost::filesystem::exists( sDbName ) ) {
    boost::filesystem::remove( sDbName );
  }
  m_db.Open( sDbName );


  FrameMain::vpItems_t vItems;
  typedef FrameMain::structMenuItem mi;  // vxWidgets takes ownership of the objects
  vItems.push_back( new mi( "a1 New Symbol List Remote", MakeDelegate( this, &AppHedgedBollinger::HandleMenuAction0ObtainNewIQFeedSymbolListRemote ) ) );
  vItems.push_back( new mi( "a2 New Symbol List Local", MakeDelegate( this, &AppHedgedBollinger::HandleMenuAction1ObtainNewIQFeedSymbolListLocal ) ) );
  vItems.push_back( new mi( "a3 Load Symbol List", MakeDelegate( this, &AppHedgedBollinger::HandleMenuAction2LoadIQFeedSymbolList ) ) );
  m_pFrameMain->AddDynamicMenu( "Actions", vItems );

  return 1;

}

void AppHedgedBollinger::HandleMenuAction0ObtainNewIQFeedSymbolListRemote( void ) {
  // need to lock out from running HandleLoadIQFeedSymbolList at the same time
  m_worker.Run( MakeDelegate( this, &AppHedgedBollinger::HandleObtainNewIQFeedSymbolListRemote ) );
}

void AppHedgedBollinger::HandleObtainNewIQFeedSymbolListRemote( void ) {
  std::cout << "Downloading Text File ... " << std::endl;
  ou::tf::iqfeed::LoadMktSymbols( m_listIQFeedSymbols, ou::tf::iqfeed::MktSymbolLoadType::Download, true ); 
  std::cout << "Saving Binary File ... " << std::endl;
  m_listIQFeedSymbols.SaveToFile( "symbols.ser" );
  std::cout << " ... done." << std::endl;
}

void AppHedgedBollinger::HandleMenuAction1ObtainNewIQFeedSymbolListLocal( void ) {
  // need to lock out from running HandleLoadIQFeedSymbolList at the same time
  m_worker.Run( MakeDelegate( this, &AppHedgedBollinger::HandleObtainNewIQFeedSymbolListLocal ) );
}

void AppHedgedBollinger::HandleObtainNewIQFeedSymbolListLocal( void ) {
  std::cout << "Loading From Text File ... " << std::endl;
  ou::tf::iqfeed::LoadMktSymbols( m_listIQFeedSymbols, ou::tf::iqfeed::MktSymbolLoadType::LoadTextFromDisk, false ); 
  std::cout << "Saving Binary File ... " << std::endl;
  m_listIQFeedSymbols.SaveToFile( "symbols.ser" );
  std::cout << " ... done." << std::endl;
}

void AppHedgedBollinger::HandleMenuAction2LoadIQFeedSymbolList( void ) {
  // need to lock out from running HandleObtainNewIQFeedSymbolList at the same time
  m_worker.Run( MakeDelegate( this, &AppHedgedBollinger::HandleLoadIQFeedSymbolList ) );
}

void AppHedgedBollinger::HandleLoadIQFeedSymbolList( void ) {
  std::cout << "Loading From Binary File ..." << std::endl;
  m_listIQFeedSymbols.LoadFromFile( "phisymbols.ser" );
  std::cout << " ... completed." << std::endl;
}


void AppHedgedBollinger::HandleGuiRefresh( wxTimerEvent& event ) {
  // update portfolio results and tracker timeseries for portfolio value
//  double dblUnRealized;
//  double dblRealized;
//  double dblCommissionsPaid;
/*  m_pPortfolio->QueryStats( dblUnRealized, dblRealized, dblCommissionsPaid );
  double dblCurrent = dblUnRealized + dblRealized - dblCommissionsPaid;
  m_dblMaxPL = std::max<double>( m_dblMaxPL, dblCurrent );
  m_dblMinPL = std::min<double>( m_dblMinPL, dblCurrent );
  m_pPanelPortfolioStats->SetStats( 
    boost::lexical_cast<std::string>( m_dblMinPL ),
    boost::lexical_cast<std::string>( dblCurrent ),
    boost::lexical_cast<std::string>( m_dblMaxPL )
    );
    */
}

int AppHedgedBollinger::OnExit() {
  // Exit Steps: #4
//  DelinkFromPanelProviderControl();  generates stack errors
  //m_timerGuiRefresh.Stop();
  m_listIQFeedSymbols.Clear();
  if ( m_db.IsOpen() ) m_db.Close();

  return wxAppConsole::OnExit();
}

//void AppWeeklies::HandlePanelFocusPropogate( unsigned int ix ) {
//}


void AppHedgedBollinger::OnClose( wxCloseEvent& event ) {
  // Exit Steps: #2 -> FrameMain::OnClose
  m_timerGuiRefresh.Stop();
  DelinkFromPanelProviderControl();
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a 
  event.Skip();  // auto followed by Destroy();
}

void AppHedgedBollinger::OnData1Connected( int ) {
  m_bData1Connected = true;
  //ou::tf::libor::SetWatchOn( m_pData1Provider );
//  m_libor.SetWatchOn( m_pData1Provider );
//  AutoStartCollection();
  if ( m_bData1Connected & m_bExecConnected ) {
    // set start to enabled
  }
}

void AppHedgedBollinger::OnData2Connected( int ) {
  m_bData2Connected = true;
//  AutoStartCollection();
  if ( m_bData2Connected & m_bExecConnected ) {
    // set start to enabled
  }
}

void AppHedgedBollinger::OnExecConnected( int ) {
  m_bExecConnected = true;
  if ( m_bData1Connected & m_bExecConnected ) {
    // set start to enabled
  }
}


void AppHedgedBollinger::OnData1Disconnected( int ) {
  m_bData1Connected = false;
}

void AppHedgedBollinger::OnData2Disconnected( int ) {
  m_bData2Connected = false;
}

void AppHedgedBollinger::OnExecDisconnected( int ) {
  m_bExecConnected = false;
}

void AppHedgedBollinger::HandleRegisterTables(  ou::db::Session& session ) {
}

void AppHedgedBollinger::HandleRegisterRows(  ou::db::Session& session ) {
}

void AppHedgedBollinger::HandlePopulateDatabase( void ) {
/*

  ou::tf::AccountManager::pAccountAdvisor_t pAccountAdvisor 
    = ou::tf::AccountManager::Instance().ConstructAccountAdvisor( "aaRay", "Raymond Burkholder", "One Unified" );

  ou::tf::AccountManager::pAccountOwner_t pAccountOwner
    = ou::tf::AccountManager::Instance().ConstructAccountOwner( "aoRay", "aaRay", "Raymond", "Burkholder" );

  ou::tf::AccountManager::pAccount_t pAccountIB
    = ou::tf::AccountManager::Instance().ConstructAccount( "ib01", "aoRay", "Raymond Burkholder", ou::tf::keytypes::EProviderIB, "Interactive Brokers", "acctid", "login", "password" );

  ou::tf::AccountManager::pAccount_t pAccountIQFeed
    = ou::tf::AccountManager::Instance().ConstructAccount( "iq01", "aoRay", "Raymond Burkholder", ou::tf::keytypes::EProviderIQF, "IQFeed", "acctid", "login", "password" );

  ou::tf::AccountManager::pAccount_t pAccountSimulator
    = ou::tf::AccountManager::Instance().ConstructAccount( "sim01", "aoRay", "Raymond Burkholder", ou::tf::keytypes::EProviderSimulator, "Sim", "acctid", "login", "password" );

  m_pPortfolio
    = ou::tf::PortfolioManager::Instance().ConstructPortfolio( 
    "Master", "aoRay", "", ou::tf::Portfolio::Master, ou::tf::Currency::Name[ ou::tf::Currency::USD ], "phi" );

  m_pPortfolio
    = ou::tf::PortfolioManager::Instance().ConstructPortfolio( 
    m_idPortfolio, "aoRay", "Master", ou::tf::Portfolio::CurrencySummary, ou::tf::Currency::Name[ ou::tf::Currency::USD ], "phi" );

*/
}

