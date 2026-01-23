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
      std::cerr << "file " << sConfigFileName << " does not exist" << std::endl;
    }
    else {
      po::store( po::parse_config_file( ifs, config), vm );
    }

    int cnt {};

    if ( 0 < vm.count( sNameInterval ) ) {
      cnt++;
      std::cerr << "interval: " << vm[sNameInterval].as<std::string>() << std::endl;
    }

    if ( 1 != cnt ) {
      throw std::runtime_error( "incorrect number of parameters" );
    }

  }
  catch ( std::exception& e ) {
    std::cerr << "IntervalTrader config parse error: " << e.what() << std::endl;
    //throw e; // need to terminate without config
    return false;
  }

  std::string sInterval = vm[sNameInterval].as<std::string>();

  m_nIntervalSeconds = boost::lexical_cast<size_t>( sInterval );
  if ( 0 >= m_nIntervalSeconds ) {
    std::cerr << sConfigFileName << ": interval needs to be greater than 0, is " << m_nIntervalSeconds << std::endl;
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
  sizerMain->Add( m_pPanelLogging, 1, wxALL | wxEXPAND, 0);
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
      m_pPortfolio
        = std::make_shared<ou::tf::Portfolio>(
          "trader", "self", "aggregate", ou::tf::Portfolio::EPortfolioType::Basket, "USD", "aggregate" );

      m_bIQFeedConnected = false;
      m_pIQFeed = std::make_shared<ou::tf::iqfeed::Provider>(); // use ProviderManager::Construct?
      m_pIQFeed->OnConnecting.Add( MakeDelegate( this, &AppIntervalTrader::HandleIQFeedConnecting ) );
      m_pIQFeed->OnConnected.Add( MakeDelegate( this, &AppIntervalTrader::HandleIQFeedConnected ) );
      m_pIQFeed->OnDisconnecting.Add( MakeDelegate( this, &AppIntervalTrader::HandleIQFeedDisconnecting ) );
      m_pIQFeed->OnDisconnected.Add( MakeDelegate( this, &AppIntervalTrader::HandleIQFeedDisconnected ) );
      m_pIQFeed->OnError.Add( MakeDelegate( this, &AppIntervalTrader::HandleIQFeedError ) );
      m_pIQFeed->Connect();

      m_bIBConnected = false;
      m_pIB = std::make_shared<ou::tf::ib::TWS>(); // use ProviderManager::Construct?
      m_pIB->OnConnecting.Add( MakeDelegate( this, &AppIntervalTrader::HandleIBConnecting ) );
      m_pIB->OnConnected.Add( MakeDelegate( this, &AppIntervalTrader::HandleIBConnected ) );
      m_pIB->OnDisconnecting.Add( MakeDelegate( this, &AppIntervalTrader::HandleIBDisconnecting ) );
      m_pIB->OnDisconnected.Add( MakeDelegate( this, &AppIntervalTrader::HandleIBDisconnected ) );
      m_pIB->OnError.Add( MakeDelegate( this, &AppIntervalTrader::HandleIBError ) );
      m_pIB->Connect();
    }
  }

  m_bPolling = false;
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

  m_bIQFeedConnected = true;
  std::cout << "IQFeed connected." << std::endl;

  using pWatch_t = ou::tf::Watch::pWatch_t;

  vSymbol_t::const_iterator iterSymbol = m_vSymbol.begin();
  m_vInstance.reserve( m_vSymbol.size() );
  for ( vSymbol_t::value_type& sSymbol: m_vSymbol ) {

    // lib/TFIQFeed/BuildInstrument.cpp
    ou::tf::Instrument::pInstrument_t pInstrument
      = std::make_shared<ou::tf::Instrument>( sSymbol, ou::tf::InstrumentType::Stock, "SMART" );
    pInstrument->SetMultiplier( 1 );  // default
    pInstrument->SetMinTick( 0.01 );
    pInstrument->SetSignificantDigits( 2 );

    pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_pIQFeed );
    pPosition_t pPosition = std::make_shared<ou::tf::Position>( pWatch, m_pIB );
    m_pPortfolio->AddPosition( sSymbol, pPosition );
    m_vInstance.emplace_back( Instance( pPosition ) );
  }

std::cout << "vInstance=" << m_vInstance.size() << ",vSymbol=" << m_vSymbol.size() << std::endl;

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

void AppIntervalTrader::HandleIBConnecting( int ) {
  std::cout << "Interactive Brokers connecting ..." << std::endl;
}

void AppIntervalTrader::HandleIBConnected( int ) {
  m_bIBConnected = true;
  std::cout << "Interactive Brokers connected." << std::endl;
}

void AppIntervalTrader::HandleIBDisconnecting( int ) {
  std::cout << "Interactive Brokers disconnecting ..." << std::endl;
}

void AppIntervalTrader::HandleIBDisconnected( int ) {
  m_bIBConnected = false;
  std::cout << "Interactive Brokers disconnected." << std::endl;
}

void AppIntervalTrader::HandleIBError( size_t e ) {
  std::cout << "HandleIBError: " << e << std::endl;
}

void AppIntervalTrader::StartPoll() { // this doesn't appear to be called, dead wood?
  if ( !m_bPolling ) {
    if ( m_bIQFeedConnected && m_bIBConnected ) {

      boost::posix_time::ptime now = ou::TimeSource::GlobalInstance().External();
      m_dtInterval = boost::posix_time::ptime( now.date(), boost::posix_time::time_duration( now.time_of_day().hours(), 0, 0 ) );
      while ( m_dtInterval <= now ) {
        m_dtInterval = m_dtInterval + boost::posix_time::time_duration( 0, 0, m_nIntervalSeconds );
      }

      assert( false ); // doesn't appear to be used
      // to fix, look at Collector/main.cpp line ~106
      m_ptimerInterval = std::make_unique<boost::asio::system_timer>( m_context );
      //m_ptimerInterval->expires_at( m_dtInterval );
      m_ptimerInterval->async_wait( std::bind( &AppIntervalTrader::HandlePoll, this, std::placeholders::_1 ) );

      m_bPolling = true;
    }
  }

}

void AppIntervalTrader::HandlePoll( const boost::system::error_code& error ) {

  if ( error ) {
    std::cout << "AppIntervalTrader::HandlePoll error: " << error.message() << std::endl;
  }
  else {

    // TODO: may be do the time zone conversion once where m_dtInterval is initially assigned?
    boost::local_time::local_date_time ltInterval( m_dtInterval, ou::TimeSource::TimeZoneNewYork() );
    boost::posix_time::ptime dtInterval = ltInterval.local_time();
    static const boost::posix_time::time_duration start( 9, 30, 0 ); // start of trading
    static const boost::posix_time::time_duration end(  16,  0, 0 ); // end of trading

    // TODO: refactor to one time calculation
    boost::posix_time::time_duration interval( 0, 0, m_nIntervalSeconds );

    boost::posix_time::time_duration current = dtInterval.time_of_day();
    if ( ( current >= start ) && ( current < ( end - interval ) ) ) {

      // close existing position
      if ( m_pActivePosition ) {
        std::cout << "closing position: " << m_pActivePosition->GetInstrument()->GetInstrumentName() << std::endl;
        m_pActivePosition->CancelOrders();
        m_pActivePosition->ClosePosition( ou::tf::OrderType::Market );
        m_pActivePosition.reset();
      }

      // calculate new max dollar volume
      double dblMaxAccumulatedDollarVolume {};
      pPosition_t pNewPosition;

      for ( vInstance_t::value_type& instance: m_vInstance ) {
        instance.Evaluate(
          [this,&dblMaxAccumulatedDollarVolume,&pNewPosition]( double dblInstrumentAccumulatedDollarVolume, pPosition_t pPosition ){
            if ( dblMaxAccumulatedDollarVolume < dblInstrumentAccumulatedDollarVolume ) {
              dblMaxAccumulatedDollarVolume = dblInstrumentAccumulatedDollarVolume;
              pNewPosition = pPosition;
            }
          } );
      }

      using pOrder_t = ou::tf::Order::pOrder_t;

      if ( pNewPosition ) {
        std::cout << "opening position: " << pNewPosition->GetInstrument()->GetInstrumentName() << std::endl;
        m_pActivePosition = pNewPosition;
        if ( 0 == m_pActivePosition->GetInstrument()->GetContract() ) {
          assert( m_pIB );
          pInstrument_t pInstrument = m_pActivePosition->GetInstrument();
          m_pIB->RequestContractDetails(
            pInstrument->GetInstrumentName(),
            pInstrument,
            [this](const ou::tf::ib::TWS::ContractDetails& details, pInstrument_t& pInstrument){
              assert( 0 != pInstrument->GetContract() );
              // TODO: refactor the common bits out - see below
              pOrder_t pOrder = m_pActivePosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 100 );
              m_pActivePosition->PlaceOrder( pOrder );
              },
            nullptr
          );
        }
        else {
          // TODO: refactor the common bits out - see above
          pOrder_t pOrder = m_pActivePosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 100 );
          m_pActivePosition->PlaceOrder( pOrder );
        }
      }
    }

    m_dtInterval = m_dtInterval + interval;
    //m_ptimerInterval->expires_at( m_dtInterval );
    m_ptimerInterval->async_wait( std::bind( &AppIntervalTrader::HandlePoll, this, std::placeholders::_1 ) );
  }

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

