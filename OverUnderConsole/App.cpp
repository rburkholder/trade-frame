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

#include <boost/bind.hpp>

#include "App.h"


App::App(void) 
  : m_mgrInstrument( ou::tf::CInstrumentManager::Instance() ),
  m_ptws( new ou::tf::CIBTWS ), m_piqfeed( new ou::tf::CIQFeedProvider )
{

}

App::~App(void) {
}

void App::Run( void ) {

  ou::tf::CProviderManager::Instance().Register( "ib01", static_cast<ou::tf::CProviderManager::pProvider_t>( m_ptws ) );
  ou::tf::CProviderManager::Instance().Register( "iq01", static_cast<ou::tf::CProviderManager::pProvider_t>( m_piqfeed ) );

//  m_pInstrument = m_mgrInstrument.ConstructFuture( "GC", "SMART", 2011, 12 );
//  m_pInstrument->SetAlternateName( m_iqfeed.ID(), "+GCZ11" );

  m_ptws->OnConnected.Add( MakeDelegate( this, &App::Connected ) );
  m_piqfeed->OnConnected.Add( MakeDelegate( this, &App::Connected ) );

  m_ptws->OnDisconnected.Add( MakeDelegate( this, &App::DisConnected ) );
  m_piqfeed->OnDisconnected.Add( MakeDelegate( this, &App::DisConnected ) );

  m_ptws->Connect();
  m_piqfeed->Connect();

  // start up worker thread here
  m_pwork = new boost::asio::io_service::work(m_io);  // keep the asio service running 
  m_asioThread = boost::thread( boost::bind( &App::WorkerThread, this ) );

  m_md.initiate();  // start state chart for market data
  m_md.process_event( rtd::EvInitialize() );

  // handle console input while thread is working in background
  // http://www.cplusplus.com/doc/tutorial/basic_io/
  std::string s;
  do {
    std::cout << "command: ";
    std::cin >> s;
  } while ( "x" != s );

  // clean up 
  StopWatch();
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
    ou::tf::CIBTWS::Contract contract;
    contract.currency = "USD";
    //contract.exchange = "SMART";  in this case is NYMEX
    contract.secType = "FUT";
    contract.symbol = "GC";
    contract.expiry = "201112";
    // IB responds only when symbol is found, bad symbols will not illicit a response
    m_ptws->RequestContractDetails( contract, MakeDelegate( this, &App::HandleIBContractDetails ), MakeDelegate( this, &App::HandleIBContractDetailsDone ) );

    //m_tws.GetSymbol( m_pInstrument );
  }
}

void App::DisConnected( int i ) {
  if ( !m_ptws->Connected() && ! m_piqfeed->Connected() ) {
    delete m_pwork;  // stop the asio service (let it run out of work, which at this point should be none)
  }
}

void App::HandleQuote( const ou::tf::CQuote& quote ) {
  m_quotes.Append( quote );
}

void App::HandleTrade( const ou::tf::CTrade& trade ) {
  m_trades.Append( trade );
}

void App::HandleOpen( const ou::tf::CTrade& trade ) {
}

// separate thread 
void App::WorkerThread( void ) {
  m_io.run();  // deal with the submitted work
}

void App::HandleIBContractDetails( const ou::tf::CIBTWS::ContractDetails& details, const pInstrument_t& pInstrument ) {
  m_pInstrument = pInstrument;
  m_pInstrument->SetAlternateName( m_piqfeed->ID(), "+GCZ11" );
  m_io.post( boost::bind( &App::StartWatch, this ) );
}

void App::HandleIBContractDetailsDone( void ) {
}

void App::StartWatch( void ) {
  m_piqfeed->AddQuoteHandler( m_pInstrument, MakeDelegate( this, &App::HandleQuote ) );
  m_piqfeed->AddTradeHandler( m_pInstrument, MakeDelegate( this, &App::HandleTrade ) );
  m_piqfeed->AddOnOpenHandler( m_pInstrument, MakeDelegate( this, &App::HandleOpen ) );
}

void App::StopWatch( void ) {
  m_piqfeed->RemoveQuoteHandler( m_pInstrument, MakeDelegate( this, &App::HandleQuote ) );
  m_piqfeed->RemoveTradeHandler( m_pInstrument, MakeDelegate( this, &App::HandleTrade ) );
  m_piqfeed->RemoveOnOpenHandler( m_pInstrument, MakeDelegate( this, &App::HandleOpen ) );
}

