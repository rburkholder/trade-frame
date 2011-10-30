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

#include <sstream>

#include <TFHDF5TimeSeries/HDF5DataManager.h>
#include <TFHDF5TimeSeries/HDF5WriteTimeSeries.h>
#include <TFHDF5TimeSeries/HDF5IterateGroups.h>
#include <TFHDF5TimeSeries/HDF5Attribute.h>

#include "Operation.h"

//================= Operation ========================
  
Operation::Operation( const structSymbolInfo& si, ou::tf::CIQFeedProvider::pProvider_t piqfeed, ou::tf::CIBTWS::pProvider_t ptws ) 
  : m_si( si ), m_piqfeed( piqfeed ), m_ptws( ptws )
{
}

Operation::~Operation(void) {
}

unsigned int Operation::CalcShareCount( double dblFunds ) {
  InstrumentState& is( m_md.data );
  return ( static_cast<unsigned int>( dblFunds / is.dblClose ) / 100 ) * 100;  // round to nearest 100
}

void Operation::Start( double dblAmountToTrade ) {

  InstrumentState& is( m_md.data );

  is.vZeroMarks.push_back( m_si.R3 );
  is.vZeroMarks.push_back( m_si.R2 );
  is.vZeroMarks.push_back( m_si.R1 );
//  is.vZeroMarks.push_back( m_si.PV );  // should we or should we not include this as a trading level?
  is.vZeroMarks.push_back( m_si.S1 );
  is.vZeroMarks.push_back( m_si.S2 );
  is.vZeroMarks.push_back( m_si.S3 );

  is.dblAmountToTrade = dblAmountToTrade;
  is.nSharesToTrade = CalcShareCount( dblAmountToTrade );

  ou::tf::CIBTWS::Contract contract;
  contract.currency = "USD";
  contract.exchange = "SMART";  
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

void Operation::Stop( void ) {
  // may want to close positions, save values, etc.
  StopWatch();
}

void Operation::HandleIBContractDetails( const ou::tf::CIBTWS::ContractDetails& details, const pInstrument_t& pInstrument ) {
  m_pInstrument = pInstrument;
  //m_pInstrument->SetAlternateName( m_piqfeed->ID(), "+GCZ11" );
  m_md.data.pPosition.reset( new ou::tf::CPosition( m_pInstrument, m_ptws, m_piqfeed ) );
  m_md.data.tdMarketOpen = m_pInstrument->GetTimeTrading().begin().time_of_day();
  m_md.data.tdMarketClosed = m_pInstrument->GetTimeTrading().end().time_of_day();
}

void Operation::HandleIBContractDetailsDone( void ) {
  StartWatch();
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
  is.stochSlow.Update();
  is.stochMed.Update();
  is.stochFast.Update();
  //is.statsFast.Update();
  is.statsMed.Update();
  //is.statsSlow.Update();
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
  is.iterNextMark = is.iterZeroMark;  // set next same as zero as don't know which direction next will be
  //return transit<App::StatePreTrading>();
  return transit<Operation::StateTrading>();
}

sc::result Operation::StatePreTrading::Handle( const EvQuote& quote ) {  // not currently used

  InstrumentState& is( context<Operation::MachineMarketStates>().data );

  if ( quote.Quote().DateTime() >= is.dtPreTradingStop ) {
    return transit<Operation::StateTrading>();
  }

  return discard_event();
}

sc::result Operation::StateCancelOrders::Handle( const EvQuote& quote ) {
  InstrumentState& is( context<Operation::MachineMarketStates>().data );
  is.pPosition->CancelOrders();
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
  is.pPosition->ClosePosition();
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
    is.pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, is.nSharesToTrade );
    return transit<Operation::StateLong>();
  }
  else {
    //if ( ( 0 > is.statsFast.Slope() ) && ( mid < is.dblOpeningTrade ) ) {
    if ( quote.Quote().Ask() < *is.iterZeroMark ) {
      if ( is.vZeroMarks.begin() != is.iterZeroMark ) is.iterNextMark--;
      std::cout << "Zero Position going short" << std::endl;
      // go short
      is.pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, is.nSharesToTrade );
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
    is.pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, is.nSharesToTrade );
    is.pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, is.nSharesToTrade );
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
    is.pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, is.nSharesToTrade );
    is.pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, is.nSharesToTrade );
    return transit<Operation::StateLong>();
  }
  else {
    if ( quote.Quote().Bid() <= *is.iterNextMark ) {
      if ( is.iterZeroMark != is.iterNextMark ) {
        is.iterZeroMark = is.iterNextMark;
        std::cout << "short crossing next zero: " << *is.iterZeroMark << std::endl;
        if ( is.vZeroMarks.begin() != is.iterZeroMark ) is.iterNextMark--;
      }
    }
  }

  return discard_event();
}

void Operation::SaveSeries( const std::string& sPrefix ) {

  InstrumentState& is( m_md.data );

  std::string sPathName;

//  CHDF5Attributes::structFuture future( m_pInstrument->GetExpiryYear(), m_pInstrument->GetExpiryMonth(), m_pInstrument->GetExpiryDay() );

  if ( 0 != is.quotes.Size() ) {
    sPathName = sPrefix + "/quotes/" + m_pInstrument->GetInstrumentName();
    ou::tf::CHDF5WriteTimeSeries<ou::tf::CQuotes, ou::tf::CQuote> wtsQuotes;
    wtsQuotes.Write( sPathName, &is.quotes );
    ou::tf::CHDF5Attributes attrQuotes( sPathName, ou::tf::InstrumentType::Stock );
    //CHDF5Attributes attrQuotes( sPathName, future );
    attrQuotes.SetProviderType( m_piqfeed->ID() );
  }

  if ( 0 != is.trades.Size() ) {
    sPathName = sPrefix + "/trades/" + m_pInstrument->GetInstrumentName();
    ou::tf::CHDF5WriteTimeSeries<ou::tf::CTrades, ou::tf::CTrade> wtsTrades;
    wtsTrades.Write( sPathName, &is.trades );
    ou::tf::CHDF5Attributes attrTrades( sPathName, ou::tf::InstrumentType::Stock );
    //CHDF5Attributes attrTrades( sPathName, future );
    attrTrades.SetProviderType( m_piqfeed->ID() );
  }
}

