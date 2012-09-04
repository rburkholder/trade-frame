/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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

#include <boost/phoenix/core.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>

//using namespace fastdelegate;

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
  Register( pInstrument );
  return pInstrument;
}

CInstrumentManager::pInstrument_t CInstrumentManager::ConstructFuture(     
  idInstrument_cref sInstrumentName, const std::string& sExchangeName,  // future
  boost::uint16_t year, boost::uint16_t month ) {
  pInstrument_t pInstrument(
    new CInstrument( sInstrumentName, InstrumentType::Future, sExchangeName, year, month ) );
  Register( pInstrument );
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
  Register( pInstrument );
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
  Register( pInstrument );
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
  Register( pInstrument );
  return pInstrument;
}

void CInstrumentManager::Register( pInstrument_t& pInstrument ) {
  if ( Exists( pInstrument ) ) {
    throw std::runtime_error( "CInstrumentManager::Construct: instrument already exists" );
  }
  Assign( pInstrument );
  if ( 0 != m_pSession ) {
    ou::db::QueryFields<CInstrument::TableRowDef>::pQueryFields_t pQuery 
      = m_pSession->Insert<CInstrument::TableRowDef>( const_cast<CInstrument::TableRowDef&>( pInstrument->GetRow() ) );
    // save alternate instrument names
    pInstrument->ScanAlternateNames( boost::phoenix::bind( &CInstrumentManager::SaveAlternateInstrumentName, this, 
      boost::phoenix::arg_names::arg1, boost::phoenix::arg_names::arg2, boost::phoenix::arg_names::arg3 ) );
  }
}

void CInstrumentManager::SaveAlternateInstrumentName( 
    const keytypes::eidProvider_t& idProvider, const keytypes::idInstrument_t& idAlternate, const keytypes::idInstrument_t& idInstrument ) {
  AlternateInstrumentName::TableRowDef row( idProvider, idAlternate, idInstrument );
  SaveAlternateInstrumentName( row );
}

void CInstrumentManager::SaveAlternateInstrumentName( const AlternateInstrumentName::TableRowDef& row ) {
  if ( 0 != m_pSession ) {
    ou::db::QueryFields<AlternateInstrumentName::TableRowDef>::pQueryFields_t pQuery 
      = m_pSession->Insert<AlternateInstrumentName::TableRowDef>( const_cast<AlternateInstrumentName::TableRowDef&>( row ) );
  }
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
    if ( 0 != m_pSession ) {
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
    if ( 0 != m_pSession ) {
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
  assert( 0 != m_pSession );
  assert( m_map.end() == m_map.find( id ) );

  bool bFound = false;
  InstrumentManagerQueries::InstrumentKey idInstrument( id );
  ou::db::QueryFields<InstrumentManagerQueries::InstrumentKey>::pQueryFields_t pExistsQuery // shouldn't do a * as fields may change order
    = m_pSession->SQL<InstrumentManagerQueries::InstrumentKey>( "select * from instruments", idInstrument ).Where( "instrumentid = ?" ).NoExecute();
  m_pSession->Bind<InstrumentManagerQueries::InstrumentKey>( pExistsQuery );
  if ( m_pSession->Execute( pExistsQuery ) ) {  // <- need to be able to execute on query pointer, since there is session pointer in every query
    CInstrument::TableRowDef instrument;
    m_pSession->Columns<InstrumentManagerQueries::InstrumentKey, CInstrument::TableRowDef>( pExistsQuery, instrument );
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
     = m_pSession->SQL<InstrumentManagerQueries::InstrumentKey>( "select * from altinstrumentnames", idInstrument ).Where( "instrumentid = ?" ).NoExecute();
  m_pSession->Bind<InstrumentManagerQueries::InstrumentKey>( pExistsQuery );
  AlternateInstrumentName::TableRowDef altname;
  while ( m_pSession->Execute( pExistsQuery ) ) {
    m_pSession->Columns<InstrumentManagerQueries::InstrumentKey, AlternateInstrumentName::TableRowDef>( pExistsQuery, altname );
    pInstrument->SetAlternateName( altname.idProvider, altname.idAlternate );
  }
}

void CInstrumentManager::HandleAlternateNameAdded( const CInstrument::AlternateNameChangeInfo_t& info ) {
  iterMap iterKey = m_map.find( info.s1 );
  iterMap iterAlt = m_map.find( info.s2 );
  if ( m_map.end() == iterKey ) 
    throw std::runtime_error( "CInstrumentManager::HandleAlternateNameAdded key does not exist" );
  if ( m_map.end() != iterAlt ) 
    throw std::runtime_error( "CInstrumentManager::HandleAlternateNameAdded alt exists" );
  m_map.insert( pair_t( info.s2, iterKey->second ) );
  SaveAlternateInstrumentName( info.id, info.s2, info.s1 );
}

void CInstrumentManager::HandleAlternateNameChanged( const CInstrument::AlternateNameChangeInfo_t& info ) { // todo: need to update database
  iterMap iterOld = m_map.find( info.s1 );
  if ( m_map.end() == iterOld ) 
    throw std::runtime_error( "CInstrumentManager::HandleAlternateNameChanged old name not found" );
  iterMap iterNew = m_map.find( info.s2 );
  if ( m_map.end() != iterNew ) 
    throw std::runtime_error( "CInstrumentManager::HandleAlternateNameChanged new name already exists" );
  m_map.insert( pair_t( info.s2, iterOld->second ) );
  iterOld = m_map.find( info.s1 );  // load again to ensure proper copy
  m_map.erase( iterOld );
  // need database delete
  // need database insert
}

void CInstrumentManager::HandleRegisterTables( ou::db::Session& session ) {
  session.RegisterTable<CExchange::TableCreateDef>( tablenames::sExchange );
  session.RegisterTable<CInstrument::TableCreateDef>( tablenames::sInstrument );
  session.RegisterTable<AlternateInstrumentName::TableCreateDef>( tablenames::sAltInstrumentName );
}

void CInstrumentManager::HandleRegisterRows( ou::db::Session& session ) {
  session.MapRowDefToTableName<CExchange::TableRowDef>( tablenames::sExchange );
  session.MapRowDefToTableName<CInstrument::TableRowDef>( tablenames::sInstrument );
  session.MapRowDefToTableName<AlternateInstrumentName::TableRowDef>( tablenames::sAltInstrumentName );
}

void CInstrumentManager::HandlePopulateTables( ou::db::Session& session ) {

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

  ou::db::QueryFields<CExchange::TableRowDef>::pQueryFields_t pExchange = session.Insert<CExchange::TableRowDef>( exchange ).NoExecute();

  while ( vsExchangesPreload.end() != iter ) {
    exchange.idExchange = *(iter++);
    exchange.sName = *(iter++);
    session.Reset( pExchange );
    session.Bind<CExchange::TableRowDef>( pExchange );
    session.Execute( pExchange );
  }

}

// this stuff could probably be rolled into Session with a template
void CInstrumentManager::AttachToSession( ou::db::Session* pSession ) {
  ManagerBase::AttachToSession( pSession );
  pSession->OnRegisterTables.Add( MakeDelegate( this, &CInstrumentManager::HandleRegisterTables ) );
  pSession->OnRegisterRows.Add( MakeDelegate( this, &CInstrumentManager::HandleRegisterRows ) );
  pSession->OnPopulate.Add( MakeDelegate( this, &CInstrumentManager::HandlePopulateTables ) );

}

void CInstrumentManager::DetachFromSession( ou::db::Session* pSession ) {
  pSession->OnRegisterTables.Remove( MakeDelegate( this, &CInstrumentManager::HandleRegisterTables ) );
  pSession->OnRegisterRows.Remove( MakeDelegate( this, &CInstrumentManager::HandleRegisterRows ) );
  pSession->OnPopulate.Remove( MakeDelegate( this, &CInstrumentManager::HandlePopulateTables ) );
  ManagerBase::DetachFromSession( pSession );
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
