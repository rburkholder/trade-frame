/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

#include "StdAfx.h"

#include <TFTimeSeries/TimeSeries.h>

#include <TFHDF5TimeSeries/HDF5DataManager.h>
#include <TFHDF5TimeSeries/HDF5WriteTimeSeries.h>
//#include <TFHDF5TimeSeries/HDF5IterateGroups.h>
#include <TFHDF5TimeSeries/HDF5Attribute.h>

#include "Position.h"

Position::Position( const std::string& sName, const ou::tf::Bar& bar ) 
  : ou::tf::DailyTradeTimeFrame<Position>(),
  m_sName( sName ), 
  m_bToBeTraded( false ), m_barInfo( bar )
{
}

Position::~Position(void) {
}

void Position::Start( double dblFundsToTrade ) {

  m_dblFundsToTrade = dblFundsToTrade;
  m_nSharesToTrade = CalcShareCount( dblFundsToTrade );

  ou::tf::IBTWS::Contract contract;
  contract.currency = "USD";
  contract.exchange = "SMART";  
  //contract.secType = "FUT";
  contract.secType = "STK";
  //contract.symbol = "GC";
  contract.symbol = m_sName;
  //contract.expiry = "201112";
  // IB responds only when symbol is found, bad symbols will not illicit a response
  m_pIB->RequestContractDetails( contract, MakeDelegate( this, &Position::HandleIBContractDetails ), MakeDelegate( this, &Position::HandleIBContractDetailsDone ) );

}

 void Position::HandleQuote( const ou::tf::Quote& quote ) {
 }

 void Position::HandleTrade( const ou::tf::Trade& trade ) {
 }

 void Position::HandleOpen( const ou::tf::Trade& trade ) {
 }

void Position::HandleIBContractDetails( const ou::tf::IBTWS::ContractDetails& details, pInstrument_t& pInstrument ) {
  m_pInstrument = pInstrument;
  //m_pInstrument->SetAlternateName( m_piqfeed->ID(), "+GCZ11" );
//  m_md.data.pPosition.reset( n1ew ou::tf::CPosition( m_pInstrument, m_ptws, m_piqfeed ) );
//  m_md.data.tdMarketOpen = m_pInstrument->GetTimeTrading().begin().time_of_day();
//  m_md.data.tdMarketClosed = m_pInstrument->GetTimeTrading().end().time_of_day();
}

void Position::HandleIBContractDetailsDone( void ) {
//  StartWatch();
}

void Position::SaveSeries( const std::string& sPrefix ) {

  std::string sPathName;

//  HDF5Attributes::structFuture future( m_pInstrument->GetExpiryYear(), m_pInstrument->GetExpiryMonth(), m_pInstrument->GetExpiryDay() );

  ou::tf::HDF5DataManager dm( ou::tf::HDF5DataManager::RDWR );

  if ( 0 != m_quotes.Size() ) {
    sPathName = sPrefix + "/quotes/" + m_pInstrument->GetInstrumentName();
    ou::tf::HDF5WriteTimeSeries<ou::tf::Quotes> wtsQuotes( dm );
    wtsQuotes.Write( sPathName, &m_quotes );
    ou::tf::HDF5Attributes attrQuotes( dm, sPathName, ou::tf::InstrumentType::Stock );
    //HDF5Attributes attrQuotes( sPathName, future );
    attrQuotes.SetProviderType( m_pIQ->ID() );
  }

  if ( 0 != m_trades.Size() ) {
    sPathName = sPrefix + "/trades/" + m_pInstrument->GetInstrumentName();
    ou::tf::HDF5WriteTimeSeries<ou::tf::Trades> wtsTrades( dm );
    wtsTrades.Write( sPathName, &m_trades );
    ou::tf::HDF5Attributes attrTrades( dm, sPathName, ou::tf::InstrumentType::Stock );
    //HDF5Attributes attrTrades( sPathName, future );
    attrTrades.SetProviderType( m_pIQ->ID() );
  }
}

ou::tf::DatedDatum::volume_t Position::CalcShareCount( double dblFunds ) {
  return ( static_cast<ou::tf::DatedDatum::volume_t>( dblFunds / m_barInfo.Close() ) / 100 ) * 100;  // round to nearest 100
}

