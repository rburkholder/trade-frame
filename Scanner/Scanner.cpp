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

//#include <boost/phoenix/bind/bind_member_function.hpp>

#include <TFTimeSeries/TimeSeries.h>
#include <TFBitsNPieces/InstrumentFilter.h>

#include "Scanner.h"

IMPLEMENT_APP(AppScanner)

bool AppScanner::OnInit() {

  m_pFrameMain = new FrameMain( 0, wxID_ANY, "Phi v2" );
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

  wxBoxSizer* m_sizerStatus = new wxBoxSizer( wxHORIZONTAL );
  m_sizerMain->Add( m_sizerStatus, 1, wxEXPAND|wxALL, 5 );

  m_pPanelLogging = new ou::tf::PanelLogging( m_pFrameMain, wxID_ANY );
  m_sizerStatus->Add( m_pPanelLogging, 1, wxALL | wxEXPAND|wxALIGN_LEFT|wxALIGN_RIGHT|wxALIGN_TOP|wxALIGN_BOTTOM, 0);
  m_pPanelLogging->Show( true );

  m_pFrameMain->Show( true );

//  m_db.OnRegisterTables.Add( MakeDelegate( this, &AppPhi::HandleRegisterTables ) );
//  m_db.OnRegisterRows.Add( MakeDelegate( this, &AppPhi::HandleRegisterRows ) );
//  m_db.SetOnPopulateDatabaseHandler( MakeDelegate( this, &AppPhi::HandlePopulateDatabase ) );

//  m_bData1Connected = false;
//  m_bData2Connected = false;
//  m_bExecConnected = false;

//  m_timerGuiRefresh.SetOwner( this );

//  Bind( wxEVT_TIMER, &AppPhi::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppScanner::OnClose, this );  // start close of windows and controls

  std::string sDbName( "scanner.db" );
  if ( boost::filesystem::exists( sDbName ) ) {
    boost::filesystem::remove( sDbName );
  }
//  m_db.Open( sDbName );


  FrameMain::vpItems_t vItems;
  typedef FrameMain::structMenuItem mi;  // vxWidgets takes ownership of the objects
  vItems.push_back( new mi( "Scan", MakeDelegate( this, &AppScanner::HandleMenuActionScan ) ) );
  m_pFrameMain->AddDynamicMenu( "Actions", vItems );

  ptime dt;
  ou::TimeSource::Instance().External( &dt );

  std::cout << "UTC: " << dt << " Local: " << ou::TimeSource::Instance().Local() << std::endl;

  return 1;

}

struct AverageVolume {
private:
  ou::tf::Bar::volume_t m_nTotalVolume;
  unsigned long m_nNumberOfValues;
protected:
public:
  AverageVolume() : m_nTotalVolume( 0 ), m_nNumberOfValues( 0 ) {};
  void operator() ( const ou::tf::Bar& bar ) {
    m_nTotalVolume += bar.Volume();
    ++m_nNumberOfValues;
  }
  operator ou::tf::Bar::volume_t() { return m_nTotalVolume / m_nNumberOfValues; };
};

bool AppScanner::HandleCallBackUseGroup( s_t&, const std::string& sPath, const std::string& sGroup ) {
  return true;
}

bool AppScanner::HandleCallBackFilter( s_t& data, const std::string& sObject, ou::tf::Bars& bars ) {
  bool b( false );
  ++data.nEnteredFilter;
  ou::tf::Bars::const_iterator iterVolume = bars.begin();
  data.nAverageVolume = std::for_each( iterVolume, bars.end(), AverageVolume() );
  if ( ( 1000000 < data.nAverageVolume ) 
    && ( 12.0 <= bars.Last()->Close() )
    && ( 80.0 >= bars.Last()->Close() ) ) {
//      Info info( sObjectName, *bars.Last() );
//      m_mapInfoRankedByVolume.insert( pairInfoRankedByVolume_t( volAverage, info ) );
      //std::cout << sObject << " vol=" << volAverage << std::endl;
      ++data.nPassedFilter;
      b = true;
  }
  return b;
}


void AppScanner::HandleCallBackResults( s_t& data, const std::string& sObject, ou::tf::Bars& bars ) {
  std::cout << sObject << ": " << data.nAverageVolume << "," << data.nEnteredFilter << "," << data.nPassedFilter << std::endl;
}

void AppScanner::ScanBars( void ) {
  namespace args = boost::phoenix::placeholders;
  ou::tf::InstrumentFilter<s_t,ou::tf::Bars> filter( 
    "/bar/86400", 
    ptime( date( 2013, 9, 4 ), time_duration( 0, 0, 0 ) ), 
    ptime( date( 2013, 9, 19 ), time_duration( 0, 0, 0 ) ), 
    10,
    boost::phoenix::bind( &AppScanner::HandleCallBackUseGroup, this, args::arg1, args::arg2, args::arg3 ),
    boost::phoenix::bind( &AppScanner::HandleCallBackFilter, this, args::arg1, args::arg2, args::arg3 ),
    boost::phoenix::bind( &AppScanner::HandleCallBackResults, this, args::arg1, args::arg2, args::arg3 )
    );
  try {
    filter.Run();
  }
  catch( ... ) {
    std::cout << "Scan Problems" << std::endl;
  }
}

void AppScanner::HandleMenuActionScan( void ) {
  m_worker.Run( MakeDelegate( this, &AppScanner::ScanBars ) );
}

//void AppScanner::HandleHdf5Object( const std::string& sPath, const std::string& sObject ) {
//  std::cout << sObject << std::endl;
//}

int AppScanner::OnExit() {
  // Exit Steps: #4
//  DelinkFromPanelProviderControl();  generates stack errors
  //m_timerGuiRefresh.Stop();
//  m_listIQFeedSymbols.Clear();
//  if ( m_db.IsOpen() ) m_db.Close();

  return wxAppConsole::OnExit();
}

void AppScanner::OnClose( wxCloseEvent& event ) {
  // Exit Steps: #2 -> FrameMain::OnClose
//  m_timerGuiRefresh.Stop();
  DelinkFromPanelProviderControl();
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a 
  event.Skip();  // auto followed by Destroy();
}

void AppScanner::OnData1Connected( int ) {
  m_bData1Connected = true;
  //ou::tf::libor::SetWatchOn( m_pData1Provider );
//  m_libor.SetWatchOn( m_pData1Provider );
//  AutoStartCollection();
  if ( m_bData1Connected & m_bExecConnected ) {
    // set start to enabled
  }
}

void AppScanner::OnData2Connected( int ) {
  m_bData2Connected = true;
//  AutoStartCollection();
  if ( m_bData2Connected & m_bExecConnected ) {
    // set start to enabled
  }
}

void AppScanner::OnExecConnected( int ) {
  m_bExecConnected = true;
  if ( m_bData1Connected & m_bExecConnected ) {
    // set start to enabled
  }
}

void AppScanner::OnData1Disconnected( int ) {
  m_bData1Connected = false;
}

void AppScanner::OnData2Disconnected( int ) {
  m_bData2Connected = false;
}

void AppScanner::OnExecDisconnected( int ) {
  m_bExecConnected = false;
}

