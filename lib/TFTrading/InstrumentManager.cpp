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

#include <stdexcept>

#include "InstrumentManager.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

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

CInstrumentManager::pInstrument_t CInstrumentManager::ConstructInstrument( 
  idInstrument_cref sInstrumentName, const std::string& sExchangeName, // generic
  InstrumentType::enumInstrumentTypes type ) {
  pInstrument_t pInstrument( new CInstrument( sInstrumentName, type, sExchangeName ) );
  Assign( pInstrument );
  return pInstrument;
}

CInstrumentManager::pInstrument_t CInstrumentManager::ConstructFuture(     
  idInstrument_cref sInstrumentName, const std::string& sExchangeName,  // future
  boost::uint16_t year, boost::uint16_t month ) {
  pInstrument_t pInstrument(
    new CInstrument( sInstrumentName, InstrumentType::Future, sExchangeName, year, month ) );
  Assign( pInstrument );
  return pInstrument;
}

CInstrumentManager::pInstrument_t CInstrumentManager::ConstructOption(
  idInstrument_cref sInstrumentName, const std::string& sExchangeName,  // option with yymm
  boost::uint16_t year, boost::uint16_t month,
  pInstrument_t pUnderlying,
  OptionSide::enumOptionSide side, 
  double strike ) {
  pInstrument_t pInstrument( 
    new CInstrument( sInstrumentName, InstrumentType::Option, sExchangeName, 
    year, month, pUnderlying, side, strike ) );
  Assign( pInstrument );
  return pInstrument;
}

CInstrumentManager::pInstrument_t CInstrumentManager::ConstructOption(
  idInstrument_cref sInstrumentName, const std::string& sExchangeName,  // option with yymmdd
  boost::uint16_t year, boost::uint16_t month, boost::uint16_t day,
  pInstrument_t pUnderlying,
  OptionSide::enumOptionSide side, 
  double strike ) {
  pInstrument_t pInstrument( 
    new CInstrument( sInstrumentName, InstrumentType::Option, sExchangeName, 
    year, month, day, pUnderlying, side, strike ) );
  Assign( pInstrument );
  return pInstrument;
}

CInstrumentManager::pInstrument_t CInstrumentManager::ConstructCurrency( 
  idInstrument_cref idInstrumentName, 
  idInstrument_cref idCounterInstrument,
//  pInstrument_t pUnderlying,
  const std::string& sExchangeName, 
  Currency::enumCurrency base, Currency::enumCurrency counter ) {
  pInstrument_t pInstrument(
    new CInstrument( idInstrumentName, idCounterInstrument, InstrumentType::Currency, sExchangeName, base, counter ) );
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

namespace InstrumentManagerQueries {
  struct InstrumentExists {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "instrumentid", idInstrument );
    }
    const ou::tf::keytypes::idInstrument_t& idInstrument;
    InstrumentExists( const ou::tf::keytypes::idInstrument_t& idInstrument_ ): idInstrument( idInstrument_ ) {};
  };
}

bool CInstrumentManager::Exists( idInstrument_cref id ) {
  bool bFound = ( m_map.end() != m_map.find( id ) );
  if ( !bFound ) {
    if ( 0 != m_pDbSession.get() ) {
      InstrumentManagerQueries::InstrumentExists paramExists( id );
      ou::db::QueryFields<InstrumentManagerQueries::InstrumentExists>::pQueryFields_t pExistsQuery // shouldn't do a * as fields may change order
        = m_pDbSession->SQL<InstrumentManagerQueries::InstrumentExists>( "select * from instruments", paramExists ).Where( "idinstrument = ?" ).NoExecute();
      m_pDbSession->Bind<InstrumentManagerQueries::InstrumentExists>( pExistsQuery );
      if ( m_pDbSession->Execute( pExistsQuery ) ) {  // <- need to be able to execute on query pointer
        CInstrument::TableRowDef instrument;
        m_pDbSession->Columns<InstrumentManagerQueries::InstrumentExists, CInstrument::TableRowDef>( pExistsQuery, instrument );
      }
      // check if exists in database (check idInstrument as well as alternate names?)
      // if in database:
      //   load instrument
      //   load underlying, if needed
      //   load alternate names for each
      // set bFound on total success
      // ** as an aside, need transaction when writing instrument, underlying, and alternate names to database to ensure correctness
    }
  }
  return bFound;
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

} // namespace tf
} // namespace ou
