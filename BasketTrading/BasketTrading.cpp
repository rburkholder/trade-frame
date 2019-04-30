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

// needs daily bars downloaded via IQFeedGetHistory
// rebuild after changing date in Worker.cpp.

// TODO:  graphical P/L loss summary
//    VWAP used to build one minute bars, and use 1min, 2min, 3min bars to generate trend trade?

#include "stdafx.h"

#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/lexical_cast.hpp>

#include <TFTrading/InstrumentManager.h>
#include <TFTrading/AccountManager.h>
#include <TFTrading/OrderManager.h>

#include "BasketTrading.h"

IMPLEMENT_APP(AppBasketTrading)

namespace {
 const std::string sFileNameMarketSymbolSubset( "BasketTrading.ser" );
}

bool AppBasketTrading::OnInit() {

  wxApp::OnInit();
  wxApp::SetAppDisplayName( "Basket Trading" );
  wxApp::SetVendorName( "One Unified Net Limited" );
  wxApp::SetVendorDisplayName( "(c) 2019 One Unified Net Limited" );

  m_sDbName = "BasketTrading.db";

  m_dtLatestEod = ptime( date( 2019, 4, 29 ), time_duration( 23, 59, 59 ) );

  m_pFrameMain = new FrameMain( 0, wxID_ANY, "Basket Trading" );
  wxWindowID idFrameMain = m_pFrameMain->GetId();
  //m_pFrameMain->Bind( wxEVT_SIZE, &AppStrategy1::HandleFrameMainSize, this, idFrameMain );
  //m_pFrameMain->Bind(
  //  wxEVT_SIZE,
  //  [this](wxSizeEvent& event){
  //    std::cout << "w=" << event.GetSize().GetWidth() << ",h=" << event.GetSize().GetHeight() << std::endl;
  //    event.Skip();
  //    },
  //  idFrameMain );
  //m_pFrameMain->Bind( wxEVT_MOVE, &AppStrategy1::HandleFrameMainMove, this, idFrameMain );
  //m_pFrameMain->Center();
  m_pFrameMain->Move( 200, 100 );
  m_pFrameMain->SetSize( 1400, 800 );
  SetTopWindow( m_pFrameMain );

  wxBoxSizer* sizerMain;
  sizerMain = new wxBoxSizer(wxHORIZONTAL);
  m_pFrameMain->SetSizer( sizerMain );

  wxBoxSizer* sizerLeft = new wxBoxSizer( wxVERTICAL );
  sizerMain->Add( sizerLeft, 0,  wxEXPAND | wxALL, 2 );

  wxBoxSizer* sizerRight = new wxBoxSizer( wxVERTICAL );
  sizerMain->Add( sizerRight, 1,  wxEXPAND | wxALL, 2 );

  wxBoxSizer* sizerControls;
  sizerControls = new wxBoxSizer( wxHORIZONTAL );
  sizerLeft->Add( sizerControls, 0, wxLEFT|wxTOP|wxRIGHT, 2 );

  // populate variable in FrameWork01
  m_pPanelProviderControl = new ou::tf::PanelProviderControl( m_pFrameMain, wxID_ANY );
  sizerControls->Add( m_pPanelProviderControl, 0, wxEXPAND|wxALIGN_LEFT|wxRIGHT, 5);
  m_pPanelProviderControl->Show( true );

  //m_pPanelBasketTradingMain = new PanelBasketTradingMain( m_pFrameMain, wxID_ANY );
  //m_sizerControls->Add( m_pPanelBasketTradingMain, 0, wxEXPAND|wxALIGN_LEFT|wxRIGHT, 5);
  //m_pPanelBasketTradingMain->Show( true );

  LinkToPanelProviderControl();

  m_pPanelPortfolioStats = new PanelPortfolioStats( m_pFrameMain, wxID_ANY );
  //m_sizerMain->Add( m_pPanelPortfolioStats, 1, wxEXPAND|wxALIGN_LEFT|wxRIGHT, 5);
  sizerLeft->Add( m_pPanelPortfolioStats, 0, wxLEFT|wxTOP|wxRIGHT, 5);
  m_pPanelPortfolioStats->Show( true );

  wxBoxSizer* m_sizerStatus = new wxBoxSizer( wxHORIZONTAL );
  sizerLeft->Add( m_sizerStatus, 1, wxEXPAND|wxALL, 5 );

  m_pPanelLogging = new ou::tf::PanelLogging( m_pFrameMain, wxID_ANY );
  m_sizerStatus->Add( m_pPanelLogging, 1, wxALL | wxEXPAND|wxALIGN_LEFT|wxALIGN_RIGHT|wxALIGN_TOP|wxALIGN_BOTTOM, 0);
  m_pPanelLogging->Show( true );

  m_pPanelFinancialChart = new ou::tf::PanelFinancialChart( m_pFrameMain, wxID_ANY );
  sizerRight->Add( m_pPanelFinancialChart, 1, wxEXPAND | wxALL, 2 );

  m_pFrameMain->Show( true );

  m_bData1Connected = false;
  m_bExecConnected = false;

  m_dblMinPL = m_dblMaxPL = 0.0;

  m_timerGuiRefresh.SetOwner( this );
  Bind( wxEVT_TIMER, &AppBasketTrading::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );
  m_timerGuiRefresh.Start( 250 );

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppBasketTrading::OnClose, this );  // start close of windows and controls

  // maybe set scenario with database and with in memory data structure?

  m_sPortfolioStrategyAggregate = "Basket-" + boost::gregorian::to_iso_string( boost::gregorian::day_clock::local_day() );

  try {
    if ( boost::filesystem::exists( m_sDbName ) ) {
      boost::filesystem::remove( m_sDbName );
    }

    m_db.OnRegisterTables.Add( MakeDelegate( this, &AppBasketTrading::HandleRegisterTables ) );
    m_db.OnRegisterRows.Add( MakeDelegate( this, &AppBasketTrading::HandleRegisterRows ) );
    m_db.SetOnPopulateDatabaseHandler( MakeDelegate( this, &AppBasketTrading::HandlePopulateDatabase ) );
    //m_db.SetOnLoadDatabaseHandler( MakeDelegate( this, &AppComboTrading::HandleLoadDatabase ) );

    m_db.Open( m_sDbName );
  }
  catch(...) {
    std::cout << "database fault on " << m_sDbName << std::endl;
  }

  using pChartDataView_t = MasterPortfolio::pChartDataView_t;

  m_pMasterPortfolio.reset( new MasterPortfolio(
    m_pExecutionProvider, m_pData1Provider, m_pData2Provider,
    [this](const std::string& sUnderlying, MasterPortfolio::fOptionDefinition_t f){
      m_listIQFeedSymbols.SelectOptionsByUnderlying( sUnderlying, f );
    },
    [this](const std::string& sIQFeedSymbolName)->const MasterPortfolio::trd_t& {
      return m_listIQFeedSymbols.GetTrd( sIQFeedSymbolName );
    },
    [this](MasterPortfolio::EStrategyChart esc, const std::string& sName, pChartDataView_t pChartDataView){
      switch ( esc ) {
        case MasterPortfolio::EStrategyChart::Root:
          m_pPanelFinancialChart->UpdateRoot( sName, pChartDataView );
          break;
        case MasterPortfolio::EStrategyChart::Active:
          m_pPanelFinancialChart->AppendActive( sName, pChartDataView );
          break;
        case MasterPortfolio::EStrategyChart::Info:
          m_pPanelFinancialChart->AppendInfo( sName, pChartDataView );
          break;
      }
      
    },
    m_pPortfolioStrategyAggregate
    ) );

  // this needs to come before the menu
  m_pIQFeedSymbolListOps = new ou::tf::IQFeedSymbolListOps( m_listIQFeedSymbols );
  m_pIQFeedSymbolListOps->Status.connect( [this]( const std::string sStatus ){
    CallAfter( [sStatus](){
      std::cout << sStatus << std::endl;
    });
  });

  // build menu last
  using mi = FrameMain::structMenuItem;  // vxWidgets takes ownership of the objects
  FrameMain::vpItems_t vItems;
  vItems.push_back( new mi( "a1 Test Selection", MakeDelegate( this, &AppBasketTrading::HandleMenuActionTestSelection ) ) );
  vItems.push_back( new mi( "b1 Load List", MakeDelegate( m_pIQFeedSymbolListOps, &ou::tf::IQFeedSymbolListOps::LoadIQFeedSymbolList ) ) );
  vItems.push_back( new mi( "b2 Save Subset", MakeDelegate( this, &AppBasketTrading::HandleMenuActionSaveSymbolSubset ) ) );
  vItems.push_back( new mi( "b3 Load Subset", MakeDelegate( this, &AppBasketTrading::HandleMenuActionLoadSymbolSubset ) ) );
  m_pFrameMain->AddDynamicMenu( "Symbols", vItems );

  vItems.clear();
  vItems.push_back( new mi( "a1 Load", MakeDelegate( this, &AppBasketTrading::HandleLoadButton ) ) );
  vItems.push_back( new mi( "a2 Start", MakeDelegate( this, &AppBasketTrading::HandleStartButton ) ) );
  //vItems.push_back( new mi( "a3 Exit Positions", MakeDelegate( this, &AppBasketTrading::HandleExitPositionsButton ) ) ); // doesn't do anything at the moment
  vItems.push_back( new mi( "a4 Save Series", MakeDelegate( this, &AppBasketTrading::HandleSaveButton ) ) );
  //vItems.push_back( new mi( "a5 Test", MakeDelegate( this, &AppBasketTrading::HandleTestButton ) ) ); // tests itm/atm/otm selector
  m_pFrameMain->AddDynamicMenu( "Trade", vItems );

  return true;
}

void AppBasketTrading::HandleTestButton() {
  CallAfter( std::bind( &MasterPortfolio::Test, m_pMasterPortfolio.get() ) );
}

void AppBasketTrading::HandleGuiRefresh( wxTimerEvent& event ) {
  // update portfolio results and tracker timeseries for portfolio value
  double dblUnRealized {};
  double dblRealized {};
  double dblCommissionsPaid {};
  double dblCurrent {};

  if ( m_pPortfolioStrategyAggregate ) {
    m_pPortfolioStrategyAggregate->QueryStats( dblUnRealized, dblRealized, dblCommissionsPaid, dblCurrent );
    //double dblCurrent = dblUnRealized + dblRealized - dblCommissionsPaid;
    m_dblMaxPL = std::max<double>( m_dblMaxPL, dblCurrent );
    m_dblMinPL = std::min<double>( m_dblMinPL, dblCurrent );
    m_pMasterPortfolio->UpdateChart( dblCurrent, dblUnRealized, dblRealized, dblCommissionsPaid );
  }

  size_t nUp;
  size_t nDown;
  m_pMasterPortfolio->GetSentiment( nUp, nDown );

  m_pPanelPortfolioStats->SetStats(
    boost::lexical_cast<std::string>( m_dblMinPL ),
    boost::lexical_cast<std::string>( dblCurrent ),
    boost::lexical_cast<std::string>( m_dblMaxPL ),
    boost::lexical_cast<std::string>( nUp ),
    boost::lexical_cast<std::string>( nDown )
    );
}

void AppBasketTrading::HandleLoadButton() {
  CallAfter( // eliminates debug session lock up when gui/menu is not yet finished
    [this](){
      if ( 0 == m_pPortfolioStrategyAggregate.get() ) {  // if not newly created in HandlePopulateDatabase, then load previously created portfolio
        // code currently does not allow a restart of session
        std::cout << "Cannot create new portfolio: " << m_sPortfolioStrategyAggregate << std::endl;
        //m_pPortfolio = ou::tf::PortfolioManager::Instance().GetPortfolio( sDbPortfolioName );
        // this may create issues on mid-trading session restart.  most logic in the basket relies on newly created positions.
      }
      else {
        // need to change this later.... only start up once providers have been started
        // worker will change depending upon provider type
        // big worker when going live, hdf5 worker when simulating
        std::cout << "Starting Symbol Load ... " << std::endl;
        // TODO: convert worker to something informative and use
        //   established wx based threading arrangements
        //m_pWorker = new Worker( MakeDelegate( this, &AppBasketTrading::HandleWorkerCompletion ) );
        m_pMasterPortfolio->Load( m_dtLatestEod, true );
      }
    });
}

void AppBasketTrading::HandleMenuActionTestSelection( void ) {
  CallAfter(
    [this](){
      std::cout << "Starting Symbol Test ... " << std::endl;
      m_pMasterPortfolio->Load( m_dtLatestEod, false );
    });
}

void AppBasketTrading::HandleStartButton(void) {
  CallAfter(
    [this](){
      m_pMasterPortfolio->Start();
    } );
}

void AppBasketTrading::HandleStopButton(void) {
  CallAfter(
    [this](){
      m_pMasterPortfolio->Stop();
    });
}

void AppBasketTrading::HandleExitPositionsButton(void) {
  // to implement
}

void AppBasketTrading::HandleSaveButton(void) {
  CallAfter(
    [this](){
      std::cout << "Saving ... ";
      if ( m_worker.joinable() ) m_worker.join(); // need to finish off any previous thread
      m_worker = std::thread(
        [this](){
          m_pMasterPortfolio->SaveSeries( "/app/BasketTrading/" );
        } );
    });
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

void AppBasketTrading::OnClose( wxCloseEvent& event ) {
  if ( m_worker.joinable() ) m_worker.join();
  m_timerGuiRefresh.Stop();
  DelinkFromPanelProviderControl();
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a
  if ( m_db.IsOpen() ) m_db.Close();
  event.Skip();  // auto followed by Destroy();
}

int AppBasketTrading::OnExit() {

  return 0;
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
    "Master", "aoRay", "", ou::tf::Portfolio::Master, ou::tf::Currency::Name[ ou::tf::Currency::USD ], "Aggregate of all Portfolios" );

  m_pPortfolioCurrencyUSD
    = ou::tf::PortfolioManager::Instance().ConstructPortfolio(
    "USD", "aoRay", "Master", ou::tf::Portfolio::CurrencySummary, ou::tf::Currency::Name[ ou::tf::Currency::USD ], "Aggregate of all USD Portfolios" );

  m_pPortfolioStrategyAggregate
    = ou::tf::PortfolioManager::Instance().ConstructPortfolio(
    m_sPortfolioStrategyAggregate, "aoRay", "USD", ou::tf::Portfolio::MultiLeggedPosition, ou::tf::Currency::Name[ ou::tf::Currency::USD ], "Aggregate of Instrument Instances" );

}

// maybe put this into background thread
void AppBasketTrading::HandleMenuActionSaveSymbolSubset( void ) {

  m_vExchanges.clear();
  m_vExchanges.insert( "NYSE" );
  m_vExchanges.insert( "NYSE_AMERICAN" );
  m_vExchanges.insert( "NYSE,NYSE_ARCA" );
  m_vExchanges.insert( "NASDAQ,NGSM" );
  m_vExchanges.insert( "NASDAQ,NGM" );
  //m_vExchanges.push_back( "NASDAQ,NMS" );
  //m_vExchanges.push_back( "NASDAQ,SMCAP" );
  //m_vExchanges.push_back( "NASDAQ,OTCBB" );
  //m_vExchanges.push_back( "NASDAQ,OTC" );
  m_vExchanges.insert( "OPRA" );
  //m_vExchanges.insert( "COMEX" );
  //m_vExchanges.insert( "COMEX,COMEX_GBX" );
  //m_vExchanges.insert( "TSE" );
  //m_vExchanges.insert( "CANADIAN,TSE" );  // don't do yet, simplifies contract creation for IB

  m_vClassifiers.clear();
  m_vClassifiers.insert( ou::tf::IQFeedSymbolListOps::classifier_t::Equity );
  m_vClassifiers.insert( ou::tf::IQFeedSymbolListOps::classifier_t::IEOption );
  //m_vClassifiers.insert( ou::tf::IQFeedSymbolListOps::classifier_t::Future );
  //m_vClassifiers.insert( ou::tf::IQFeedSymbolListOps::classifier_t::FOption );

  std::cout << "Subsetting symbols ... " << std::endl;

  CallAfter( [this](){
    if ( m_worker.joinable() ) m_worker.join(); // need to finish off any previous thread
    m_worker = std::thread(
      [this](){
        ou::tf::iqfeed::InMemoryMktSymbolList listIQFeedSymbols;
        ou::tf::IQFeedSymbolListOps::SelectSymbols selection( m_vClassifiers, listIQFeedSymbols );
        m_listIQFeedSymbols.SelectSymbolsByExchange( m_vExchanges.begin(), m_vExchanges.end(), selection );
        std::cout << "  " << listIQFeedSymbols.Size() << " symbols in subset." << std::endl;

        std::cout << "Saving subset to " << sFileNameMarketSymbolSubset << " ..." << std::endl;
        listIQFeedSymbols.SaveToFile( sFileNameMarketSymbolSubset );  // __.ser
        std::cout << " ... done." << std::endl;
      });
  });

}

// TODO: set flag to only load once?  Otherwise, is the structure cleared first?
void AppBasketTrading::HandleMenuActionLoadSymbolSubset( void ) {
  std::cout << "Loading From " << sFileNameMarketSymbolSubset << " ..." << std::endl;
  CallAfter( [this](){
    if ( m_worker.joinable() ) m_worker.join(); // need to finish off any previous thread
    m_worker = std::thread(
      [this](){
        m_listIQFeedSymbols.LoadFromFile( sFileNameMarketSymbolSubset );  // __.ser
        std::cout << "  " << m_listIQFeedSymbols.Size() << " symbols loaded." << std::endl;
      } );
  });
}

