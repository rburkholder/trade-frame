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

#include <vector>
#include <string>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <boost/filesystem.hpp>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <boost/lexical_cast.hpp>

#include <OUCommon/TimeSource.h>
#include <OUCommon/ReadSymbolFile.h>

#include "IntervalSampler.h"

IMPLEMENT_APP(AppIntervalSampler)

namespace {
   static const boost::gregorian::date dateBad;
}


bool AppIntervalSampler::OnInit() {

  m_nSequence = 0;
  m_dateSequence = dateBad;

  wxApp::OnInit();
  wxApp::SetAppDisplayName( "Interval Sampler" );
  wxApp::SetVendorName( "One Unified Net Limited" );
  wxApp::SetVendorDisplayName( "(c) 2019 One Unified Net Limited" );

  static const std::string sNameInterval( "interval" );
  static const std::string sNameStaleOrFiller( "stale_or_filler" );
  static const std::string sNameFiller( "filler" );
  static const std::string sNameCollectAt( "collect_at" );
  static const std::string sNameMethod( "method" );
  static const std::string sNameRegion( "region" );

  static const std::string sConfigFileName( "../IntervalSampler.cfg" );

  m_sStateFileName = "IntervalSampler.state";

  m_eCollectionMethod = ECollectionMethod::unknown;
  m_eDefaultContent   = EDefaultContent::unknown;

  po::variables_map vm;

  try {

    po::options_description config( "options" );
    config.add_options()
      ( sNameInterval.c_str(),      po::value<std::string>(), "interval (seconds)" )
      ( sNameStaleOrFiller.c_str(), po::value<std::string>(), "stale or filler" )
      ( sNameFiller.c_str(),        po::value<std::string>(), "filler" )
      ( sNameCollectAt.c_str(),     po::value<std::vector<std::string> >(), "collect at hh:mm (multiple instances allowed)" )
      ( sNameMethod.c_str(),        po::value<std::string>(), "collection method: interval|time" )
      ( sNameRegion.c_str(),        po::value<std::string>(), "time zone region (https://en.wikipedia.org/wiki/List_of_tz_database_time_zones)" )
      ;

    std::ifstream ifs( sConfigFileName.c_str() );
    if ( !ifs ) {
      std::cout << "file " << sConfigFileName << " does not exist" << std::endl;
    }
    else {
      po::store( po::parse_config_file( ifs, config), vm );
    }

    if ( 0 < vm.count( sNameCollectAt ) ) {
      std::cout << "collect_at entries: " << vm[sNameCollectAt].as<std::vector<std::string> >().size() << std::endl;
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

    if ( 0 == vm.count( sNameMethod ) ) {
      throw std::runtime_error( "please define collection method" );
    }
    else {
      std::string sMethod = vm[sNameMethod].as<std::string>();
      if ( "interval" == sMethod ) {

        if ( 3 != cnt ) {
          throw std::runtime_error( "incorrect number of interval parameters" );
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
            throw std::runtime_error( "Unknown default content type: " + sStaleOrFiller );
          }
        }

        m_nIntervalSeconds = boost::lexical_cast<size_t>( sInterval );
        if ( 0 >= m_nIntervalSeconds ) {
          throw std::runtime_error( sConfigFileName + ": interval needs to be greater than 0, is " + boost::lexical_cast<std::string>( m_nIntervalSeconds ) );
        }

        m_eCollectionMethod = ECollectionMethod::interval;

      }
      else {
        if ( "time" == sMethod ) {
          if ( 0 == vm.count( sNameRegion ) ) {
            throw std::runtime_error( "time zone region required" );
          }
          else {
            std::string sTimeZoneRegion = vm[ sNameRegion ].as<std::string>();
            m_ptz = ou::TimeSource::GlobalInstance().LoadTimeZone( sTimeZoneRegion );
            std::cout << sTimeZoneRegion << ": offset=" << m_ptz->base_utc_offset() << ", dst=" << m_ptz->dst_offset() << std::endl;
          }
          for ( auto& sCollectAt: vm[sNameCollectAt].as<std::vector<std::string> >() ) {
            m_vtdCollectAt.emplace_back( boost::posix_time::duration_from_string( sCollectAt ) );
          }
          if ( 0 == m_vtdCollectAt.size() ) {
            throw std::runtime_error( "method:time requires at least one collect_at entry" );
          }
          else {
            for ( vtdCollectAt_t::value_type td: m_vtdCollectAt ) {
              std::cout << "collect_at: " << boost::posix_time::to_simple_string( td ) << std::endl;
            }
            m_eCollectionMethod = ECollectionMethod::time;
          }
        }
        else {
          throw std::runtime_error( "unknown method selected: " + sMethod );
        }
      }
    }

  }
  catch ( std::exception& e ) {
    std::cout << "IntervalSampler config parse error: " << e.what() << std::endl;
    //throw e; // need to terminate without config
    return false;
  }

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
  sizerMain->Add( m_pPanelLogging, 1, wxALL | wxEXPAND, 0);
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
      m_pIQFeed = std::make_shared<ou::tf::iqfeed::Provider>(); // use ProviderManager::Construct?
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

void AppIntervalSampler::OutputFileOpen( boost::gregorian::date date ) {
  std::string sDate = boost::gregorian::to_iso_string( date );
  std::string sFileName = "is_" + sDate + ".csv";
  m_out.open( sFileName, std::ofstream::out | std::ofstream::app );
  if ( !m_out.is_open() ) {
    std::cerr << "cannot open file for output " << sFileName << std::endl;
    throw std::runtime_error( "file not opened" );
  }
  std::cout << "streaming to " << sFileName << std::endl;
}

void AppIntervalSampler::OutputFileCheck( boost::gregorian::date date ) {

  if ( !m_out.is_open() ) {
    OutputFileOpen( date );
    if ( dateBad != m_dateSequence ) {
      if ( m_dateSequence != date ) {
        m_nSequence = 0;
        std::cout << "  sequence restarted" << std::endl;
      }
    }
    m_dateNextRotation = m_dateSequence = date;
    if ( m_dateNextRotation <= date ) {
      m_dateNextRotation = m_dateNextRotation + boost::gregorian::date_duration( 1 );
    }
    std::cout << "  next rotation at " << boost::gregorian::to_iso_string( m_dateNextRotation ) << std::endl;
    //m_dateNextRotation = pt::ptime( dt.date() + boost::gregorian::date_duration( 1 ), pt::time_duration( 21, 30, 0 ) ); // UTC for eastern time
    //std::cout << "  note: trade values are open of next bar" << std::endl;
  }
  else {
    if ( m_dateNextRotation <= date ) {
      m_out.close();
      OutputFileCheck( date ); // recursive one step max
    }
  }
}

void AppIntervalSampler::CalcNextPollTime() {
  bool bFound( false );
  boost::posix_time::ptime now = ou::TimeSource::GlobalInstance().External();
  boost::posix_time::time_duration offset = m_ptz->base_utc_offset();
  if ( m_ptz->has_dst() ) { // not the most precise of calculations
    const boost::posix_time::ptime dst_bgn = m_ptz->dst_local_start_time( now.date().year() );
    const boost::posix_time::ptime dst_end = m_ptz->dst_local_end_time( now.date().year() );
    if ( ( dst_bgn <= now ) && ( now < dst_end ) ) {
      offset += m_ptz->dst_offset();
    }
  }
  boost::posix_time::ptime nowLocal = now + offset;
  boost::gregorian::date dateNowLocal( nowLocal.date() );
  for ( auto td: m_vtdCollectAt ) {
    boost::posix_time::ptime dtCollectAtLocal( dateNowLocal, td );
    boost::posix_time::ptime dtCollectAtUtc( dtCollectAtLocal - offset );
    if ( now < dtCollectAtUtc ) {
      m_dtInterval = dtCollectAtUtc;
      bFound = true;
      break;
    }
  }
  if ( !bFound ) {
    boost::posix_time::ptime dtCollectAtLocal( dateNowLocal + boost::gregorian::date_duration( 1 ), m_vtdCollectAt.front() );
    boost::posix_time::ptime dtCollectAtUtc( dtCollectAtLocal - offset );
    m_dtInterval = dtCollectAtUtc;
  }
  std::cout << "next collect_at(utc): " << m_dtInterval << std::endl;
}

void AppIntervalSampler::CalcNextPoll() {
  switch ( m_eCollectionMethod ) {
    case ECollectionMethod::interval:
      m_dtInterval = m_dtInterval + boost::posix_time::time_duration( 0, 0, m_nIntervalSeconds );
      break;
    case ECollectionMethod::time:
      CalcNextPollTime();
      break;
  }
}

void AppIntervalSampler::SubmitPoll() {
  assert( false );  // next statement needs fixing, see IntervalTrader
  //m_ptimerInterval->expires_at( m_dtInterval );
  m_ptimerInterval->async_wait( std::bind( &AppIntervalSampler::HandlePoll, this, std::placeholders::_1 ) );
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
      = std::make_shared<ou::tf::Instrument>( *iterSymbol, ou::tf::InstrumentType::Stock, "SMART" );
    pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_pIQFeed );
    //(*iterInstance) = std::move( std::make_unique<Capture>() );
    (*iterInstance).m_sInstrument = *iterSymbol;
    (*iterInstance).m_pCapture->Assign(
        // NOTE: this code path isn't used, and may be out of date
                       m_nIntervalSeconds, pWatch,
                       [this](
                            const ou::tf::Instrument::idInstrument_t& idInstrument,
                            size_t nSequence,
                            const ou::tf::Bar& bar,
                            const ou::tf::Quote& quote,
                            const ou::tf::Trade& trade){
                         OutputFileCheck( trade.DateTime().date() );
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

  switch ( m_eCollectionMethod ) {
    case ECollectionMethod::interval: {
      boost::posix_time::ptime now( ou::TimeSource::GlobalInstance().External() );
      m_dtInterval = boost::posix_time::ptime( now.date(), boost::posix_time::time_duration( now.time_of_day().hours(), 0, 0 ) );
      while ( m_dtInterval <= now ) {
        m_dtInterval = m_dtInterval + boost::posix_time::time_duration( 0, 0, m_nIntervalSeconds );
      }
      }
      break;
    case ECollectionMethod::time:
      CalcNextPollTime();
      break;
  }

  m_ptimerInterval = std::make_unique<boost::asio::system_timer>( m_context );
  SubmitPoll();

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
      OutputFileCheck( dtInterval.date() );
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

  // close the file, which will allow a download of data-to-date
  if ( m_out.is_open() ) {
    m_out.close();
  }

  if ( bSequenceUsed ) {
    m_nSequence = nSequence;
    CallAfter(
      [this](){
        SaveState( true );
      }
    );

  }

  if ( !error ) {
    CalcNextPoll();
    SubmitPoll();
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

void AppIntervalSampler::SaveState( bool bSilent ) {
  if ( !bSilent ) std::cout << "Saving Config ..." << std::endl;
  std::ofstream ofs( m_sStateFileName );
  boost::archive::text_oarchive oa(ofs);
  oa & *this;
  if ( !bSilent ) std::cout << "  done." << std::endl;
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

