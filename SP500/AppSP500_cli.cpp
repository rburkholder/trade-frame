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
 * File:    AppSP500_cli.cpp
 * Author:  raymond@burkholder.net
 * Project: SP500
 * Created: July 12, 2025 17:21:29
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

#include <boost/filesystem.hpp>

#include <boost/asio/post.hpp>
#include <boost/asio/execution/context.hpp>
#include <boost/asio/executor_work_guard.hpp>

#include "Config.hpp"
#include "StrategyManager.hpp"

namespace {
  static const std::string c_sVendorName( "One Unified Net Limited" );
  static const std::string c_sAppTitle(        "SP500 Trading" );
  static const std::string c_sAppNamePrefix(   "sp500" );
  static const std::string c_sChoicesFilename( c_sAppNamePrefix + ".cfg" );
  static const std::string c_sDbName(          c_sAppNamePrefix + ".db" );
  static const std::string c_sStateFileName(   c_sAppNamePrefix + ".state" );
  //static const std::string c_sTimeZoneSpec( "../date_time_zonespec.csv" );
}

config::Choices m_choices;

int main( int argc, char* argv[] ) {

  boost::asio::io_context m_context;
  std::unique_ptr<boost::asio::executor_work_guard<boost::asio::io_context::executor_type> > m_pWork
    = std::make_unique<boost::asio::executor_work_guard<boost::asio::io_context::executor_type> >( boost::asio::make_work_guard( m_context) );

  if ( config::Load( c_sChoicesFilename, m_choices ) ) {
    using EMode = config::Choices::EMode;

    if ( ( EMode::train_then_validate == m_choices.eMode ) || ( EMode::view_training == m_choices.eMode ) ) {
      for ( const auto& sFileName: m_choices.m_vFileTraining ) {
        if ( boost::filesystem::exists( sFileName ) ) {}
        else {
          BOOST_LOG_TRIVIAL(error) << "training file " << sFileName << " does not exist";
          return 1; // EXIT_FAILURE
        }
      }
    }

    if ( ( EMode::train_then_validate == m_choices.eMode ) || ( EMode::view_validate == m_choices.eMode ) ) {
      if ( boost::filesystem::exists( m_choices.m_sFileValidate ) ) {}
      else {
        BOOST_LOG_TRIVIAL(error) << "validation file " << m_choices.m_sFileValidate << " does not exist";
        return 2; // EXIT_FAILURE
      }
    }
  }
  else {
    return 3; // EXIT_FAILURE
  }

  /*
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
  */

  // todo: maybe need a non-visual result baserd constructor
  using pStrategyManager_t = std::unique_ptr<StrategyManager>;
  pStrategyManager_t m_pStrategyManager;
  m_pStrategyManager = std::make_unique<StrategyManager>(
    m_choices
  , [&m_context]( StrategyManager::fTask_t&& f ){ boost::asio::post( m_context, std::move( f ) );  } // fQueueTask_t
  , []( ou::tf::WinChartView::EView state, ou::ChartDataView* pcdv ){ // fSetChartDataView_t
      //m_pwcv->SetChartDataView( pcdv );
      //m_pwcv->Set( state );
    }
  , [](const ou::tf::Quotes& quotes, const ou::tf::Trades& trades)->StrategyManager::fUpdateDateTime_t{ // fSetTimeSeriesModel_t
      return nullptr;
    }
  , [&m_pWork](){
    m_pWork.reset();
    }
  );

  m_context.run();

  std::stringstream ss;
  //m_sim->EmitStats( ss );
  std::cout << "Sim Stats: " << ss.str() << std::endl;

  m_pStrategyManager.reset();

  return EXIT_SUCCESS;

}

