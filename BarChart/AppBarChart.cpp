/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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

/*
 * File:    AppBarChart.cpp
 * Author:  raymond@burkholder.net
 * Project: BarChart
 * Created: April 8, 2025 21:29:02
 */

/*
  show daily bars for a select number instruments
  show live data for a select number of instruments

  * load up tree from config file
  * use indicator trading for example of loading daily bars
*/

#include <boost/log/trivial.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <wx/sizer.h>

#include <TFHDF5TimeSeries/HDF5Attribute.h>
#include <TFHDF5TimeSeries/HDF5IterateGroups.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelFinancialChart.hpp>

#include "AppBarChart.hpp"

namespace {
  static const std::string c_sAppTitle(        "Daily Bars Review" );
  static const std::string c_sAppNamePrefix(   "barchart" );
  static const std::string c_sChoicesFilename( c_sAppNamePrefix + ".cfg" );
  static const std::string c_sDbName(          c_sAppNamePrefix + ".db" );
  static const std::string c_sStateFileName(   c_sAppNamePrefix + ".state" );
  //static const std::string c_sTimeZoneSpec( "../date_time_zonespec.csv" );
  static const std::string c_sVendorName( "One Unified Net Limited" );
}

IMPLEMENT_APP(AppBarChart)

bool AppBarChart::OnInit() {

  wxApp::SetAppDisplayName( c_sAppTitle );
  wxApp::SetVendorName( c_sVendorName );
  wxApp::SetVendorDisplayName( "(c)2025 " + c_sVendorName );

  if ( !wxApp::OnInit() ) {
    return false;
  }

  if ( config::Load( c_sChoicesFilename, m_choices ) ) {
  }
  else {
    return false;
  }

  m_pFrameMain = new FrameMain( 0, wxID_ANY, c_sAppTitle );
  wxWindowID idFrameMain = m_pFrameMain->GetId();

  m_pFrameMain->SetSize( 800, 500 );

  SetTopWindow( m_pFrameMain );

  wxBoxSizer* sizerFrame;
  wxBoxSizer* sizerUpper;
  wxBoxSizer* sizerLower;

  sizerFrame = new wxBoxSizer( wxVERTICAL );
  m_pFrameMain->SetSizer( sizerFrame );

  m_pPanelFinancialChart = new ou::tf::PanelFinancialChart( m_pFrameMain );
  sizerFrame->Add( m_pPanelFinancialChart, 1, wxALL | wxEXPAND, 0 );

  //m_pwcv = new ou::tf::WinChartView( m_pFrameMain );
  //sizerFrame->Add( m_pwcv, 1,wxALL | wxEXPAND, 0 );

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppBarChart::OnClose, this );  // start close of windows and controls
  m_pFrameMain->Bind( wxEVT_MOVE, &AppBarChart::OnFrameMainAutoMove, this ); // intercept first move
  m_pFrameMain->Show( true ); // triggers the auto move

  return true;

}

void AppBarChart::OnFrameMainAutoMove( wxMoveEvent& event ) {
  // load state works properly _after_ first move (library initiated)

  CallAfter(
    [this](){
      LoadState();
      m_pFrameMain->Layout();
      LoadPanelFinancialChart();
    }
  );

  m_pFrameMain->Unbind( wxEVT_MOVE, &AppBarChart::OnFrameMainAutoMove, this );

  event.Skip(); // set to false if we want to ignore auto move

}

void AppBarChart::LoadPanelFinancialChart() {

  m_ptiRoot = m_pPanelFinancialChart->SetRoot( "/", nullptr );

  for ( const std::string& sSecurityName: m_choices.m_vSecurityName ) {
    mapSymbolInfo_t::iterator iterSymbolInfo = m_mapSymbolInfo.find( sSecurityName );
    if ( m_mapSymbolInfo.end() != iterSymbolInfo ) {
      BOOST_LOG_TRIVIAL(error) << "Ignoring duplicate security: " << sSecurityName;
    }
    else {
      auto result = m_mapSymbolInfo.emplace( sSecurityName, SymbolInfo() );
      assert( result.second );
      iterSymbolInfo = result.first;
      SymbolInfo& si( iterSymbolInfo->second );

      si.m_pti = m_ptiRoot->AppendChild( sSecurityName );
      si.m_cePriceBars.SetName( "Daily" );

      si.m_dvChart.Add( EChartSlot::Price, &si.m_cePriceBars );
      si.m_dvChart.Add( EChartSlot::Volume, &si.m_ceVolume );

      //bars.ForEach( [this]( const ou::tf::Bar& bar ){
      //  m_cePriceBars.AppendBar( bar );
      //  m_ceVolume.Append( bar );
      //} );

    }







  }

}

void AppBarChart::SaveState() {
  std::cout << "Saving Config ..." << std::endl;
  std::ofstream ofs( c_sStateFileName );
  boost::archive::text_oarchive oa(ofs);
  oa & *this;
  std::cout << "  done." << std::endl;
}

void AppBarChart::LoadState() {
  try {
    std::cout << "Loading Config ..." << std::endl;
    std::ifstream ifs( c_sStateFileName );
    boost::archive::text_iarchive ia(ifs);
    ia & *this;
    std::cout << "  done." << std::endl;
  }
  catch(...) {
    std::cout << "load exception" << std::endl;
  }
}

int AppBarChart::OnExit() {
  // Exit Steps: #4
//  DelinkFromPanelProviderControl();  generates stack errors

  return wxAppConsole::OnExit();
}

void AppBarChart::OnClose( wxCloseEvent& event ) {
  // Exit Steps: #2 -> FrameMain::OnClose

  //m_pwcv->SetChartDataView( nullptr, false );

  //DelinkFromPanelProviderControl();
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a
  SaveState();
  event.Skip();  // auto followed by Destroy();
}

