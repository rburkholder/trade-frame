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

#include "Operation.h"

//================= InstrumentState ========================

// following are specific for gold futures
/*
InstrumentState::InstrumentState( void ):
  tdMarketOpen( time_duration( 19, 0, 0 ) ), // time relative to day  // pick up from symbol
  tdMarketOpenIdle( time_duration( 0, 0, 30 ) ),  // time relative to tdMarketOpen to allow initial collection of data
  tdCancelOrders( time_duration( 17, 50, 0 ) ),// time relative to day
  tdClosePositions( time_duration( 17, 51, 0 ) ),// time relative to day
  tdAfterMarket( time_duration( 18, 15, 0 ) ), // time relative to day
  tdMarketClosed( time_duration( 18, 15, 0 ) ), // time relative to day
  stochFast( &quotes, 60 ), stochMed( &quotes, 300 ), stochSlow( &quotes, 1800 ), // 1, 5, 30 min
  statsFast( &quotes, 60 ), statsMed( &quotes, 180 ), statsSlow( &quotes, 600 ), // 1, 3, 5 min
  bDaySession( true )
  {
    bMarketHoursCrossMidnight = tdMarketOpen > tdMarketClosed;
  }
  */
// following are specific for standard equities
InstrumentState::InstrumentState( void ):
  tdMarketOpen( time_duration( 10, 30, 0 ) ), // time relative to day  // pick up from symbol
  tdMarketOpenIdle( time_duration( 0, 0, 30 ) ),  // time relative to tdMarketOpen to allow initial collection of data
  tdCancelOrders( time_duration( 16, 50, 0 ) ),// time relative to day
  tdClosePositions( time_duration( 16, 51, 0 ) ),// time relative to day
  tdAfterMarket( time_duration( 17, 0, 0 ) ), // time relative to day
  tdMarketClosed( time_duration( 18, 0, 0 ) ), // time relative to day
  stochFast( &quotes, 60 ), stochMed( &quotes, 300 ), stochSlow( &quotes, 1800 ), // 1, 5, 30 min
  statsFast( &quotes, 60 ), statsMed( &quotes, 180 ), statsSlow( &quotes, 600 ), // 1, 3, 5 min
  bDaySession( true )
  {
    bMarketHoursCrossMidnight = tdMarketOpen > tdMarketClosed;
  }

//================= Operation ========================
  
  Operation::Operation( const structSymbolInfo& si, ou::tf::CIQFeedProvider::pProvider_t piqfeed, ou::tf::CIBTWS::pProvider_t ptws ) 
  : m_si( si ), m_piqfeed( piqfeed ), m_ptws( ptws )
{
}

Operation::~Operation(void) {
}

void Operation::Start( void ) {

  InstrumentState& is( m_md.data );
  is.vZeroMarks.push_back( m_si.R3 );
  is.vZeroMarks.push_back( m_si.R2 );
  is.vZeroMarks.push_back( m_si.R1 );
  is.vZeroMarks.push_back( m_si.PV );
  is.vZeroMarks.push_back( m_si.S1 );
  is.vZeroMarks.push_back( m_si.S2 );
  is.vZeroMarks.push_back( m_si.S3 );

  ou::tf::CIBTWS::Contract contract;
  contract.currency = "USD";
  //contract.exchange = "SMART";  in this case is NYMEX
  //contract.secType = "FUT";
  contract.secType = "STK";
  //contract.symbol = "GC";
  contract.symbol = m_si.sName;
  //contract.expiry = "201112";
  // IB responds only when symbol is found, bad symbols will not illicit a response
  m_ptws->RequestContractDetails( contract, MakeDelegate( this, &Operation::HandleIBContractDetails ), MakeDelegate( this, &Operation::HandleIBContractDetailsDone ) );

  m_md.initiate();  // start state chart for market data
  m_md.process_event( ou::tf::EvInitialize() );

}

void Operation::HandleIBContractDetails( const ou::tf::CIBTWS::ContractDetails& details, const pInstrument_t& pInstrument ) {
  m_pInstrument = pInstrument;
  //m_pInstrument->SetAlternateName( m_piqfeed->ID(), "+GCZ11" );
  m_md.data.pPosition.reset( new ou::tf::CPosition( m_pInstrument, m_ptws, m_piqfeed ) );
  m_md.data.tdMarketOpen = m_pInstrument->GetTimeTrading().begin().time_of_day();
  m_md.data.tdMarketClosed = m_pInstrument->GetTimeTrading().end().time_of_day();
}

void Operation::HandleIBContractDetailsDone( void ) {
  //this->Connect();
}

void Operation::StartWatch( void ) {
  m_piqfeed->AddQuoteHandler( m_pInstrument, MakeDelegate( this, &Operation::HandleQuote ) );
  m_piqfeed->AddTradeHandler( m_pInstrument, MakeDelegate( this, &Operation::HandleTrade ) );
  m_piqfeed->AddOnOpenHandler( m_pInstrument, MakeDelegate( this, &Operation::HandleOpen ) );
}

void Operation::StopWatch( void ) {
  m_piqfeed->RemoveQuoteHandler( m_pInstrument, MakeDelegate( this, &Operation::HandleQuote ) );
  m_piqfeed->RemoveTradeHandler( m_pInstrument, MakeDelegate( this, &Operation::HandleTrade ) );
  m_piqfeed->RemoveOnOpenHandler( m_pInstrument, MakeDelegate( this, &Operation::HandleOpen ) );
}

void Operation::HandleQuote( const ou::tf::CQuote& quote ) {
  InstrumentState& is( m_md.data );
  if ( is.bMarketHoursCrossMidnight ) {
    is.bDaySession = quote.DateTime().time_of_day() <= is.tdMarketClosed;
  }
  assert( is.bDaySession || is.bMarketHoursCrossMidnight );
  is.quotes.Append( quote );
  is.stochFast.Update();
  is.stochMed.Update();
  is.stochSlow.Update();
  is.statsFast.Update();
  m_md.process_event( ou::tf::EvQuote( quote ) );
}

void Operation::HandleTrade( const ou::tf::CTrade& trade ) {
  InstrumentState& is( m_md.data );
  if ( is.bMarketHoursCrossMidnight ) {
    is.bDaySession = trade.DateTime().time_of_day() <= is.tdMarketClosed;
  }
  assert( is.bDaySession || is.bMarketHoursCrossMidnight );
  is.trades.Append( trade );
  m_md.process_event( ou::tf::EvTrade( trade ) );
}

void Operation::HandleOpen( const ou::tf::CTrade& trade ) {
}

sc::result Operation::StatePreMarket::Handle( const EvQuote& quote ) {  // requires quotes to come before trades.
  InstrumentState& is( context<Operation::MachineMarketStates>().data );
  if ( is.bMarketHoursCrossMidnight && is.bDaySession ) { // transit
    is.dtPreTradingStop = quote.Quote().DateTime() + is.tdMarketOpenIdle;
    is.dblMidQuoteAtOpen = ( quote.Quote().Ask() + quote.Quote().Bid() ) / 2.0;
    return transit<Operation::StateMarketOpen>();  // late but transit anyway
  }
  else { // test
    if ( quote.Quote().DateTime().time_of_day() >= is.tdMarketOpen ) {
      is.dtPreTradingStop = quote.Quote().DateTime() + is.tdMarketOpenIdle;
      is.dblMidQuoteAtOpen = ( quote.Quote().Ask() + quote.Quote().Bid() ) / 2.0;
      return transit<Operation::StateMarketOpen>();
    }
  }
  return discard_event();
}

sc::result Operation::StatePreMarket::Handle( const EvTrade& trade ) {
  InstrumentState& is( context<Operation::MachineMarketStates>().data ); 
  if ( is.bMarketHoursCrossMidnight && is.bDaySession ) { // transit
    //return transit<App::StateMarketOpen>();  // late but transit anyway
    return discard_event();  // see if we get a pre-market trade for GC futures, possibly when starting mid market
  }
  else { // test
    if ( trade.Trade().DateTime().time_of_day() >= is.tdMarketOpen ) {
      return discard_event();  // see if we get a pre-market trade for GC futures, possibly when starting mid market
    }
  }
  return discard_event();
}

sc::result Operation::StateMarketOpen::Handle( const EvTrade& trade ) {
  InstrumentState& is( context<Operation::MachineMarketStates>().data );
  is.dblOpeningTrade = trade.Trade().Trade();
  is.vZeroMarks.push_back( is.dblOpeningTrade );
  std::sort( is.vZeroMarks.begin(), is.vZeroMarks.end() );
  is.iterZeroMark = is.vZeroMarks.begin();
  while ( is.dblOpeningTrade != *is.iterZeroMark ) {
    is.iterZeroMark++;
    if ( is.vZeroMarks.end() == is.iterZeroMark ) 
      throw std::runtime_error( "can't find our zero mark" );
  }
  is.iterNextMark = is.iterZeroMark;
  //return transit<App::StatePreTrading>();
  return transit<Operation::StateTrading>();
}

sc::result Operation::StatePreTrading::Handle( const EvQuote& quote ) {
  InstrumentState& is( context<Operation::MachineMarketStates>().data );

  if ( quote.Quote().DateTime() >= is.dtPreTradingStop ) {
    return transit<Operation::StateTrading>();
  }

  return discard_event();
}

sc::result Operation::StateCancelOrders::Handle( const EvQuote& quote ) {
  InstrumentState& is( context<Operation::MachineMarketStates>().data );
  return transit<Operation::StateCancelOrdersIdle>();
}

sc::result Operation::StateCancelOrdersIdle::Handle( const EvQuote& quote ) {
  InstrumentState& is( context<Operation::MachineMarketStates>().data );

  if ( is.bDaySession ) { // transit
    if ( quote.Quote().DateTime().time_of_day() >= is.tdClosePositions ) {
      return transit<Operation::StateClosePositions>();
    }
  }
  return discard_event();
}

sc::result Operation::StateClosePositions::Handle( const EvQuote& quote ) {
  InstrumentState& is( context<Operation::MachineMarketStates>().data );
  return transit<Operation::StateClosePositionsIdle>();
}

sc::result Operation::StateClosePositionsIdle::Handle( const EvQuote& quote ) {
  InstrumentState& is( context<Operation::MachineMarketStates>().data );

  if ( is.bDaySession ) { // transit
    if ( quote.Quote().DateTime().time_of_day() >= is.tdAfterMarket ) {
      return transit<Operation::StateAfterMarket>();
    }
  }
  return discard_event();
}

sc::result Operation::StateAfterMarket::Handle( const EvQuote& quote ) {
  InstrumentState& is( context<Operation::MachineMarketStates>().data );

  if ( is.bDaySession ) { // transit
    if ( quote.Quote().DateTime().time_of_day() >= is.tdMarketClosed ) {
      return transit<Operation::StateMarketClosed>();
    }
  }
  return discard_event();
}

sc::result Operation::StateMarketClosed::Handle( const EvQuote& quote ) {
  InstrumentState& is( context<Operation::MachineMarketStates>().data );
  return discard_event();
}

sc::result Operation::StateZeroPosition::Handle( const EvQuote& quote ) {

  InstrumentState& is( context<Operation::MachineMarketStates>().data );
  if ( is.bDaySession ) { // transit
    if ( quote.Quote().DateTime().time_of_day() >= is.tdCancelOrders ) {
      return transit<Operation::StateCancelOrders>();
    }
  }

  //double mid = ( quote.Quote().Ask() + quote.Quote().Bid() ) / 2.0;
  //if ( ( 0 < is.statsFast.Slope() ) && ( mid > is.dblOpeningTrade ) ) {
  if ( quote.Quote().Bid() > *is.iterZeroMark ) {
    if ( is.vZeroMarks.end() != is.iterZeroMark ) is.iterNextMark++;
    std::cout << "Zero Position going long" << std::endl;
    // go long
    is.pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 1 );
    return transit<Operation::StateLong>();
  }
  else {
    //if ( ( 0 > is.statsFast.Slope() ) && ( mid < is.dblOpeningTrade ) ) {
    if ( quote.Quote().Ask() < *is.iterZeroMark ) {
      if ( is.vZeroMarks.begin() != is.iterZeroMark ) is.iterNextMark--;
      std::cout << "Zero Position going short" << std::endl;
      // go short
      is.pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 1 );
      return transit<Operation::StateShort>();
    }
  }

  return discard_event();
}

sc::result Operation::StateLong::Handle( const EvQuote& quote ) {
  InstrumentState& is( context<Operation::MachineMarketStates>().data );
  if ( is.bDaySession ) { // transit
    if ( quote.Quote().DateTime().time_of_day() >= is.tdCancelOrders ) {
      return transit<Operation::StateCancelOrders>();
    }
  }

  if ( is.pPosition->OrdersPending() ) {
    return discard_event();
  }

  //double mid = ( quote.Quote().Ask() + quote.Quote().Bid() ) / 2.0;

  //if ( ( 0 > is.statsFast.Slope() ) && ( mid < is.dblOpeningTrade ) ) {
  if ( quote.Quote().Ask() < *is.iterZeroMark ) {
    is.iterNextMark = is.iterZeroMark;
    if ( is.vZeroMarks.begin() != is.iterZeroMark ) is.iterNextMark--;
    std::cout << "long going short" << std::endl;
    // go short
    is.pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 1 );
    is.pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 1 );
    return transit<Operation::StateShort>();
  }
  else {
    if ( quote.Quote().Ask() >= *is.iterNextMark ) {
      if ( is.iterZeroMark != is.iterNextMark ) {
        is.iterZeroMark = is.iterNextMark;
        std::cout << "long crossing next zero: " << *is.iterZeroMark << std::endl;
        if ( is.vZeroMarks.end() != is.iterZeroMark ) is.iterNextMark++;
      }
    }
  }

  return discard_event();
}

sc::result Operation::StateShort::Handle( const EvQuote& quote ) {
  InstrumentState& is( context<Operation::MachineMarketStates>().data );
  if ( is.bDaySession ) { // transit
    if ( quote.Quote().DateTime().time_of_day() >= is.tdCancelOrders ) {
      return transit<Operation::StateCancelOrders>();
    }
  }

  if ( is.pPosition->OrdersPending() ) {
    return discard_event();
  }

  //double mid = ( quote.Quote().Ask() + quote.Quote().Bid() ) / 2.0;

  //if ( ( 0 < is.statsFast.Slope() ) && ( mid > is.dblOpeningTrade ) ) {
  if ( quote.Quote().Bid() > *is.iterZeroMark ) {
    is.iterNextMark = is.iterZeroMark;
    if ( is.vZeroMarks.end() != is.iterZeroMark ) is.iterNextMark++;
    std::cout << "short going long" << std::endl;
    // go long
    is.pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 1 );
    is.pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 1 );
    return transit<Operation::StateLong>();
  }
  else {
    if ( quote.Quote().Bid() <= *is.iterNextMark ) {
      if ( is.iterZeroMark != is.iterNextMark ) {
        is.iterZeroMark = is.iterNextMark;
        std::cout << "short crossing next zero: " << *is.iterZeroMark << std::endl;
        if ( is.vZeroMarks.begin() != is.iterZeroMark ) is.iterNextMark --;
      }
    }
  }

  return discard_event();
}

