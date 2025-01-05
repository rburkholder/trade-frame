/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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
 * File:    Main.cpp
 * Author:  raymond@burkholder.net
 * Project: Collector
 * Created: January 5, 2025 18:04:09
 */

#include <string>
#include <sstream>

#include <boost/log/trivial.hpp>

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <boost/asio/signal_set.hpp>
#include <boost/asio/execution/context.hpp>
#include <boost/asio/executor_work_guard.hpp>

#include <OUCommon/TimeSource.h>

#include <TFTrading/Instrument.h>

#include "Config.hpp"
#include "Process.hpp"

/*
  * symbol from config file
  * convert continuous name to specific
  * auto start iqfeed
  * level 1, level 2 data
  * switch to new save date at 17:30 EST each day
  * console based

  * atomic variable[2]
    * switching between: 0) filling, 1) writing to disk
    * once a minute or so
*/

// ==========

int main( int argc, char* argv[] ) {

  const static std::string sConfigFileName( "collector.cfg" );

  boost::asio::io_context m_context;
  std::unique_ptr<boost::asio::executor_work_guard<boost::asio::io_context::executor_type> > m_pWork
    = std::make_unique<boost::asio::executor_work_guard<boost::asio::io_context::executor_type> >( boost::asio::make_work_guard( m_context) );

  std::string sTSDataStreamStarted;

  boost::posix_time::ptime dtStop;

  config::Choices choices;

  if ( Load( sConfigFileName, choices ) ) {
  }
  else {
    return EXIT_FAILURE;
  }

  {
    const auto dt = ou::TimeSource::GlobalInstance().External();

    std::stringstream ss;
    ss
      << ou::tf::Instrument::BuildDate( dt.date() )
      << "-"
      << dt.time_of_day()
      ;
    sTSDataStreamStarted = ss.str();  // will need to make this generic if need some for multiple providers.

    // app will need to be started after an expiry time to ensure cycling to next day
    dtStop = boost::posix_time::ptime( ou::TimeSource::GlobalInstance().Local().date(), choices.m_tdStopTime );
    dtStop = ou::TimeSource::GlobalInstance().ConvertEasternToUtc( dtStop );
    if ( dt >= dtStop ) {
      dtStop = dtStop + boost::gregorian::days( 1 );
    }

    BOOST_LOG_TRIVIAL(info)
      << "now=" << dt << "(UTC)"
      << ",then=" << dtStop << "(UTC)"
      ;
  }

  // write data every 60 seconds
  boost::asio::deadline_timer timerWrite( m_context );
  timerWrite.expires_from_now( boost::posix_time::seconds( 60 ) );

  boost::asio::deadline_timer timerStop( m_context );
  timerStop.expires_at( dtStop );

  // https://www.boost.org/doc/libs/1_79_0/doc/html/boost_asio/reference/signal_set.html
  boost::asio::signal_set signals( m_context, SIGINT ); // SIGINT is called
  //signals.add( SIGKILL ); // not allowed here
  signals.add( SIGHUP ); // use this as a day change?
  //signals.add( SIGINFO ); // control T - doesn't exist on linux
  //signals.add( SIGTERM );
  //signals.add( SIGQUIT );
  //signals.add( SIGABRT );

  // unique ptr?  for daily start/stop?
  // one process per instrument? or aggregate instrument/watch
  Process process( choices, sTSDataStreamStarted );

  signals.async_wait(
    [&process,&timerStop,&timerWrite,&m_pWork](const boost::system::error_code& error_code, int signal_number){
      BOOST_LOG_TRIVIAL(error)
        << "signal"
        << "(" << error_code.category().name()
        << "," << error_code.value()
        << "," << signal_number
        << "): "
        << error_code.message()
        << std::endl;

      if ( SIGINT == signal_number) {
        timerWrite.cancel();
        m_pWork->reset();
        process.Finish();
        timerStop.cancel();
      }
    } );

  using fWrite_t = std::function<void(const boost::system::error_code&)>;

  fWrite_t fWrite = [&process,&timerWrite,&fWrite]( const boost::system::error_code& error_code ){
    if ( 0 == error_code.value() ) {
      process.Write();
      timerWrite.expires_from_now( boost::posix_time::seconds( 60 ) );
      timerWrite.async_wait( fWrite );
    }
  };

  timerWrite.async_wait(
    [&process,&timerWrite,&fWrite]( const boost::system::error_code& error_code ){
      fWrite( error_code );
    }
  );

  timerStop.async_wait(
    [&process,&signals,&timerWrite,&m_pWork]( const boost::system::error_code& error_code ){
      BOOST_LOG_TRIVIAL(error)
        << "timer "
        << "(" << error_code.category().name()
        << "," << error_code.value()
        << "): "
        << error_code.message()
        << std::endl;
      if ( 0 == error_code.value() ) { // 125 is op canceled
        timerWrite.cancel();
        m_pWork->reset();
        process.Finish();
        signals.cancel();
      }
    });

  m_context.run();

  signals.clear();
  signals.cancel();

  return EXIT_SUCCESS;
}

