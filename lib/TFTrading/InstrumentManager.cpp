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

#include <vector>
#include <stdexcept>

#include <boost/phoenix/core.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>

#include "Exchange.h"
#include "InstrumentManager.h"
#include "AlternateInstrumentNames.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

InstrumentManager::InstrumentManager() {
}

InstrumentManager::~InstrumentManager() {
  for ( iterInstruments_t iter = m_mapInstruments.begin(); iter != m_mapInstruments.end(); ++iter ) {
    iter->second->OnAlternateNameAdded.Remove( MakeDelegate( this, &InstrumentManager::HandleAlternateNameAdded ) );
    iter->second->OnAlternateNameChanged.Remove( MakeDelegate( this, &InstrumentManager::HandleAlternateNameChanged ) );
  }
  m_mapInstruments.clear();
}

InstrumentManager::pInstrument_t InstrumentManager::ConstructInstrument(
  idInstrument_cref sInstrumentName, const std::string& sExchangeName, // generic
  InstrumentType::EInstrumentType type ) {
  pInstrument_t pInstrument( new Instrument( sInstrumentName, type, sExchangeName ) );
  Register( pInstrument );
  return pInstrument;
}

InstrumentManager::pInstrument_t InstrumentManager::ConstructFuture(
  idInstrument_cref sInstrumentName, const std::string& sExchangeName,  // future
  boost::uint16_t year, boost::uint16_t month, boost::uint16_t day ) {
  pInstrument_t pInstrument(
    new Instrument( sInstrumentName, InstrumentType::Future, sExchangeName, year, month, day ) );
  Register( pInstrument );
  return pInstrument;
}

InstrumentManager::pInstrument_t InstrumentManager::ConstructFuturesOption(
  idInstrument_cref sInstrumentName, const std::string& sExchangeName,  // option with yy mm dd
  boost::uint16_t year, boost::uint16_t month, boost::uint16_t day,
  OptionSide::EOptionSide side,
  double strike ) {
  pInstrument_t pInstrument(
    new Instrument( sInstrumentName, InstrumentType::FuturesOption, sExchangeName,
    year, month, day, side, strike ) );
  Register( pInstrument );
  return pInstrument;
}

InstrumentManager::pInstrument_t InstrumentManager::ConstructOption(
  idInstrument_cref sInstrumentName, const std::string& sExchangeName,  // option with yy mm dd
  boost::uint16_t year, boost::uint16_t month, boost::uint16_t day,
  OptionSide::EOptionSide side,
  double strike ) {
  pInstrument_t pInstrument(
    new Instrument( sInstrumentName, InstrumentType::Option, sExchangeName,
    year, month, day, side, strike ) );
  Register( pInstrument );
  return pInstrument;
}

InstrumentManager::pInstrument_t InstrumentManager::ConstructCurrency(
  idInstrument_cref idInstrumentName,
  const std::string& sExchangeName,
  Currency::ECurrency base, Currency::ECurrency counter ) {
  pInstrument_t pInstrument(
    new Instrument( idInstrumentName, /*idCounterInstrument,*/ InstrumentType::Currency, sExchangeName, base, counter ) );
  Register( pInstrument );
  return pInstrument;
}

void InstrumentManager::Register( pInstrument_t& pInstrument ) {
  // TODO: validate that IB id, IQF id, and IB contract present? ( or put into separate validator?)
  if ( Exists( pInstrument ) ) {
    throw std::runtime_error( "InstrumentManager::Register: instrument already exists" );
  }
  Assign( pInstrument );
  if ( nullptr != inherited_t::m_pSession ) {
    ou::db::QueryFields<Instrument::TableRowDef>::pQueryFields_t pQuery
      = m_pSession->Insert<Instrument::TableRowDef>( const_cast<Instrument::TableRowDef&>( pInstrument->GetRow() ) );
    // save alternate instrument names
    pInstrument->ScanAlternateNames(
      boost::phoenix::bind(
        static_cast<void(InstrumentManager::*)(const keytypes::eidProvider_t&, const keytypes::idInstrument_t&, const keytypes::idInstrument_t&, pInstrument_t)>(&InstrumentManager::SaveAlternateInstrumentName),
          this, boost::phoenix::arg_names::arg1, boost::phoenix::arg_names::arg2, boost::phoenix::arg_names::arg3, pInstrument
        ) );
  }
}

void InstrumentManager::SaveAlternateInstrumentName(
  const keytypes::eidProvider_t& idProvider,
  const keytypes::idInstrument_t& idAlternate,
  const keytypes::idInstrument_t& idInstrument,
  pInstrument_t pInstrument
) {
  // NOTE: this doesn't check if it exists in the database -- todo later?
  keyAltName_ref_t key( idProvider, idAlternate );
  mapAltNames_t::const_iterator iter = m_mapAltNames.find( key );
  if ( m_mapAltNames.end() != iter ) {
    std::cerr
      << "InstrumentManager::SaveAlternateInstrumentName alt name exists: "
      << idProvider << "," << idAlternate
      << " for " << pInstrument->GetInstrumentName()
      << std::endl;
  }
  else {
    m_mapAltNames.emplace( std::make_pair( key, pInstrument ) );
    const AlternateInstrumentName::TableRowDef row( idProvider, idAlternate, idInstrument );
    if ( nullptr != m_pSession ) {
      ou::db::QueryFields<AlternateInstrumentName::TableRowDef>::pQueryFields_t pQuery
        = m_pSession->Insert<AlternateInstrumentName::TableRowDef>( const_cast<AlternateInstrumentName::TableRowDef&>( row ) );
    }
  }
}

void InstrumentManager::Assign( pInstrument_cref pInstrument ) {
  //std::lock_guard<std::mutex> lock( m_mutex );
  if ( m_mapInstruments.end() != m_mapInstruments.find( pInstrument->GetInstrumentName() ) ) {
    throw std::runtime_error( "InstrumentManager::Assign instrument already exists: " + pInstrument->GetInstrumentName() );
  }
  else {
    m_mapInstruments.insert( mapInstruments_t::value_type( pInstrument->GetInstrumentName(), pInstrument ) );
  }
  pInstrument->OnAlternateNameAdded.Add( MakeDelegate( this, &InstrumentManager::HandleAlternateNameAdded ) );
  pInstrument->OnAlternateNameChanged.Add( MakeDelegate( this, &InstrumentManager::HandleAlternateNameChanged ) );
}

InstrumentManager::pInstrument_t InstrumentManager::Get( idInstrument_cref idName ) {
  //std::lock_guard<std::mutex> lock( m_mutex );
  pInstrument_t pInstrument;
  iterInstruments_t iter = m_mapInstruments.find( idName );
  if ( m_mapInstruments.end() != iter ) {
    pInstrument = iter->second;
  }
  else {
    bool bFound = false;
    if ( nullptr != m_pSession ) {
      bFound = LoadInstrument( idName, pInstrument );
    }
    if ( !bFound ) {
      throw std::runtime_error( "InstrumentManager::Get can't find idInstrument: " + pInstrument->GetInstrumentName() );
    }
  }
  return pInstrument;
}

bool InstrumentManager::Exists( idInstrument_cref id ) {  // todo:  cache the query to make the get faster rather than searching the map again
  //std::lock_guard<std::mutex> lock( m_mutex );
  bool bFound = ( m_mapInstruments.end() != m_mapInstruments.find( id ) );
  if ( !bFound ) {
    if ( nullptr != m_pSession ) {
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
  //std::lock_guard<std::mutex> lock( m_mutex );
  bool bFound( false );
  mapInstruments_t::const_iterator iter( m_mapInstruments.find( id ) );
  if ( m_mapInstruments.end() != iter ) {
    pInstrument = iter->second;
    bFound = true;
  }
  else {
    if ( nullptr != m_pSession ) {
//      Instrument::pInstrument_t pInstrument;
      bFound = LoadInstrument( id, pInstrument );
    }
  }
  return bFound;
}

struct InstrumentKey {
  template<class A>
  void Fields( A& a ) {
    ou::db::Field( a, "instrumentid", idInstrument );
  }
  const ou::tf::keytypes::idInstrument_t& idInstrument;
  InstrumentKey( const ou::tf::keytypes::idInstrument_t& idInstrument_ ): idInstrument( idInstrument_ ) {};
};

bool InstrumentManager::LoadInstrument( idInstrument_t id, pInstrument_t& pInstrument ) {
  std::lock_guard<std::mutex> lock( m_mutexLoadInstrument );
      // TODO: need transaction when writing instrument, underlying, and alternate names to database to ensure consistency
  assert( nullptr != m_pSession );
  assert( m_mapInstruments.end() == m_mapInstruments.find( id ) );  // ensures we havn't already loaded an instrument

  bool bFound = false;
  InstrumentKey idInstrument( id );
  ou::db::QueryFields<InstrumentKey>::pQueryFields_t pExistsQuery // shouldn't do a * as fields may change order
    = m_pSession->SQL<InstrumentKey>( "select * from instruments", idInstrument ).Where( "instrumentid = ?" ).NoExecute();
  m_pSession->Bind<InstrumentKey>( pExistsQuery );
  if ( m_pSession->Execute( pExistsQuery ) ) {  // <- need to be able to execute on query pointer, since there is session pointer in every query
    Instrument::TableRowDef instrument;
    m_pSession->Columns<InstrumentKey, Instrument::TableRowDef>( pExistsQuery, instrument );
    pInstrument = std::make_shared<Instrument>( instrument );
    Assign( pInstrument );
    LoadAlternateInstrumentNames( pInstrument );  // comes after assign
    bFound = true;
  }
  return bFound;
}

struct AltNameKey {
  template<class A>
  void Fields( A& a ) {
    ou::db::Field( a, "providerid", idProvider ); // part of unique key
    ou::db::Field( a, "alternateid", idAlternate ); // part of unique key
  }
  const ou::tf::keytypes::eidProvider_t& idProvider;
  const ou::tf::keytypes::idInstrument_t& idAlternate;
  AltNameKey(
    const ou::tf::keytypes::eidProvider_t& idProvider_,
    const ou::tf::keytypes::idInstrument_t& idAlternate_
    ):
    idProvider( idProvider_ ), idAlternate( idAlternate_ )
  {};
};

struct InstrumentName {
  template<class A>
  void Fields( A& a ) {
    ou::db::Field( a, "instrumentid", idInstrument );
  }
  ou::tf::keytypes::idInstrument_t idInstrument;
  InstrumentName() {};
};

InstrumentManager::pInstrument_t InstrumentManager::LoadInstrument( keytypes::eidProvider_t idProvider, const idInstrument_t& idInstrument ) {
  pInstrument_t pInstrument;
  AltNameKey key( idProvider, idInstrument );
  ou::db::QueryFields<AltNameKey>::pQueryFields_t pExistsQuery
    = m_pSession->SQL<AltNameKey>( "select instrumentid from altinstrumentnames", key )
        .Where( "providerid = ? and alternateid = ?" ).NoExecute();
  if ( m_pSession ) {
    m_pSession->Bind<AltNameKey>( pExistsQuery );
    InstrumentName result;
    if ( m_pSession->Execute( pExistsQuery ) ) { // should only be once
      m_pSession->Columns<AltNameKey, InstrumentName>( pExistsQuery, result );
      //LoadInstrument( result.idInstrument, pInstrument );
      bool bFound = Exists( result.idInstrument, pInstrument );
    }
  }
  return pInstrument;
}

void InstrumentManager::Delete( idInstrument_cref idInstrument ) {
  // check if has dependencies first, and exception if there are
  // then delete alternate instrument names
  // then delete instrument
}

void InstrumentManager::LoadAlternateInstrumentNames( pInstrument_t& pInstrument ) {
  assert( pInstrument );
  InstrumentKey idInstrument( pInstrument->GetInstrumentName() );
  ou::db::QueryFields<InstrumentKey>::pQueryFields_t pExistsQuery // shouldn't do a * as fields may change order
     = m_pSession->SQL<InstrumentKey>( "select * from altinstrumentnames", idInstrument ).Where( "instrumentid = ?" ).NoExecute();
  m_pSession->Bind<InstrumentKey>( pExistsQuery );
  AlternateInstrumentName::TableRowDef altname;
  while ( m_pSession->Execute( pExistsQuery ) ) {
    m_pSession->Columns<InstrumentKey, AlternateInstrumentName::TableRowDef>( pExistsQuery, altname );
    pInstrument->SetAlternateName( altname.idProvider, altname.idAlternate );
  }
}

// TODO: not functional, fix
// current reference:  lib/TFTrading/Instrument.cpp:235
void InstrumentManager::HandleAlternateNameAdded( const Instrument::AlternateNameChangeInfo_t& info ) {
  // TODO: need name of instrument being changed
  assert( false ); // TODO: need to fix implementation
  iterInstruments_t iterKey = m_mapInstruments.find( info.s1 );
  iterInstruments_t iterAlt = m_mapInstruments.find( info.s2 );
  if ( m_mapInstruments.end() == iterKey )
    throw std::runtime_error( "InstrumentManager::HandleAlternateNameAdded key does not exist" );
  if ( m_mapInstruments.end() != iterAlt )
    throw std::runtime_error( "InstrumentManager::HandleAlternateNameAdded alt exists" );
  m_mapInstruments.insert( mapInstruments_t::value_type( info.s2, iterKey->second ) );
  //SaveAlternateInstrumentName( info.id, info.s2, info.s1 );
}

// TODO: not functional, fix
void InstrumentManager::HandleAlternateNameChanged( const Instrument::AlternateNameChangeInfo_t& info ) { // todo: need to update database
// TODO: need name of instrument being changed
  assert( false );  // TODO: not a complete implementation
  iterInstruments_t iterOld = m_mapInstruments.find( info.s1 );
  if ( m_mapInstruments.end() == iterOld )
    throw std::runtime_error( "InstrumentManager::HandleAlternateNameChanged old name not found" );
  iterInstruments_t iterNew = m_mapInstruments.find( info.s2 );
  if ( m_mapInstruments.end() != iterNew )
    throw std::runtime_error( "InstrumentManager::HandleAlternateNameChanged new name already exists" );
  m_mapInstruments.insert( mapInstruments_t::value_type( info.s2, iterOld->second ) );
  iterOld = m_mapInstruments.find( info.s1 );  // load again to ensure proper copy
  m_mapInstruments.erase( iterOld );
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

  // TODO: need to add IB COMEX, CME, CME_MINI?
  std::vector<std::string> vsExchangesPreload {
    "NYSE", "New York Stock Exchange",
    "CBOT", "Chicago Board of Trade",
    "CBOE", "Chicago Board Options Exchange",
    "OPRA", "Options Price Reporting Authority",
    "NASDAQ", "NASDAQ",
    "NMS", "NMS",
    "SMART", "Interactive Brokers Smart"
  }
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

struct OptionSelection {
  template<class A>
  void Fields( A& a ) {
    ou::db::Field( a, "underlyingid", idInstrument );  // 20151227 idUnderlying has been removed, is this variable still useful?
    ou::db::Field( a, "year", nYear );
    ou::db::Field( a, "month", nMonth );
    ou::db::Field( a, "day", nDay );
  }
  const ou::tf::keytypes::idInstrument_t& idInstrument;
  boost::uint16_t nYear; // future, option
  boost::uint16_t nMonth; // future, option
  boost::uint16_t nDay; // future, option
  OptionSelection( const ou::tf::keytypes::idInstrument_t& idInstrument_, boost::uint16_t nYear_, boost::uint16_t nMonth_, boost::uint16_t nDay_ )
    : idInstrument( idInstrument_ ), nYear( nYear_ ), nMonth( nMonth_ ), nDay( nDay_ ) {};
};

struct OptionSymbolName {
  template<class A>
  void Fields( A& a ) {
    ou::db::Field( a, "instrumentid", idInstrument );
  }
  ou::tf::keytypes::idInstrument_t idInstrument;
  OptionSymbolName() {};
};

// currently unused
template<typename F>
void InstrumentManager::ScanOptions( F f, idInstrument_cref id, boost::uint16_t nYear, boost::uint16_t nMonth, boost::uint16_t nDay ) {
  OptionSelection idInstrument( id, nYear, nMonth, nDay );
  ou::db::QueryFields<OptionSelection>::pQueryFields_t pExistsQuery // shouldn't do a * as fields may change order
    = m_pSession->SQL<OptionSelection>(
      "select instrumentid from instruments", idInstrument ).Where( "underlyingid = ? and year = ? and month = ? and day = ?" ).NoExecute();
  m_pSession->Bind<OptionSelection>( pExistsQuery );
  OptionSymbolName name;
  pInstrument_t pInstrument;
  while ( m_pSession->Execute( pExistsQuery ) ) {  // <- need to be able to execute on query pointer, since there is session pointer in every query
    m_pSession->Columns<OptionSelection, OptionSymbolName>( pExistsQuery, name );
    pInstrument = Get( name.idInstrument );
    f( pInstrument );
  }
}

} // namespace tf
} // namespace ou
