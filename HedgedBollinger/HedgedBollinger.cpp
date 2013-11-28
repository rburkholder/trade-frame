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
#include <wx/splitter.h>
#include <wx/panel.h>

#include <OUCommon/TimeSource.h>

#include <TFTrading/InstrumentManager.h>
#include <TFTrading/AccountManager.h>
#include <TFTrading/OrderManager.h>

#include <TFOptions/CalcExpiry.h>

//#include "EventUpdateOptionTree.h"
#include "HedgedBollinger.h"

IMPLEMENT_APP(AppHedgedBollinger)

size_t atm = 125;

unsigned int AppHedgedBollinger::m_nthIVCalc( 4 );

bool AppHedgedBollinger::OnInit() {

  m_pFrameMain = new FrameMain( 0, wxID_ANY, "Hedged Bollinger" );
  wxWindowID idFrameMain = m_pFrameMain->GetId();
  //m_pFrameMain->Bind( wxEVT_SIZE, &AppStrategy1::HandleFrameMainSize, this, idFrameMain );
  //m_pFrameMain->Bind( wxEVT_MOVE, &AppStrategy1::HandleFrameMainMove, this, idFrameMain );
  //m_pFrameMain->Center();
//  m_pFrameMain->Move( -2500, 50 );
  m_pFrameMain->SetSize( 800, 500 );
  SetTopWindow( m_pFrameMain );

  wxBoxSizer* m_sizerFrame;
  m_sizerFrame = new wxBoxSizer(wxVERTICAL);
  m_pFrameMain->SetSizer(m_sizerFrame);

  wxBoxSizer* m_sizerFrameRow1;
  m_sizerFrameRow1 = new wxBoxSizer( wxHORIZONTAL );
  m_sizerFrame->Add(m_sizerFrameRow1, 0, wxGROW|wxALL, 2 );

  // splitter
  wxSplitterWindow* m_splitterRow1;
  m_splitterRow1 = new wxSplitterWindow( m_pFrameMain );
  m_splitterRow1->SetMinimumPaneSize(10);
  m_splitterRow1->SetSashGravity(0.2);

  // tree for viewed symbols
  m_ptreeChartables = new wxTreeCtrl( m_splitterRow1 );
  wxTreeItemId idRoot = m_ptreeChartables->AddRoot( "/", -1, -1, 0 );

  // panel for right side of splitter
  wxPanel* m_panelSplitterRight;
  m_panelSplitterRight = new wxPanel( m_splitterRow1 );

  // sizer for right side of splitter
  wxBoxSizer* m_sizerSplitterRight;
  m_sizerSplitterRight = new wxBoxSizer( wxHORIZONTAL );
  m_panelSplitterRight->SetSizer( m_sizerSplitterRight );

  // m_pPanelProviderControl
  m_pPanelProviderControl = new ou::tf::PanelProviderControl( m_panelSplitterRight, wxID_ANY );
  m_sizerSplitterRight->Add( m_pPanelProviderControl, 0, wxEXPAND|wxALIGN_LEFT|wxRIGHT, 1);

  // m_pPanelLogging
  m_pPanelLogging = new ou::tf::PanelLogging( m_panelSplitterRight, wxID_ANY );
  m_sizerSplitterRight->Add( m_pPanelLogging, 1, wxALL | wxEXPAND|wxALIGN_LEFT|wxALIGN_RIGHT|wxALIGN_TOP|wxALIGN_BOTTOM, 1);

  // startup splitter
  m_splitterRow1->SplitVertically(m_ptreeChartables, m_panelSplitterRight, 10);
  m_sizerFrameRow1->Add(m_splitterRow1, 1, wxEXPAND|wxALL, 1);

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

//  wxBoxSizer* m_sizerStatus = new wxBoxSizer( wxHORIZONTAL );
//  m_sizerMain->Add( m_sizerStatus, 1, wxEXPAND|wxALL, 5 );

  m_pFrameMain->Show( true );

  m_sNameUnderlying = "+GC#";
  m_sNameOptionUnderlying = "QGC";  // GC is regular open outcry symbol, QGC are options tradeable 24 hours

  m_pChartBitmap = 0;
  m_bInDrawChart = false;
  m_bReadyToDrawChart = false;
  m_winChart = new wxWindow( m_pFrameMain, wxID_ANY, wxDefaultPosition, wxSize(160, 90), wxNO_BORDER );
  m_sizerFrame->Add( m_winChart, 1, wxALL|wxEXPAND, 3);
  wxWindowID idChart = m_winChart->GetId();
  m_winChart->Bind( wxEVT_PAINT, &AppHedgedBollinger::HandlePaint, this, idChart );
  m_winChart->Bind( wxEVT_SIZE, &AppHedgedBollinger::HandleSize, this, idChart );

  m_tdViewPortWidth = boost::posix_time::time_duration( 0, 10, 0 );  // viewport width is 10 minutes, until we make it adjustable

  m_bData1Connected = false;
  m_bData2Connected = false;
  m_bExecConnected = false;

  m_pBundle = 0;
  m_pStrategy = 0;

  m_cntIVCalc = m_nthIVCalc;
  m_timerGuiRefresh.SetOwner( this );
  Bind( wxEVT_TIMER, &AppHedgedBollinger::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );

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
  
  std::string sDbName( "HedgedBollinger.db" );
  if ( boost::filesystem::exists( sDbName ) ) {
    boost::filesystem::remove( sDbName );
  }

  m_db.OnRegisterTables.Add( MakeDelegate( this, &AppHedgedBollinger::HandleRegisterTables ) );
  m_db.OnRegisterRows.Add( MakeDelegate( this, &AppHedgedBollinger::HandleRegisterRows ) );
  m_db.SetOnPopulateDatabaseHandler( MakeDelegate( this, &AppHedgedBollinger::HandlePopulateDatabase ) );

  m_db.Open( sDbName );

  FrameMain::vpItems_t vItems;
  typedef FrameMain::structMenuItem mi;  // vxWidgets takes ownership of the objects
  vItems.push_back( new mi( "a1 New Symbol List Remote", MakeDelegate( this, &AppHedgedBollinger::HandleMenuAction0ObtainNewIQFeedSymbolListRemote ) ) );
  vItems.push_back( new mi( "a2 New Symbol List Local", MakeDelegate( this, &AppHedgedBollinger::HandleMenuAction1ObtainNewIQFeedSymbolListLocal ) ) );
  vItems.push_back( new mi( "a3 Load Symbol List", MakeDelegate( this, &AppHedgedBollinger::HandleMenuAction2LoadIQFeedSymbolList ) ) );
  vItems.push_back( new mi( "a4 Save Symbol Subset", MakeDelegate( this, &AppHedgedBollinger::HandleMenuActionSaveSymbolSubset ) ) );
  vItems.push_back( new mi( "a5 Load Symbol Subset", MakeDelegate( this, &AppHedgedBollinger::HandleMenuActionLoadSymbolSubset ) ) );
  vItems.push_back( new mi( "b1 Initialize Watch", MakeDelegate( this, &AppHedgedBollinger::HandleMenuActionInitializeSymbolSet ) ) );
  vItems.push_back( new mi( "c1 Start Watch", MakeDelegate( this, &AppHedgedBollinger::HandleMenuActionStartWatch ) ) );
  vItems.push_back( new mi( "c2 Stop Watch", MakeDelegate( this, &AppHedgedBollinger::HandleMenuActionStopWatch ) ) );
  vItems.push_back( new mi( "d1 Save Values", MakeDelegate( this, &AppHedgedBollinger::HandleMenuActionSaveValues ) ) );
  vItems.push_back( new mi( "e1 Libor Yield Curve", MakeDelegate( this, &AppHedgedBollinger::HandleMenuActionEmitYieldCurve ) ) );
  vItems.push_back( new mi( "f1 Start Chart", MakeDelegate( this, &AppHedgedBollinger::HandleMenuActionStartChart ) ) );
  vItems.push_back( new mi( "f2 Stop Chart", MakeDelegate( this, &AppHedgedBollinger::HandleMenuActionStopChart ) ) );
  m_pFrameMain->AddDynamicMenu( "Actions", vItems );

  m_bThreadDrawChartActive = true;
  m_pThreadDrawChart = new boost::thread( &AppHedgedBollinger::ThreadDrawChart1, this );

  return 1;

}

void AppHedgedBollinger::HandleMenuActionStartChart( void ) {
  m_bReadyToDrawChart = true;
}

void AppHedgedBollinger::HandleMenuActionStopChart( void ) {
  m_bReadyToDrawChart = false;
}

void AppHedgedBollinger::HandleSize( wxSizeEvent& event ) { 
  StartDrawChart();
}

void AppHedgedBollinger::HandlePaint( wxPaintEvent& event ) {
  if ( event.GetId() == m_winChart->GetId() ) {
    wxPaintDC dc( m_winChart );
//    dc.DrawBitmap( *m_pChartBitmap, 0, 0);
//    m_bInDrawChart = false;
  }
  else event.Skip();
}

void AppHedgedBollinger::StartDrawChart( void ) {
  if ( m_bReadyToDrawChart ) {
    m_bInDrawChart = true;
    m_cvThreadDrawChart.notify_one();
  }
}

void AppHedgedBollinger::ThreadDrawChart1( void ) {
  boost::unique_lock<boost::mutex> lock(m_mutexThreadDrawChart);
  while ( m_bThreadDrawChartActive ) {
    m_cvThreadDrawChart.wait( lock );

    if ( m_bThreadDrawChartActive ) {  // exit thread if false without doing anything
      // need to deal with market closing time frame on expiry friday, no further calcs after market close on that day
      ptime now = ou::TimeSource::Instance().External();

      static boost::posix_time::time_duration::fractional_seconds_type fs( 1 );
      boost::posix_time::time_duration td( 0, 0, 0, fs - now.time_of_day().fractional_seconds() );
      ptime dtEnd = now + td; 

      ptime dtBegin = dtEnd - m_tdViewPortWidth;
      m_pStrategy->GetChartDataView().SetViewPort( dtBegin, dtEnd );
      m_pStrategy->GetChartDataView().SetThreadSafe( true );

      wxSize size = m_winChart->GetClientSize();  // may not be able to do this cross thread
      m_chart.SetChartDimensions( size.GetWidth(), size.GetHeight() );
      m_chart.SetChartDataView( &m_pStrategy->GetChartDataView() );
      m_chart.SetOnDrawChart( MakeDelegate( this, &AppHedgedBollinger::ThreadDrawChart2 ) );  // this line could be factored out?
      m_chart.DrawChart( );
    }
  }
}

void AppHedgedBollinger::ThreadDrawChart2( const MemBlock& m ) {
  wxMemoryInputStream in( m.data, m.len );  // need this
  wxBitmap* p = new wxBitmap( wxImage( in, wxBITMAP_TYPE_BMP) ); // and need this to keep the drawn bitmap, then memblock can be reclaimed
  QueueEvent( new EventDrawChart( EVENT_DRAW_CHART, -1, p ) );
}

void AppHedgedBollinger::HandleGuiDrawChart( EventDrawChart& event ) {
  if ( 0 != m_pChartBitmap ) delete m_pChartBitmap;
  m_pChartBitmap = event.GetBitmap();
  wxClientDC dc( m_winChart );
  dc.DrawBitmap( *m_pChartBitmap, 0, 0);
  m_bInDrawChart = false;
}

void AppHedgedBollinger::HandleMenuActionStartWatch( void ) {

  m_pBundle->StartWatch();

  m_pIVCalc = 0;
  m_bIVCalcActive = false;

  ptime dt;
  ou::TimeSource::Instance().Internal( &dt );
  m_dtTopOfMinute = dt + time_duration( 0, 1, 0 ) - time_duration( 0, 0, dt.time_of_day().seconds() );
  m_timerGuiRefresh.Start( 250 );

}

void AppHedgedBollinger::HandleMenuActionStopWatch( void ) {

  m_timerGuiRefresh.Stop();

  m_pBundle->StopWatch();

}

void AppHedgedBollinger::HandleMenuActionSaveValues( void ) {
  m_worker.Run( MakeDelegate( this, &AppHedgedBollinger::HandleSaveValues ) );
}

void AppHedgedBollinger::HandleSaveValues( void ) {
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

void AppHedgedBollinger::HandleMenuActionInitializeSymbolSet( void ) {
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
      // opra equity calc
//      boost::gregorian::date dateFrontMonth = ou::tf::option::CurrentFrontMonthExpiry( now.date() );
//      boost::gregorian::date dateSecondMonth = ou::tf::option::Next3rdFriday( dateFrontMonth );
      // gold future calc:
      // http://www.cmegroup.com/trading/metals/files/pm264-fact-card-gold-options.pdf
      // expiry: four business days prior to end of month, not on friday, 13:30pm, assignments notify 16:30, excercise 20:00
      // trading: sunday - friday 18:00 - 17:15 et
      // http://www.cmegroup.com/trading/metals/precious/gold_product_calendar_futures.html
      // in trading state machine, indicate when 8 days prior to expiry of front month in order to
      //   liquidate remaining positions, possibly make use of Augen's book on option expiry trading to do so
      //boost::gregorian::date dateFrontMonth( boost::gregorian::date( 2013, 10, 28 ) );
      boost::gregorian::date dateFrontMonth( boost::gregorian::date( 2013, 12, 26 ) );
      boost::gregorian::date dateSecondMonth( boost::gregorian::date( 2014, 1, 28 ) );

      // GC Futures:
      ptime dtFrontMonthExpiryUtc( 
        ou::TimeSource::Instance().ConvertRegionalToUtc( dateFrontMonth, time_duration( 13, 30 , 0 ), "America/New_York", true ) );
      ptime dtSecondMonthExpiryUtc( 
        ou::TimeSource::Instance().ConvertRegionalToUtc( dateSecondMonth, time_duration( 13, 30 , 0 ), "America/New_York", true ) );

      // use 16:00 est as time of expiry, as that is when they cease trading (for OPRA equities)
      // 18:30 deals with after hours trading and settlements on the underlying.  the options cease trading at 16:00.

    // http://www.cboe.com/products/EquityOptionSpecs.aspx
    //Expiration Date:
    //Saturday immediately following the third Friday of the expiration month until February 15, 2015. 
    //  On and after February 15, 2015, the expiration date will be the third Friday of the expiration month.

    //Expiration Months:
    //Two near-term months plus two additional months from the January, February or March quarterly cycles.

      std::cout << "Expiry strings: " << dtFrontMonthExpiryUtc << ", " << dtSecondMonthExpiryUtc << std::endl;
      std::cout << "Expiry strings: " << dateFrontMonth << ", " << dateSecondMonth << std::endl;

      m_pBundle = new ou::tf::option::MultiExpiryBundle( m_sNameUnderlying );

      pInstrument_t pInstrumentUnderlying;
      pInstrumentUnderlying.reset( 
        new ou::tf::Instrument( m_sNameUnderlying, ou::tf::InstrumentType::Future, "SMART" ) );  // need to register this with InstrumentManager before trading

      m_pBundle->SetWatchUnderlying( pInstrumentUnderlying, m_pData1Provider );

      m_pBundle->CreateExpiryBundle( dtFrontMonthExpiryUtc );
      m_pBundle->CreateExpiryBundle( dtSecondMonthExpiryUtc );

      pProvider_t pNull;
      m_listIQFeedSymbols.SelectOptionsByUnderlying( m_sNameOptionUnderlying, ou::tf::option::PopulateMultiExpiryBundle( *m_pBundle, m_pData1Provider, pNull ) );

      std::cout << *m_pBundle;

      m_pBundle->Portfolio()
        = ou::tf::PortfolioManager::Instance().ConstructPortfolio( 
          m_sNameOptionUnderlying, "aoRay", "USD", ou::tf::Portfolio::MultiLeggedPosition, ou::tf::Currency::Name[ ou::tf::Currency::USD ], m_sNameUnderlying + " Hedge" );

      m_pStrategy = new Strategy( m_pBundle );
      m_pStrategy->GetChartDataView().SetNames( "HedgedBollinger", "+GC#" );

      Bind( EVENT_UPDATE_OPTION_TREE, &AppHedgedBollinger::HandleGuiUpdateOptionTree, this );
      Bind( EVENT_DRAW_CHART, &AppHedgedBollinger::HandleGuiDrawChart, this );



      m_pBundle->AddOnStrikeWatchOn( MakeDelegate( this, &AppHedgedBollinger::HandleStrikeWatchOn ) );
      m_pBundle->AddOnStrikeWatchOff( MakeDelegate( this, &AppHedgedBollinger::HandleStrikeWatchOff ) );

      std::cout << "Initialized." << std::endl;

    }
  }
}

void AppHedgedBollinger::HandleGuiUpdateOptionTree( EventUpdateOptionTree& event ) {
  UpdateTree( event.GetStrike().Call(), event.GetWatch() );
  UpdateTree( event.GetStrike().Put(),  event.GetWatch() );
}

void AppHedgedBollinger::HandleMenuActionSaveSymbolSubset( void ) {
  ou::tf::iqfeed::InMemoryMktSymbolList listIQFeedSymbols;
  listIQFeedSymbols.HandleParsedStructure( m_listIQFeedSymbols.GetTrd( m_sNameUnderlying ) );
  m_listIQFeedSymbols.SelectOptionsByUnderlying( m_sNameOptionUnderlying, listIQFeedSymbols );
  listIQFeedSymbols.SaveToFile( "HedgedBollinger.ser" );
  std::cout << "Symbols saved." << std::endl;
}

void AppHedgedBollinger::HandleMenuActionLoadSymbolSubset( void ) {
  std::cout << "Loading From Binary File ..." << std::endl;
  m_listIQFeedSymbols.LoadFromFile( "HedgedBollinger.ser" );
  std::cout << " ... completed." << std::endl;
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
  m_listIQFeedSymbols.LoadFromFile( "symbols.ser" );
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

  StartDrawChart();

//  if ( dt > m_dtTopOfMinute ) {
//    m_dtTopOfMinute = dt + time_duration( 0, 1, 0 ) - time_duration( 0, 0, dt.time_of_day().seconds(), dt.time_of_day().fractional_seconds() );
//    std::cout << "Current: " << dt << " Next: " << m_dtTopOfMinute << std::endl;
    if ( !m_bIVCalcActive ) {
      --m_cntIVCalc;
      if ( 0 == m_cntIVCalc ) {
        m_cntIVCalc = m_nthIVCalc;
        if ( 0 != m_pIVCalc ) delete m_pIVCalc;
        m_bIVCalcActive = true;
        m_pIVCalc = new boost::thread( boost::bind( &AppHedgedBollinger::CalcIV, this, ou::TimeSource::Instance().External() ) );
      }
    }
  //}
}

// runs in thread
void AppHedgedBollinger::CalcIV( ptime dt ) {
//    boost::timer::auto_cpu_timer t;
    m_pBundle->CalcIV( dt, m_libor );
  m_bIVCalcActive = false;
}

void AppHedgedBollinger::HandleMenuActionEmitYieldCurve( void ) {
  //ou::tf::libor::EmitYieldCurve();
  //m_libor.EmitYieldCurve();
  std::cout << m_libor;
}

void AppHedgedBollinger::UpdateTree( ou::tf::option::Option* pOption, bool bWatching ) {
  // don't do this in worker thread, needs to be performed in gui thread
  wxTreeItemIdValue idCookie;
  const std::string& sName( pOption->GetInstrument()->GetInstrumentName() );
  wxTreeItemId idRoot = m_ptreeChartables->GetRootItem();
  wxTreeItemId idChild = m_ptreeChartables->GetFirstChild( idRoot, idCookie ); 
  bool bFound( false );
  while ( idChild.IsOk() ) {
    if ( sName == reinterpret_cast<ou::tf::option::Option*>( m_ptreeChartables->GetItemData( idChild ) )->GetInstrument()->GetInstrumentName() ) {
      m_ptreeChartables->SetItemBold( idChild, bWatching );
      bFound = true;
      break;
    }
    idChild = m_ptreeChartables->GetNextChild( idChild, idCookie );
  }
  if ( !bFound ) {
    wxTreeItemId idNewChild = m_ptreeChartables->AppendItem( idRoot, sName, -1, -1, reinterpret_cast<wxTreeItemData*>( pOption ) );
    m_ptreeChartables->SetItemBold( idNewChild, bWatching );
  }
}

void AppHedgedBollinger::HandleStrikeWatchOn( ou::tf::option::Strike& strike ) {
  QueueEvent( new EventUpdateOptionTree( EVENT_UPDATE_OPTION_TREE, -1, strike, true ) );
}

void AppHedgedBollinger::HandleStrikeWatchOff( ou::tf::option::Strike& strike ) {
  QueueEvent( new EventUpdateOptionTree( EVENT_UPDATE_OPTION_TREE, -1, strike, false ) );
}

int AppHedgedBollinger::OnExit() {
  // Exit Steps: #4
//  DelinkFromPanelProviderControl();  generates stack errors
  //m_timerGuiRefresh.Stop();

  m_bThreadDrawChartActive = false;
  m_cvThreadDrawChart.notify_one();
  m_pThreadDrawChart->join();
  delete m_pThreadDrawChart;
  m_pThreadDrawChart = 0;

  if ( 0 != m_pChartBitmap ) {
    delete m_pChartBitmap;
    m_pChartBitmap = 0;
  }

  delete m_pStrategy;
  m_pStrategy = 0;

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
  m_libor.SetWatchOn( m_pData1Provider );
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
  m_libor.SetWatchOff();
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


}

