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
 * File:    AppSP500.cpp
 * Author:  raymond@burkholder.net
 * Project: SP500
 * Created: March 30, 2025 13:49:15
 */

// inspiration from AutoTrade

/*
  * start by viewing SPY or SPX as primary chart from HDF5 file - done
  * add in the SP Tick/Trin/Advance/Decline/ratio indicators - done
  * add in some indicators, maybe try the garch studies?
  * run simulator for validation
  * add in the ml ability?
  * run live simulation - iqfeed
  * run live - ib
*/

#include <boost/log/trivial.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <wx/sizer.h>

#include <TFVuTrading/FrameMain.h>

#include "AppSP500.hpp"

namespace {
  static const std::string c_sVendorName( "One Unified Net Limited" );
  static const std::string c_sAppTitle(        "SP500 Trading" );
  static const std::string c_sAppNamePrefix(   "sp500" );
  static const std::string c_sChoicesFilename( c_sAppNamePrefix + ".cfg" );
  static const std::string c_sDbName(          c_sAppNamePrefix + ".db" );
  static const std::string c_sStateFileName(   c_sAppNamePrefix + ".state" );
  //static const std::string c_sTimeZoneSpec( "../date_time_zonespec.csv" );
}

IMPLEMENT_APP(AppSP500)

bool AppSP500::OnInit() {

  wxApp::SetAppDisplayName( c_sAppTitle );
  wxApp::SetVendorName( c_sVendorName );
  wxApp::SetVendorDisplayName( "(C)2025 " + c_sVendorName );

  if ( !wxApp::OnInit() ) {
    return false;
  }

  if ( config::Load( c_sChoicesFilename, m_choices ) ) {
    if ( boost::filesystem::exists( m_choices.m_sFileTraining ) ) {}
    else {
      BOOST_LOG_TRIVIAL(error) << "training file " << m_choices.m_sFileTraining << " does not exist";
      return false;
    }
    if ( boost::filesystem::exists( m_choices.m_sFileValidate ) ) {}
    else {
      BOOST_LOG_TRIVIAL(error) << "validation file " << m_choices.m_sFileValidate << " does not exist";
      return false;
    }
  }
  else {
    return false;
  }

  if ( m_choices.m_bRunSim ) {
    if ( boost::filesystem::exists( c_sDbName ) ) {
      boost::filesystem::remove( c_sDbName );
    }
  }
  else {
    BOOST_LOG_TRIVIAL(error) << "simulation mode only available";
    return false;
  }

  m_pdb = std::make_unique<ou::tf::db>( c_sDbName );

  m_pFrameMain = new FrameMain( 0, wxID_ANY, c_sAppTitle );
  wxWindowID idFrameMain = m_pFrameMain->GetId();

  m_pFrameMain->SetSize( 800, 500 );

  SetTopWindow( m_pFrameMain );

  wxBoxSizer* sizerFrame;
  wxBoxSizer* sizerUpper;
  wxBoxSizer* sizerLower;

  sizerFrame = new wxBoxSizer( wxVERTICAL );
  m_pFrameMain->SetSizer( sizerFrame );

  //m_pPanelFinancialChart = new ou::tf::PanelFinancialChart( m_pFrameMain );
  //sizerFrame->Add( m_pPanelFinancialChart, 1, wxALL | wxEXPAND, 0 );

  m_pwcv = new ou::tf::WinChartView( m_pFrameMain );
  sizerFrame->Add( m_pwcv, 1,wxALL | wxEXPAND, 0 );
  m_pwcv->SetSim();

  if ( m_choices.m_bRunSim ) {
    FrameMain::vpItems_t vItems;
    using mi = FrameMain::structMenuItem;  // vxWidgets takes ownership of the objects

    vItems.push_back( new mi( "Start", MakeDelegate( this, &AppSP500::HandleMenuActionSimStart ) ) );
    vItems.push_back( new mi( "Stop",  MakeDelegate( this, &AppSP500::HandleMenuActionSimStop ) ) );
    vItems.push_back( new mi( "Stats",  MakeDelegate( this, &AppSP500::HandleMenuActionSimEmitStats ) ) );
    m_pFrameMain->AddDynamicMenu( "Simulation", vItems );
  }

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppSP500::OnClose, this );  // start close of windows and controls
  m_pFrameMain->Bind( wxEVT_MOVE, &AppSP500::OnFrameMainAutoMove, this ); // intercept first move
  m_pFrameMain->Show( true ); // triggers the auto move

  return true;

}

void AppSP500::OnFrameMainAutoMove( wxMoveEvent& event ) {

  m_pFrameMain->Unbind( wxEVT_MOVE, &AppSP500::OnFrameMainAutoMove, this );

  CallAfter(
    [this](){
      LoadState();
      m_pFrameMain->Layout();
      m_pStrategyManager = std::make_unique<StrategyManager>(
        m_choices
      , [this]( StrategyManager::fTask_t&& f ){ CallAfter( f ); } // fQueueTask_t
      , [this]( ou::ChartDataView& cdv ){ m_pwcv->SetChartDataView( &cdv ); } // fSetChartDataView_t
      );
    }
  );

  event.Skip(); // set to false if we want to ignore auto move

}

void AppSP500::HandleMenuActionSimStart() {
  CallAfter(
    [this](){
      //m_sim->Run();
    }
  );
}

void AppSP500::HandleMenuActionSimStop() {
  CallAfter(
    [this](){
      //m_sim->Stop();
    }
  );
}

void AppSP500::HandleMenuActionSimEmitStats() {
  std::stringstream ss;
  //m_sim->EmitStats( ss );
  std::cout << "Sim Stats: " << ss.str() << std::endl;
}

void AppSP500::SaveState() {
  std::cout << "Saving Config ..." << std::endl;
  std::ofstream ofs( c_sStateFileName );
  boost::archive::text_oarchive oa(ofs);
  oa & *this;
  std::cout << "  done." << std::endl;
}

void AppSP500::LoadState() {
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

void AppSP500::OnClose( wxCloseEvent& event ) {
  // Exit Steps: #2 -> FrameMain::OnClose

  m_pwcv->SetChartDataView( nullptr );

  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a
  SaveState();
  event.Skip();  // auto followed by Destroy();
}

int AppSP500::OnExit() {
  // Exit Steps: #4

  m_pStrategyManager.reset();

  return wxAppConsole::OnExit();
}
