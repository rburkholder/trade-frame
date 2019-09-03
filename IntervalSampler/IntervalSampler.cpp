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
 * File:    main.cpp
 * Author:  raymond@burkholder.net
 * Project: IntervalSampler
 * Created on August 6, 2019, 11:08 AM
 */

// An IQFeed based project to collect data from symbols at an interval

// assumption:  all symbols are 'stocks'.
//   lookups will be required if non-stock symbols are provided (or not, if no special processing is required)

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <boost/filesystem.hpp>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <boost/lexical_cast.hpp>

#include <OUCommon/TimeSource.h>

#include "ReadSymbolFile.h"
#include "IntervalSampler.h"

IMPLEMENT_APP(AppIntervalSampler)

bool AppIntervalSampler::OnInit() {

  m_nSequence = 0;

  wxApp::OnInit();
  wxApp::SetAppDisplayName( "Interval Sampler" );
  wxApp::SetVendorName( "One Unified Net Limited" );
  wxApp::SetVendorDisplayName( "(c) 2019 One Unified Net Limited" );

  static const std::string sNameInterval( "interval" );
  static const std::string sNameStaleOrFiller( "stale_or_filler" );
  static const std::string sNameFiller( "filler" );

  static const std::string sConfigFileName( "../IntervalSampler.cfg" );

  po::variables_map vm;

  try {

    po::options_description config( "options" );
    config.add_options()
      ( sNameInterval.c_str(),      po::value<std::string>(), "interval (seconds)" )
      ( sNameStaleOrFiller.c_str(), po::value<std::string>(), "stale or filler" )
      ( sNameFiller.c_str(),        po::value<std::string>(), "filler" )
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

    if ( 0 < vm.count( sNameStaleOrFiller ) ) {
      cnt++;
      std::cout << "stale or filler: " << vm[sNameStaleOrFiller].as<std::string>() << std::endl;
    }

    if ( 0 < vm.count( sNameFiller ) ) {
      cnt++;
      std::cout << "filler: " << vm[sNameFiller].as<std::string>() << std::endl;
    }

    if ( 3 != cnt ) {
      throw std::runtime_error( "incorrect number of parameters" );
    }

  }
  catch ( std::exception& e ) {
    std::cout << "IntervalSampler config parse error: " << e.what() << std::endl;
    //throw e; // need to terminate without config
    return false;
  }

  std::string sInterval = vm[sNameInterval].as<std::string>();
  std::string sStaleOrFiller = vm[sNameStaleOrFiller].as<std::string>();
  m_sFieldFiller = vm[sNameFiller].as<std::string>();

  if ( "stale" == sStaleOrFiller ) {
    m_eDefaultContent = EDefaultContent::stale;
  }
  else {
    if ( "filler" == sStaleOrFiller ) {
      m_eDefaultContent = EDefaultContent::filler;
    }
    else {
      std::cout << "Unknown default content: " << sStaleOrFiller << std::endl;
      return false;
    }
  }

  m_nIntervalSeconds = boost::lexical_cast<size_t>( sInterval );
  if ( 0 >= m_nIntervalSeconds ) {
    std::cout << sConfigFileName << ": interval needs to be greater than 0, is " << m_nIntervalSeconds << std::endl;
    return false;
  }

  m_sStateFileName = "IntervalSampler.state";

  m_pFrameMain = new FrameMain( 0, wxID_ANY, "Interval Sampler" );
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

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppIntervalSampler::OnClose, this );  // start close of windows and controls
  m_pFrameMain->Show( true );

  static const std::string sFileName( "../is_symbols.txt" );

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

      m_pIQFeed->OnConnecting.Add( MakeDelegate( this, &AppIntervalSampler::HandleIQFeedConnecting ) );
      m_pIQFeed->OnConnected.Add( MakeDelegate( this, &AppIntervalSampler::HandleIQFeedConnected ) );
      m_pIQFeed->OnDisconnecting.Add( MakeDelegate( this, &AppIntervalSampler::HandleIQFeedDisconnecting ) );
      m_pIQFeed->OnDisconnected.Add( MakeDelegate( this, &AppIntervalSampler::HandleIQFeedDisconnected ) );
      m_pIQFeed->OnError.Add( MakeDelegate( this, &AppIntervalSampler::HandleIQFeedError ) );

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

void AppIntervalSampler::HandleIQFeedConnecting( int e ) {  // cross thread event
  std::cout << "IQFeed connecting ..." << std::endl;
}

void AppIntervalSampler::OutputFileOpen( ptime dt ) {
  std::string sdt = boost::posix_time::to_iso_string( dt );
  std::string sFileName = sdt + ".csv";
  m_out.open( sFileName, std::fstream::out );
  if ( !m_out.is_open() ) {
    std::cerr << "cannot open file for output " << sFileName << std::endl;
    std::runtime_error( "file not opened" );
  }
  std::cout << "streaming to " << sFileName << std::endl;
}

void AppIntervalSampler::OutputFileCheck( ptime dt ) {

  namespace pt = boost::posix_time;

  //ptime dt( ou::TimeSource::Instance().External() );
  if ( !m_out.is_open() ) {
    OutputFileOpen( dt );
    m_dtNextRotation = pt::ptime( dt.date(), pt::time_duration( 0, 0, 0 ) );
    if ( m_dtNextRotation <= dt ) {
      m_dtNextRotation = m_dtNextRotation + boost::gregorian::date_duration( 1 );
    }
    std::cout << "  next rotation at " << boost::posix_time::to_iso_string( m_dtNextRotation ) << std::endl;
    //m_dtNextRotation = pt::ptime( dt.date() + boost::gregorian::date_duration( 1 ), pt::time_duration( 21, 30, 0 ) ); // UTC for eastern time
    //std::cout << "  note: trade values are open of next bar" << std::endl;
  }
  else {
    if ( m_dtNextRotation <= dt ) {
      m_out.close();
      OutputFileCheck( dt ); // recursive one step max
    }
  }
}

void AppIntervalSampler::HandleIQFeedConnected( int e ) {  // cross thread event

  using pWatch_t = ou::tf::Watch::pWatch_t;

  m_bIQFeedConnected = true;
  std::cout << "IQFeed connected." << std::endl;

  assert( 1 < m_vSymbol.size() );
  //size_t nSeconds = boost::lexical_cast<size_t>( m_vSymbol[ 0 ] );
  //std::cout << "interval: " << m_vSymbol[ 0 ] << " seconds" << std::endl;

  vSymbol_t::const_iterator iterSymbol = m_vSymbol.begin();
  //iterSymbol++; // pass over the first line of duration
  //m_vInstance.resize( m_vSymbol.size() - 1 );
  m_vInstance.resize( m_vSymbol.size() );
  vInstance_t::iterator iterInstance = m_vInstance.begin();
  while ( m_vSymbol.end() != iterSymbol ) {
    ou::tf::Instrument::pInstrument_t pInstrument
      = boost::make_shared<ou::tf::Instrument>( *iterSymbol, ou::tf::InstrumentType::Stock, "SMART" );
    pWatch_t pWatch = boost::make_shared<ou::tf::Watch>( pInstrument, m_pIQFeed );
    //(*iterInstance) = std::move( std::make_unique<Capture>() );
    (*iterInstance).m_sInstrument = *iterSymbol;
    (*iterInstance).m_pCapture->Assign(
                       m_nIntervalSeconds, pWatch,
                       [this](
                            const ou::tf::Instrument::idInstrument_t& idInstrument,
                            size_t nSequence,
                            const ou::tf::Bar& bar,
                            const ou::tf::Quote& quote,
                            const ou::tf::Trade& trade){
                         OutputFileCheck( trade.DateTime() );
                         m_out
                           << idInstrument
                           << "," << nSequence
                           << "," << boost::posix_time::to_iso_string( trade.DateTime() )
                           << "," << bar.High()
                           << "," << bar.Low()
                           << "," << bar.Open()
                           << "," << bar.Close()
                           << "," << bar.Volume()
                           << "," << trade.Price() << "," << trade.Volume()
                           << "," << quote.Ask() << "," << quote.AskSize()
                           << "," << quote.Bid() << "," << quote.BidSize()
                           << std::endl;
                       } );
    iterSymbol++;
    iterInstance++;
  }

  boost::posix_time::ptime now = ou::TimeSource::Instance().External();
  m_dtInterval = boost::posix_time::ptime( now.date(), boost::posix_time::time_duration( now.time_of_day().hours(), 0, 0 ) );
  while ( m_dtInterval <= now ) {
    m_dtInterval = m_dtInterval + boost::posix_time::time_duration( 0, 0, m_nIntervalSeconds );
  }

  m_ptimerInterval = std::make_unique<boost::asio::deadline_timer>( m_context );
  m_ptimerInterval->expires_at( m_dtInterval );
  m_ptimerInterval->async_wait( std::bind( &AppIntervalSampler::HandlePoll, this, std::placeholders::_1 ) );

  std::cout << "vInstance=" << m_vInstance.size() << ",vSymbol=" << m_vSymbol.size() << std::endl;

}

void AppIntervalSampler::HandlePoll( const boost::system::error_code& error ) {

  if ( !error ) {
    // TODO: fill this in
  }

  size_t nSequence = m_nSequence + 1;
  bool bSequenceUsed( false );
  bool bFileTested( false );

  bool bQuoteFound;
  ou::tf::Quote quote;
  bool bTradeFound;
  ou::tf::Trade trade;
  bool bBarFound;
  ou::tf::Bar bar;

  // TODO: may be do the time zone conversion once where m_dtInterval is initially assigned?
  boost::local_time::local_date_time ltInterval( m_dtInterval, ou::TimeSource::TimeZoneNewYork() );
  boost::posix_time::ptime dtInterval = ltInterval.local_time();

  for ( vInstance_t::value_type& vt: m_vInstance ) {
    vt.m_pCapture->Pull( bBarFound, bar, bQuoteFound, quote, bTradeFound, trade );
    if ( !bFileTested ) {
      OutputFileCheck( dtInterval );
      bFileTested = true;
    }
    m_out
      << vt.m_sInstrument
      << "," << nSequence
      ;
    if ( bBarFound || ( ( EDefaultContent::stale == m_eDefaultContent ) && !bar.DateTime().is_not_a_date_time() ) ) {
      m_out
        << "," << boost::posix_time::to_iso_string( dtInterval )
        << "," << bar.Open()
        << "," << bar.High()
        << "," << bar.Low()
        << "," << bar.Close()
        << "," << bar.Volume()
        ;
    }
    else {
      m_out
        << "," << boost::posix_time::to_iso_string( dtInterval )
        << "," << m_sFieldFiller
        << "," << m_sFieldFiller
        << "," << m_sFieldFiller
        << "," << m_sFieldFiller
        << "," << m_sFieldFiller
        ;
    }
    if ( bQuoteFound || ( ( EDefaultContent::stale == m_eDefaultContent ) && !quote.DateTime().is_not_a_date_time() ) ) {
      boost::local_time::local_date_time ltQuote( quote.DateTime(), ou::TimeSource::TimeZoneNewYork() );
      boost::posix_time::ptime dtQuote = ltQuote.local_time();
      m_out
        << "," << boost::posix_time::to_iso_string( dtQuote )
        << "," << quote.Ask() << "," << quote.AskSize()
        << "," << quote.Bid() << "," << quote.BidSize()
        ;
    }
    else {
      m_out
        << "," << m_sFieldFiller
        << "," << m_sFieldFiller << "," << m_sFieldFiller
        << "," << m_sFieldFiller << "," << m_sFieldFiller
        ;
    }
    if ( bTradeFound || ( (EDefaultContent::stale == m_eDefaultContent ) && !trade.DateTime().is_not_a_date_time() ) ) {
      boost::local_time::local_date_time ltTrade( trade.DateTime(), ou::TimeSource::TimeZoneNewYork() );
      boost::posix_time::ptime dtTrade = ltTrade.local_time();
      m_out
        << "," << boost::posix_time::to_iso_string( dtTrade )
        << "," << trade.Price() << "," << trade.Volume()
        ;
    }
    else {
      m_out
        << "," << m_sFieldFiller
        << "," << m_sFieldFiller << "," << m_sFieldFiller
        ;
    }
    m_out << std::endl;
    bSequenceUsed = true;
  }
  if ( bSequenceUsed ) {
    m_nSequence = nSequence;
  }

  if ( !error ) {
    m_dtInterval = m_dtInterval + boost::posix_time::time_duration( 0, 0, m_nIntervalSeconds );
    m_ptimerInterval->expires_at( m_dtInterval );
    m_ptimerInterval->async_wait( std::bind( &AppIntervalSampler::HandlePoll, this, std::placeholders::_1 ) );
  }
}

void AppIntervalSampler::HandleIQFeedDisconnecting( int e ) {  // cross thread event
  std::cout << "IQFeed disconnecting ..." << std::endl;
}

void AppIntervalSampler::HandleIQFeedDisconnected( int e ) { // cross thread event
  if ( m_out.is_open() ) {
    m_out.close();
  }
  m_bIQFeedConnected = false;
  std::cout << "IQFeed disconnected." << std::endl;
}

void AppIntervalSampler::HandleIQFeedError( size_t e ) {
  std::cout << "HandleIQFeedError: " << e << std::endl;
}

void AppIntervalSampler::SaveState() {
  std::cout << "Saving Config ..." << std::endl;
  std::ofstream ofs( m_sStateFileName );
  boost::archive::text_oarchive oa(ofs);
  oa & *this;
  std::cout << "  done." << std::endl;
}

void AppIntervalSampler::LoadState() {
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

void AppIntervalSampler::OnClose( wxCloseEvent& event ) { // step 1

  m_ptimerInterval->cancel();
  m_pWork->reset();
  m_thread.join();

  if ( m_out.is_open() ) {
    m_out.close();
  }

  SaveState();

  event.Skip();  // auto followed by Destroy();
}

int AppIntervalSampler::OnExit() { // step 2

  m_vInstance.clear();

  if ( m_pIQFeed ) {
    if ( m_pIQFeed->Connected() ) {
      m_pIQFeed->Disconnect();

      m_pIQFeed->OnConnecting.Remove( MakeDelegate( this, &AppIntervalSampler::HandleIQFeedConnecting ) );
      m_pIQFeed->OnConnected.Remove( MakeDelegate( this, &AppIntervalSampler::HandleIQFeedConnected ) );
      m_pIQFeed->OnDisconnecting.Remove( MakeDelegate( this, &AppIntervalSampler::HandleIQFeedDisconnecting ) );
      m_pIQFeed->OnDisconnected.Remove( MakeDelegate( this, &AppIntervalSampler::HandleIQFeedDisconnected ) );
      m_pIQFeed->OnError.Remove( MakeDelegate( this, &AppIntervalSampler::HandleIQFeedError ) );
    }

    //if ( m_out.is_open() ) { // TODO: duplicate of 'disconnected, but disconnected not reached, need a join'
    //  m_out.close();
    //}
  }

  return wxApp::OnExit();
}

