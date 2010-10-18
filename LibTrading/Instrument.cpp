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

const std::string CInstrument::m_sSqlCreate( 
  "create table instruments ( \
   instrumentid TEXT CONSTRAINT pk_instruments PRIMARY KEY, \
   version SMALLINT DEFAULT 1, \
   description TEXT default '', \
   exchangeid TEXT NOT NULL, \
   type SMALLINT NOT NULL, \
   currency SMALLINT NOT NULL, \
   countercurrency SMALLINT NOT NULL, \
   optionside SMALLINT NOT NULL, \
   underlying TEXT default '', \
   year SMALLINT NOT NULL, \
   month SMALLINT NOT NULL, \
   day SMALLINT NOT NULL, \
   strike DOUBLE NOT NULL, \
   contract INTEGER NOT NULL, \
   multiplier UNSIGNED INTEGER NOT NULL, \
   CONSTRAINT fk_instruments_exchangeid \
     FOREIGN KEY(exchangeid) REFERENCES exchanges(exchangeid) \
     ON DELETE RESTRICT ON UPDATE CASCADE, \
   CONSTRAINT fk_instruments_underlying \
     FOREIGN KEY(underlying) REFERENCES instruments(instrumentid) \
     ON DELETE RESTRICT ON UPDATE CASCADE \
  " );   
const std::string CInstrument::m_sSqlSelect( 
  "SELECT description, exchangeid, type, currency, countercurrency, optionside, underlying, year, month, day, strike, contract, multiplier \
   from instruments where instrumentid = :id; \
  " );
const std::string CInstrument::m_sSqlInsert( 
  "INSERT INTO instruments (description, exchangeid, type, currency, countercurrency, optionside, underlying, year, month, day, strike, contract, multiplier ) \
  VALUES (:description, :exchangeid, :type, :currency, :countercurrency, :optionside, :underlying, :year, :month, :day, :strike, :contract, :multiplier) \
  " );
const std::string CInstrument::m_sSqlUpdate( 
  "UPDATE instruments SET \
  description = :description, \
  exchangeid = :exchangeid, \
  type = :type, \
  currency = :currency, \
  countercurrency = :countercurrency, \
  optionside = :optionside, \
  underlying = :underlying, \
  year = :year, \
  month = :month, \
  day = :day, \
  strike = :strike, \
  contract = :contract, \
  multiplier = :multiplier \
  WHERE instrumentid = :id \
  " );
const std::string CInstrument::m_sSqlDelete( "DELETE FROM instruments WHERE instrumentid = :id;" );

// equity / generic creation
CInstrument::CInstrument(idInstrument_cref sInstrumentName, const std::string &sExchangeName,
                         InstrumentType::enumInstrumentTypes type)
: m_sInstrumentName( sInstrumentName ), m_sExchange( sExchangeName ),
  m_InstrumentType( type ), 
  m_Currency( Currency::USD ), m_CurrencyCounter( Currency::USD ), 
  m_nYear( 0 ), m_nMonth( 0 ), m_OptionSide( OptionSide::Unknown ), m_dblStrike( 0 ),
  m_nMultiplier( 1 ),
  m_nContract( 0 )
{
  assert( type < InstrumentType::_Count );
  assert( type > InstrumentType::Unknown );
  assert( 0 < m_sInstrumentName.size() );
  //assert( 0 < m_sExchange.size() );
}

 // future
CInstrument::CInstrument( 
  idInstrument_cref sInstrumentName, const std::string &sExchangeName,
  InstrumentType::enumInstrumentTypes type, 
  unsigned short year, unsigned short month ) 
: m_sInstrumentName( sInstrumentName ), m_sExchange( sExchangeName ),
  m_InstrumentType( type ),
  m_OptionSide( OptionSide::Unknown ), 
  m_Currency( Currency::USD ), m_CurrencyCounter( Currency::USD ), 
  m_nYear( year ), m_nMonth( month ), m_dblStrike( 0 ),
  m_nMultiplier( 1 ),
  m_nContract( 0 )
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
  m_nContract( 0 )
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
  m_nContract( 0 )
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
  m_sExchange( "" ), m_nContract( 0 )
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
  m_nYear( sqlite3_column_int( pStmt, 7 ) ),
  m_nMonth( sqlite3_column_int( pStmt, 8 ) ),
  m_nDay( sqlite3_column_int( pStmt, 9 ) ),
  m_dblStrike( sqlite3_column_double( pStmt, 10 ) ),
  m_nContract( sqlite3_column_int( pStmt, 11 ) ),
  m_nMultiplier( sqlite3_column_int( pStmt, 12 ) )
{
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
  m_nContract( 0 )
{
  mapAlternateNames_t::const_iterator iter = instrument.m_mapAlternateNames.begin();
  while ( instrument.m_mapAlternateNames.end() != iter ) {
    m_mapAlternateNames.insert( mapAlternateNames_pair_t( iter->first, iter->second ) );
  }
}

CInstrument::~CInstrument(void) {
}

void CInstrument::SetAlternateName( enumProviderId_t id, idInstrument_cref name ) {
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

CInstrument::idInstrument_cref CInstrument::GetInstrumentName( enumProviderId_t id ) {
  mapAlternateNames_t::iterator iter = m_mapAlternateNames.find( id );
  if ( m_mapAlternateNames.end() != iter ) {
    //throw std::runtime_error( "CInstrument::GetAlternateName no alternate name" );
    return iter->second;
  }
  return m_sInstrumentName;
}

CInstrument::idInstrument_cref CInstrument::GetUnderlyingName( void ) {
  if ( NULL == m_pUnderlying.get() ) {
    throw std::runtime_error( "CInstrument::GetUnderlyingName: no underlying" );
  }
  return m_pUnderlying->GetInstrumentName();
}

CInstrument::idInstrument_cref CInstrument::GetUnderlyingName( enumProviderId_t id ) {
  if ( NULL == m_pUnderlying.get() ) {
    throw std::runtime_error( "CInstrument::GetUnderlyingName: no underlying" );
  }
  return m_pUnderlying->GetInstrumentName(id);
}

void CInstrument::CreateDbTable( sqlite3* pDb ) {

  char* pMsg;
  int rtn;

  rtn = sqlite3_exec( pDb, m_sSqlCreate.c_str(), 0, 0, &pMsg );

  if ( SQLITE_OK != rtn ) {
    std::string sErr( "Error creating table instruments: " );
    sErr += pMsg;
    sqlite3_free( pMsg );
    throw std::runtime_error( sErr );
  }

}

int CInstrument::BindDbKey( sqlite3_stmt* pStmt ) {

  int rtn( 0 );
  rtn = sqlite3_bind_text( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":id" ), m_sInstrumentName.c_str(), -1, SQLITE_STATIC );
  return rtn;
}

int CInstrument::BindDbVariables( sqlite3_stmt* pStmt ) {

  int rtn( 0 );

  rtn += sqlite3_bind_text( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":description" ), m_sDescription.c_str(), -1, SQLITE_STATIC );
  rtn += sqlite3_bind_text( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":exchangeid" ), m_sExchange.c_str(), -1, SQLITE_STATIC );
  rtn += sqlite3_bind_int( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":type" ), m_InstrumentType );
  rtn += sqlite3_bind_int( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":currency" ), m_Currency );
  rtn += sqlite3_bind_int( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":countercurrency" ), m_CurrencyCounter );
  rtn += sqlite3_bind_int( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":optionside" ), m_OptionSide );
  rtn += sqlite3_bind_text(   // ** this needs to be conditional
    pStmt, sqlite3_bind_parameter_index( pStmt, ":underlying" ), m_pUnderlying->GetInstrumentName().c_str(), -1, SQLITE_STATIC );
  rtn += sqlite3_bind_int( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":year" ), m_nYear );
  rtn += sqlite3_bind_int( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":month" ), m_nMonth );
  rtn += sqlite3_bind_int( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":day" ), m_nDay );
  rtn += sqlite3_bind_double( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":strike" ), m_dblStrike );
  rtn += sqlite3_bind_int( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":contract" ), m_nContract );
  rtn += sqlite3_bind_int( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":multiplier" ), m_nMultiplier );

  return rtn;  // should be 0 if all goes well
}


