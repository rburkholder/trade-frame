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
  
Operation::Operation( const structSymbolInfo& si, ou::tf::CIQFeedProvider::pProvider_t piqfeed, ou::tf::IBTWS::pProvider_t ptws ) 
  : m_si( si ), m_piqfeed( piqfeed ), m_ptws( ptws )
{
}

Operation::~Operation(void) {
}

unsigned int Operation::CalcShareCount( double dblFunds ) {
  //InstrumentState& is( m_md.data );
  return ( static_cast<unsigned int>( dblFunds / m_si.dblClose ) / 100 ) * 100;  // round to nearest 100
}

void Operation::Start( double dblAmountToTrade ) {

  InstrumentState& is( m_md.data );

  is.vZeroMarks.push_back( m_si.R3 );
  is.vZeroMarks.push_back( m_si.R2 );
  is.vZeroMarks.push_back( m_si.R1 );
  is.vZeroMarks.push_back( m_si.PV );
  is.vZeroMarks.push_back( m_si.S1 );
  is.vZeroMarks.push_back( m_si.S2 );
  is.vZeroMarks.push_back( m_si.S3 );

  is.dblAmountToTrade = dblAmountToTrade;
  is.nSharesToTrade = CalcShareCount( dblAmountToTrade );

  ou::tf::IBTWS::Contract contract;
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

void Operation::HandleIBContractDetails( const ou::tf::IBTWS::ContractDetails& details, const pInstrument_t& pInstrument ) {
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

void Operation::HandleQuote( const ou::tf::Quote& quote ) {
  InstrumentState& is( m_md.data );
  if ( is.bMarketHoursCrossMidnight ) {
    is.bDaySession = quote.DateTime().time_of_day() <= is.tdMarketClosed;
  }
//  is.bDaySession = ( quote.DateTime().time_of_day() >= is.pPosition->GetInstrument()->GetTimeTrading().begin().time_of_day() )
//    && ( quote.DateTime().time_of_day() < is.pPosition->GetInstrument()->GetTimeTrading().last().time_of_day() );
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

void Operation::HandleTrade( const ou::tf::Trade& trade ) {
  InstrumentState& is( m_md.data );
//  if ( is.bMarketHoursCrossMidnight ) {
//    is.bDaySession = trade.DateTime().time_of_day() <= is.tdMarketClosed;
//  }
  assert( is.bDaySession || is.bMarketHoursCrossMidnight );
  is.trades.Append( trade );
  m_md.process_event( ou::tf::EvTrade( trade ) );
}

void Operation::HandleOpen( const ou::tf::Trade& trade ) {
}

void Operation::SaveSeries( const std::string& sPrefix ) {

  InstrumentState& is( m_md.data );

  std::string sPathName;

//  HDF5Attributes::structFuture future( m_pInstrument->GetExpiryYear(), m_pInstrument->GetExpiryMonth(), m_pInstrument->GetExpiryDay() );

  ou::tf::HDF5DataManager dm( ou::tf::HDF5DataManager::RDWR );

  if ( 0 != is.quotes.Size() ) {
    sPathName = sPrefix + "/quotes/" + m_pInstrument->GetInstrumentName();
    ou::tf::HDF5WriteTimeSeries<ou::tf::Quotes, ou::tf::Quote> wtsQuotes( dm );
    wtsQuotes.Write( sPathName, &is.quotes );
    ou::tf::HDF5Attributes attrQuotes( dm, sPathName, ou::tf::InstrumentType::Stock );
    //HDF5Attributes attrQuotes( sPathName, future );
    attrQuotes.SetProviderType( m_piqfeed->ID() );
  }

  if ( 0 != is.trades.Size() ) {
    sPathName = sPrefix + "/trades/" + m_pInstrument->GetInstrumentName();
    ou::tf::HDF5WriteTimeSeries<ou::tf::Trades, ou::tf::Trade> wtsTrades( dm );
    wtsTrades.Write( sPathName, &is.trades );
    ou::tf::HDF5Attributes attrTrades( dm, sPathName, ou::tf::InstrumentType::Stock );
    //HDF5Attributes attrTrades( sPathName, future );
    attrTrades.SetProviderType( m_piqfeed->ID() );
  }
}

