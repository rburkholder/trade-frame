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
#include "Instrument.h"

#include <cassert>

// equity / generic creation
CInstrument::CInstrument(idInstrument_cref sInstrumentName, const std::string &sExchangeName,
                         InstrumentType::enumInstrumentTypes type)
: m_sInstrumentName( sInstrumentName ), m_sExchange( sExchangeName ),
  m_InstrumentType( type ), 
  m_Currency( Currency::USD ), m_CurrencyCounter( Currency::USD ), 
  m_nYear( 0 ), m_nMonth( 0 ), m_OptionSide( OptionSide::Unknown ), m_dblStrike( 0 ),
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
  m_nContract( 0 )
{
  assert( side < OptionSide::_Count );
  assert( side > OptionSide::Unknown );
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
  m_nContract( 0 )
{
  assert( side < OptionSide::_Count );
  assert( side > OptionSide::Unknown );
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
  m_sExchange( "" ), m_nContract( 0 )
{
  assert( m_InstrumentType == InstrumentType::Currency );
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

