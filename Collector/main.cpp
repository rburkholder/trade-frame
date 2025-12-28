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

#include <memory>
#include <string>
#include <iomanip>
#include <sstream>

#include <boost/log/trivial.hpp>

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <boost/asio/signal_set.hpp>
#include <boost/asio/steady_timer.hpp>
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

  std::string sDateStreamStarted;
  std::string sTSDataStreamStarted;

  boost::posix_time::ptime dtStop;

  config::Choices choices;

  if ( Load( sConfigFileName, choices ) ) {
  }
  else {
    return EXIT_FAILURE;
  }

  {
    const auto dt = ou::TimeSource::GlobalInstance().External(); // UTC

    sDateStreamStarted = ou::tf::Instrument::BuildDate( dt.date() );

    std::stringstream ss;
    ss
      << sDateStreamStarted
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

  // a number of conversion steps for ptime to time_t for expires_at
  const std::time_t ttStop( boost::posix_time::to_time_t( dtStop ) );

  const std::tm tmStop = *std::gmtime( &ttStop );
  BOOST_LOG_TRIVIAL(info) << "dtStop=" << dtStop << ',' << "ttStop=" << std::put_time( &tmStop, "%Y-%m-%d %H:%M:%S" );;

  const boost::asio::chrono::system_clock::time_point tpStop( boost::asio::chrono::system_clock::from_time_t( ttStop ) );

  boost::asio::system_timer timerStop( m_context );
  timerStop.expires_at( tpStop );

  // write data every 60 seconds
  boost::asio::steady_timer timerWrite( m_context );
  timerWrite.expires_after( boost::asio::chrono::seconds( 60 ) );

  // https://www.boost.org/doc/libs/1_79_0/doc/html/boost_asio/reference/signal_set.html
  boost::asio::signal_set signals( m_context, SIGINT ); // SIGINT is called
  //signals.add( SIGKILL ); // not allowed here
  signals.add( SIGHUP ); // use this as a day change?
  //signals.add( SIGINFO ); // control T - doesn't exist on linux
  //signals.add( SIGTERM );
  //signals.add( SIGQUIT );
  //signals.add( SIGABRT );

  using pProcess_t = std::unique_ptr<Process>;
  pProcess_t pProcess = std::make_unique<Process>( choices, sDateStreamStarted, sTSDataStreamStarted, dtStop );

  signals.async_wait(
    [&pProcess,&timerStop,&timerWrite,&m_pWork](const boost::system::error_code& error_code, int signal_number){
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
        pProcess.reset();
        timerStop.cancel();
      }
    } );

  using fWrite_t = std::function<void(const boost::system::error_code&)>;

  fWrite_t fWrite = [&pProcess,&timerWrite,&fWrite]( const boost::system::error_code& error_code ){
    if ( 0 == error_code.value() ) {
      pProcess->Write();
      timerWrite.expires_after( boost::asio::chrono::seconds( 60 ) );
      timerWrite.async_wait( fWrite );
    }
  };

  timerWrite.async_wait(
    [&timerWrite,&fWrite]( const boost::system::error_code& error_code ){
      fWrite( error_code );
    }
  );

  timerStop.async_wait(
    [&pProcess,&signals,&timerWrite,&m_pWork]( const boost::system::error_code& error_code ){
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
        pProcess.reset();
        signals.cancel();
      }
    });

  m_context.run();

  signals.clear();
  signals.cancel();

  return EXIT_SUCCESS;
}

