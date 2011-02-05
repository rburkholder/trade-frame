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

#include <cassert>
#include <stdexcept>

#include "Instrument.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

const std::string CInstrument::m_sTableName = "instruments";

/*
  if ( NULL != m_pUnderlying.get() ) {
    rtn += sqlite3_bind_text(  
      pStmt, sqlite3_bind_parameter_index( pStmt, ":underlying" ), m_pUnderlying->GetInstrumentName().c_str(), -1, SQLITE_STATIC );
  }
  else {
    rtn += sqlite3_bind_text(   
      pStmt, sqlite3_bind_parameter_index( pStmt, ":underlying" ), "", -1, SQLITE_STATIC );
  }
*/

CInstrument::CInstrument( const TableRowDef& row ) 
  : m_row( row ) 
{
  // other clean up and pointer stuff still yet to be done here
}

// equity / generic creation
CInstrument::CInstrument(
  idInstrument_cref idInstrument, InstrumentType::enumInstrumentTypes eType,
  const idExchange_t &idExchange
                         )
: m_row( idInstrument, eType, idExchange ),
  m_eUnderlyingStatus( EUnderlyingNotSettable )
{
}

 // future
CInstrument::CInstrument( 
  idInstrument_cref idInstrument, const std::string &idExchange,
  InstrumentType::enumInstrumentTypes type, 
  unsigned short year, unsigned short month ) 
: m_sInstrumentName( sInstrumentName ), m_sExchange( sExchangeName ),
  m_InstrumentType( type ),
  m_OptionSide( OptionSide::Unknown ), 
  m_Currency( Currency::USD ), m_CurrencyCounter( Currency::USD ), 
  m_nYear( year ), m_nMonth( month ), m_dblStrike( 0 ),
  m_nMultiplier( 1 ),
  m_nContract( 0 ),
  m_eUnderlyingStatus( EUnderlyingNotSettable )
{
  assert( m_InstrumentType == InstrumentType::Future );
  //assert( 0 < m_sSymbolName.size() );
  //assert( 0 < m_sExchange.size() );
}

 // option yymm
CInstrument::CInstrument( 
  idInstrument_cref sInstrumentName, const std::string &sExchangeName,
  InstrumentType::enumInstrumentTypes type, 
  unsigned short year, unsigned short month,
  //idInstrument_cref sUnderlyingName,
  pInstrument_t pUnderlying,
  OptionSide::enumOptionSide side, 
  double strike ) 
: m_sInstrumentName( sInstrumentName ), m_sExchange( sExchangeName ),
  m_pUnderlying( pUnderlying ), 
  m_InstrumentType( type ),
  m_OptionSide( side ), 
  m_Currency( Currency::USD ), m_CurrencyCounter( Currency::USD ), 
  m_nYear( year ), m_nMonth( month ), 
  m_dblStrike( strike ),
  m_nMultiplier( 100 ),
  m_nContract( 0 ),
  m_eUnderlyingStatus( EUnderlyingSet )
{
  assert( ( OptionSide::Call == side ) || ( OptionSide::Put == side ) );
  //assert( side > OptionSide::Unknown );
  assert( ( m_InstrumentType == InstrumentType::Option )
       || ( m_InstrumentType == InstrumentType::FuturesOption )
    );
  //assert( 0 < m_sSymbolName.size() );
  //assert( 0 < m_sExchange.size() );
}

 // option yymmdd
CInstrument::CInstrument( 
  idInstrument_cref sInstrumentName, const std::string &sExchangeName,
  InstrumentType::enumInstrumentTypes type, 
  unsigned short year, unsigned short month, unsigned short day,
  //idInstrument_cref sUnderlyingName,
  pInstrument_t pUnderlying,
  OptionSide::enumOptionSide side, 
  double strike ) 
: m_sInstrumentName( sInstrumentName ), m_sExchange( sExchangeName ),
  //m_sUnderlyingName( sUnderlyingName ), 
  m_pUnderlying( pUnderlying ), 
  m_InstrumentType( type ),
  m_OptionSide( side ), 
  m_Currency( Currency::USD ), m_CurrencyCounter( Currency::USD ), 
  m_nYear( year ), m_nMonth( month ), m_nDay( day ),
  m_dblStrike( strike ),
  m_nMultiplier( 100 ),
  m_nContract( 0 ),
  m_eUnderlyingStatus( EUnderlyingSet )
{
  assert( ( OptionSide::Call == side ) || ( OptionSide::Put == side ) );
  //assert( side > OptionSide::Unknown );
  assert( ( m_InstrumentType == InstrumentType::Option )
       || ( m_InstrumentType == InstrumentType::FuturesOption )
    );
  //assert( 0 < m_sSymbolName.size() );
  //assert( 0 < m_sExchange.size() );
}

// currency
CInstrument::CInstrument(
  const std::string &sInstrumentName, 
  pInstrument_t pUnderlying,
  //const std::string& sUnderlyingName,
  InstrumentType::enumInstrumentTypes type,
  Currency::enumCurrency base, Currency::enumCurrency counter
  ) 
: m_sInstrumentName( sInstrumentName ), 
  //m_sUnderlyingName( sUnderlyingName ),
  m_pUnderlying( pUnderlying ), 
  m_InstrumentType( type ),
  m_Currency( base ), m_CurrencyCounter( counter ),
  m_nMultiplier( 1 ),
  m_sExchange( "" ), m_nContract( 0 ),
  m_eUnderlyingStatus( EUnderlyingSet )
{
  assert( m_InstrumentType == InstrumentType::Currency );
}

CInstrument::CInstrument( idInstrument_cref sInstrumentId, sqlite3_stmt* pStmt ) 
: m_sInstrumentName( sInstrumentId ),
  m_sDescription( reinterpret_cast<const char*>( sqlite3_column_text( pStmt, 0 ) ) ),
  m_sExchange( reinterpret_cast<const char*>( sqlite3_column_text( pStmt, 1 ) ) ),
  m_InstrumentType( static_cast<InstrumentType::enumInstrumentTypes>( sqlite3_column_int( pStmt, 2 ) ) ),
  m_Currency( static_cast<Currency::enumCurrency>(sqlite3_column_int( pStmt, 3 ) ) ),
  m_CurrencyCounter( static_cast<Currency::enumCurrency>( sqlite3_column_int( pStmt, 4 ) ) ),
  m_OptionSide( static_cast<OptionSide::enumOptionSide>( sqlite3_column_int( pStmt, 5 ) ) ),
  // underlying requires something special
  m_sUnderlying( reinterpret_cast<const char*>( sqlite3_column_text( pStmt, 6 ) ) ), 
  m_nYear( sqlite3_column_int( pStmt, 7 ) ),
  m_nMonth( sqlite3_column_int( pStmt, 8 ) ),
  m_nDay( sqlite3_column_int( pStmt, 9 ) ),
  m_dblStrike( sqlite3_column_double( pStmt, 10 ) ),
  m_nContract( sqlite3_column_int( pStmt, 11 ) ),
  m_nMultiplier( sqlite3_column_int( pStmt, 12 ) )
{
  m_eUnderlyingStatus = EUnderlyingNotSettable;
  if ( InstrumentType::Option == m_InstrumentType ) m_eUnderlyingStatus = EUnderlyingNotSet;
  if ( InstrumentType::Currency == m_InstrumentType ) m_eUnderlyingStatus = EUnderlyingNotSet;
  if ( InstrumentType::FuturesOption == m_InstrumentType ) m_eUnderlyingStatus = EUnderlyingNotSet;
}

CInstrument::CInstrument( idInstrument_cref sInstrumentId, sqlite3_stmt* pStmt, pInstrument_t pUnderlying ) 
: m_sInstrumentName( sInstrumentId ),
  m_sDescription( reinterpret_cast<const char*>( sqlite3_column_text( pStmt, 0 ) ) ),
  m_sExchange( reinterpret_cast<const char*>( sqlite3_column_text( pStmt, 1 ) ) ),
  m_InstrumentType( static_cast<InstrumentType::enumInstrumentTypes>( sqlite3_column_int( pStmt, 2 ) ) ),
  m_Currency( static_cast<Currency::enumCurrency>(sqlite3_column_int( pStmt, 3 ) ) ),
  m_CurrencyCounter( static_cast<Currency::enumCurrency>( sqlite3_column_int( pStmt, 4 ) ) ),
  m_OptionSide( static_cast<OptionSide::enumOptionSide>( sqlite3_column_int( pStmt, 5 ) ) ),
  m_sUnderlying( reinterpret_cast<const char*>( sqlite3_column_text( pStmt, 6 ) ) ), 
  m_nYear( sqlite3_column_int( pStmt, 7 ) ),
  m_nMonth( sqlite3_column_int( pStmt, 8 ) ),
  m_nDay( sqlite3_column_int( pStmt, 9 ) ),
  m_dblStrike( sqlite3_column_double( pStmt, 10 ) ),
  m_nContract( sqlite3_column_int( pStmt, 11 ) ),
  m_nMultiplier( sqlite3_column_int( pStmt, 12 ) ),
  m_pUnderlying( pUnderlying )
{
  m_eUnderlyingStatus = EUnderlyingNotSet;
  if ( InstrumentType::Option == m_InstrumentType ) m_eUnderlyingStatus = EUnderlyingSet;
  if ( InstrumentType::Currency == m_InstrumentType ) m_eUnderlyingStatus = EUnderlyingSet;
  if ( InstrumentType::FuturesOption == m_InstrumentType ) m_eUnderlyingStatus = EUnderlyingSet;
  if ( EUnderlyingNotSet == m_eUnderlyingStatus ) {
    throw std::runtime_error( "CInstrument::CInstrument: underlying not accepted" );
  }
  if ( NULL == pUnderlying.get() ) {
    throw std::runtime_error( "CInstrument::CInstrument: non null underlying required" );
  }
  if ( m_sUnderlying != pUnderlying->GetInstrumentName() ) {
    throw std::runtime_error( "CInstrument::CInstrument: underlying name does not match expected name" );
  }
}

CInstrument::CInstrument(const CInstrument& instrument) 
:
  m_sInstrumentName( instrument.m_sInstrumentName ), 
  m_pUnderlying( instrument.m_pUnderlying ),
  m_sExchange( instrument.m_sExchange ), 
  m_InstrumentType( instrument.m_InstrumentType ), 
  m_Currency( instrument.m_Currency ), 
  m_CurrencyCounter( instrument.m_CurrencyCounter ),
  m_OptionSide( instrument.m_OptionSide ),
  m_nYear( instrument.m_nYear ), 
  m_nMonth( instrument.m_nMonth ), 
  m_nDay( instrument.m_nDay ),
  m_dblStrike( instrument.m_dblStrike ),
  m_nMultiplier( instrument.m_nMultiplier ),
  m_nContract( 0 ),
  m_eUnderlyingStatus( instrument.m_eUnderlyingStatus )
{
  mapAlternateNames_t::const_iterator iter = instrument.m_mapAlternateNames.begin();
  while ( instrument.m_mapAlternateNames.end() != iter ) {
    m_mapAlternateNames.insert( mapAlternateNames_pair_t( iter->first, iter->second ) );
  }
}

CInstrument::~CInstrument(void) {
}

void CInstrument::SetAlternateName( eidProvider_t id, idInstrument_cref name ) {
  mapAlternateNames_t::iterator iter = m_mapAlternateNames.find( id );
  if ( m_mapAlternateNames.end() == iter ) {
    m_mapAlternateNames.insert( mapAlternateNames_pair_t( id, name ) );
    OnAlternateNameAdded( pairNames_t( m_sInstrumentName, name ) );
  }
  else {
    if ( iter->second != name ) {
      idInstrument_t old = iter->second;
      iter->second.assign( name );
      OnAlternateNameChanged( pairNames_t( old, name ) );
    }
  }
}

CInstrument::idInstrument_cref CInstrument::GetInstrumentName( eidProvider_t id ) {
  mapAlternateNames_t::iterator iter = m_mapAlternateNames.find( id );
  if ( m_mapAlternateNames.end() != iter ) {
    //throw std::runtime_error( "CInstrument::GetAlternateName no alternate name" );
    return iter->second;
  }
  return m_sInstrumentName;
}

CInstrument::idInstrument_cref CInstrument::GetUnderlyingName( void ) {
  if ( EUnderlyingNotSet != m_eUnderlyingStatus ) {
    throw std::runtime_error( "CInstrument::GetUnderlyingName: underlying not set" );
  }
  return m_pUnderlying->GetInstrumentName();
}

CInstrument::idInstrument_cref CInstrument::GetUnderlyingName( eidProvider_t id ) {
  if ( EUnderlyingNotSet != m_eUnderlyingStatus ) {
    throw std::runtime_error( "CInstrument::GetUnderlyingName: underlying not set" );
  }
  return m_pUnderlying->GetInstrumentName(id);
}

void CInstrument::SetUnderlying( pInstrument_t pUnderlying ) {
  if ( EUnderlyingNotSettable == m_eUnderlyingStatus ) {
    throw std::runtime_error( "CInstrument::SetUnderlying: can not set underlying" );
  }
  if ( EUnderlyingSet == m_eUnderlyingStatus ) {
    throw std::runtime_error( "CInstrument::SetUnderlying: underlying already set" );
  }
  if ( m_sUnderlying != pUnderlying->GetInstrumentName() ) {
    throw std::runtime_error( "CInstrument::SetUnderlying: underlying name does not match expected name" );
  }
  m_pUnderlying = pUnderlying;
  m_eUnderlyingStatus = EUnderlyingSet;
}

} // namespace tf
} // namespace ou

