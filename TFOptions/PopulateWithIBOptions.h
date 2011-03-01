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

#pragma once

#include <string>
#include <sstream>
#include <stdexcept>

#include "boost/date_time/gregorian/gregorian.hpp"

#include <OUCommon/Delegate.h>

#include <OUSQL/Session.h>

#include <TFInteractiveBrokers/IBTWS.h>
#include <TFTrading/Instrument.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

  // handle mor than one request simultaneously?  if not, then set flag to capture

// returns number of options retrieved
template<class DB>
class PopulateOptions {
public:

  PopulateOptions( ou::db::CSession<DB>& session, CIBTWS& tws );
  ~PopulateOptions( void );

  typedef FastDelegate1<unsigned int> OnPopulateCompleteHandler_t;
  void SetOnPopulateCompleteHandler( OnPopulateCompleteHandler_t function )  {
    OnPopulateComplete = function;
  }

  Populate( const std::string& sUnderlying, boost::gregorian::date expiry, bool bCall, bool bPut );
protected:
private:

  Contract m_contract; // re-usable, persistant contract scratchpad
  ou::db::CSession<ou::db::ISqlite3> m_session;
  CIBTWS& m_tws;

  bool m_bActive;

  OnPopulateCompleteHandler_t OnPopulateComplete;

  void HandleOptionContractNotFound( void );
  void HandleOptionContractDetails( const ContractDetails& details );
  void HandleOptionContractDetailsDone( void );

  PopulateOptions( void );
};

template<class DB>
PopulateOptions::PopulateOptions( ou::db::CSession<DB>& session, CIBTWS& tws ) 
  : m_tws( tws ), m_session( session ), m_bActive( false )
{
  // assert( session active? );
  assert( tws.Connected() );

  m_contract.currency = "USD";
  m_contract.exchange = "SMART";
  m_contract.secType = "OPT";

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
void PopulateOptions::Populate( const std::string& sUnderlying, boost::gregorian::date expiry, bool bCall, bool bPut ) {
  if ( m_bActive ) {
    throw std::runtime_error( "already in process" );
  }
  m_bActive = true;
  unsigned int n( 0 );  // start with no options retrieved
  m_contract.symbol = sUnderlying;
  m_tws->RequestContractDetails( m_contract );
}

template<class DB>
void PopulateOptions::HandleOptionContractNotFound( void ) {
}

template<class DB>
void PopulateOptions::HandleOptionContractDetails( const ContractDetails& details ) {
}

template<class DB>
void PopulateOptions::HandleOptionContractDetailsDone( void ) {
}


} // namespace tf
} // namespace ou

