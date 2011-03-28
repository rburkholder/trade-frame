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
#include <vector>

#include <boost/assign/std/vector.hpp>
using namespace boost::assign;

#include "InstrumentManager.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

CInstrumentManager::CInstrumentManager(void) {
//  file.OpenIQFSymbols();
}

CInstrumentManager::~CInstrumentManager(void) {
//  file.CloseIQFSymbols();
  for ( iterMap iter = m_map.begin(); iter != m_map.end(); ++iter ) {
    iter->second->OnAlternateNameAdded.Remove( MakeDelegate( this, &CInstrumentManager::HandleAlternateNameAdded ) );
    iter->second->OnAlternateNameChanged.Remove( MakeDelegate( this, &CInstrumentManager::HandleAlternateNameChanged ) );
  }
  m_map.clear();
}

CInstrumentManager::pInstrument_t CInstrumentManager::ConstructInstrument( 
  idInstrument_cref sInstrumentName, const std::string& sExchangeName, // generic
  InstrumentType::enumInstrumentTypes type ) {
  pInstrument_t pInstrument( new CInstrument( sInstrumentName, type, sExchangeName ) );
  Construct( pInstrument );
  return pInstrument;
}

CInstrumentManager::pInstrument_t CInstrumentManager::ConstructFuture(     
  idInstrument_cref sInstrumentName, const std::string& sExchangeName,  // future
  boost::uint16_t year, boost::uint16_t month ) {
  pInstrument_t pInstrument(
    new CInstrument( sInstrumentName, InstrumentType::Future, sExchangeName, year, month ) );
  Construct( pInstrument );
  return pInstrument;
}

CInstrumentManager::pInstrument_t CInstrumentManager::ConstructOption(
  idInstrument_cref sInstrumentName, const std::string& sExchangeName,  // option with yy mm
  boost::uint16_t year, boost::uint16_t month,
  pInstrument_t pUnderlying,
  OptionSide::enumOptionSide side, 
  double strike ) {
    assert( 0 != pUnderlying.get() );
  pInstrument_t pInstrument( 
    new CInstrument( sInstrumentName, InstrumentType::Option, sExchangeName, 
    year, month, pUnderlying, side, strike ) );
  Construct( pInstrument );
  return pInstrument;
}

CInstrumentManager::pInstrument_t CInstrumentManager::ConstructOption(
  idInstrument_cref sInstrumentName, const std::string& sExchangeName,  // option with yy mm dd
  boost::uint16_t year, boost::uint16_t month, boost::uint16_t day,
  pInstrument_t pUnderlying,
  OptionSide::enumOptionSide side, 
  double strike ) {
    assert( 0 != pUnderlying.get() );
  pInstrument_t pInstrument( 
    new CInstrument( sInstrumentName, InstrumentType::Option, sExchangeName, 
    year, month, day, pUnderlying, side, strike ) );
  Construct( pInstrument );
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
  Construct( pInstrument );
  return pInstrument;
}

void CInstrumentManager::Construct( pInstrument_t& pInstrument ) {
  if ( Exists( pInstrument ) ) {
    throw std::runtime_error( "CInstrumentManager::Construct instrument already exists" );
  }
  Assign( pInstrument );
  ou::db::QueryFields<CInstrument::TableRowDef>::pQueryFields_t pQuery 
    = m_pDbSession->Insert<CInstrument::TableRowDef>( const_cast<CInstrument::TableRowDef&>( pInstrument->GetRow() ) );
  // todo: save alternate instrument names
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
  pInstrument_t pInstrument;
  iterMap iter = m_map.find( idName );
  if ( m_map.end() != iter ) {
    pInstrument = iter->second;
  }
  else {
    bool bFound = false;
    if ( 0 != m_pDbSession.get() ) {
      bFound = LoadInstrument( idName, pInstrument );
    }
    if ( !bFound ) {
      throw std::runtime_error( "CInstrumentManager::Get can't find idInstrument" );
    }
  }
  return pInstrument;
}

bool CInstrumentManager::Exists( idInstrument_cref id ) {  // todo:  cache the query to make the get faster rather than searching the map again
  bool bFound = ( m_map.end() != m_map.find( id ) );
  if ( !bFound ) {
    if ( 0 != m_pDbSession.get() ) {
      CInstrument::pInstrument_t pInstrument;
      bFound = LoadInstrument( id, pInstrument );
    }
  }
  return bFound;
}

bool CInstrumentManager::Exists( pInstrument_cref pInstrument ) {
  return Exists( pInstrument->GetInstrumentName() );
}

namespace InstrumentManagerQueries {
  struct InstrumentKey {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "instrumentid", idInstrument );
    }
    const ou::tf::keytypes::idInstrument_t& idInstrument;
    InstrumentKey( const ou::tf::keytypes::idInstrument_t& idInstrument_ ): idInstrument( idInstrument_ ) {};
  };
}

bool CInstrumentManager::LoadInstrument( idInstrument_t id, pInstrument_t& pInstrument ) {
      // ** as an aside, need transaction when writing instrument, underlying, and alternate names to database to ensure correctness
  assert( 0 != m_pDbSession.get() );
  assert( m_map.end() != m_map.find( id ) );

  bool bFound = false;
  InstrumentManagerQueries::InstrumentKey idInstrument( id );
  ou::db::QueryFields<InstrumentManagerQueries::InstrumentKey>::pQueryFields_t pExistsQuery // shouldn't do a * as fields may change order
    = m_pDbSession->SQL<InstrumentManagerQueries::InstrumentKey>( "select * from instruments", idInstrument ).Where( "instrumentid = ?" ).NoExecute();
  m_pDbSession->Bind<InstrumentManagerQueries::InstrumentKey>( pExistsQuery );
  if ( m_pDbSession->Execute( pExistsQuery ) ) {  // <- need to be able to execute on query pointer, since there is session pointer in every query
    CInstrument::TableRowDef instrument;
    m_pDbSession->Columns<InstrumentManagerQueries::InstrumentKey, CInstrument::TableRowDef>( pExistsQuery, instrument );
    assert( ( ( "" != instrument.idUnderlying ) && ( ( InstrumentType::Option == instrument.eType ) || ( InstrumentType::FuturesOption == instrument.eType ) ) )
         || ( ( "" == instrument.idUnderlying ) && (   InstrumentType::Option != instrument.eType ) && ( InstrumentType::FuturesOption != instrument.eType ) ) 
      );
    if ( "" == instrument.idUnderlying ) {
      pInstrument.reset( new CInstrument( instrument ) );
      Assign( pInstrument );
      LoadAlternateInstrumentNames( pInstrument );  // comes after assign
      bFound = true;
    }
    else {
      pInstrument_t pUnderlying;
      iterMap iter = m_map.find( instrument.idUnderlying );
      if ( m_map.end() != iter ) {
        pUnderlying = iter->second;
        bFound = true;
      }
      else {
        bFound = LoadInstrument( instrument.idUnderlying, pUnderlying );
        if ( !bFound ) {
          throw std::runtime_error( "LoadInstrument: underlying does not exist in database" );
        }
      }
      assert( ( InstrumentType::Stock == pUnderlying->GetInstrumentType() ) || ( InstrumentType::Future == pUnderlying->GetInstrumentType() ) );
      pInstrument.reset( new CInstrument( instrument, pUnderlying ) );
      Assign( pInstrument );
      LoadAlternateInstrumentNames( pInstrument );  // comes after assign
    }
  }
  return bFound;
}

void CInstrumentManager::Delete( idInstrument_cref idInstrument ) {
  // check if has dependencies first, and exception if there are 
  // then delete alternate instrument names
  // then delete instrument
}

void CInstrumentManager::LoadAlternateInstrumentNames( pInstrument_t& pInstrument ) {
  assert( 0 != pInstrument.get() );
  InstrumentManagerQueries::InstrumentKey idInstrument( pInstrument->GetInstrumentName() );
   ou::db::QueryFields<InstrumentManagerQueries::InstrumentKey>::pQueryFields_t pExistsQuery // shouldn't do a * as fields may change order
     = m_pDbSession->SQL<InstrumentManagerQueries::InstrumentKey>( "select * from altinstrumentnames", idInstrument ).Where( "instrumentid = ?" ).NoExecute();
  m_pDbSession->Bind<InstrumentManagerQueries::InstrumentKey>( pExistsQuery );
  CAlternateInstrumentName::TableRowDef altname;
  while ( m_pDbSession->Execute( pExistsQuery ) ) {
    m_pDbSession->Columns<InstrumentManagerQueries::InstrumentKey, CAlternateInstrumentName::TableRowDef>( pExistsQuery, altname );
    pInstrument->SetAlternateName( altname.idProvider, altname.idAlternate );
  }
}

void CInstrumentManager::HandleAlternateNameAdded( CInstrument::pairNames_t pair ) { //todo: need to update database
  iterMap iterKey = m_map.find( pair.first );
  iterMap iterAlt = m_map.find( pair.second );
  if ( m_map.end() == iterKey ) 
    throw std::runtime_error( "CInstrumentManager::HandleAlternateNameAdded key does not exist" );
  if ( m_map.end() != iterKey ) 
    throw std::runtime_error( "CInstrumentManager::HandleAlternateNameAdded alt exists" );
  m_map.insert( pair_t( pair.second, iterKey->second ) );
}

void CInstrumentManager::HandleAlternateNameChanged( CInstrument::pairNames_t pair ) { // todo: need to update database
  iterMap iterOld = m_map.find( pair.first );
  if ( m_map.end() == iterOld ) 
    throw std::runtime_error( "CInstrumentManager::HandleAlternateNameChanged old name not found" );
  iterMap iterNew = m_map.find( pair.second );
  if ( m_map.end() != iterNew ) 
    throw std::runtime_error( "CInstrumentManager::HandleAlternateNameChanged new name already exists" );
  m_map.insert( pair_t( pair.second, iterOld->second ) );
  iterOld = m_map.find( pair.first );  // load again to ensure proper copy
  m_map.erase( iterOld );
}

void CInstrumentManager::RegisterTablesForCreation( void ) {
  m_pDbSession->RegisterTable<CExchange::TableCreateDef>( tablenames::sExchange );
  m_pDbSession->RegisterTable<CInstrument::TableCreateDef>( tablenames::sInstrument );
  m_pDbSession->RegisterTable<CAlternateInstrumentName::TableCreateDef>( tablenames::sAltInstrumentName );
}

void CInstrumentManager::RegisterRowDefinitions( void ) {
  m_pDbSession->MapRowDefToTableName<CExchange::TableRowDef>( tablenames::sExchange );
  m_pDbSession->MapRowDefToTableName<CInstrument::TableRowDef>( tablenames::sInstrument );
  m_pDbSession->MapRowDefToTableName<CAlternateInstrumentName::TableRowDef>( tablenames::sAltInstrumentName );
}

void CInstrumentManager::PopulateTables( void ) {

  std::vector<std::string> vsExchangesPreload;
  vsExchangesPreload +=
    "NYSE", "New York Stock Exchange",
    "CBOT", "Chicago Board of Trade",
    "CBOE", "Chicago Board Options Exchange",
    "OPRA", "Options Price Reporting Authority",
    "NASDAQ", "NASDAQ",
    "NMS", "NMS",
    "SMART", "Interactive Brokers Smart"
  ;
  
  assert( 0 < vsExchangesPreload.size() );
  assert( 0 == ( vsExchangesPreload.size() % 2 ) );

  CExchange::TableRowDef exchange;
  exchange.idCountry = "US";

  std::vector<std::string>::iterator iter = vsExchangesPreload.begin();

  ou::db::QueryFields<CExchange::TableRowDef>::pQueryFields_t pExchange = m_pDbSession->Insert<CExchange::TableRowDef>( exchange ).NoExecute();

  while ( vsExchangesPreload.end() != iter ) {
    exchange.idExchange = *(iter++);
    exchange.sName = *(iter++);
    m_pDbSession->Reset( pExchange );
    m_pDbSession->Bind<CExchange::TableRowDef>( pExchange );
    m_pDbSession->Execute( pExchange );
  }

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
