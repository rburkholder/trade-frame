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

#include <boost/bind.hpp>

#include "App.h"

// following are specific for gold futures
InstrumentState::InstrumentState( void ):
  tdMarketOpen( time_duration( 19, 0, 0 ) ), // time relative to day
  tdMarketOpenIdle( time_duration( 0, 0, 30 ) ),  // time relative to tdMarketOpen to allow initial collection of data
  tdCancelOrders( time_duration( 18, 0, 0 ) ),// time relative to day
  tdClosePositions( time_duration( 18, 10, 0 ) ),// time relative to day
  tdAfterMarket( time_duration( 18, 45, 0 ) ), // time relative to day
  tdMarketClosed( time_duration( 18, 45, 0 ) ), // time relative to day
  stoch30sec( &quotes, 30 ), stoch5min( &quotes, 300 ), stoch30min( &quotes, 1800 ),
  stats30sec( &quotes, 30 ),
  bDaySession( true )
  {
    bMarketHoursCrossMidnight = tdMarketOpen > tdMarketClosed;
  }

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
  m_asioThread = boost::thread( boost::bind( &App::WorkerThread, this ) );

  m_md.initiate();  // start state chart for market data
  m_md.process_event( ou::tf::EvInitialize() );

  // handle console input while thread is working in background
  // http://www.cplusplus.com/doc/tutorial/basic_io/
  std::string s;
  do {
    std::cout << "command: ";
    std::cin >> s;
    if ( "s" == s ) {
      std::cout << "Q:" << m_md.data.quotes.Size() << ", T:" << m_md.data.trades.Size() << std::endl;
    }
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

  }
}

void App::DisConnected( int i ) {
  if ( !m_ptws->Connected() && ! m_piqfeed->Connected() ) {
    delete m_pwork;  // stop the asio service (let it run out of work, which at this point should be none)
  }
}

void App::HandleQuote( const ou::tf::CQuote& quote ) {
  InstrumentState& is( m_md.data );
  if ( is.bMarketHoursCrossMidnight ) {
    is.bDaySession = quote.DateTime().time_of_day() <= is.tdMarketClosed;
  }
  assert( is.bDaySession || is.bMarketHoursCrossMidnight );
  is.quotes.Append( quote );
  is.stoch30sec.Update();
  is.stoch5min.Update();
  is.stoch30min.Update();
  is.stats30sec.Update();
  m_md.process_event( ou::tf::EvQuote( quote ) );
}

void App::HandleTrade( const ou::tf::CTrade& trade ) {
  InstrumentState& is( m_md.data );
  if ( is.bMarketHoursCrossMidnight ) {
    is.bDaySession = trade.DateTime().time_of_day() <= is.tdMarketClosed;
  }
  assert( is.bDaySession || is.bMarketHoursCrossMidnight );
  is.trades.Append( trade );
  m_md.process_event( ou::tf::EvTrade( trade ) );
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
  m_md.data.pPosition.reset( new ou::tf::CPosition( m_pInstrument, m_ptws, m_piqfeed ) );
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

sc::result App::StatePreMarket::Handle( const EvQuote& quote ) {
  InstrumentState& is( context<App::MachineMarketStates>().data );
  if ( is.bMarketHoursCrossMidnight && is.bDaySession ) { // transit
    return transit<App::StateMarketOpen>();  // late but transit anyway
  }
  else { // test
    if ( quote.Quote().DateTime().time_of_day() >= is.tdMarketOpen ) {
      return transit<App::StateMarketOpen>();
    }
  }
  return discard_event();
}

sc::result App::StateMarketOpen::Handle( const EvQuote& quote ) {
  InstrumentState& is( context<App::MachineMarketStates>().data );
  is.dtPreTradingStop = quote.Quote().DateTime() + is.tdMarketOpenIdle;
  is.dblMidQuoteAtOpen = ( quote.Quote().Ask() + quote.Quote().Bid() ) / 2.0;
  return transit<App::StatePreTrading>();
}

sc::result App::StatePreTrading::Handle( const EvQuote& quote ) {
  InstrumentState& is( context<App::MachineMarketStates>().data );

  if ( quote.Quote().DateTime() >= is.dtPreTradingStop ) {
    return transit<App::StateTrading>();
  }

  return discard_event();
}

sc::result App::StateCancelOrders::Handle( const EvQuote& quote ) {
  InstrumentState& is( context<App::MachineMarketStates>().data );
  return transit<App::StateCancelOrdersIdle>();
}

sc::result App::StateCancelOrdersIdle::Handle( const EvQuote& quote ) {
  InstrumentState& is( context<App::MachineMarketStates>().data );

  if ( is.bDaySession ) { // transit
    if ( quote.Quote().DateTime().time_of_day() >= is.tdClosePositions ) {
      return transit<App::StateClosePositions>();
    }
  }
  return discard_event();
}

sc::result App::StateClosePositions::Handle( const EvQuote& quote ) {
  InstrumentState& is( context<App::MachineMarketStates>().data );
  return transit<App::StateClosePositionsIdle>();
}

sc::result App::StateClosePositionsIdle::Handle( const EvQuote& quote ) {
  InstrumentState& is( context<App::MachineMarketStates>().data );

  if ( is.bDaySession ) { // transit
    if ( quote.Quote().DateTime().time_of_day() >= is.tdAfterMarket ) {
      return transit<App::StateAfterMarket>();
    }
  }
  return discard_event();
}

sc::result App::StateAfterMarket::Handle( const EvQuote& quote ) {
  InstrumentState& is( context<App::MachineMarketStates>().data );

  if ( is.bDaySession ) { // transit
    if ( quote.Quote().DateTime().time_of_day() >= is.tdMarketClosed ) {
      return transit<App::StateMarketClosed>();
    }
  }
  return discard_event();
}

sc::result App::StateMarketClosed::Handle( const EvQuote& quote ) {
  InstrumentState& is( context<App::MachineMarketStates>().data );
  return discard_event();
}

sc::result App::StateZeroPosition::Handle( const EvQuote& quote ) {
  InstrumentState& is( context<App::MachineMarketStates>().data );
  if ( is.bDaySession ) { // transit
    if ( quote.Quote().DateTime().time_of_day() >= is.tdCancelOrders ) {
      return transit<App::StateCancelOrders>();
    }
  }

  double mid = ( quote.Quote().Ask() + quote.Quote().Bid() ) / 2.0;
  if ( ( 0 < is.stats30sec.Slope() ) && ( mid > is.dblMidQuoteAtOpen ) ) {
    // go long
    is.pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 1 );
    return transit<App::StateLong>();
  }
  else {
    if ( ( 0 > is.stats30sec.Slope() ) && ( mid < is.dblMidQuoteAtOpen ) ) {
      // go short
      is.pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 1 );
      return transit<App::StateShort>();
    }
  }

  return discard_event();
}

sc::result App::StateLong::Handle( const EvQuote& quote ) {
  InstrumentState& is( context<App::MachineMarketStates>().data );
  if ( is.bDaySession ) { // transit
    if ( quote.Quote().DateTime().time_of_day() >= is.tdCancelOrders ) {
      return transit<App::StateCancelOrders>();
    }
  }

  double mid = ( quote.Quote().Ask() + quote.Quote().Bid() ) / 2.0;

  if ( ( 0 > is.stats30sec.Slope() ) && ( mid < is.dblMidQuoteAtOpen ) ) {
    // go short
    is.pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 1 );
    is.pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 1 );
    return transit<App::StateShort>();
  }

  return discard_event();
}

sc::result App::StateShort::Handle( const EvQuote& quote ) {
  InstrumentState& is( context<App::MachineMarketStates>().data );
  if ( is.bDaySession ) { // transit
    if ( quote.Quote().DateTime().time_of_day() >= is.tdCancelOrders ) {
      return transit<App::StateCancelOrders>();
    }
  }

  double mid = ( quote.Quote().Ask() + quote.Quote().Bid() ) / 2.0;

  if ( ( 0 < is.stats30sec.Slope() ) && ( mid > is.dblMidQuoteAtOpen ) ) {
    // go long
    is.pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 1 );
    is.pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 1 );
    return transit<App::StateLong>();
  }

  return discard_event();
}

