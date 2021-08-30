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

#include "stdafx.h"

#include <cassert>
#include <stdexcept>

#include <OUCommon/TimeSource.h>

#include "Instrument.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

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

//static const boost::gregorian::date dayDefault( boost::gregorian::not_a_date_time );
static const boost::posix_time::ptime dtDefault( boost::posix_time::not_a_date_time );

std::string Instrument::BuildDate( uint16_t year, uint16_t month, uint16_t day ) {
  return
    boost::lexical_cast<std::string>( year )
    + ( ( 9 < month ) ? "" : "0" ) + boost::lexical_cast<std::string>( month )
    + ( ( 9 < day ) ? "" : "0" ) + boost::lexical_cast<std::string>( day )
    ;
}

std::string Instrument::BuildGenericOptionName( const std::string& sUnderlying, OptionSide::enumOptionSide side, uint16_t year, uint16_t month, uint16_t day, double strike ) { // month is 1 based
  std::string sGenericName;
  std::string::size_type pos = sUnderlying.find_first_of( "-" );
  if ( std::string::npos == pos ) {
    sGenericName = sUnderlying;
  }
  else {
    sGenericName = sUnderlying.substr( 0, pos );
  }
  //std::string sGenericName( sUnderlying );
  sGenericName += "-" + BuildDate( year, month, day );
  sGenericName += "-";
  sGenericName += side;
  sGenericName += "-" + boost::lexical_cast<std::string>( strike );
  return sGenericName;
}

std::string Instrument::BuildGenericFutureName( const std::string& sUnderlying, uint16_t year, uint16_t month, uint16_t day ) { // month is 1 based
 std::string sGenericName( sUnderlying );
  sGenericName += "-" + boost::lexical_cast<std::string>( year )
    + ( ( 9 < month ) ? "" : "0" ) + boost::lexical_cast<std::string>( month )
    + ( ( 0 == day ) ? "" : ( ( ( 9 < day ) ? "" : "0" ) + boost::lexical_cast<std::string>( day ) ) )
    ;
  return sGenericName;
}

Instrument::Instrument( const TableRowDef& row )
  : m_row( row ),
//  m_eUnderlyingStatus( EUnderlyingNotSettable ),
  m_dtrTimeLiquid( dtDefault, dtDefault ),  m_dtrTimeTrading( dtDefault, dtDefault ),
  m_dateCommonCalc( boost::gregorian::not_a_date_time )
{
  switch ( row.eType ) {
    case InstrumentType::Stock:
      break;
    case InstrumentType::Index:
      break;
    case InstrumentType::Option:
      break;
    case InstrumentType::FuturesOption:
      break;
    case InstrumentType::Future:
      break;
    default:
      assert( 0 );
  }
}

//Instrument::Instrument( const TableRowDef& row, pInstrument_t& pUnderlying )
//  : m_row( row ), m_eUnderlyingStatus( EUnderlyingSet ), m_pUnderlying( pUnderlying ),
//  m_dtrTimeLiquid( dtDefault, dtDefault ),  m_dtrTimeTrading( dtDefault, dtDefault ),
//  m_dateCommonCalc( boost::gregorian::not_a_date_time )
//{
//  assert( ( InstrumentType::Option == row.eType ) || ( InstrumentType::FuturesOption == row.eType ) );
//}

// equity / generic creation
Instrument::Instrument(
  idInstrument_cref idInstrument, InstrumentType::enumInstrumentType eType,
  const idExchange_t &idExchange
                         )
: m_row( idInstrument, eType, idExchange ),
//  m_eUnderlyingStatus( EUnderlyingNotSettable ),
  m_dtrTimeLiquid( dtDefault, dtDefault ),  m_dtrTimeTrading( dtDefault, dtDefault ),
  m_dateCommonCalc( boost::gregorian::not_a_date_time )
{
}

 // future
Instrument::Instrument(
  idInstrument_cref idInstrument, InstrumentType::enumInstrumentType eType,
  const idExchange_t& idExchange,
  boost::uint16_t year, boost::uint16_t month, boost::uint16_t day )
: m_row( idInstrument, eType, idExchange, year, month, day ),
//  m_eUnderlyingStatus( EUnderlyingNotSettable ),
  m_dtrTimeLiquid( dtDefault, dtDefault ),  m_dtrTimeTrading( dtDefault, dtDefault ),
  m_dateCommonCalc( boost::gregorian::not_a_date_time )
{
  //assert( 0 < m_sSymbolName.size() );
  //assert( 0 < m_sExchange.size() );
}

 // option yymm
Instrument::Instrument(
  idInstrument_cref idInstrument, InstrumentType::enumInstrumentType eType,
  const idExchange_t& idExchange,
  boost::uint16_t year, boost::uint16_t month,
//  pInstrument_t pUnderlying,
  OptionSide::enumOptionSide eOptionSide,
  double dblStrike )
  : m_row( idInstrument, eType, idExchange,
//  pUnderlying->GetInstrumentName(),
  year, month, eOptionSide, dblStrike ),
//  m_pUnderlying( pUnderlying ),
//  m_eUnderlyingStatus( EUnderlyingSet ),
  m_dtrTimeLiquid( dtDefault, dtDefault ),  m_dtrTimeTrading( dtDefault, dtDefault ),
  m_dateCommonCalc( boost::gregorian::not_a_date_time )
{
  //assert( 0 < m_sExchange.size() );
//  assert( 0 != pUnderlying.get() );
//  assert( "" != pUnderlying->GetInstrumentName() );
  //m_eUnderlyingStatus = EUnderlyingSet;
  //m_eUnderlyingStatus = EUnderlyingNotSettable;  // not sure which to use
}

 // option yymmdd
Instrument::Instrument(
  idInstrument_cref idInstrument,
  InstrumentType::enumInstrumentType eType,
  const idExchange_t& idExchange,
  boost::uint16_t year, boost::uint16_t month, boost::uint16_t day,
//  pInstrument_t pUnderlying,
  OptionSide::enumOptionSide eOptionSide,
  double dblStrike )
  : m_row( idInstrument, eType, idExchange,
//  pUnderlying->GetInstrumentName(),
  year, month, day, eOptionSide, dblStrike ),
//  m_pUnderlying( pUnderlying ),
//  m_eUnderlyingStatus( EUnderlyingSet ),
  m_dtrTimeLiquid( dtDefault, dtDefault ),  m_dtrTimeTrading( dtDefault, dtDefault ),
  m_dateCommonCalc( boost::gregorian::not_a_date_time )
{
  //assert( 0 < m_sExchange.size() );
//  assert( 0 != pUnderlying.get() );
//  assert( "" != pUnderlying->GetInstrumentName() );
  //m_eUnderlyingStatus = EUnderlyingSet;
  //m_eUnderlyingStatus = EUnderlyingNotSettable;  // not sure which to use
}

// currency
Instrument::Instrument(
  const idInstrument_t& idInstrument,
//                       const idInstrument_t& idCounterInstrument,
  InstrumentType::enumInstrumentType eType,
  const idExchange_t& idExchange,
  Currency::enumCurrency base,
  Currency::enumCurrency counter
  )
  : m_row( idInstrument,
//  idCounterInstrument,
  eType, idExchange, base, counter ),
//  m_pUnderlying( pUnderlying ),
//  m_eUnderlyingStatus( EUnderlyingNotSettable ),
  m_dtrTimeLiquid( dtDefault, dtDefault ),  m_dtrTimeTrading( dtDefault, dtDefault ),
  m_dateCommonCalc( boost::gregorian::not_a_date_time )
{

}

/*
  m_eUnderlyingStatus = EUnderlyingNotSet;
  if ( InstrumentType::Option == m_InstrumentType ) m_eUnderlyingStatus = EUnderlyingSet;
  if ( InstrumentType::Currency == m_InstrumentType ) m_eUnderlyingStatus = EUnderlyingSet;
  if ( InstrumentType::FuturesOption == m_InstrumentType ) m_eUnderlyingStatus = EUnderlyingSet;
  if ( EUnderlyingNotSet == m_eUnderlyingStatus ) {
    throw std::runtime_error( "Instrument::Instrument: underlying not accepted" );
  }
  if ( NULL == pUnderlying.get() ) {
    throw std::runtime_error( "Instrument::Instrument: non null underlying required" );
  }
  if ( m_sUnderlying != pUnderlying->GetInstrumentName() ) {
    throw std::runtime_error( "Instrument::Instrument: underlying name does not match expected name" );
  }
}
*/

Instrument::Instrument(const Instrument& instrument)
:
  m_row( instrument.m_row ),
//  m_pUnderlying( instrument.m_pUnderlying ),
//  m_eUnderlyingStatus( instrument.m_eUnderlyingStatus ),
  m_dtrTimeLiquid( dtDefault, dtDefault ),  m_dtrTimeTrading( dtDefault, dtDefault ),
  m_dateCommonCalc( boost::gregorian::not_a_date_time )
{
  mapAlternateNames_t::const_iterator iter = instrument.m_mapAlternateNames.begin();
  while ( instrument.m_mapAlternateNames.end() != iter ) {
    m_mapAlternateNames.insert( mapAlternateNames_pair_t( iter->first, iter->second ) );
  }
}

Instrument::~Instrument(void) {
}

void Instrument::SetAlternateName( eidProvider_t id, idInstrument_cref name ) {
  mapAlternateNames_t::iterator iter = m_mapAlternateNames.find( id );
  if ( m_mapAlternateNames.end() == iter ) {
    m_mapAlternateNames.insert( mapAlternateNames_pair_t( id, name ) );
//    OnAlternateNameAdded( AlternateNameChangeInfo_t( id, m_row.idInstrument, name ) );  // 2012/02/05  this creates a loop when loading alt names in instrument manager
  }
  else {
    if ( iter->second != name ) {
      idInstrument_t old = iter->second;
      iter->second.assign( name );
      OnAlternateNameChanged( AlternateNameChangeInfo_t( id, old, name ) );
    }
  }
}

Instrument::idInstrument_cref Instrument::GetInstrumentName( eidProvider_t id ) const {
  mapAlternateNames_t::const_iterator iter = m_mapAlternateNames.find( id );
  if ( m_mapAlternateNames.end() != iter ) {
    //throw std::runtime_error( "Instrument::GetAlternateName no alternate name" );
    return iter->second;
  }
  return m_row.idInstrument;
}

//Instrument::idInstrument_cref Instrument::GetUnderlyingName( void ) {
//  if ( EUnderlyingSet != m_eUnderlyingStatus ) {
//    throw std::runtime_error( "Instrument::GetUnderlyingName: underlying not set" );
//  }
//  return m_pUnderlying->GetInstrumentName();
//}

//Instrument::idInstrument_cref Instrument::GetUnderlyingName( eidProvider_t id ) {
//  if ( EUnderlyingSet != m_eUnderlyingStatus ) {
//    throw std::runtime_error( "Instrument::GetUnderlyingName: underlying not set" );
//  }
//  return m_pUnderlying->GetInstrumentName(id);
//}

bool Instrument::operator==( const Instrument& rhs ) const {
  return (
    ( m_row.idInstrument == rhs.m_row.idInstrument )
    && ( m_row.idExchange == rhs.m_row.idExchange )
    && ( m_row.eType == rhs.m_row.eType )
//    && ( m_row.idUnderlying == rhs.m_row.idUnderlying )
    && ( m_row.dblStrike == rhs.m_row.dblStrike )
    && ( m_row.eOptionSide == rhs.m_row.eOptionSide )
    );
}

// use 16:00 est as time of expiry, as that is when they cease trading (for OPRA equities)
// 18:30 deals with after hours trading and settlements on the underlying.  the options cease trading at 16:00.

boost::posix_time::ptime Instrument::GetExpiryUtc( void ) const {
  // may require further time refinements with other option types
  // may require a table for handling other markets and time zones
  boost::gregorian::date dateExpiry( m_row.nYear, m_row.nMonth, m_row.nDay );
  switch ( m_row.eType ) {
    case InstrumentType::Option:  // for equities options
      return ou::TimeSource::Instance().
              ConvertRegionalToUtc( dateExpiry, boost::posix_time::time_duration( 16, 0, 0 ), "America/New_York", true );
      break;
    case InstrumentType::FuturesOption:  // for metals options
      return ou::TimeSource::Instance().
              ConvertRegionalToUtc( dateExpiry, boost::posix_time::time_duration( 13, 30, 0 ), "America/New_York", true );
      break;
  }
  return ou::TimeSource::Instance().
          ConvertRegionalToUtc( dateExpiry, boost::posix_time::time_duration( 16, 0, 0 ), "America/New_York", true );
}

double Instrument::NormalizeOrderPrice( double price ) const {
  assert( 0.0 <= price );
  assert( 0.0 < m_row.dblMinTick );
  const double round = m_row.dblMinTick / 2.0;
  const double multiple_rough = price / m_row.dblMinTick;
  const double multiple_floor = std::floor( multiple_rough );
  const double lower = multiple_floor * m_row.dblMinTick;
  return ( price < ( lower + round ) ) ? lower : ( lower + m_row.dblMinTick );
}

/*
void Instrument::SetUnderlying( pInstrument_t pUnderlying ) {
  if ( EUnderlyingNotSettable == m_eUnderlyingStatus ) {
    throw std::runtime_error( "Instrument::SetUnderlying: can not set underlying" );
  }
  if ( EUnderlyingSet == m_eUnderlyingStatus ) {
    throw std::runtime_error( "Instrument::SetUnderlying: underlying already set" );
  }
  if ( m_row.idUnderlying != pUnderlying->GetInstrumentName() ) {
    throw std::runtime_error( "Instrument::SetUnderlying: underlying name does not match expected name" );
  }
  m_pUnderlying = pUnderlying;
  m_eUnderlyingStatus = EUnderlyingSet;
}
*/
} // namespace tf
} // namespace ou
