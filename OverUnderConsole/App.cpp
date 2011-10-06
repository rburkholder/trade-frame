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

#include "App.h"


App::App(void) 
  : m_mgrInstrument( ou::tf::CInstrumentManager::Instance() )
{

}

App::~App(void) {
}

void App::Run( void ) {

  ou::tf::CProviderManager::Instance().Register( "ib01", static_cast<pProvider_t>( &m_tws ) );
  ou::tf::CProviderManager::Instance().Register( "iq01", static_cast<pProvider_t>( &m_iqfeed ) );

//  m_pInstrument = m_mgrInstrument.ConstructFuture( "GC", "SMART", 2011, 12 );
//  m_pInstrument->SetAlternateName( m_iqfeed.ID(), "+GCZ11" );

  m_tws.OnConnected.Add( MakeDelegate( this, &App::Connected ) );
  m_iqfeed.OnConnected.Add( MakeDelegate( this, &App::Connected ) );

  m_tws.OnDisconnected.Add( MakeDelegate( this, &App::DisConnected ) );
  m_iqfeed.OnDisconnected.Add( MakeDelegate( this, &App::DisConnected ) );

  m_tws.Connect();
  m_iqfeed.Connect();


  // start up worker thread here
  m_pwork = new boost::asio::io_service::work(m_io);  // keep the asio service running 
  m_asioThread = boost::thread( boost::bind( &App::WorkerThread, this ) );

  // handle console input while thread is working in background
  // http://www.cplusplus.com/doc/tutorial/basic_io/
  std::string s;
  do {
    std::cout << "command: ";
    std::cin >> s;
  } while ( "x" != s );

  // clean up 
  m_tws.Disconnect();
  m_iqfeed.Disconnect();

  // wait for worker thread to end
//  delete m_pwork;  // stop the asio service (let it run out of work, which at this point should be none)
  m_asioThread.join();  // wait for i/o thread to cleanup and terminate

}

void App::Connected( int i ) {

  if ( m_tws.Connected() ) {
  }

  if ( m_tws.Connected() && m_iqfeed.Connected() ) {
    ou::tf::CIBTWS::Contract contract;
    contract.currency = "USD";
    //contract.exchange = "SMART";  in this case is NYMEX
    contract.secType = "FUT";
    contract.symbol = "GC";
    contract.expiry = "201112";
    // IB responds only when symbol is found, bad symbols will not illicit a response
    m_tws.RequestContractDetails( contract, MakeDelegate( this, &App::HandleIBContractDetails ), MakeDelegate( this, &App::HandleIBContractDetailsDone ) );

    //m_tws.GetSymbol( m_pInstrument );
  }
}

void App::DisConnected( int i ) {
  if ( !m_tws.Connected() && ! m_iqfeed.Connected() ) {
    delete m_pwork;  // stop the asio service (let it run out of work, which at this point should be none)
  }
}

void App::HandleQuote( const ou::tf::CQuote& quote ) {
}

void App::HandleTrade( const ou::tf::CTrade& trade ) {
}

// separate thread 
void App::WorkerThread( void ) {
  m_io.run();  // deal with the submitted work
}

void App::HandleIBContractDetails( const ou::tf::CIBTWS::ContractDetails& details, const pInstrument_t& pInstrument ) {
  m_pInstrument = pInstrument;
  m_pInstrument->SetAlternateName( m_iqfeed.ID(), "+GCZ11" );
}

void App::HandleIBContractDetailsDone( void ) {
}