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

// dates around line 30 need to be adjusted for date of last daily bar

#include "stdafx.h"

#include <TFTimeSeries/TimeSeries.h>
#include <TFBitsNPieces/InstrumentFilter.h>

#include <TFIndicators/Pivots.h>

#include "Scanner.h"

IMPLEMENT_APP(AppScanner)

bool AppScanner::OnInit() {

  m_dtEnd   = ptime( date( 2019, 3, 22 ), time_duration( 23, 59, 59 ) );
  m_dtBegin  = m_dtEnd - days( 50 );

  m_pFrameMain = new FrameMain( 0, wxID_ANY, "Scanner" );
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

  wxBoxSizer* m_sizerStatus = new wxBoxSizer( wxHORIZONTAL );
  m_sizerMain->Add( m_sizerStatus, 1, wxEXPAND|wxALL, 5 );

  m_pPanelLogging = new ou::tf::PanelLogging( m_pFrameMain, wxID_ANY );
  m_sizerStatus->Add( m_pPanelLogging, 1, wxALL | wxEXPAND|wxALIGN_LEFT|wxALIGN_RIGHT|wxALIGN_TOP|wxALIGN_BOTTOM, 0);
  m_pPanelLogging->Show( true );

  m_pFrameMain->Show( true );

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppScanner::OnClose, this );  // start close of windows and controls

  std::string sDbName( "scanner.db" );
  if ( boost::filesystem::exists( sDbName ) ) {
    boost::filesystem::remove( sDbName );
  }

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
  data.nAverageVolume = std::for_each( bars.begin(), bars.end(), AverageVolume() );
//  std::cout << sObject << ": " << bars.Last()->DateTime() << " - " << m_dtEnd << std::endl;
  if ( ( 1000000 < data.nAverageVolume )
    && ( 12.0 <= bars.Last()->Close() )
    && ( 90.0 >= bars.Last()->Close() )
    && ( m_nMinBarCount <= bars.Size() )
    && ( m_dtEnd.date() == bars.Last()->DateTime().date() )
    ) {
//      Info info( sObjectName, *bars.Last() );
//      m_mapInfoRankedByVolume.insert( pairInfoRankedByVolume_t( volAverage, info ) );
      //std::cout << sObject << " vol=" << volAverage << std::endl;
      ou::tf::Bars::const_iterator iter1, iter2;
      iter2 = bars.end();
      iter1 = iter2 - m_nMinBarCount;
      iter2 = iter1;
      ++iter2;
      data.nPVCrossings = 0;
      data.nUpAndR1Crossings = 0;
      data.nDnAndS1Crossings = 0;
      data.nPVAndR1Crossings = 0;
      data.nPVAndS1Crossings = 0;
      while ( bars.end() != iter2 ) {
        ou::tf::PivotSet pivot( "pv", *iter1 );
        double pv = pivot.GetPivotValue( ou::tf::PivotSet::PV );
        if ( ( pv <= iter2->High() ) && ( pv >= iter2->Low() ) ) {
          ++(data.nPVCrossings);
        }

        if ( iter2->Open() < pv ) {
          double r1 = pivot.GetPivotValue( ou::tf::PivotSet::R1 );
          if ( ( r1 <= iter2->High() ) && ( r1 >= iter2->Low() ) ) ++(data.nPVAndR1Crossings);
        }
        if ( iter2->Open() > pv ) {
          double s1 = pivot.GetPivotValue( ou::tf::PivotSet::S1 );
          if ( ( s1 <= iter2->High() ) && ( s1 >= iter2->Low() ) ) ++(data.nPVAndS1Crossings);
        }

        if ( iter2->Open() > pv ) {
          double r1 = pivot.GetPivotValue( ou::tf::PivotSet::R1 );
          if ( ( r1 <= iter2->High() ) && ( r1 >= iter2->Low() ) ) ++(data.nUpAndR1Crossings);
        }
        if ( iter2->Open() < pv ) {
          double s1 = pivot.GetPivotValue( ou::tf::PivotSet::S1 );
          if ( ( s1 <= iter2->High() ) && ( s1 >= iter2->Low() ) ) ++(data.nDnAndS1Crossings);
        }

        ++iter1;
        ++iter2;
      }
      ++data.nPassedFilter;
      b = true;
  }
  return b;
}


void AppScanner::HandleCallBackResults( s_t& data, const std::string& sObject, ou::tf::Bars& bars ) {
  std::cout
    << sObject << ","
    << data.nAverageVolume << ","
    << data.nEnteredFilter << ","
    << data.nPassedFilter << ","
    << data.nUpAndR1Crossings << ","
    << data.nPVAndR1Crossings << ","
    << data.nPVCrossings << ","
    << data.nPVAndS1Crossings << ","
    << data.nDnAndS1Crossings << ","
    << data.nUpAndR1Crossings + data.nDnAndS1Crossings << ","
    << data.nPVAndR1Crossings + data.nPVAndS1Crossings << ","
    << data.nUpAndR1Crossings + data.nDnAndS1Crossings +
       data.nPVAndR1Crossings + data.nPVAndS1Crossings
    << std::endl;
}

void AppScanner::ScanBars( void ) {
  namespace ph = std::placeholders;
  m_nMinBarCount = 20;  // tie this approx to the date range below
  ou::tf::InstrumentFilter<s_t,ou::tf::Bars> filter(
    "/bar/86400",
    m_dtBegin,
    m_dtEnd,
    20,
    std::bind( &AppScanner::HandleCallBackUseGroup, this, ph::_1, ph::_2, ph::_3 ),
    std::bind( &AppScanner::HandleCallBackFilter,   this, ph::_1, ph::_2, ph::_3 ),
    std::bind( &AppScanner::HandleCallBackResults,  this, ph::_1, ph::_2, ph::_3 )
    );
  try {
    filter.Run();
  }
  catch( ... ) {
    std::cout << "Scan Problems" << std::endl;
  }
  std::cout << "Scan Complete" << std::endl;
}

void AppScanner::HandleMenuActionScan( void ) {
  CallAfter( [this](){
    if ( m_worker.joinable() ) m_worker.join(); // need to finish off any previous thread
    m_worker = std::thread( &AppScanner::ScanBars, this );
  });
}

int AppScanner::OnExit() {
  // Exit Steps: #4

  return wxAppConsole::OnExit();
}

void AppScanner::OnClose( wxCloseEvent& event ) {
  if ( m_worker.joinable() )
    m_worker.join();
  // Exit Steps: #2 -> FrameMain::OnClose
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a
  event.Skip();  // auto followed by Destroy();
}

void AppScanner::OnData1Connected( int ) {
  m_bData1Connected = true;
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

