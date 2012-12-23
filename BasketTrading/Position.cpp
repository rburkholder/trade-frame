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
#include <TFHDF5TimeSeries/HDF5Attribute.h>

#include "Position.h"

Position::Position( const std::string& sName, const ou::tf::Bar& bar ) 
  : ou::tf::DailyTradeTimeFrame<Position>(),
  m_sName( sName ),
  m_bToBeTraded( false ), m_barInfo( bar ), 
  m_dblFundsToTrade( 0 )
{
}

Position::~Position(void) {
}

void Position::Start( void ) {

  assert( 0.0 != m_dblFundsToTrade );
  m_nSharesToTrade = CalcShareCount( m_dblFundsToTrade );

  assert( 0 != m_pPosition.get() );

  std::cout << "Position: starting for " << m_pPosition->GetInstrument()->GetInstrumentName() << std::endl;

}

 void Position::HandleQuote( const ou::tf::Quote& quote ) {
 }

 void Position::HandleTrade( const ou::tf::Trade& trade ) {
 }

 void Position::HandleOpen( const ou::tf::Trade& trade ) {
 }

void Position::SaveSeries( const std::string& sPrefix ) {

  std::string sPathName;

//  HDF5Attributes::structFuture future( m_pInstrument->GetExpiryYear(), m_pInstrument->GetExpiryMonth(), m_pInstrument->GetExpiryDay() );

  ou::tf::HDF5DataManager dm( ou::tf::HDF5DataManager::RDWR );

  if ( 0 != m_quotes.Size() ) {
    sPathName = sPrefix + "/quotes/" + m_pPosition->GetInstrument()->GetInstrumentName();
    ou::tf::HDF5WriteTimeSeries<ou::tf::Quotes> wtsQuotes( dm );
    wtsQuotes.Write( sPathName, &m_quotes );
    ou::tf::HDF5Attributes attrQuotes( dm, sPathName, ou::tf::InstrumentType::Stock );
    //HDF5Attributes attrQuotes( sPathName, future );
//    attrQuotes.SetProviderType( m_pPosition->->ID() );
  }

  if ( 0 != m_trades.Size() ) {
    sPathName = sPrefix + "/trades/" + m_pPosition->GetInstrument()->GetInstrumentName();
    ou::tf::HDF5WriteTimeSeries<ou::tf::Trades> wtsTrades( dm );
    wtsTrades.Write( sPathName, &m_trades );
    ou::tf::HDF5Attributes attrTrades( dm, sPathName, ou::tf::InstrumentType::Stock );
    //HDF5Attributes attrTrades( sPathName, future );
//    attrTrades.SetProviderType( m_pIQ->ID() );
  }
}

ou::tf::DatedDatum::volume_t Position::CalcShareCount( double dblFunds ) {
  return ( static_cast<ou::tf::DatedDatum::volume_t>( dblFunds / m_barInfo.Close() ) / 100 ) * 100;  // round down to nearest 100
}

