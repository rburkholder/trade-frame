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

#include <TFHDF5TimeSeries/HDF5DataManager.h>
#include <TFHDF5TimeSeries/HDF5WriteTimeSeries.h>
#include <TFHDF5TimeSeries/HDF5IterateGroups.h>
#include <TFHDF5TimeSeries/HDF5Attribute.h>

#include "Option.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

Option::Option( pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider )
: m_pInstrument( pInstrument ), 
  m_pDataProvider( pDataProvider ), m_pGreekProvider( pGreekProvider ),
  m_dblBid( 0 ), m_dblAsk( 0 ), m_dblTrade( 0 ),
  m_dblStrike( pInstrument->GetStrike() ),
  m_bWatching( false ),
  m_sSide( "-" ),
  m_bMonitoring( false )
{
  Initialize();
}

Option::Option( const Option& rhs ) 
: m_dblBid( rhs.m_dblBid ), m_dblAsk( rhs.m_dblAsk ), m_dblTrade( rhs.m_dblTrade ),
  m_dblStrike( rhs.m_dblStrike ),
  m_greek( rhs.m_greek ),
  m_bWatching( false ),
  m_sSide( rhs.m_sSide ),
  m_pInstrument( rhs.m_pInstrument ),
  m_pDataProvider( rhs.m_pDataProvider ), m_pGreekProvider( rhs.m_pGreekProvider ),
  m_bMonitoring( false )
{
  assert( !rhs.m_bWatching );
  Initialize();
}

Option::~Option( void ) {
  StopMonitoring();
}

Option& Option::operator=( const Option& rhs ) {
  assert( !rhs.m_bWatching );
  assert( !m_bWatching );
  m_dblStrike = rhs.m_dblStrike;
  m_greek = rhs.m_greek;
  m_sSide = rhs.m_sSide;
  m_pInstrument = rhs.m_pInstrument;
  m_pDataProvider = rhs.m_pDataProvider;
  m_pGreekProvider = rhs.m_pGreekProvider;
  m_bMonitoring = false;
  Initialize();
  return *this;
}

void Option::Initialize( void ) {
  assert( m_pDataProvider->ProvidesQuotes() );
  assert( m_pDataProvider->ProvidesTrades() );
  assert( m_pGreekProvider->ProvidesGreeks() );
}

void Option::StartMonitoring( void ) {
  if ( m_bMonitoring ) {
  }
  else {
    m_bMonitoring = true;
    m_pDataProvider->AddQuoteHandler( m_pInstrument, MakeDelegate( this, &Option::HandleQuote ) );
    m_pDataProvider->AddTradeHandler( m_pInstrument, MakeDelegate( this, &Option::HandleTrade ) );
    m_pGreekProvider->AddGreekHandler( m_pInstrument, MakeDelegate( this, &Option::HandleGreek ) );
  }
}

void Option::StopMonitoring( void ) {
  if ( m_bMonitoring ) {
    m_pDataProvider->RemoveQuoteHandler( m_pInstrument, MakeDelegate( this, &Option::HandleQuote ) );
    m_pDataProvider->RemoveTradeHandler( m_pInstrument, MakeDelegate( this, &Option::HandleTrade ) );
    m_pGreekProvider->RemoveGreekHandler( m_pInstrument, MakeDelegate( this, &Option::HandleGreek ) );
    m_bMonitoring = false;
  }
  else {
  }
}

void Option::HandleQuote( const CQuote& quote ) {
  m_dblBid = quote.Bid();
  m_dblAsk = quote.Ask();
  m_quotes.Append( quote );
}

void Option::HandleTrade( const CTrade& trade ) {
  m_dblTrade = trade.Trade();
  m_trades.Append( trade );
}

void Option::HandleGreek( const CGreek& greek ) {
  m_greek = greek;
  m_greeks.Append( greek );
}

void Option::SaveSeries( const std::string& sPrefix ) {

  std::string sPathName;

  CHDF5Attributes::structOption option( 
    m_dblStrike, m_pInstrument->GetExpiryYear(), m_pInstrument->GetExpiryMonth(), m_pInstrument->GetExpiryDay(), m_pInstrument->GetOptionSide() );

  if ( 0 != m_quotes.Size() ) {
    sPathName = sPrefix + "/quotes/" + m_pInstrument->GetInstrumentName();
    CHDF5Attributes attrQuotes( sPathName, option );
    attrQuotes.SetMultiplier( m_pInstrument->GetMultiplier() );
    attrQuotes.SetSignificantDigits( m_pInstrument->GetSignificantDigits() ); 
    attrQuotes.SetProviderType( m_pDataProvider->ID() );
    CHDF5WriteTimeSeries<CQuotes, CQuote> wtsQuotes;
    wtsQuotes.Write( sPathName, &m_quotes );
  }

  if ( 0 != m_trades.Size() ) {
    sPathName = sPrefix + "/trades/" + m_pInstrument->GetInstrumentName();
    CHDF5Attributes attrTrades( sPathName, option );
    attrTrades.SetMultiplier( m_pInstrument->GetMultiplier() );
    attrTrades.SetSignificantDigits( m_pInstrument->GetSignificantDigits() );
    attrTrades.SetProviderType( m_pDataProvider->ID() );
    CHDF5WriteTimeSeries<CTrades, CTrade> wtsTrades;
    wtsTrades.Write( sPathName, &m_trades );
  }

  if ( 0 != m_greeks.Size() ) {
    sPathName = sPrefix + "/greeks/" + m_pInstrument->GetInstrumentName();
    CHDF5Attributes attrGreeks( sPathName, option );
    attrGreeks.SetMultiplier( m_pInstrument->GetMultiplier() );
    attrGreeks.SetSignificantDigits( m_pInstrument->GetSignificantDigits() );
    attrGreeks.SetProviderType( m_pGreekProvider->ID() );
    CHDF5WriteTimeSeries<CGreeks, CGreek> wtsGreeks;
    wtsGreeks.Write( sPathName, &m_greeks );
  }

}


//
// ==================
//

Call::Call( pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider )
: Option( pInstrument, pDataProvider,pGreekProvider )
{
  // assert instrument is a call
  assert( ou::tf::OptionSide::Call == pInstrument->GetOptionSide() );
  m_sSide = "C";
}

//
// ==================
//

Put::Put( pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider )
: Option( pInstrument, pDataProvider,pGreekProvider )
{
  // assert instrument is a put
  assert( ou::tf::OptionSide::Put == pInstrument->GetOptionSide() );
  m_sSide = "P";
}


//
// ==================
//

} // namespace option
} // namespace tf
} // namespace ou

