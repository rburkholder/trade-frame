/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

// started 2013/07/14  de-templating the original class

#include "PopulateWithIBOptions.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option {  // Option

PopulateOptions::PopulateOptions( ou::tf::DBOps& session, pProvider_t pProvider )
  : m_pProvider( pProvider ), m_session( session ), m_bActive( false ), m_cntInstruments( 0 )
{
  // assert( session active? );
  assert( pProvider->Connected() );

  m_contract.currency = "USD";
  m_contract.exchange = "SMART";
  m_contract.secType = "OPT";

  m_pProvider->SetOnSecurityDefinitionNotFoundHandler( MakeDelegate( this, &PopulateOptions::HandleOptionContractNotFound ) );

}

PopulateOptions::~PopulateOptions( void ) {
  m_pProvider->SetOnSecurityDefinitionNotFoundHandler( NULL );
}

void PopulateOptions::Populate( const std::string& sUnderlying, boost::gregorian::date dateExpiry, bool bCall, bool bPut ) {

  if ( m_bActive ) {
    throw std::runtime_error( "already in process" );
  }
  if ( !bCall && !bPut ) {
    throw std::runtime_error( "neither call nor put set" );
  }
  m_bActive = true;
  unsigned int n( 0 );  // start with no options retrieved
  m_contract.symbol = sUnderlying;
  m_contract.right = "";
  if ( bCall && bPut ) {
    // leave .right empty to get both
  }
  else {
    if ( bCall ) m_contract.right = "CALL";
    if ( bPut  ) m_contract.right = "PUT";
  }
  m_contract.lastTradeDateOrContractMonth = boost::gregorian::to_iso_string( dateExpiry );

  // delete any pre-existing first?
  m_pProvider->RequestContractDetails(
    m_contract,
    MakeDelegate( this, &PopulateOptions::HandleOptionContractDetails ),
    MakeDelegate( this, &PopulateOptions::HandleOptionContractDetailsDone )
  );
}

void PopulateOptions::HandleOptionContractDetails( const ContractDetails& details, pInstrument_t& pInstrument ) {
//  pInstrument_t pInstrument = m_pProvider->BuildInstrumentFromContract( details.summary );
//  ou::db::QueryFields<Instrument::TableRowDef>::pQueryFields_t pInsert
//    = m_session.Insert<Instrument::TableRowDef>( pInstrument->GetRow() );
  ++m_cntInstruments;
  if ( 0 != OnInstrumentBuilt ) OnInstrumentBuilt( pInstrument );
}

void PopulateOptions::HandleOptionContractDetailsDone( void ) {
  if ( 0 != OnPopulateComplete ) OnPopulateComplete( m_cntInstruments );
  m_bActive = false;
}

void PopulateOptions::HandleOptionContractNotFound( void ) {
  if ( 0 != OnPopulateComplete ) OnPopulateComplete( m_cntInstruments );
  m_bActive = false;
}

} // namespace option
} // namespace tf
} // namespace ou
