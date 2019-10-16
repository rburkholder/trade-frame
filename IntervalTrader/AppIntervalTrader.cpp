/************************************************************************
 * Copyright(c) 2019, One Unified. All rights reserved.                 *
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
 * File:   AppIntervalTrader.cpp
 * Author: raymond@burkholder.net
 * Project: IntervalTrader
 * Created on October 6, 2019, 1:44 PM
 */

/* As an example, perhaps one where you take the stock that had the 
 * highest total dollar volume (volume * (open+close)/2) since the last iteration 
 * (volume and open and close being for that iteration), 
 * and also a BidPrice/AskPrice difference of less than 10 cents, 
 * and buy that, and sell it the next iteration.
 */

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <boost/filesystem.hpp>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <boost/lexical_cast.hpp>

#include <OUCommon/TimeSource.h>
#include <OUCommon/ReadSymbolFile.h>

#include <TFTrading/Watch.h>

#include "AppIntervalTrader.h"

IMPLEMENT_APP(AppIntervalTrader)

bool AppIntervalTrader::OnInit() {

  wxApp::OnInit();
  wxApp::SetAppDisplayName( "Interval Trader" );
  wxApp::SetVendorName( "One Unified Net Limited" );
  wxApp::SetVendorDisplayName( "(c) 2019 One Unified Net Limited" );

  static const std::string sNameInterval( "interval" );
  static const std::string sConfigFileName( "../IntervalTrader.cfg" );
  static const std::string sFileName( "../is_symbols.txt" );

  m_sStateFileName = "IntervalTrader.state";

  po::variables_map vm;

  try {

    po::options_description config( "options" );
    config.add_options()
      ( sNameInterval.c_str(),      po::value<std::string>(), "interval (seconds)" )
      ;

    std::ifstream ifs( sConfigFileName.c_str() );
    if ( !ifs ) {
      std::cout << "file " << sConfigFileName << " does not exist" << std::endl;
    }
    else {
      po::store( po::parse_config_file( ifs, config), vm );
    }

    int cnt {};

    if ( 0 < vm.count( sNameInterval ) ) {
      cnt++;
      std::cout << "interval: " << vm[sNameInterval].as<std::string>() << std::endl;
    }

    if ( 1 != cnt ) {
      throw std::runtime_error( "incorrect number of parameters" );
    }

  }
  catch ( std::exception& e ) {
    std::cout << "IntervalTrader config parse error: " << e.what() << std::endl;
    //throw e; // need to terminate without config
    return false;
  }

  std::string sInterval = vm[sNameInterval].as<std::string>();

  m_nIntervalSeconds = boost::lexical_cast<size_t>( sInterval );
  if ( 0 >= m_nIntervalSeconds ) {
    std::cout << sConfigFileName << ": interval needs to be greater than 0, is " << m_nIntervalSeconds << std::endl;
    return false;
  }

  m_pFrameMain = new FrameMain( 0, wxID_ANY, "Interval Trader" );
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
  //m_pFrameMain->Move( 200, 100 );
  //m_pFrameMain->SetSize( 1400, 800 );
  SetTopWindow( m_pFrameMain );

  wxBoxSizer* sizerMain = new wxBoxSizer( wxVERTICAL );
  m_pFrameMain->SetSizer( sizerMain );

  m_pPanelLogging = new ou::tf::PanelLogging( m_pFrameMain, wxID_ANY );
  sizerMain->Add( m_pPanelLogging, 1, wxALL | wxEXPAND|wxALIGN_LEFT|wxALIGN_RIGHT|wxALIGN_TOP|wxALIGN_BOTTOM, 0);
  m_pPanelLogging->Show( true );

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppIntervalTrader::OnClose, this );  // start close of windows and controls
  m_pFrameMain->Show( true );

  bool bOk( true );

  if ( !boost::filesystem::exists( sFileName ) ) {
    std::cout << "file " << sFileName << " cannot be found" << std::endl;
    bOk = false;
  }
  else {
    try {
      ReadSymbolFile symbols( sFileName, m_vSymbol );
    }
    catch( std::exception& e ) {
      std::cout << "error during parsing symbols.txt: " << e.what() << std::endl;
      //wxApp::Exit();
      bOk = false;
    }

    if ( bOk ) {
      m_pIQFeed = boost::make_shared<ou::tf::IQFeedProvider>();
      m_bIQFeedConnected = false;

      m_pIQFeed->OnConnecting.Add( MakeDelegate( this, &AppIntervalTrader::HandleIQFeedConnecting ) );
      m_pIQFeed->OnConnected.Add( MakeDelegate( this, &AppIntervalTrader::HandleIQFeedConnected ) );
      m_pIQFeed->OnDisconnecting.Add( MakeDelegate( this, &AppIntervalTrader::HandleIQFeedDisconnecting ) );
      m_pIQFeed->OnDisconnected.Add( MakeDelegate( this, &AppIntervalTrader::HandleIQFeedDisconnected ) );
      m_pIQFeed->OnError.Add( MakeDelegate( this, &AppIntervalTrader::HandleIQFeedError ) );

      m_pIQFeed->Connect();
    }
  }

  m_pWork = std::make_unique<boost::asio::executor_work_guard<boost::asio::io_context::executor_type> >( boost::asio::make_work_guard( m_context ) );
  m_thread = std::move( std::thread( [this ]{ m_context.run(); }) );

  CallAfter(
    [this](){
      LoadState();
    }
  );

  return true;
}

void AppIntervalTrader::HandleIQFeedConnecting( int e ) {  // cross thread event
  std::cout << "IQFeed connecting ..." << std::endl;
}

void AppIntervalTrader::HandleIQFeedConnected( int e ) {  // cross thread event

  assert( 0 < m_vSymbol.size() );

  using pWatch_t = ou::tf::Watch::pWatch_t;

  m_bIQFeedConnected = true;
  std::cout << "IQFeed connected." << std::endl;

  vSymbol_t::const_iterator iterSymbol = m_vSymbol.begin();
  iterSymbol++; // pass over the first line of duration
  m_vInstance.resize( m_vSymbol.size() );
  vInstance_t::iterator iterInstance = m_vInstance.begin();
  while ( m_vSymbol.end() != iterSymbol ) {
    ou::tf::Instrument::pInstrument_t pInstrument
      = boost::make_shared<ou::tf::Instrument>( *iterSymbol, ou::tf::InstrumentType::Stock, "SMART" );
    pWatch_t pWatch = boost::make_shared<ou::tf::Watch>( pInstrument, m_pIQFeed );
    //(*iterInstance) = std::move( std::make_unique<Capture>() );
//    (*iterInstance).m_sInstrument = *iterSymbol;
    iterSymbol++;
    iterInstance++;
  }

//  boost::posix_time::ptime now = ou::TimeSource::Instance().External();
//  m_dtInterval = boost::posix_time::ptime( now.date(), boost::posix_time::time_duration( now.time_of_day().hours(), 0, 0 ) );
//  while ( m_dtInterval <= now ) {
//    m_dtInterval = m_dtInterval + boost::posix_time::time_duration( 0, 0, m_nIntervalSeconds );
//  }

//  m_ptimerInterval = std::make_unique<boost::asio::deadline_timer>( m_context );
//  m_ptimerInterval->expires_at( m_dtInterval );
//  m_ptimerInterval->async_wait( std::bind( &AppIntervalSampler::HandlePoll, this, std::placeholders::_1 ) );

//  std::cout << "vInstance=" << m_vInstance.size() << ",vSymbol=" << m_vSymbol.size() << std::endl;

}

void AppIntervalTrader::HandlePoll( const boost::system::error_code& error ) {

  if ( !error ) {
    // TODO: fill this in
  }

  bool bQuoteFound;
  ou::tf::Quote quote;
  bool bTradeFound;
  ou::tf::Trade trade;
  bool bBarFound;
  ou::tf::Bar bar;

  // TODO: may be do the time zone conversion once where m_dtInterval is initially assigned?
  boost::local_time::local_date_time ltInterval( m_dtInterval, ou::TimeSource::TimeZoneNewYork() );
  boost::posix_time::ptime dtInterval = ltInterval.local_time();

//  if ( !error ) {
//    m_dtInterval = m_dtInterval + boost::posix_time::time_duration( 0, 0, m_nIntervalSeconds );
//    m_ptimerInterval->expires_at( m_dtInterval );
//    m_ptimerInterval->async_wait( std::bind( &AppIntervalSampler::HandlePoll, this, std::placeholders::_1 ) );
//  }
}

void AppIntervalTrader::HandleIQFeedDisconnecting( int e ) {  // cross thread event
  std::cout << "IQFeed disconnecting ..." << std::endl;
}

void AppIntervalTrader::HandleIQFeedDisconnected( int e ) { // cross thread event
  m_bIQFeedConnected = false;
  std::cout << "IQFeed disconnected." << std::endl;
}

void AppIntervalTrader::HandleIQFeedError( size_t e ) {
  std::cout << "HandleIQFeedError: " << e << std::endl;
}

void AppIntervalTrader::SaveState( bool bSilent ) {
  if ( !bSilent ) std::cout << "Saving Config ..." << std::endl;
  std::ofstream ofs( m_sStateFileName );
  boost::archive::text_oarchive oa(ofs);
  oa & *this;
  if ( !bSilent ) std::cout << "  done." << std::endl;
}

void AppIntervalTrader::LoadState() {
  try {
    std::cout << "Loading Config ..." << std::endl;
    std::ifstream ifs( m_sStateFileName );
    boost::archive::text_iarchive ia(ifs);
    ia & *this;
    std::cout << "  done." << std::endl;
  }
  catch(...) {
    std::cout << "load exception" << std::endl;
  }
}

void AppIntervalTrader::OnClose( wxCloseEvent& event ) { // step 1

  m_ptimerInterval->cancel();
  m_pWork->reset();
  m_thread.join();

  SaveState();

  event.Skip();  // auto followed by Destroy();
}

int AppIntervalTrader::OnExit() { // step 2

  if ( m_pIQFeed ) {
    if ( m_pIQFeed->Connected() ) {
      m_pIQFeed->Disconnect();

      m_pIQFeed->OnConnecting.Remove( MakeDelegate( this, &AppIntervalTrader::HandleIQFeedConnecting ) );
      m_pIQFeed->OnConnected.Remove( MakeDelegate( this, &AppIntervalTrader::HandleIQFeedConnected ) );
      m_pIQFeed->OnDisconnecting.Remove( MakeDelegate( this, &AppIntervalTrader::HandleIQFeedDisconnecting ) );
      m_pIQFeed->OnDisconnected.Remove( MakeDelegate( this, &AppIntervalTrader::HandleIQFeedDisconnected ) );
      m_pIQFeed->OnError.Remove( MakeDelegate( this, &AppIntervalTrader::HandleIQFeedError ) );
    }

    //if ( m_out.is_open() ) { // TODO: duplicate of 'disconnected, but disconnected not reached, need a join'
    //  m_out.close();
    //}
  }

  return wxApp::OnExit();
}

