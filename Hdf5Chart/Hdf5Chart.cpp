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

#include <TFHDF5TimeSeries/HDF5IterateGroups.h>

#include <TFOptions/CalcExpiry.h>

#include "Hdf5Chart.h"

IMPLEMENT_APP(AppHdf5Chart)

size_t atm = 125;

bool AppHdf5Chart::OnInit() {

  m_pdm = new ou::tf::HDF5DataManager( ou::tf::HDF5DataManager::RO );

  m_pFrameMain = new FrameMain( 0, wxID_ANY, "Hdf5 Chart" );
  wxWindowID idFrameMain = m_pFrameMain->GetId();
  //m_pFrameMain->Bind( wxEVT_SIZE, &AppStrategy1::HandleFrameMainSize, this, idFrameMain );
  //m_pFrameMain->Bind( wxEVT_MOVE, &AppStrategy1::HandleFrameMainMove, this, idFrameMain );
  //m_pFrameMain->Center();
//  m_pFrameMain->Move( -2500, 50 );
  m_pFrameMain->SetSize( 800, 500 );
  SetTopWindow( m_pFrameMain );

  // Sizer for FrameMain
  wxBoxSizer* m_sizerMain;
  m_sizerMain = new wxBoxSizer(wxVERTICAL);
  m_pFrameMain->SetSizer(m_sizerMain);

  // splitter
  wxSplitterWindow* splitter;
  splitter = new wxSplitterWindow( m_pFrameMain );
  splitter->SetMinimumPaneSize(10);
  splitter->SetSashGravity(0.2);

  // tree
  //wxTreeCtrl* tree;
  m_pHdf5Root = new wxTreeCtrl( splitter );
  m_eLatestDatumType = CustomItemData::NoDatum;
  wxTreeItemId idRoot = m_pHdf5Root->AddRoot( "/", -1, -1, new CustomItemData( CustomItemData::Root, m_eLatestDatumType ) );
  m_pHdf5Root->Bind( wxEVT_COMMAND_TREE_SEL_CHANGED, &AppLiveChart::HandleTreeEventItemActivated, this, m_pHdf5Root->GetId() );
  //m_pFrameMain->Bind( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, &AppLiveChart::HandleTreeEventItemActivated, this, m_pHdf5Root->GetId() );
//  m_pFrameMain->Bind( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, &AppLiveChart::HandleTreeEventItemActivated, this, m_pHdf5Root->GetId()  );
//  m_pFrameMain->Bind( wxEVT_COMMAND_TREE_SEL_CHANGED, &AppLiveChart::HandleTreeEventItemActivated, this, m_pHdf5Root->GetId()  );
//  m_pFrameMain->Bind( wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, &AppLiveChart::HandleTreeEventItemActivated, this, m_pHdf5Root->GetId()  );
//  m_pFrameMain->Bind( wxEVT_COMMAND_TREE_ITEM_MENU, &AppLiveChart::HandleTreeEventItemActivated, this, m_pHdf5Root->GetId()  );
//  tree->AppendItem( idRoot, "second" );
//  tree->AppendItem( idRoot, "third" );

  // panel for right side of splitter
  wxPanel* panelSplitterRightPanel;
  panelSplitterRightPanel = new wxPanel( splitter );

  // sizer for right side of splitter
  wxBoxSizer* sizerRight;
  sizerRight = new wxBoxSizer( wxVERTICAL );
  panelSplitterRightPanel->SetSizer( sizerRight );

  // Sizer for Controls
  wxBoxSizer* m_sizerControls;
  m_sizerControls = new wxBoxSizer( wxHORIZONTAL );
  //m_sizerMain->Add( m_sizerControls, 0, wxEXPAND|wxLEFT|wxTOP|wxRIGHT, 5 );
  //panelSplitterRight->SetSizer( m_sizerControls );
  sizerRight->Add( m_sizerControls, 0, wxStretch::wxEXPAND|wxALL, 5 );

  // populate variable in FrameWork01
  m_pPanelProviderControl = new ou::tf::PanelProviderControl( panelSplitterRightPanel, wxID_ANY );
  m_sizerControls->Add( m_pPanelProviderControl, 0, wxALIGN_LEFT|wxRIGHT, 5);
//  m_pPanelProviderControl->Show( true );

  LinkToPanelProviderControl();

  m_pPanelLogging = new ou::tf::PanelLogging( panelSplitterRightPanel, wxID_ANY );
  m_sizerControls->Add( m_pPanelLogging, 1, wxALL | wxEXPAND|wxALIGN_LEFT|wxALIGN_RIGHT|wxALIGN_TOP|wxALIGN_BOTTOM, 0);
//  m_pPanelLogging->Show( true );

  splitter->SplitVertically( m_pHdf5Root, panelSplitterRightPanel, 0 );
  m_sizerMain->Add( splitter, 1, wxGROW|wxALL, 5 );



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

  m_bPaintingChart = false;
  m_bReadyToDrawChart = false;
  m_winChart = new wxWindow( panelSplitterRightPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  sizerRight->Add( m_winChart, 1, wxALL|wxEXPAND, 5);
  wxWindowID idChart = m_winChart->GetId();
  m_winChart->Bind( wxEVT_PAINT, &AppLiveChart::HandlePaint, this, idChart );
  m_winChart->Bind( wxEVT_SIZE, &AppLiveChart::HandleSize, this, idChart );

  wxBoxSizer* m_sizerStatus = new wxBoxSizer( wxHORIZONTAL );
  m_sizerMain->Add( m_sizerStatus, 1, wxEXPAND|wxALL, 5 );

  m_pFrameMain->Show( true );

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
  vItems.push_back( new mi( "c2 Start Chart", MakeDelegate( this, &AppLiveChart::HandleMenuActionStartChart ) ) );
  vItems.push_back( new mi( "c3 Stop Watch", MakeDelegate( this, &AppLiveChart::HandleMenuActionStopWatch ) ) );
  vItems.push_back( new mi( "d1 Save Values", MakeDelegate( this, &AppLiveChart::HandleMenuActionSaveValues ) ) );
//  vItems.push_back( new mi( "e1 Libor Yield Curve", MakeDelegate( this, &AppLiveChart::HandleMenuActionEmitYieldCurve ) ) );
  vItems.push_back( new mi( "e1 Load Tree", MakeDelegate( this, &AppLiveChart::HandleMenuActionLoadTree ) ) );
  m_pFrameMain->AddDynamicMenu( "Actions", vItems );

  this->m_pData1Provider->Connect();

  return 1;

}

void AppHdf5Chart::HandleMenuActionLoadTree( void ) {

  m_pHdf5Root->DeleteChildren( m_pHdf5Root->GetRootItem() );

  m_sCurrentPath = "/";  

  namespace args = boost::phoenix::placeholders;
  ou::tf::hdf5::IterateGroups ig( 
    "/", 
    boost::phoenix::bind( &AppLiveChart::HandleLoadTreeHdf5Group, this, args::arg1, args::arg2 ), 
    boost::phoenix::bind( &AppLiveChart::HandleLoadTreeHdf5Object, this, args::arg1, args::arg2 ) 
    );
}

void AppHdf5Chart::HandleLoadTreeHdf5Group( const std::string& s1, const std::string& s2 ) {
  if ( "quotes" == s2 ) m_eLatestDatumType = CustomItemData::Quotes;
  if ( "trades" == s2 ) m_eLatestDatumType = CustomItemData::Trades;
  if ( "bar" == s2 ) m_eLatestDatumType = CustomItemData::Bars;
  namespace args = boost::phoenix::placeholders;
  m_sCurrentPath = s1;
  m_curTreeItem = m_pHdf5Root->GetRootItem();  // should be '/'
  m_pdm->IteratePathParts( s1, boost::phoenix::bind( &AppLiveChart::HandleBuildTreePathParts, this, args::arg1 ) );
}

void AppHdf5Chart::HandleLoadTreeHdf5Object( const std::string& s1, const std::string& s2 ) {
  // assume group has us in the correct place, just add in the object now
  m_pHdf5Root->AppendItem( m_curTreeItem, s2, -1, -1, new CustomItemData( CustomItemData::Object, m_eLatestDatumType ) );
}

void AppHdf5Chart::HandleBuildTreePathParts( const std::string& sPathPart ) {
  wxTreeItemIdValue tiv;
  wxTreeItemId ti = m_pHdf5Root->GetFirstChild( m_curTreeItem, tiv );
  bool bItemFound( false );
  while ( ti.IsOk() ) {
    if ( sPathPart == m_pHdf5Root->GetItemText( ti ) ) {
      m_curTreeItem = ti;
      bItemFound = true;
      break;
    }
    else {
      ti = m_pHdf5Root->GetNextChild( m_curTreeItem, tiv );
    }
  }
  if ( !bItemFound ) {
    m_curTreeItem = m_pHdf5Root->AppendItem( m_curTreeItem, sPathPart, -1, -1, new CustomItemData( CustomItemData::Group, CustomItemData::NoDatum ) );
  }

}

void AppHdf5Chart::HandleTreeEventItemActivated( wxTreeEvent& event ) {

  wxTreeItemId id = event.GetItem();

  wxTreeItemId id2 = id;
  std::string sPath = m_pHdf5Root->GetItemText( id2 ); // start here and prefix the path
  while ( true ) {
    id2 = m_pHdf5Root->GetItemParent( id2 );
    if ( !id2.IsOk() ) break;
    sPath = "/" + sPath;
    std::string sTmpElement( m_pHdf5Root->GetItemText( id2 ) );
    if ( "/" != sTmpElement ) {
      sPath = sTmpElement + sPath;
    }
  }

  switch ( dynamic_cast<CustomItemData*>( m_pHdf5Root->GetItemData( id ) )->m_eNodeType ) {
  case CustomItemData::Root:
    break;
  case CustomItemData::Group:
    sPath += "/";
    break;
  case CustomItemData::Object:
    // load and view time series here
    break;
  }

  std::cout << sPath << std::endl;
  
}

void AppHdf5Chart::HandleMenuActionStartChart( void ) {
  m_bReadyToDrawChart = true;
  m_pChart = new ChartTest( m_pData1Provider );

  m_winChart->RefreshRect( m_winChart->GetClientRect(), false );
}

void AppHdf5Chart::HandlePaint( wxPaintEvent& event ) {
  if ( m_bReadyToDrawChart && !m_bPaintingChart ) {
    try {
      m_bPaintingChart = true;
      wxSize size = m_winChart->GetClientSize();
      m_chartMaster.SetChartDimensions( size.GetWidth(), size.GetHeight() );
      m_chartMaster.SetChartDataView( &m_pChart->GetChartDataView() );
      m_chartMaster.SetOnDrawChart( MakeDelegate( this, &AppLiveChart::HandleDrawChart ) );
      m_chartMaster.DrawChart( );
    }
    catch (...) {
    }
  }
  m_bPaintingChart = false;
}

void AppHdf5Chart::HandleSize( wxSizeEvent& event ) { 
  m_winChart->RefreshRect( m_winChart->GetClientRect(), false );
}

// http://www.chartdir.com/forum/download_thread.php?bn=chartdir_support&thread=1144757575#N1144760096
void AppLiveChart::HandleDrawChart( const MemBlock& m ) {
  wxMemoryInputStream in( m.data, m.len );
  wxBitmap bmp( wxImage( in, wxBITMAP_TYPE_BMP) );
  wxPaintDC cdc( m_winChart );
  cdc.DrawBitmap(bmp, 0, 0);
}

void AppHdf5Chart::HandleGuiRefresh( wxTimerEvent& event ) {
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

void AppHdf5Chart::HandleMenuActionStartWatch( void ) {
/*
  m_pBundle->StartWatch();

  m_bIVCalcActive = false;
  ptime dt;
  ou::TimeSource::Instance().Internal( &dt );
  m_dtTopOfMinute = dt + time_duration( 0, 1, 0 ) - time_duration( 0, 0, dt.time_of_day().seconds() );
  */
  m_timerGuiRefresh.Start( 250 );
}

void AppHdf5Chart::HandleMenuActionStopWatch( void ) {

  m_timerGuiRefresh.Stop();

//  m_pBundle->StopWatch();

}

void AppLiveChart::HandleMenuActionSaveValues( void ) {
  m_worker.Run( MakeDelegate( this, &AppLiveChart::HandleSaveValues ) );
}

void AppHdf5Chart::HandleSaveValues( void ) {
  std::cout << "Saving collected values ... " << std::endl;
  try {
    //std::string sPrefixSession( "/app/LiveChart/" + m_sTSDataStreamStarted + "/" + m_pBundle->Name() );
    //std::string sPrefix86400sec( "/bar/86400/AtmIV/" + iter->second.sName.substr( 0, 1 ) + "/" + iter->second.sName );
    //std::string sPrefix86400sec( "/bar/86400/AtmIV/" + m_pBundle->Name() );
    //m_pBundle->SaveData( sPrefixSession, sPrefix86400sec );
    std::string sPrefixSession( "/app/LiveChart/" + m_sTSDataStreamStarted + "/" 
      + m_pChart->GetWatch()->GetInstrument()->GetInstrumentName() );
    m_pChart->GetWatch()->SaveSeries( sPrefixSession );
  }
  catch(...) {
    std::cout << " ... issues with saving ... " << std::endl;
  }
  std::cout << "  ... Done " << std::endl;
}

void AppHdf5Chart::HandleMenuActionInitializeSymbolSet( void ) {
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

void AppHdf5Chart::HandleMenuAction0ObtainNewIQFeedSymbolListRemote( void ) {
  // need to lock out from running HandleLoadIQFeedSymbolList at the same time
  m_worker.Run( MakeDelegate( this, &AppLiveChart::HandleObtainNewIQFeedSymbolListRemote ) );
}

void AppHdf5Chart::HandleObtainNewIQFeedSymbolListRemote( void ) {
  std::cout << "Downloading Text File ... " << std::endl;
  ou::tf::iqfeed::LoadMktSymbols( m_listIQFeedSymbols, ou::tf::iqfeed::MktSymbolLoadType::Download, true ); 
  std::cout << "Saving Binary File ... " << std::endl;
  m_listIQFeedSymbols.SaveToFile( "symbols.ser" );
  std::cout << " ... done." << std::endl;
}

void AppHdf5Chart::HandleMenuAction1ObtainNewIQFeedSymbolListLocal( void ) {
  // need to lock out from running HandleLoadIQFeedSymbolList at the same time
  m_worker.Run( MakeDelegate( this, &AppLiveChart::HandleObtainNewIQFeedSymbolListLocal ) );
}

void AppHdf5Chart::HandleObtainNewIQFeedSymbolListLocal( void ) {
  std::cout << "Loading From Text File ... " << std::endl;
  ou::tf::iqfeed::LoadMktSymbols( m_listIQFeedSymbols, ou::tf::iqfeed::MktSymbolLoadType::LoadTextFromDisk, false ); 
  std::cout << "Saving Binary File ... " << std::endl;
  m_listIQFeedSymbols.SaveToFile( "symbols.ser" );
  std::cout << " ... done." << std::endl;
}

void AppHdf5Chart::HandleMenuAction2LoadIQFeedSymbolList( void ) {
  // need to lock out from running HandleObtainNewIQFeedSymbolList at the same time
  m_worker.Run( MakeDelegate( this, &AppLiveChart::HandleLoadIQFeedSymbolList ) );
}

void AppHdf5Chart::HandleLoadIQFeedSymbolList( void ) {
  std::cout << "Loading From Binary File ..." << std::endl;
  m_listIQFeedSymbols.LoadFromFile( "phisymbols.ser" );
  std::cout << " ... completed." << std::endl;
}


// runs in thread
void AppHdf5Chart::CalcIV( ptime dt ) {
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

void AppHdf5Chart::HandleMenuActionEmitYieldCurve( void ) {
  //ou::tf::libor::EmitYieldCurve();
  //m_libor.EmitYieldCurve();
  std::cout << m_libor;
}

int AppHdf5Chart::OnExit() {
  // Exit Steps: #4
//  DelinkFromPanelProviderControl();  generates stack errors
  //m_timerGuiRefresh.Stop();
  this->m_pData1Provider->Disconnect();
  m_listIQFeedSymbols.Clear();
  if ( m_db.IsOpen() ) m_db.Close();

  delete m_pdm;
  m_pdm = 0;

  return wxAppConsole::OnExit();
}

//void AppWeeklies::HandlePanelFocusPropogate( unsigned int ix ) {
//}


void AppHdf5Chart::OnClose( wxCloseEvent& event ) {
  // Exit Steps: #2 -> FrameMain::OnClose
  m_timerGuiRefresh.Stop();
  DelinkFromPanelProviderControl();
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a 
  event.Skip();  // auto followed by Destroy();
}

void AppHdf5Chart::OnData1Connected( int ) {
  m_bData1Connected = true;
  //ou::tf::libor::SetWatchOn( m_pData1Provider );
//  m_libor.SetWatchOn( m_pData1Provider );
//  AutoStartCollection();
  if ( m_bData1Connected & m_bExecConnected ) {
    // set start to enabled
  }
}

void AppHdf5Chart::OnData2Connected( int ) {
  m_bData2Connected = true;
//  AutoStartCollection();
  if ( m_bData2Connected & m_bExecConnected ) {
    // set start to enabled
  }
}

void AppHdf5Chart::OnExecConnected( int ) {
  m_bExecConnected = true;
  if ( m_bData1Connected & m_bExecConnected ) {
    // set start to enabled
  }
}

void AppHdf5Chart::OnData1Disconnected( int ) {
//  m_libor.SetWatchOff();
  m_bData1Connected = false;
}

void AppHdf5Chart::OnData2Disconnected( int ) {
  m_bData2Connected = false;
}

void AppHdf5Chart::OnExecDisconnected( int ) {
  m_bExecConnected = false;
}

void AppHdf5Chart::HandleRegisterTables(  ou::db::Session& session ) {
}

void AppHdf5Chart::HandleRegisterRows(  ou::db::Session& session ) {
}

void AppHdf5Chart::HandlePopulateDatabase( void ) {
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

