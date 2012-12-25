/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#pragma once
 
#include <string>
#include <sstream>
#include <stdexcept>

#include <boost/date_time/gregorian/gregorian.hpp>

#include <OUCommon/Delegate.h>

#include <TFInteractiveBrokers/IBTWS.h>
#include <TFTrading/Instrument.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

  // handle mor than one request simultaneously?  if not, then set flag to capture

// returns number of options retrieved
template<class DB>
class PopulateOptions {
public:

  PopulateOptions( ou::db::Session<DB>& session, IBTWS& tws );
  ~PopulateOptions( void );

  typedef FastDelegate1<unsigned int> OnPopulateCompleteHandler_t;
  void SetOnPopulateCompleteHandler( OnPopulateCompleteHandler_t function )  {
    OnPopulateComplete = function;
  }

  Populate( const std::string& sUnderlying, boost::gregorian::date expiry, bool bCall, bool bPut );
protected:
private:

  Contract m_contract; // re-usable, persistant contract scratchpad
  ou::db::Session<ou::db::ISqlite3> m_session;
  IBTWS& m_tws;

  bool m_bActive;

  unsigned int m_cntInstruments;

  Instrument::TableRowDef m_rowInstrument;

  OnPopulateCompleteHandler_t OnPopulateComplete;

  void HandleOptionContractNotFound( void );
  void HandleOptionContractDetails( const ContractDetails& details );
  void HandleOptionContractDetailsDone( void );

  PopulateOptions( void );
};

template<class DB>
PopulateOptions::PopulateOptions( ou::db::Session<DB>& session, IBTWS& tws ) 
  : m_tws( tws ), m_session( session ), m_bActive( false ), m_cntInstruments( 0 )
{
  // assert( session active? );
  assert( tws.Connected() );

  m_contract.currency = "USD";
  m_contract.exchange = "SMART";
  m_contract.secType = "OPT";

  m_rowInstrument.idExchange = "SMART";

  m_tws->SetOnContractDetailsHandler( MakeDelegate( this, &PopulateOptions::HandleUnderlyingContractDetails ) );
  m_tws->SetOnContractDetailsDoneHandler( MakeDelegate( this, &PopulateOptions::HandleUnderlyingContractDetailsDone ) );
  m_tws->SetOnSecurityDefinitionNotFoundHandler( MakeDelegate( this, &PopulateOptions::HandleUnderlyingContractNotFound ) );

}

template<class DB>
PopulateOptions::~PopulateOptions( void ) {
  m_tws->SetOnContractDetailsHandler( NULL );
  m_tws->SetOnContractDetailsDoneHandler( NULL );
  m_tws->SetOnSecurityDefinitionNotFoundHandler( NULL );
}
  
template<class DB>
void PopulateOptions::Populate( const std::string& sUnderlying, boost::gregorian::date dExpiry, bool bCall, bool bPut ) {

  if ( m_bActive ) {
    throw std::runtime_error( "already in process" );
  }
  if ( !bCall and !bPut ) {
    throw std::runtime_error( "neither call nor put set" );
  }
  m_bActive = true;
  unsigned int n( 0 );  // start with no options retrieved
  m_contract.symbol = sUnderlying;
  m_contract.right = "";
  if ( bCall and bPut ) {
    // leave .right empty to get both
  }
  else {
    if ( bCall ) m_contract.right = "CALL";
    if ( bPut  ) m_contract.right = "PUT";
  }

  m_rowInstrument.eType = InstrumentType::Option;  // need to handle futuresoption as well?
  m_rowInstrument.idUnderlying = sUnderlying;
  m_rowInstrument.nYear = dExpiry.year();
  m_rowInstrument.nMonth = dExpiry.month();
  m_rowInstrument.nDay = dExpiry.day();

  // delete any pre-existing first?
  m_tws->RequestContractDetails( m_contract );
}

template<class DB>
void PopulateOptions::HandleOptionContractNotFound( void ) {
  if ( 0 != OnPopulateComplete ) OnPopulateComplete( m_cntInstruments );
}

template<class DB>
void PopulateOptions::HandleOptionContractDetails( const ContractDetails& details ) {
  IBTWS::pInstrument_t pInstrument = m_tws->BuildInstrumentFromContract( details.summary );
  ou::db::QueryFields<Instrument::TableRowDef>::pQueryFields_t pInsert 
    = session.Insert<Instrument::TableRowDef>( pInstrument->GetRow() );
  ++m_cntInstruments;
}

template<class DB>
void PopulateOptions::HandleOptionContractDetailsDone( void ) {
  if ( 0 != OnPopulateComplete ) OnPopulateComplete( m_cntInstruments );
}


} // namespace tf
} // namespace ou

