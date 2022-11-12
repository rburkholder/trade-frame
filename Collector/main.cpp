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
 * Created: October 20, 2022 17:48:11
 */

#include <string>
#include <sstream>

#include <boost/date_time/posix_time/posix_time_types.hpp>

//#include <boost/asio/post.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/execution/context.hpp>
//#include <boost/asio/executor_work_guard.hpp>

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

void clean_up() {
}

// ==========

void signal_handler(
  const boost::system::error_code& error_code,
  int signal_number
) {

  std::cout
    << "signal"
    << "(" << error_code.category().name()
    << "," << error_code.value()
    << "," << signal_number
    << "): "
    << error_code.message() << ", clean up"
    << std::endl;

  if ( !error_code ) {
    clean_up();
    std::terminate(); // can this change into a conditional variable, and drop out the bottom?
  }
}

// ==========

int main( int argc, char* argv[] ) {

  const static std::string sConfigFileName( "futuresl1l2.cfg" );

  boost::asio::io_context m_context;
  std::unique_ptr<boost::asio::executor_work_guard<boost::asio::io_context::executor_type> > m_pWork
    = std::make_unique<boost::asio::executor_work_guard<boost::asio::io_context::executor_type> >( boost::asio::make_work_guard( m_context) );

  std::string sTSDataStreamStarted;

  config::Choices choices;

  if ( Load( sConfigFileName, choices ) ) {
  }
  else {
    return EXIT_FAILURE;
  }

  {
    std::stringstream ss;
    auto dt = ou::TimeSource::GlobalInstance().External();
    ss
      << ou::tf::Instrument::BuildDate( dt.date() )
      << "-"
      << dt.time_of_day()
      ;
    sTSDataStreamStarted = ss.str();  // will need to make this generic if need some for multiple providers.
  }

  // https://www.boost.org/doc/libs/1_79_0/doc/html/boost_asio/reference/signal_set.html
  boost::asio::signal_set signals( m_context, SIGINT ); // SIGINT is called
  //signals.add( SIGKILL ); // not allowed here
  signals.add( SIGHUP ); // use this as a day change?
  //signals.add( SIGTERM );
  //signals.add( SIGQUIT );
  //signals.add( SIGABRT );
  signals.async_wait( signal_handler ); // convert to method call in Process

  Process process( choices, sTSDataStreamStarted );
  //boost::asio::post( m_context, [&process](){ process.Wait(); } );

  m_context.run();

  //signals.clear();
  //signals.cancel();

  return EXIT_SUCCESS;
}

