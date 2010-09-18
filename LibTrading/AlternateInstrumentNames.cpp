/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#include <LibTrading/TradingEnumerations.h>

#include "AlternateInstrumentNames.h"

CAlternateInstrumentNames::CAlternateInstrumentNames() 
  : CCommonDatabaseFunctions<CAlternateInstrumentNames>( Trading::DbFileName, "AlternateInstrumentNames" ) {
}

CAlternateInstrumentNames::~CAlternateInstrumentNames(void) {
}

void CAlternateInstrumentNames::Save(const std::string &ProviderName, const std::string &InstrumentName, const std::string &AlternateInstrumentName) {
  structKey key( ProviderName, InstrumentName );
  Dbt k( (void*) &key, sizeof( structKey ) - nMaxKeySize + key.nKeyLength );
  structValue value( AlternateInstrumentName );
  Dbt v( (void*) &value, sizeof( structValue ) - nMaxSymbolNameSize + value.nValueLength );
  int ret = m_pdb->put( 0, &k, &v, 0 );
  if ( 0 != ret ) throw std::runtime_error( "CAlternateInstrumentNames::Save put had error" );
}

void CAlternateInstrumentNames::Get(const std::string &ProviderName, const std::string &InstrumentName, std::string *pAlternate) {
  structKey key( ProviderName, InstrumentName );
  Dbt k( (void*) &key, sizeof( structKey ) - nMaxKeySize + key.nKeyLength );
  Dbt v;
  structValue val;
  v.set_flags( DB_DBT_USERMEM );
  v.set_ulen( sizeof( structValue ) );
  v.set_size( sizeof( structValue ) );
  v.set_data( &val );  
  int ret = m_pdb->get( 0, &k, &v, 0 );
  if ( DB_NOTFOUND == ret ) throw std::out_of_range( "CAlternateInstrumentNames::Get key not found" );
  if ( 0 != ret ) throw std::runtime_error( "CAlternateInstrumentNames::Get get had error" );
  pAlternate->assign( val.Value, val.nValueLength );
}
