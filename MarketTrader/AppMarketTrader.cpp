/************************************************************************
 * Copyright(c) 2024, One Unified. All rights reserved.                 *
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
 * File:    AppMarketTrader.cpp
 * Author:  raymond@burkholder.net
 * Project: MarketTrader
 * Created: 2024/11/10 12:18:19
 */

// TODO: add telegram, lua,
// TODO: live charts, charts from history
// TODO: futures, equities, option combos
// TODO: IB FA
// TODO: multiple currency management

#include <string>

#include <boost/log/trivial.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <TFVuTrading/FrameMain.h>

#include "AppMarketTrader.hpp"

namespace {
  static const std::string c_sAppName( "MarketTrader" );
  static const std::string c_sVendorName( "One Unified Net Limited" );
  static const std::string c_sVendorCopyright( "(c)2024 " + c_sVendorName );

  static const std::string c_sDirectory( "MarketTrader" );
  static const std::string c_sDbName(          c_sDirectory + '/' + c_sAppName + ".db" );
  static const std::string c_sStateFileName(   c_sDirectory + '/' + c_sAppName + ".state" );
  static const std::string c_sChoicesFilename( c_sDirectory + '/' + c_sAppName + ".cfg" );
  static const std::string c_sDirectoryLua(    c_sDirectory + '/' + "lua" + '/' );  // scripts

  //static const std::string c_sMenuItemPortfolio( "_USD" );

  //static const std::string c_sPortfolioCurrencyName( "USD" ); // pre-created, needs to be uppercase
  //static const std::string c_sPortfolioSimulationName( "sim" );
  //static const std::string c_sPortfolioRealTimeName( "live" );
}

IMPLEMENT_APP(AppMarketTrader)

bool AppMarketTrader::OnInit() {

  wxApp::SetVendorName( c_sVendorName );
  wxApp::SetAppDisplayName( c_sAppName );
  wxApp::SetVendorDisplayName( c_sVendorCopyright );

  wxApp::OnInit();

  m_bProvidersConnected = false;
  EnableProviders();

  m_pFrameMain = new FrameMain( 0, wxID_ANY, c_sAppName );
  wxWindowID idFrameMain = m_pFrameMain->GetId();

  SetTopWindow( m_pFrameMain );

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppMarketTrader::OnClose, this );  // start close of windows and controls

  m_pFrameMain->Bind( wxEVT_MOVE, &AppMarketTrader::OnFrameMainAutoMove, this );
  m_pFrameMain->Show( true ); // triggers the auto move

  m_LuaControl.AddPath( c_sDirectoryLua, []( ou::FileNotify::EType, const std::string& sFileName ){} );

  return true;
}

void AppMarketTrader::OnFrameMainAutoMove( wxMoveEvent& event ) {

  CallAfter(
    [this](){
      LoadState();
      m_pFrameMain->Layout();
    }
  );

  m_pFrameMain->Unbind( wxEVT_MOVE, &AppMarketTrader::OnFrameMainAutoMove, this );

  event.Skip(); // set to false if we want to ignore auto move

}

void AppMarketTrader::EnableProviders() {

  m_iqf = ou::tf::iqfeed::Provider::Factory();
  //m_iqf->SetThreadCount( m_choices.nThreads );
  m_iqf->SetName( "iq01" );
  m_data = m_iqf;
  m_iqf->OnConnected.Add( MakeDelegate( this, &AppMarketTrader::ProviderConnected ) );
  m_iqf->OnDisconnected.Add( MakeDelegate( this, &AppMarketTrader::ProviderDisconnected ) );
  m_iqf->Connect();

  m_tws = ou::tf::ib::TWS::Factory();
  //m_tws->SetClientId( m_choices.m_nIbInstance );
  m_tws->SetName( "ib01" );
  m_exec = m_tws;
  m_tws->OnConnected.Add( MakeDelegate( this, &AppMarketTrader::ProviderConnected ) );
  m_tws->OnDisconnected.Add( MakeDelegate( this, &AppMarketTrader::ProviderDisconnected ) );
  m_tws->Connect();

}

void AppMarketTrader::DisableProviders() {
  m_iqf->Disconnect();
  m_iqf->OnConnected.Remove( MakeDelegate( this, &AppMarketTrader::ProviderConnected ) );
  m_iqf->OnDisconnected.Remove( MakeDelegate( this, &AppMarketTrader::ProviderDisconnected ) );

  m_tws->Disconnect();
  m_tws->OnConnected.Remove( MakeDelegate( this, &AppMarketTrader::ProviderConnected ) );
  m_tws->OnDisconnected.Remove( MakeDelegate( this, &AppMarketTrader::ProviderDisconnected ) );
}



void AppMarketTrader::ProviderConnected( int ) {
  if ( m_iqf->Connected() && m_tws->Connected() ) {
    if ( !m_bProvidersConnected ) {
      m_bProvidersConnected = true;
      BOOST_LOG_TRIVIAL(info) << "providers connected";
    }
  }
}

void AppMarketTrader::ProviderDisconnected( int ) {
  if ( !m_iqf->Connected() || !m_tws->Connected() ) {
    m_bProvidersConnected = false;
    BOOST_LOG_TRIVIAL(info) << "providers dis-connected";
  }
}

void AppMarketTrader::SaveState() {
  BOOST_LOG_TRIVIAL(info) << "Saving state ...";
  std::ofstream ofs( c_sStateFileName );
  boost::archive::text_oarchive oa(ofs);
  oa & *this;
  BOOST_LOG_TRIVIAL(info) << "Saving state done.";
}

void AppMarketTrader::LoadState() {
  try {
    BOOST_LOG_TRIVIAL(info) << "Loading state ...";
    std::ifstream ifs( c_sStateFileName );
    boost::archive::text_iarchive ia(ifs);
    ia & *this;
    BOOST_LOG_TRIVIAL(info) << "Loading state done.";
  }
  catch( const boost::archive::archive_exception& e ) {
    // https://www.boost.org/doc/libs/1_87_0/libs/serialization/doc/index.html
    BOOST_LOG_TRIVIAL(error) << "Exception (archive): state load " << e.what();
  }
  catch( const std::exception& e ) {
    BOOST_LOG_TRIVIAL(error) << "Exception (std): state load " << e.what();
  }
}

void AppMarketTrader::OnClose( wxCloseEvent& event ) {
  // Exit Steps: #2 -> FrameMain::OnClose

  //if ( m_timerOneSecond.IsRunning() ) {
  //  m_timerOneSecond.Stop();
  //  Unbind( wxEVT_TIMER, &AppCurrencyTrader::HandleTimer, this, m_timerOneSecond.GetId() );
  //}

  //m_pWinChartView->SetChartDataView( nullptr, false );

  SaveState();

  m_LuaControl.DelPath( c_sDirectoryLua );

  DisableProviders();

  //if ( m_pdb ) m_pdb.reset();

  // NOTE: when running the simuliation, perform a deletion instead
  //   use the boost file system utilities?
  //   or the object Delete() operator may work
//  if ( m_choices.bStartSimulator ) {
//    if ( m_pFile ) { // performed at exit to ensure no duplication in file
      //m_pFile->Delete(); // double free or corruption here
//    }
//  }
//  else {
//    if ( m_pFile ) { // performed at exit to ensure no duplication in file
//      m_pFile->Write();
//    }
//  }

  event.Skip();  // auto followed by Destroy();
}

int AppMarketTrader::OnExit() {
  // Exit Steps: #4
  return wxAppConsole::OnExit();
}

