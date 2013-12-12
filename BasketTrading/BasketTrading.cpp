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

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;

#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/lexical_cast.hpp>

#include <TFTrading/InstrumentManager.h>
#include <TFTrading/AccountManager.h>
#include <TFTrading/OrderManager.h>

#include "BasketTrading.h"

wxDEFINE_EVENT( EVT_WorkerDone, WorkerDoneEvent );

IMPLEMENT_APP(AppBasketTrading)

bool AppBasketTrading::OnInit() {

  m_pFrameMain = new FrameMain( 0, wxID_ANY, "Basket Trading" );
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

  // populate variable in FrameWork01
  m_pPanelProviderControl = new ou::tf::PanelProviderControl( m_pFrameMain, wxID_ANY );
  m_sizerControls->Add( m_pPanelProviderControl, 0, wxEXPAND|wxALIGN_LEFT|wxRIGHT, 5);
  m_pPanelProviderControl->Show( true );

  m_pPanelBasketTradingMain = new PanelBasketTradingMain( m_pFrameMain, wxID_ANY );
  m_sizerControls->Add( m_pPanelBasketTradingMain, 0, wxEXPAND|wxALIGN_LEFT|wxRIGHT, 5);
  m_pPanelBasketTradingMain->Show( true );

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
  m_pPanelPortfolioStats = new PanelPortfolioStats( m_pFrameMain, wxID_ANY );
  //m_sizerMain->Add( m_pPanelPortfolioStats, 1, wxEXPAND|wxALIGN_LEFT|wxRIGHT, 5);
  m_sizerMain->Add( m_pPanelPortfolioStats, 0, wxLEFT|wxTOP|wxRIGHT, 5);
  m_pPanelPortfolioStats->Show( true );

  wxBoxSizer* m_sizerStatus = new wxBoxSizer( wxHORIZONTAL );
  m_sizerMain->Add( m_sizerStatus, 1, wxEXPAND|wxALL, 5 );

  m_pPanelLogging = new ou::tf::PanelLogging( m_pFrameMain, wxID_ANY );
  m_sizerStatus->Add( m_pPanelLogging, 1, wxALL | wxEXPAND|wxALIGN_LEFT|wxALIGN_RIGHT|wxALIGN_TOP|wxALIGN_BOTTOM, 0);
  m_pPanelLogging->Show( true );

  m_pFrameMain->Show( true );

  m_db.OnRegisterTables.Add( MakeDelegate( this, &AppBasketTrading::HandleRegisterTables ) );
  m_db.OnRegisterRows.Add( MakeDelegate( this, &AppBasketTrading::HandleRegisterRows ) );
  m_db.SetOnPopulateDatabaseHandler( MakeDelegate( this, &AppBasketTrading::HandlePopulateDatabase ) );

  m_pWorker = 0;
  m_bData1Connected = false;
  m_bExecConnected = false;

  m_dblMinPL = m_dblMaxPL = 0.0;

  m_timerGuiRefresh.SetOwner( this );

  Bind( EVT_WorkerDone, &AppBasketTrading::HandleWorkerCompletion1, this );
  Bind( wxEVT_TIMER, &AppBasketTrading::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppBasketTrading::OnClose, this );  // start close of windows and controls

  // maybe set scenario with database and with in memory data structure
  m_sDbPortfolioName = boost::gregorian::to_iso_string( boost::gregorian::day_clock::local_day() ) + "Basket";
  m_db.Open( "BasketTrading.db" );

  m_pPanelBasketTradingMain->SetOnButtonPressedStart( MakeDelegate( this, &AppBasketTrading::HandleStartButton ) );
  m_pPanelBasketTradingMain->SetOnButtonPressedExitPositions( MakeDelegate( this, &AppBasketTrading::HandleExitPositionsButton ) );
  m_pPanelBasketTradingMain->SetOnButtonPressedStop( MakeDelegate( this, &AppBasketTrading::HandleStopButton ) );
  m_pPanelBasketTradingMain->SetOnButtonPressedSave( MakeDelegate( this, &AppBasketTrading::HandleSaveButton ) );

  return 1;

}

void AppBasketTrading::HandleGuiRefresh( wxTimerEvent& event ) {
  // update portfolio results and tracker timeseries for portfolio value
  double dblUnRealized;
  double dblRealized;
  double dblCommissionsPaid;
  double dblCurrent;
  m_pPortfolio->QueryStats( dblUnRealized, dblRealized, dblCommissionsPaid, dblCurrent );
  //double dblCurrent = dblUnRealized + dblRealized - dblCommissionsPaid;
  m_dblMaxPL = std::max<double>( m_dblMaxPL, dblCurrent );
  m_dblMinPL = std::min<double>( m_dblMinPL, dblCurrent );
  m_pPanelPortfolioStats->SetStats( 
    boost::lexical_cast<std::string>( m_dblMinPL ),
    boost::lexical_cast<std::string>( dblCurrent ),
    boost::lexical_cast<std::string>( m_dblMaxPL )
    );
}

void AppBasketTrading::HandleStartButton(void) {
  if ( 0 == m_pPortfolio.get() ) {  // if not newly created below, then load previously created portfolio
    // code currently does not allow a restart of session
    std::cout << "Cannot create new portfolio: " << m_sDbPortfolioName << std::endl;
    //m_pPortfolio = ou::tf::PortfolioManager::Instance().GetPortfolio( sDbPortfolioName );
    // this may create issues on mid-trading session restart.  most logic in the basket relies on newly created positions.
  }
  else {
    // need to change this later.... only start up once providers have been started
    // worker will change depending upon provider type
    // big worker when going live, hdf5 worker when simulating
    std::cout << "Starting Symbol Evaluation ... " << std::endl;
    m_pWorker = new Worker( MakeDelegate( this, &AppBasketTrading::HandleWorkerCompletion0 ) );
  }
}

void AppBasketTrading::HandleStopButton(void) {
  m_ManagePortfolio.Stop();
}

void AppBasketTrading::HandleExitPositionsButton(void) {
  // to implement
}

void AppBasketTrading::HandleSaveButton(void) {
  m_ManagePortfolio.SaveSeries( "/app/BasketTrading/" );
}

int AppBasketTrading::OnExit() {

//  DelinkFromPanelProviderControl();  generates stack errors
  //m_timerGuiRefresh.Stop();
  if ( 0 != m_pWorker ) {
    delete m_pWorker;
    m_pWorker = 0; 
  }
  if ( m_db.IsOpen() ) m_db.Close();

  return 0;
}

void AppBasketTrading::HandleWorkerCompletion0( void ) {  // called in worker thread, generate gui event to start processing in gui thread
  wxQueueEvent( this, new WorkerDoneEvent( EVT_WorkerDone ) ); 
}

void AppBasketTrading::HandleWorkerCompletion1( wxEvent& event ) { // process in gui thread
  m_pWorker->IterateInstrumentList( 
    boost::phoenix::bind( &ManagePortfolio::AddSymbol, &m_ManagePortfolio, boost::phoenix::arg_names::arg1, boost::phoenix::arg_names::arg2 ) );
  m_pWorker->Join();
  delete m_pWorker;
  m_pWorker = 0;
  m_ManagePortfolio.Start( m_pPortfolio, m_pExecutionProvider, m_pData1Provider, m_pData2Provider );
  m_timerGuiRefresh.Start( 250 );
}

void AppBasketTrading::HandleRegisterTables(  ou::db::Session& session ) {
}

void AppBasketTrading::HandleRegisterRows(  ou::db::Session& session ) {
}

void AppBasketTrading::HandlePopulateDatabase( void ) {

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

  m_pPortfolioMaster
    = ou::tf::PortfolioManager::Instance().ConstructPortfolio( 
    "Master", "aoRay", "", ou::tf::Portfolio::Master, ou::tf::Currency::Name[ ou::tf::Currency::USD ], "Basket of Equities" );

  m_pPortfolioCurrencyUSD
    = ou::tf::PortfolioManager::Instance().ConstructPortfolio( 
    "USD", "aoRay", "Master", ou::tf::Portfolio::CurrencySummary, ou::tf::Currency::Name[ ou::tf::Currency::USD ], "Basket of Equities" );

  m_pPortfolio
    = ou::tf::PortfolioManager::Instance().ConstructPortfolio( 
    m_sDbPortfolioName, "aoRay", "USD", ou::tf::Portfolio::MultiLeggedPosition, ou::tf::Currency::Name[ ou::tf::Currency::USD ], "Basket of Equities" );

}

void AppBasketTrading::OnClose( wxCloseEvent& event ) {
  m_timerGuiRefresh.Stop();
  DelinkFromPanelProviderControl();
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a 
  event.Skip();  // auto followed by Destroy();
}

void AppBasketTrading::OnData1Connected( int ) {
  m_bData1Connected = true;
  if ( m_bData1Connected & m_bExecConnected ) {
    // set start to enabled
  }
}

void AppBasketTrading::OnExecConnected( int ) {
  m_bExecConnected = true;
  if ( m_bData1Connected & m_bExecConnected ) {
    // set start to enabled
  }
}

void AppBasketTrading::OnData1Disconnected( int ) {
  m_bData1Connected = false;
}

void AppBasketTrading::OnExecDisconnected( int ) {
  m_bExecConnected = false;
}
