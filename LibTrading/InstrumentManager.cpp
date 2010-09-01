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
#include "InstrumentManager.h"

#include <stdexcept>

CInstrumentManager::CInstrumentManager(void) {
//  file.OpenIQFSymbols();
}

CInstrumentManager::~CInstrumentManager(void) {
//  file.CloseIQFSymbols();
  for ( iterator iter = m_map.begin(); iter != m_map.end(); ++iter ) {
    iter->second->OnAlternateNameAdded.Remove( MakeDelegate( this, &CInstrumentManager::HandleAlternateNameAdded ) );
    iter->second->OnAlternateNameChanged.Remove( MakeDelegate( this, &CInstrumentManager::HandleAlternateNameChanged ) );
  }
  m_map.clear();
}

/*
CInstrument::pInstrument_t CInstrumentManager::GetIQFeedInstrument(const std::string &sName) {
//  return GetIQFeedInstrument( sName, sName );
  try {
    CInstrument::pInstrument_t pInstrument( file.CreateInstrumentFromIQFeed( sName ) );  // todo:  need to verify proper symbol usage
    return pInstrument;
  }
  catch (...) {
    throw std::runtime_error( "CInstrumentManager::GetIQFeedInstrument problems" );
  }
}
*/
/*
CInstrument::pInstrument_t CInstrumentManager::GetIQFeedInstrument(const std::string &sName, const std::string &sAlternateName) {
  try {
    CInstrument::pInstrument_t pInstrument( file.CreateInstrumentFromIQFeed( sName, sAlternateName ) );  // todo:  need to verify proper symbol usage
    return pInstrument;
  }
  catch (...) {
    throw std::runtime_error( "CBasketTradeSymbolInfo::Initialize problems" );
  }
  //return pInstrument;
}
*/


CInstrumentManager::pInstrument_t CInstrumentManager::ConstructInstrument( 
  idInstrument_cref sInstrumentName, const std::string& sExchangeName, // generic
  InstrumentType::enumInstrumentTypes type ) {
  pInstrument_t pInstrument( 
    new CInstrument( sInstrumentName, sExchangeName, type ) );
  Assign( pInstrument );
  return pInstrument;
}

CInstrumentManager::pInstrument_t CInstrumentManager::ConstructFuture(     
  idInstrument_cref sInstrumentName, const std::string& sExchangeName,  // future
  unsigned short year, unsigned short month ) {
  pInstrument_t pInstrument(
    new CInstrument( sInstrumentName, sExchangeName, InstrumentType::Future, year, month ) );
  Assign( pInstrument );
  return pInstrument;
}

CInstrumentManager::pInstrument_t CInstrumentManager::ConstructOption(
  idInstrument_cref sInstrumentName, const std::string& sExchangeName,  // option with yymm
  unsigned short year, unsigned short month,
  pInstrument_t pUnderlying,
  OptionSide::enumOptionSide side, 
  double strike ) {
  pInstrument_t pInstrument( 
    new CInstrument( sInstrumentName, sExchangeName, InstrumentType::Option, 
    year, month, pUnderlying, side, strike ) );
  Assign( pInstrument );
  return pInstrument;
}

CInstrumentManager::pInstrument_t CInstrumentManager::ConstructOption(
  idInstrument_cref sInstrumentName, const std::string& sExchangeName,  // option with yymmdd
  unsigned short year, unsigned short month, unsigned short day,
  pInstrument_t pUnderlying,
  OptionSide::enumOptionSide side, 
  double strike ) {
  pInstrument_t pInstrument( 
    new CInstrument( sInstrumentName, sExchangeName, InstrumentType::Option, 
    year, month, day, pUnderlying, side, strike ) );
  Assign( pInstrument );
  return pInstrument;
}

CInstrumentManager::pInstrument_t CInstrumentManager::ConstructCurrency( 
  idInstrument_cref sInstrumentName, 
  pInstrument_t pUnderlying,
  Currency::enumCurrency base, Currency::enumCurrency counter ) {
  pInstrument_t pInstrument(
    new CInstrument( sInstrumentName, pUnderlying, InstrumentType::Currency, base, counter ) );
  Assign( pInstrument );
  return pInstrument;
}

void CInstrumentManager::Assign( pInstrument_cref pInstrument ) {
  if ( m_map.end() != m_map.find( pInstrument->GetInstrumentName() ) ) {
    throw std::runtime_error( "CInstrumentManager::Assign instrument already exists" );
  }
  else {
    m_map.insert( pair_t( pInstrument->GetInstrumentName(), pInstrument ) );
  }
  pInstrument->OnAlternateNameAdded.Add( MakeDelegate( this, &CInstrumentManager::HandleAlternateNameAdded ) );
  pInstrument->OnAlternateNameChanged.Add( MakeDelegate( this, &CInstrumentManager::HandleAlternateNameChanged ) );
}

CInstrumentManager::pInstrument_t CInstrumentManager::Get( idInstrument_cref idName ) {
  iterator iter = m_map.find( idName );
  if ( m_map.end() == iter ) {
    throw std::runtime_error( "CInstrumentManager::Get can't find idInstrument" );
  }
  else {
    return iter->second;
  }
}

bool CInstrumentManager::Exists( idInstrument_cref id ) {
  iterator iter = m_map.find( id );
  return m_map.end() != iter;
}

bool CInstrumentManager::Exists( pInstrument_cref pInstrument ) {
  return Exists( pInstrument->GetInstrumentName() );
}

void CInstrumentManager::HandleAlternateNameAdded( CInstrument::pairNames_t pair ) {
  iterator iterKey = m_map.find( pair.first );
  iterator iterAlt = m_map.find( pair.second );
  if ( m_map.end() == iterKey ) 
    throw std::runtime_error( "CInstrumentManager::HandleAlternateNameAdded key does not exist" );
  if ( m_map.end() != iterKey ) 
    throw std::runtime_error( "CInstrumentManager::HandleAlternateNameAdded alt exists" );
  m_map.insert( pair_t( pair.second, iterKey->second ) );
}

void CInstrumentManager::HandleAlternateNameChanged( CInstrument::pairNames_t pair ) {
  iterator iterOld = m_map.find( pair.first );
  if ( m_map.end() == iterOld ) 
    throw std::runtime_error( "CInstrumentManager::HandleAlternateNameChanged old name not found" );
  iterator iterNew = m_map.find( pair.second );
  if ( m_map.end() != iterNew ) 
    throw std::runtime_error( "CInstrumentManager::HandleAlternateNameChanged new name already exists" );
  m_map.insert( pair_t( pair.second, iterOld->second ) );
  iterOld = m_map.find( pair.first );  // load again to ensure proper copy
  m_map.erase( iterOld );
}

