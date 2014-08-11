/************************************************************************
 * Copyright(c) 2014, One Unified. All rights reserved.                 *
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

#include <boost/cstdint.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;

#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/lexical_cast.hpp>

#include <wx/panel.h>

#include <TFBitsNPieces/ReadSicCodeList.h>

#include "IQFeedMarketSymbols.h"

IMPLEMENT_APP(AppIQFeedMarketSymbols)

bool AppIQFeedMarketSymbols::OnInit() {

  m_pFrameMain = new FrameMain( 0, wxID_ANY, "IQFeed Market Symbols" );
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

  // m_pPanelLogging
  m_pPanelLogging = new ou::tf::PanelLogging( m_pFrameMain, wxID_ANY );
  //m_sizerSplitterRight->Add( m_pPanelLogging, 1, wxALL | wxEXPAND|wxALIGN_LEFT|wxALIGN_RIGHT|wxALIGN_TOP|wxALIGN_BOTTOM, 1);
  m_sizerFrame->Add( m_pPanelLogging, 1, wxALL | wxEXPAND|wxALIGN_LEFT|wxALIGN_RIGHT|wxALIGN_TOP|wxALIGN_BOTTOM, 1);

  //LinkToPanelProviderControl();

  m_pFrameMain->Show( true );

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppIQFeedMarketSymbols::OnClose, this );  // start close of windows and controls

  FrameMain::vpItems_t vItems;
  typedef FrameMain::structMenuItem mi;  // vxWidgets takes ownership of the objects
  vItems.push_back( new mi( "a1 New Symbol List Remote", MakeDelegate( this, &AppIQFeedMarketSymbols::HandleMenuAction0ObtainNewIQFeedSymbolListRemote ) ) );
  vItems.push_back( new mi( "a2 New Symbol List Local", MakeDelegate( this, &AppIQFeedMarketSymbols::HandleMenuAction1ObtainNewIQFeedSymbolListLocal ) ) );
  vItems.push_back( new mi( "a3 Load Symbol List", MakeDelegate( this, &AppIQFeedMarketSymbols::HandleMenuAction2LoadIQFeedSymbolList ) ) );
  vItems.push_back( new mi( "b1 Scan Symbols", MakeDelegate( this, &AppIQFeedMarketSymbols::HandleMenuActionScanSymbolList ) ) );
  vItems.push_back( new mi( "c1 Load SIC Codes", MakeDelegate( this, &AppIQFeedMarketSymbols::HandleMenuActionLoadSICCodes ) ) );
  m_pFrameMain->AddDynamicMenu( "Actions", vItems );

  return 1;

}

void AppIQFeedMarketSymbols::HandleMenuActionLoadSICCodes( void ) {
  ou::SicCodeList( "..\\SIC Codes List.xls" );
}

void AppIQFeedMarketSymbols::HandleMenuActionScanSymbolList( void ) {

  ou::SicCodeList sic( "..\\SIC Codes List.xls" );

  struct structFillMaps {

    mapCounts_t& mapSIC;
    boost::uint32_t nSIC;

    mapCounts_t& mapNAICS;
    boost::uint32_t nNAICS;

    structFillMaps( mapCounts_t& map1, mapCounts_t& map2 ): mapSIC( map1 ), mapNAICS( map2 ), nSIC( 0 ), nNAICS( 0 ) {};
    ~structFillMaps( void ) {
      std::cout << "nSIC=" << nSIC << ",nNAICS=" << nNAICS << std::endl;
    }

    void operator()( const trd_t& trd ) {

      if ( mapSIC.end() == mapSIC.find( trd.nSIC ) ) {
        mapSIC[ trd.nSIC ] = 1;
      }
      else {
        mapSIC[ trd.nSIC ]++;
      }
      if ( 0 != trd.nSIC ) nSIC++;

      if ( mapNAICS.end() == mapNAICS.find( trd.nNAICS ) ) {
        mapNAICS[ trd.nNAICS ] = 1;
      }
      else {
        mapNAICS[ trd.nNAICS ]++;
      }
      if ( 0 != trd.nNAICS ) nNAICS++;
    }
  };

  std::cout << "Starting scan ... " << std::endl;

  m_listIQFeedSymbols.ScanSymbols( structFillMaps( m_mapSIC, m_mapNAICS ) );

  std::cout << "SIC (" << m_mapSIC.size() << "):" << std::endl;
  for ( citerMapCounts_t iter = m_mapSIC.begin(); iter != m_mapSIC.end(); iter++ ) {
    std::cout << iter->second << " " << iter->first << " " << sic.LookupCode( iter->first ) << std::endl;
  }

  std::cout << "================" << std::endl;

  std::cout << "NAICS (" << m_mapNAICS.size() << "):" << std::endl;
  for ( citerMapCounts_t iter = m_mapNAICS.begin(); iter != m_mapNAICS.end(); iter++ ) {
    std::cout << iter->second << " " << iter->first << std::endl;
  }

  std::cout << "Scan done." << std::endl;

}

void AppIQFeedMarketSymbols::HandleMenuAction0ObtainNewIQFeedSymbolListRemote( void ) {
  // need to lock out from running HandleLoadIQFeedSymbolList at the same time
  m_worker.Run( MakeDelegate( this, &AppIQFeedMarketSymbols::HandleObtainNewIQFeedSymbolListRemote ) );
}

void AppIQFeedMarketSymbols::HandleObtainNewIQFeedSymbolListRemote( void ) {
  std::cout << "Downloading Text File ... " << std::endl;
  ou::tf::iqfeed::LoadMktSymbols( m_listIQFeedSymbols, ou::tf::iqfeed::MktSymbolLoadType::Download, true ); 
  std::cout << "Saving Binary File ... " << std::endl;
  m_listIQFeedSymbols.SaveToFile( "symbols.ser" );
  std::cout << " ... done." << std::endl;
}

void AppIQFeedMarketSymbols::HandleMenuAction1ObtainNewIQFeedSymbolListLocal( void ) {
  // need to lock out from running HandleLoadIQFeedSymbolList at the same time
  m_worker.Run( MakeDelegate( this, &AppIQFeedMarketSymbols::HandleObtainNewIQFeedSymbolListLocal ) );
}

void AppIQFeedMarketSymbols::HandleObtainNewIQFeedSymbolListLocal( void ) {
  std::cout << "Loading From Text File ... " << std::endl;
  ou::tf::iqfeed::LoadMktSymbols( m_listIQFeedSymbols, ou::tf::iqfeed::MktSymbolLoadType::LoadTextFromDisk, false ); 
  std::cout << "Saving Binary File ... " << std::endl;
  m_listIQFeedSymbols.SaveToFile( "symbols.ser" );
  std::cout << " ... done." << std::endl;
}

void AppIQFeedMarketSymbols::HandleMenuAction2LoadIQFeedSymbolList( void ) {
  // need to lock out from running HandleObtainNewIQFeedSymbolList at the same time
  m_worker.Run( MakeDelegate( this, &AppIQFeedMarketSymbols::HandleLoadIQFeedSymbolList ) );
}

void AppIQFeedMarketSymbols::HandleLoadIQFeedSymbolList( void ) {
  std::cout << "Loading From Binary File ..." << std::endl;
  m_listIQFeedSymbols.LoadFromFile( "symbols.ser" );
  std::cout << " ... completed." << std::endl;
}


int AppIQFeedMarketSymbols::OnExit() {
  // Exit Steps: #4
//  DelinkFromPanelProviderControl();  generates stack errors
  //m_timerGuiRefresh.Stop();


  m_listIQFeedSymbols.Clear();

  return wxAppConsole::OnExit();
}

void AppIQFeedMarketSymbols::OnClose( wxCloseEvent& event ) {
  // Exit Steps: #2 -> FrameMain::OnClose
  DelinkFromPanelProviderControl();
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a 
  event.Skip();  // auto followed by Destroy();
}



