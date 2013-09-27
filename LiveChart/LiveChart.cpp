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

#include <wx/mstream.h>
#include <wx/bitmap.h>

#include <OUCommon/TimeSource.h>

#include <TFTrading/InstrumentManager.h>
#include <TFTrading/AccountManager.h>
#include <TFTrading/OrderManager.h>

#include <TFOptions/CalcExpiry.h>

#include "LiveChart.h"

IMPLEMENT_APP(AppLiveChart)

size_t atm = 125;

bool AppLiveChart::OnInit() {

  m_pFrameMain = new FrameMain( 0, wxID_ANY, "LiveChart" );
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

  m_bReadyToDrawChart = false;
  m_winChart = new wxWindow( m_pFrameMain, wxID_ANY, wxDefaultPosition, wxSize(160, 90), wxNO_BORDER );
  m_sizerMain->Add( m_winChart, 1, wxALL|wxEXPAND, 5);
  wxWindowID idChart = m_winChart->GetId();
  m_winChart->Bind( wxEVT_PAINT, &AppLiveChart::HandlePaint, this, idChart );
  m_winChart->Bind( wxEVT_SIZE, &AppLiveChart::HandleSize, this, idChart );

  m_bData1Connected = false;
  m_bData2Connected = false;
  m_bExecConnected = false;

  m_pBundle = 0;
//  m_pStrategy = 0;

  m_timerGuiRefresh.SetOwner( this );

  Bind( wxEVT_TIMER, &AppLiveChart::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppLiveChart::OnClose, this );  // start close of windows and controls

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
  
//  std::string sDbName( "HedgedBollingerX.db" );
//  if ( boost::filesystem::exists( sDbName ) ) {
//    boost::filesystem::remove( sDbName );
//  }


//  m_db.OnRegisterTables.Add( MakeDelegate( this, &AppLiveChart::HandleRegisterTables ) );
//  m_db.OnRegisterRows.Add( MakeDelegate( this, &AppLiveChart::HandleRegisterRows ) );
//  m_db.SetOnPopulateDatabaseHandler( MakeDelegate( this, &AppLiveChart::HandlePopulateDatabase ) );

//  m_db.Open( sDbName );

  FrameMain::vpItems_t vItems;
  typedef FrameMain::structMenuItem mi;  // vxWidgets takes ownership of the objects
//  vItems.push_back( new mi( "a1 New Symbol List Remote", MakeDelegate( this, &AppLiveChart::HandleMenuAction0ObtainNewIQFeedSymbolListRemote ) ) );
//  vItems.push_back( new mi( "a2 New Symbol List Local", MakeDelegate( this, &AppLiveChart::HandleMenuAction1ObtainNewIQFeedSymbolListLocal ) ) );
//  vItems.push_back( new mi( "a3 Load Symbol List", MakeDelegate( this, &AppLiveChart::HandleMenuAction2LoadIQFeedSymbolList ) ) );
//  vItems.push_back( new mi( "b1 Initialize Symbols", MakeDelegate( this, &AppLiveChart::HandleMenuActionInitializeSymbolSet ) ) );
  vItems.push_back( new mi( "c1 Start Watch", MakeDelegate( this, &AppLiveChart::HandleMenuActionStartWatch ) ) );
  vItems.push_back( new mi( "c2 Stop Watch", MakeDelegate( this, &AppLiveChart::HandleMenuActionStopWatch ) ) );
//  vItems.push_back( new mi( "d1 Save Values", MakeDelegate( this, &AppLiveChart::HandleMenuActionSaveValues ) ) );
//  vItems.push_back( new mi( "e1 Libor Yield Curve", MakeDelegate( this, &AppLiveChart::HandleMenuActionEmitYieldCurve ) ) );
  vItems.push_back( new mi( "f1 Start Chart", MakeDelegate( this, &AppLiveChart::HandleMenuActionStartChart ) ) );
  m_pFrameMain->AddDynamicMenu( "Actions", vItems );

  this->m_pData1Provider->Connect();

  return 1;

}

void AppLiveChart::HandleMenuActionStartChart( void ) {
  m_bReadyToDrawChart = true;
  m_pChart = new ChartTest( m_pData1Provider );

  m_winChart->RefreshRect( m_winChart->GetClientRect(), false );
}

void AppLiveChart::HandlePaint( wxPaintEvent& event ) {
  if ( m_bReadyToDrawChart ) {
    try {
      wxSize size = m_winChart->GetClientSize();
      m_chartMaster.SetChartDimensions( size.GetWidth(), size.GetHeight() );
      m_chartMaster.SetChartDataView( &m_pChart->GetChartDataView() );
      m_chartMaster.SetOnDrawChart( MakeDelegate( this, &AppLiveChart::HandleDrawChart ) );
      m_chartMaster.DrawChart( );
    }
    catch (...) {
    }
  }
}

void AppLiveChart::HandleSize( wxSizeEvent& event ) { 
  m_winChart->RefreshRect( m_winChart->GetClientRect(), false );
}

void AppLiveChart::HandleDrawChart( const MemBlock& m ) {
  wxMemoryInputStream in( m.data, m.len );
  wxBitmap bmp( wxImage( in, wxBITMAP_TYPE_BMP) );
  wxPaintDC cdc( m_winChart );
  cdc.DrawBitmap(bmp, 0, 0);
}

void AppLiveChart::HandleGuiRefresh( wxTimerEvent& event ) {
  m_winChart->RefreshRect( m_winChart->GetClientRect(), false );

  // Process IV Calc once a minute
/*
  ptime dt;
  // need to deal with market closing time frame on expiry friday, no further calcs after market close on that day
  ou::TimeSource::Instance().Internal( &dt );
  if ( dt > m_dtTopOfMinute ) {
    m_dtTopOfMinute = dt + time_duration( 0, 1, 0 ) - time_duration( 0, 0, dt.time_of_day().seconds(), dt.time_of_day().fractional_seconds() );
    std::cout << "Current: " << dt << " Next: " << m_dtTopOfMinute << std::endl;
    if ( !m_bIVCalcActive ) {
      if ( 0 != m_pIVCalc ) delete m_pIVCalc;
      m_bIVCalcActive = true;
      m_pIVCalc = new boost::thread( boost::bind( &AppLiveChart::CalcIV, this, dt ) );
    }
  }
  */
}

void AppLiveChart::HandleMenuActionStartWatch( void ) {
/*
  m_pBundle->StartWatch();

  m_bIVCalcActive = false;
  ptime dt;
  ou::TimeSource::Instance().Internal( &dt );
  m_dtTopOfMinute = dt + time_duration( 0, 1, 0 ) - time_duration( 0, 0, dt.time_of_day().seconds() );
  */
  m_timerGuiRefresh.Start( 250 );
}

void AppLiveChart::HandleMenuActionStopWatch( void ) {

  m_timerGuiRefresh.Stop();

//  m_pBundle->StopWatch();

}

void AppLiveChart::HandleMenuActionSaveValues( void ) {
  m_worker.Run( MakeDelegate( this, &AppLiveChart::HandleSaveValues ) );
}

void AppLiveChart::HandleSaveValues( void ) {
  std::cout << "Saving collected values ... " << std::endl;
  try {
    std::string sPrefixSession( "/app/HedgedBollinger/" + m_sTSDataStreamStarted + "/" + m_pBundle->Name() );
    //std::string sPrefix86400sec( "/bar/86400/AtmIV/" + iter->second.sName.substr( 0, 1 ) + "/" + iter->second.sName );
    std::string sPrefix86400sec( "/bar/86400/AtmIV/" + m_pBundle->Name() );
    m_pBundle->SaveData( sPrefixSession, sPrefix86400sec );
  }
  catch(...) {
    std::cout << " ... issues with saving ... " << std::endl;
  }
  std::cout << "  ... Done " << std::endl;
}

void AppLiveChart::HandleMenuActionInitializeSymbolSet( void ) {
  if ( m_listIQFeedSymbols.begin() == m_listIQFeedSymbols.end() ) {
    std::cout << "Need to load symbols first" << std::endl;
  }
  else {
    if ( 0 != m_pBundle ) {
      std::cout << "Bundle Already set" << std::endl;
    }
    else {

      // work out current expiry and next expiry
      ptime now = ou::TimeSource::Instance().External();
      boost::gregorian::date dateFrontMonth = ou::tf::option::CurrentFrontMonthExpiry( now.date() );
      boost::gregorian::date dateSecondMonth = ou::tf::option::Next3rdFriday( dateFrontMonth );

      // 18:30 deals with after hours trading and settlements on the underlying.  the options cease trading at 16:00.
//      ptime dtFrontMonthExpiryUtc( 
//        ou::TimeSource::Instance().ConvertRegionalToUtc( dateFrontMonth, time_duration( 18, 30 , 0 ), "America/New_York", true ) );
//      ptime dtSecondMonthExpiryUtc( 
//        ou::TimeSource::Instance().ConvertRegionalToUtc( dateSecondMonth, time_duration( 18, 30 , 0 ), "America/New_York", true ) );

    // http://www.cboe.com/products/EquityOptionSpecs.aspx
    //Expiration Date:
    //Saturday immediately following the third Friday of the expiration month until February 15, 2015. 
    //  On and after February 15, 2015, the expiration date will be the third Friday of the expiration month.

    //Expiration Months:
    //Two near-term months plus two additional months from the January, February or March quarterly cycles.

      // **** All program calculations should normailze everything to the Friday.

      //std::cout << "Expiry strings: " << dtFrontMonthExpiryUtc << ", " << dtSecondMonthExpiryUtc << std::endl;
      std::cout << "Expiry strings: " << dateFrontMonth << ", " << dateSecondMonth << std::endl;

      std::string sName( "GLD" );

      m_pBundle = new ou::tf::option::MultiExpiryBundle( sName );
      m_pBundle->CreateExpiryBundle( dateFrontMonth );
      m_pBundle->CreateExpiryBundle( dateSecondMonth );

      pInstrument_t pInstrumentUnderlying;
      pInstrumentUnderlying.reset( 
        new ou::tf::Instrument( sName, ou::tf::InstrumentType::Stock, "SMART" ) );  // need to register this with InstrumentManager before trading
      m_pBundle->SetWatchUnderlying( pInstrumentUnderlying, m_pData1Provider );

      m_pBundle->Portfolio()
        = ou::tf::PortfolioManager::Instance().ConstructPortfolio( 
          sName, "aoRay", "USD", ou::tf::Portfolio::MultiLeggedPosition, ou::tf::Currency::Name[ ou::tf::Currency::USD ], sName + " Hedge" );

      pProvider_t pNull;
      m_listIQFeedSymbols.SelectOptionsByUnderlying( sName, ou::tf::option::PopulateMultiExpiryBundle( *m_pBundle, m_pData1Provider, pNull ) );

//      m_pStrategy = new Strategy( m_pBundle );

      std::cout << "Initialized." << std::endl;

    }
  }
  
}

void AppLiveChart::HandleMenuAction0ObtainNewIQFeedSymbolListRemote( void ) {
  // need to lock out from running HandleLoadIQFeedSymbolList at the same time
  m_worker.Run( MakeDelegate( this, &AppLiveChart::HandleObtainNewIQFeedSymbolListRemote ) );
}

void AppLiveChart::HandleObtainNewIQFeedSymbolListRemote( void ) {
  std::cout << "Downloading Text File ... " << std::endl;
  ou::tf::iqfeed::LoadMktSymbols( m_listIQFeedSymbols, ou::tf::iqfeed::MktSymbolLoadType::Download, true ); 
  std::cout << "Saving Binary File ... " << std::endl;
  m_listIQFeedSymbols.SaveToFile( "symbols.ser" );
  std::cout << " ... done." << std::endl;
}

void AppLiveChart::HandleMenuAction1ObtainNewIQFeedSymbolListLocal( void ) {
  // need to lock out from running HandleLoadIQFeedSymbolList at the same time
  m_worker.Run( MakeDelegate( this, &AppLiveChart::HandleObtainNewIQFeedSymbolListLocal ) );
}

void AppLiveChart::HandleObtainNewIQFeedSymbolListLocal( void ) {
  std::cout << "Loading From Text File ... " << std::endl;
  ou::tf::iqfeed::LoadMktSymbols( m_listIQFeedSymbols, ou::tf::iqfeed::MktSymbolLoadType::LoadTextFromDisk, false ); 
  std::cout << "Saving Binary File ... " << std::endl;
  m_listIQFeedSymbols.SaveToFile( "symbols.ser" );
  std::cout << " ... done." << std::endl;
}

void AppLiveChart::HandleMenuAction2LoadIQFeedSymbolList( void ) {
  // need to lock out from running HandleObtainNewIQFeedSymbolList at the same time
  m_worker.Run( MakeDelegate( this, &AppLiveChart::HandleLoadIQFeedSymbolList ) );
}

void AppLiveChart::HandleLoadIQFeedSymbolList( void ) {
  std::cout << "Loading From Binary File ..." << std::endl;
  m_listIQFeedSymbols.LoadFromFile( "phisymbols.ser" );
  std::cout << " ... completed." << std::endl;
}


// runs in thread
void AppLiveChart::CalcIV( ptime dt ) {
  static time_duration tdMarketOpen( 9, 30, 0, 30 );  // eastern time, plus time to settle
  static time_duration tdMarketClose( 16, 0, 0 );  // eastern time
  ptime dtMarketOpen( 
    ou::TimeSource::Instance().ConvertRegionalToUtc( dt.date(), tdMarketOpen, "America/New_York", true ) );
  ptime dtMarketClose( 
    ou::TimeSource::Instance().ConvertRegionalToUtc( dt.date(), tdMarketClose, "America/New_York", true ) );
  if ( ( dtMarketOpen < dt ) && ( dt < dtMarketClose ) ) {
    boost::timer::auto_cpu_timer t;
    m_pBundle->CalcIV( dt, m_libor );
    //for ( mapInstrumentCombo_t::iterator iter = m_mapInstrumentCombo.begin(); m_mapInstrumentCombo.end() != iter; ++iter ) {
//      iter->second.CalcIV( dt, m_libor );
    //}
  }
  m_bIVCalcActive = false;
}

void AppLiveChart::HandleMenuActionEmitYieldCurve( void ) {
  //ou::tf::libor::EmitYieldCurve();
  //m_libor.EmitYieldCurve();
  std::cout << m_libor;
}

int AppLiveChart::OnExit() {
  // Exit Steps: #4
//  DelinkFromPanelProviderControl();  generates stack errors
  //m_timerGuiRefresh.Stop();
  this->m_pData1Provider->Disconnect();
  m_listIQFeedSymbols.Clear();
  if ( m_db.IsOpen() ) m_db.Close();

  return wxAppConsole::OnExit();
}

//void AppWeeklies::HandlePanelFocusPropogate( unsigned int ix ) {
//}


void AppLiveChart::OnClose( wxCloseEvent& event ) {
  // Exit Steps: #2 -> FrameMain::OnClose
  m_timerGuiRefresh.Stop();
  DelinkFromPanelProviderControl();
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a 
  event.Skip();  // auto followed by Destroy();
}

void AppLiveChart::OnData1Connected( int ) {
  m_bData1Connected = true;
  //ou::tf::libor::SetWatchOn( m_pData1Provider );
//  m_libor.SetWatchOn( m_pData1Provider );
//  AutoStartCollection();
  if ( m_bData1Connected & m_bExecConnected ) {
    // set start to enabled
  }
}

void AppLiveChart::OnData2Connected( int ) {
  m_bData2Connected = true;
//  AutoStartCollection();
  if ( m_bData2Connected & m_bExecConnected ) {
    // set start to enabled
  }
}

void AppLiveChart::OnExecConnected( int ) {
  m_bExecConnected = true;
  if ( m_bData1Connected & m_bExecConnected ) {
    // set start to enabled
  }
}

void AppLiveChart::OnData1Disconnected( int ) {
//  m_libor.SetWatchOff();
  m_bData1Connected = false;
}

void AppLiveChart::OnData2Disconnected( int ) {
  m_bData2Connected = false;
}

void AppLiveChart::OnExecDisconnected( int ) {
  m_bExecConnected = false;
}

void AppLiveChart::HandleRegisterTables(  ou::db::Session& session ) {
}

void AppLiveChart::HandleRegisterRows(  ou::db::Session& session ) {
}

void AppLiveChart::HandlePopulateDatabase( void ) {
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

  m_pPortfolioMaster
    = ou::tf::PortfolioManager::Instance().ConstructPortfolio( 
    "Master", "aoRay", "", ou::tf::Portfolio::Master, ou::tf::Currency::Name[ ou::tf::Currency::USD ], "Hedged Bollinger" );

  m_pPortfolioCurrencyUSD
    = ou::tf::PortfolioManager::Instance().ConstructPortfolio( 
    "USD", "aoRay", "Master", ou::tf::Portfolio::CurrencySummary, ou::tf::Currency::Name[ ou::tf::Currency::USD ], "Hedged Bollinger" );
*/
}

