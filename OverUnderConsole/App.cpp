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

#include "ScanHistory.h"
#include "App.h"

#include <boost/phoenix/bind/bind_member_function.hpp>

App::App(void) 
  : m_mgrInstrument( ou::tf::CInstrumentManager::Instance() ),
  m_ptws( new ou::tf::CIBTWS ), m_piqfeed( new ou::tf::CIQFeedProvider )
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
    if ( "c" == s ) { // close position
      //m_md.data.pPosition->ClosePosition();
//      m_md.post_event(
    }
  } while ( "x" != s );

  // clean up 

//  StopWatch();

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

    for ( vOperation_t::iterator iter = m_vOperation.begin(); m_vOperation.end() != iter; iter++ ) {
      iter->Start();
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

void App::StartStateMachine( void ) {
  m_io.post( boost::phoenix::bind( &App::StartWatch, this ) );
}

void App::OnHistoryConnected( void ) {
  InstrumentState& is( m_md.data );
  is.dblOpen = is.dblHigh = is.dblLow = is.dblClose = 0.0;
  ptime dtStart = m_pInstrument->GetTimeTrading().begin();
  ptime dtEnd = m_pInstrument->GetTimeTrading().end();
  if ( 0 == dtStart.date().day_of_week() ) {
    RetrieveDatedRangeOfDataPoints( 
      m_pInstrument->GetInstrumentName( m_piqfeed->ID() ), dtStart - date_duration( 3 ), dtEnd - date_duration( 3 ) );
  }
  else {
    RetrieveDatedRangeOfDataPoints( 
      m_pInstrument->GetInstrumentName( m_piqfeed->ID() ), dtStart - date_duration( 1 ), dtEnd - date_duration( 1 ) );
  }
}

void App::OnHistoryDisconnected( void ) {
}

void App::OnHistoryTickDataPoint( structTickDataPoint* pDP ) {
  InstrumentState& is( m_md.data );
  if ( 0 == is.dblOpen ) {
    is.dblOpen = is.dblHigh = is.dblLow = is.dblClose = pDP->Last;
  }
  else {
    if ( pDP->Last > is.dblHigh ) is.dblHigh = pDP->Last;
    if ( pDP->Last < is.dblLow ) is.dblLow = pDP->Last;
    is.dblClose = pDP->Last;
  }
  is.history.Append( ou::tf::CTrade( pDP->DateTime, pDP->Last, pDP->LastSize ) );

}

void App::OnHistoryRequestDone( void ) {
  InstrumentState& is( m_md.data );
  std::cout << "History complete" << std::endl;
  StartStateMachine();
}

void App::SelectTradeableSymbols( void ) {
  ScanHistory sh;
  sh.Run();
  sh.GetInfo<Operation::structSymbolInfo>( boost::phoenix::bind( &App::AppendTradeableSymbol, this, boost::phoenix::arg_names::arg1 ) );
}

void App::AppendTradeableSymbol( const Operation::structSymbolInfo& si ) {
  Operation op( si, m_piqfeed, m_ptws );
  //std::cout << si.sName << std::endl;
  m_vOperation.push_back( op );
}