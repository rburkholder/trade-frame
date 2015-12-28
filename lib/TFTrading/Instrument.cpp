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

Instrument::Instrument( const TableRowDef& row ) 
  : m_row( row ), 
//  m_eUnderlyingStatus( EUnderlyingNotSettable ), 
  m_dtrTimeLiquid( dtDefault, dtDefault ),  m_dtrTimeTrading( dtDefault, dtDefault ),
  m_dateCommonCalc( boost::gregorian::not_a_date_time )
{
  assert( ( InstrumentType::Option != row.eType ) && ( InstrumentType::FuturesOption != row.eType ) );
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
  idInstrument_cref idInstrument, InstrumentType::enumInstrumentTypes eType,
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
  idInstrument_cref idInstrument, InstrumentType::enumInstrumentTypes eType, 
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
  idInstrument_cref idInstrument, InstrumentType::enumInstrumentTypes eType, 
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
  InstrumentType::enumInstrumentTypes eType, 
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
  InstrumentType::enumInstrumentTypes eType, 
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

Instrument::idInstrument_cref Instrument::GetInstrumentName( eidProvider_t id ) {
  mapAlternateNames_t::iterator iter = m_mapAlternateNames.find( id );
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

double Instrument::NormalizeOrderPrice( double price ) const {
  // works for 0.1, 0.01, may not work for others
  assert( 0.0 <= price );
  assert( 0.0 < m_row.dblMinTick );
  double n = price / m_row.dblMinTick;
  double t = std::floor( n );
  double r = n - t;
  return ( 0.5 <= r ) 
    ? std::ceil( n ) * m_row.dblMinTick
    : t * m_row.dblMinTick
    ;
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

