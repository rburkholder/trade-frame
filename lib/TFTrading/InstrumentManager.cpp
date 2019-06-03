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

#include "stdafx.h"

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

InstrumentManager::InstrumentManager(void) {
//  file.OpenIQFSymbols();
}

InstrumentManager::~InstrumentManager(void) {
//  file.CloseIQFSymbols();
  for ( iterMap iter = m_map.begin(); iter != m_map.end(); ++iter ) {
    iter->second->OnAlternateNameAdded.Remove( MakeDelegate( this, &InstrumentManager::HandleAlternateNameAdded ) );
    iter->second->OnAlternateNameChanged.Remove( MakeDelegate( this, &InstrumentManager::HandleAlternateNameChanged ) );
  }
  m_map.clear();
}

InstrumentManager::pInstrument_t InstrumentManager::ConstructInstrument( 
  idInstrument_cref sInstrumentName, const std::string& sExchangeName, // generic
  InstrumentType::enumInstrumentTypes type ) {
  pInstrument_t pInstrument( new Instrument( sInstrumentName, type, sExchangeName ) );
  Register( pInstrument );
  return pInstrument;
}

InstrumentManager::pInstrument_t InstrumentManager::ConstructFuture(     
  idInstrument_cref sInstrumentName, const std::string& sExchangeName,  // future
  boost::uint16_t year, boost::uint16_t month ) {
  pInstrument_t pInstrument(
    new Instrument( sInstrumentName, InstrumentType::Future, sExchangeName, year, month ) );
  Register( pInstrument );
  return pInstrument;
}

InstrumentManager::pInstrument_t InstrumentManager::ConstructOption(
  idInstrument_cref sInstrumentName, const std::string& sExchangeName,  // option with yy mm
  boost::uint16_t year, boost::uint16_t month,
//  pInstrument_t pUnderlying,
  OptionSide::enumOptionSide side, 
  double strike ) {
//    assert( 0 != pUnderlying.get() );
  pInstrument_t pInstrument( 
    new Instrument( sInstrumentName, InstrumentType::Option, sExchangeName, 
    year, month, /*pUnderlying,*/ side, strike ) );
  Register( pInstrument );
  return pInstrument;
}

InstrumentManager::pInstrument_t InstrumentManager::ConstructOption(
  idInstrument_cref sInstrumentName, const std::string& sExchangeName,  // option with yy mm dd
  boost::uint16_t year, boost::uint16_t month, boost::uint16_t day,
//  pInstrument_t pUnderlying,
  OptionSide::enumOptionSide side, 
  double strike ) {
//    assert( 0 != pUnderlying.get() );
  pInstrument_t pInstrument( 
    new Instrument( sInstrumentName, InstrumentType::Option, sExchangeName, 
    year, month, day, /*pUnderlying,*/ side, strike ) );
  Register( pInstrument );
  return pInstrument;
}

InstrumentManager::pInstrument_t InstrumentManager::ConstructCurrency( 
  idInstrument_cref idInstrumentName, 
//  idInstrument_cref idCounterInstrument,
//  pInstrument_t pUnderlying,
  const std::string& sExchangeName, 
  Currency::enumCurrency base, Currency::enumCurrency counter ) {
  pInstrument_t pInstrument(
    new Instrument( idInstrumentName, /*idCounterInstrument,*/ InstrumentType::Currency, sExchangeName, base, counter ) );
  Register( pInstrument );
  return pInstrument;
}

void InstrumentManager::Register( pInstrument_t& pInstrument ) {
  // TODO: validate that IB id, IQF id, and IB contract present? ( or put into separate validator?)
  if ( Exists( pInstrument ) ) {
    throw std::runtime_error( "InstrumentManager::Construct: instrument already exists" );
  }
  Assign( pInstrument );
  if ( 0 != m_pSession ) {
    ou::db::QueryFields<Instrument::TableRowDef>::pQueryFields_t pQuery 
      = m_pSession->Insert<Instrument::TableRowDef>( const_cast<Instrument::TableRowDef&>( pInstrument->GetRow() ) );
    // save alternate instrument names
    pInstrument->ScanAlternateNames( boost::phoenix::bind(
      static_cast<void(InstrumentManager::*)(const keytypes::eidProvider_t&, const keytypes::idInstrument_t&, const keytypes::idInstrument_t&)>(&InstrumentManager::SaveAlternateInstrumentName), 
        this, boost::phoenix::arg_names::arg1, boost::phoenix::arg_names::arg2, boost::phoenix::arg_names::arg3 ) );
  }
}

void InstrumentManager::SaveAlternateInstrumentName( 
    const keytypes::eidProvider_t& idProvider, const keytypes::idInstrument_t& idAlternate, const keytypes::idInstrument_t& idInstrument ) {
  AlternateInstrumentName::TableRowDef row( idProvider, idAlternate, idInstrument );
  SaveAlternateInstrumentName( row );
}

void InstrumentManager::SaveAlternateInstrumentName( const AlternateInstrumentName::TableRowDef& row ) {
  if ( 0 != m_pSession ) {
    ou::db::QueryFields<AlternateInstrumentName::TableRowDef>::pQueryFields_t pQuery 
      = m_pSession->Insert<AlternateInstrumentName::TableRowDef>( const_cast<AlternateInstrumentName::TableRowDef&>( row ) );
  }
}

void InstrumentManager::Assign( pInstrument_cref pInstrument ) {
  if ( m_map.end() != m_map.find( pInstrument->GetInstrumentName() ) ) {
    throw std::runtime_error( "InstrumentManager::Assign instrument already exists: " + pInstrument->GetInstrumentName() );
  }
  else {
    m_map.insert( pair_t( pInstrument->GetInstrumentName(), pInstrument ) );
  }
  pInstrument->OnAlternateNameAdded.Add( MakeDelegate( this, &InstrumentManager::HandleAlternateNameAdded ) );
  pInstrument->OnAlternateNameChanged.Add( MakeDelegate( this, &InstrumentManager::HandleAlternateNameChanged ) );
}

InstrumentManager::pInstrument_t InstrumentManager::Get( idInstrument_cref idName ) {
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
      throw std::runtime_error( "InstrumentManager::Get can't find idInstrument: " + pInstrument->GetInstrumentName() );
    }
  }
  return pInstrument;
}

bool InstrumentManager::Exists( idInstrument_cref id ) {  // todo:  cache the query to make the get faster rather than searching the map again
  bool bFound = ( m_map.end() != m_map.find( id ) );
  if ( !bFound ) {
    if ( 0 != m_pSession ) {
      Instrument::pInstrument_t pInstrument;
      bFound = LoadInstrument( id, pInstrument );
    }
  }
  return bFound;
}

bool InstrumentManager::Exists( pInstrument_cref pInstrument ) {
  return Exists( pInstrument->GetInstrumentName() );
}

bool InstrumentManager::Exists( idInstrument_cref id, pInstrument_t& pInstrument ) {
  bool bFound( false );
  map_t::const_iterator iter( m_map.find( id ) );
  if ( m_map.end() != iter ) {
    pInstrument = iter->second;
    bFound = true;
  }
  else {
    if ( 0 != m_pSession ) {
//      Instrument::pInstrument_t pInstrument;
      bFound = LoadInstrument( id, pInstrument );
    }
  }
  return bFound;
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

bool InstrumentManager::LoadInstrument( idInstrument_t id, pInstrument_t& pInstrument ) {
      // ** as an aside, need transaction when writing instrument, underlying, and alternate names to database to ensure correctness
  assert( 0 != m_pSession );
  assert( m_map.end() == m_map.find( id ) );  // ensures we havn't already loaded an instrument

  bool bFound = false;
  InstrumentManagerQueries::InstrumentKey idInstrument( id );
  ou::db::QueryFields<InstrumentManagerQueries::InstrumentKey>::pQueryFields_t pExistsQuery // shouldn't do a * as fields may change order
    = m_pSession->SQL<InstrumentManagerQueries::InstrumentKey>( "select * from instruments", idInstrument ).Where( "instrumentid = ?" ).NoExecute();
  m_pSession->Bind<InstrumentManagerQueries::InstrumentKey>( pExistsQuery );
  if ( m_pSession->Execute( pExistsQuery ) ) {  // <- need to be able to execute on query pointer, since there is session pointer in every query
    Instrument::TableRowDef instrument;
    m_pSession->Columns<InstrumentManagerQueries::InstrumentKey, Instrument::TableRowDef>( pExistsQuery, instrument );
//    assert( ( ( "" != instrument.idUnderlying ) && ( ( InstrumentType::Option == instrument.eType ) || ( InstrumentType::FuturesOption == instrument.eType ) ) )
//         || ( ( "" == instrument.idUnderlying ) && (   InstrumentType::Option != instrument.eType ) && ( InstrumentType::FuturesOption != instrument.eType ) ) 
//      );
//    if ( "" == instrument.idUnderlying ) {
      pInstrument.reset( new Instrument( instrument ) );
      Assign( pInstrument );
      LoadAlternateInstrumentNames( pInstrument );  // comes after assign
      bFound = true;
//    }
//    else {
//      pInstrument_t pUnderlying;
//      iterMap iter = m_map.find( instrument.idUnderlying );
//      if ( m_map.end() != iter ) {
//        pUnderlying = iter->second;
//        bFound = true;
//      }
//      else {
//        bFound = LoadInstrument( instrument.idUnderlying, pUnderlying );
//        if ( !bFound ) {
//          throw std::runtime_error( "LoadInstrument: underlying does not exist in database" );
//        }
//      }
//      assert( ( InstrumentType::Stock == pUnderlying->GetInstrumentType() ) || ( InstrumentType::Future == pUnderlying->GetInstrumentType() ) );
//      pInstrument.reset( new Instrument( instrument, pUnderlying ) );
//      Assign( pInstrument );
//      LoadAlternateInstrumentNames( pInstrument );  // comes after assign
//    }
  }
  return bFound;
}

void InstrumentManager::Delete( idInstrument_cref idInstrument ) {
  // check if has dependencies first, and exception if there are 
  // then delete alternate instrument names
  // then delete instrument
}

void InstrumentManager::LoadAlternateInstrumentNames( pInstrument_t& pInstrument ) {
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

void InstrumentManager::HandleAlternateNameAdded( const Instrument::AlternateNameChangeInfo_t& info ) {
  iterMap iterKey = m_map.find( info.s1 );
  iterMap iterAlt = m_map.find( info.s2 );
  if ( m_map.end() == iterKey ) 
    throw std::runtime_error( "InstrumentManager::HandleAlternateNameAdded key does not exist" );
  if ( m_map.end() != iterAlt ) 
    throw std::runtime_error( "InstrumentManager::HandleAlternateNameAdded alt exists" );
  m_map.insert( pair_t( info.s2, iterKey->second ) );
  SaveAlternateInstrumentName( info.id, info.s2, info.s1 );
}

void InstrumentManager::HandleAlternateNameChanged( const Instrument::AlternateNameChangeInfo_t& info ) { // todo: need to update database
  iterMap iterOld = m_map.find( info.s1 );
  if ( m_map.end() == iterOld ) 
    throw std::runtime_error( "InstrumentManager::HandleAlternateNameChanged old name not found" );
  iterMap iterNew = m_map.find( info.s2 );
  if ( m_map.end() != iterNew ) 
    throw std::runtime_error( "InstrumentManager::HandleAlternateNameChanged new name already exists" );
  m_map.insert( pair_t( info.s2, iterOld->second ) );
  iterOld = m_map.find( info.s1 );  // load again to ensure proper copy
  m_map.erase( iterOld );
  // need database delete
  // need database insert
}

void InstrumentManager::HandleRegisterTables( ou::db::Session& session ) {
  session.RegisterTable<Exchange::TableCreateDef>( tablenames::sExchange );
  session.RegisterTable<Instrument::TableCreateDef>( tablenames::sInstrument );
  session.RegisterTable<AlternateInstrumentName::TableCreateDef>( tablenames::sAltInstrumentName );
}

void InstrumentManager::HandleRegisterRows( ou::db::Session& session ) {
  session.MapRowDefToTableName<Exchange::TableRowDef>( tablenames::sExchange );
  session.MapRowDefToTableName<Instrument::TableRowDef>( tablenames::sInstrument );
  session.MapRowDefToTableName<AlternateInstrumentName::TableRowDef>( tablenames::sAltInstrumentName );
}

void InstrumentManager::HandlePopulateTables( ou::db::Session& session ) {

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

  Exchange::TableRowDef exchange;
  exchange.idCountry = "US";

  std::vector<std::string>::iterator iter = vsExchangesPreload.begin();

  ou::db::QueryFields<Exchange::TableRowDef>::pQueryFields_t pExchange = session.Insert<Exchange::TableRowDef>( exchange ).NoExecute();

  while ( vsExchangesPreload.end() != iter ) {
    exchange.idExchange = *(iter++);
    exchange.sName = *(iter++);
    session.Reset( pExchange );
    session.Bind<Exchange::TableRowDef>( pExchange );
    session.Execute( pExchange );
  }

}

// this stuff could probably be rolled into Session with a template
void InstrumentManager::AttachToSession( ou::db::Session* pSession ) {
  ManagerBase::AttachToSession( pSession );
  pSession->OnRegisterTables.Add( MakeDelegate( this, &InstrumentManager::HandleRegisterTables ) );
  pSession->OnRegisterRows.Add( MakeDelegate( this, &InstrumentManager::HandleRegisterRows ) );
  pSession->OnPopulate.Add( MakeDelegate( this, &InstrumentManager::HandlePopulateTables ) );

}

void InstrumentManager::DetachFromSession( ou::db::Session* pSession ) {
  pSession->OnRegisterTables.Remove( MakeDelegate( this, &InstrumentManager::HandleRegisterTables ) );
  pSession->OnRegisterRows.Remove( MakeDelegate( this, &InstrumentManager::HandleRegisterRows ) );
  pSession->OnPopulate.Remove( MakeDelegate( this, &InstrumentManager::HandlePopulateTables ) );
  ManagerBase::DetachFromSession( pSession );
}


/*
Instrument::pInstrument_t InstrumentManager::GetIQFeedInstrument(const std::string &sName) {
//  return GetIQFeedInstrument( sName, sName );
  try {
    Instrument::pInstrument_t pInstrument( file.CreateInstrumentFromIQFeed( sName ) );  // todo:  need to verify proper symbol usage
    return pInstrument;
  }
  catch (...) {
    throw std::runtime_error( "InstrumentManager::GetIQFeedInstrument problems" );
  }
}
*/
/*
Instrument::pInstrument_t InstrumentManager::GetIQFeedInstrument(const std::string &sName, const std::string &sAlternateName) {
  try {
    Instrument::pInstrument_t pInstrument( file.CreateInstrumentFromIQFeed( sName, sAlternateName ) );  // todo:  need to verify proper symbol usage
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
