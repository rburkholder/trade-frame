/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#include <cassert>

#include <OUCommon/TimeSource.h>

#include "ScanHistory.h"
#include "App.h"

#include <boost/phoenix/bind/bind_member_function.hpp>

App::App(void) 
  : m_mgrInstrument( ou::tf::CInstrumentManager::Instance() ),
  m_ptws( new ou::tf::CIBTWS ), m_piqfeed( new ou::tf::CIQFeedProvider ),
  m_dblPortfolioCashToTrade( 110000.0 ), m_dblPortfolioMargin( 0.15 )
{
}

App::~App(void) {
  m_ptws.reset();
  m_piqfeed.reset();
}

void App::Run( void ) {

  ou::tf::CProviderManager::Instance().Register( "ib01", static_cast<ou::tf::CProviderManager::pProvider_t>( m_ptws ) );
  ou::tf::CProviderManager::Instance().Register( "iq01", static_cast<ou::tf::CProviderManager::pProvider_t>( m_piqfeed ) );

  m_ptws->OnConnected.Add( MakeDelegate( this, &App::Connected ) );
  m_piqfeed->OnConnected.Add( MakeDelegate( this, &App::Connected ) );

  m_ptws->OnDisconnected.Add( MakeDelegate( this, &App::DisConnected ) );
  m_piqfeed->OnDisconnected.Add( MakeDelegate( this, &App::DisConnected ) );

  std::stringstream ss;
  ss.str( "" );
  ss << ou::CTimeSource::Instance().Internal();
  m_sTSDataStreamStarted = "/app/OverUnderConsole/" + ss.str();  // will need to make this generic if need some for multiple providers.

  m_ptws->Connect();
  m_piqfeed->Connect();

  // start up worker thread here
  m_pwork = new boost::asio::io_service::work(m_io);  // keep the asio service running 
  m_asioThread = boost::thread( boost::phoenix::bind( &App::WorkerThread, this ) );

  // handle console input while thread is working in background
  // http://www.cplusplus.com/doc/tutorial/basic_io/
  std::string s;
  do {
    std::cout << "command: ";
    std::cin >> s;
    if ( "s" == s ) {  // stats
      //std::cout << "Q:" << m_md.data.quotes.Size() << ", T:" << m_md.data.trades.Size() << std::endl;
    }
    if ( "a" == s ) { // save quotes/trades
      for ( vOperation_t::iterator iter = m_vOperation.begin(); m_vOperation.end() != iter; iter++ ) {
        (*iter)->SaveSeries( m_sTSDataStreamStarted );
      }
    }
    if ( "c" == s ) { // close position
      //m_md.data.pPosition->ClosePosition();
//      m_md.post_event(
    }
  } while ( "x" != s );

  // clean up 

  for ( vOperation_t::iterator iter = m_vOperation.begin(); m_vOperation.end() != iter; iter++ ) {
    if ( (*iter)->ToBeTraded() ) {
      (*iter)->Stop();
    }
  }

  m_ptws->Disconnect();
  m_piqfeed->Disconnect();

  // wait for worker thread to end
//  delete m_pwork;  // stop the asio service (let it run out of work, which at this point should be none)
  m_asioThread.join();  // wait for i/o thread to cleanup and terminate

  ou::tf::CProviderManager::Instance().Release( "ib01" );
  ou::tf::CProviderManager::Instance().Release( "iq01" );

}

void App::Connected( int i ) {

  if ( m_ptws->Connected() ) {
  }

  if ( m_ptws->Connected() && m_piqfeed->Connected() ) {

    SelectTradeableSymbols();  // adds symbols to m_vOperation

    // first pass: to get rough idea of which can be traded given our funding level
    double dblAmountToTradePerSymbol = ( m_dblPortfolioCashToTrade / m_dblPortfolioMargin ) / m_vOperation.size();

    unsigned int cntToBeTraded = 0;
    for ( vOperation_t::iterator iter = m_vOperation.begin(); m_vOperation.end() != iter; iter++ ) {
      if ( 200 <= (*iter)->CalcShareCount( dblAmountToTradePerSymbol ) ) {
        cntToBeTraded++;
        (*iter)->ToBeTraded() = true;
      }
      else {
        (*iter)->ToBeTraded() = false;
      }
    }

    // second pass: start trading with the ones that we can
    dblAmountToTradePerSymbol = ( m_dblPortfolioCashToTrade / m_dblPortfolioMargin ) / cntToBeTraded;

    for ( vOperation_t::iterator iter = m_vOperation.begin(); m_vOperation.end() != iter; iter++ ) {
      if ( (*iter)->ToBeTraded() ) {
        (*iter)->Start( dblAmountToTradePerSymbol );
      }
    }
  }
}

void App::DisConnected( int i ) {
  if ( !m_ptws->Connected() && ! m_piqfeed->Connected() ) {
    delete m_pwork;  // stop the asio service (let it run out of work, which at this point should be none)
  }
}

// separate thread 
void App::WorkerThread( void ) {
  m_io.run();  // deal with the submitted work
}

//void App::StartStateMachine( void ) {
//  m_io.post( boost::phoenix::bind( &App::StartWatch, this ) );
//}

void App::SelectTradeableSymbols( void ) {
  ScanHistory sh;
  sh.Run();
  sh.GetInfo<Operation::structSymbolInfo>( boost::phoenix::bind( &App::AppendTradeableSymbol, this, boost::phoenix::arg_names::arg1 ) );
}

void App::AppendTradeableSymbol( const Operation::structSymbolInfo& si ) {
  pOperation_t p( new Operation( si, m_piqfeed, m_ptws ) );
  std::cout << "Tradeable Symbol: " << si.sName << std::endl;
  m_vOperation.push_back( p );
}