/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#include <TFTrading/Managers.h>

#include "DBOps.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

DBOps::DBOps(): ou::db::Session() {
  OnInitializeManagers.Add( &ou::tf::HandleInitializeManagers );
  OnDenitializeManagers.Add( &ou::tf::HandleDenitializeManagers );
  OnPopulate.Add( MakeDelegate( this, &DBOps::HandlePopulateTables ) );
  OnLoad.Add( MakeDelegate( this, &DBOps::HandleLoadTables ) );
}

DBOps::~DBOps() {
  OnLoad.Remove( MakeDelegate( this, &DBOps::HandleLoadTables ) );
  OnPopulate.Remove( MakeDelegate( this, &DBOps::HandlePopulateTables ) );
  OnInitializeManagers.Remove( &ou::tf::HandleInitializeManagers );
  OnDenitializeManagers.Remove( &ou::tf::HandleDenitializeManagers );
}

void DBOps::HandlePopulateTables( ou::db::Session& session ) {
  if ( nullptr != OnPopulateDatabaseHandler ) OnPopulateDatabaseHandler();
}

void DBOps::HandleLoadTables( ou::db::Session& session ) {
  if ( nullptr != OnLoadDatabaseHandler ) OnLoadDatabaseHandler();
}

//struct UnderlyingQueryParameter {  // can this be simplified like PorfolioQuery?
//  template<class A>
//  void Fields( A& a ) {
//    ou::db::Field( a, "instrumentid", idInstrument );
//  }
//  const ou::tf::keytypes::idInstrument_t& idInstrument;
  //UnderlyingQuery( void ) {};
//  UnderlyingQueryParameter( const ou::tf::keytypes::idInstrument_t& idInstrument_ ) : idInstrument( idInstrument_ ) {};
//};

//void DBOps::LoadUnderlying( const ou::tf::keytypes::idInstrument_t& id, ou::tf::Instrument::pInstrument_t& pInstrument ) {
//  pInstrument = ou::tf::InstrumentManager::Instance().Get( id );
//}

// ****
// ****  in actual fact, shouldn't need this any more.
// ****

struct OptionsQueryParameters {
  template<class A>
  void Fields( A& a ) {
//    ou::db::Field( a, "underlyingid", idUnderlying );
    ou::db::Field( a, "instrumentid", idInstrument);
    ou::db::Field( a, "type", eType );
    ou::db::Field( a, "year", nYear );
    ou::db::Field( a, "month", nMonth );
    ou::db::Field( a, "day", nDay );
  }
  const ou::tf::keytypes::idInstrument_t& idInstrument;
  boost::uint16_t nYear, nMonth, nDay;
  ou::tf::InstrumentType::EInstrumentType eType;
  OptionsQueryParameters( const ou::tf::keytypes::idInstrument_t& id, boost::uint16_t nYear_, boost::uint16_t nMonth_, boost::uint16_t nDay_ )
    : idInstrument( id ), nYear( nYear_ ), nMonth( nMonth_ ), nDay( nDay_ ), eType( ou::tf::InstrumentType::Option ) {};
};

//bool DBOps::LoadOptions( ou::tf::InstrumentManager::pInstrument_t& pUnderlying, boost::uint16_t nYear, boost::uint16_t nMonth, boost::uint16_t nDay ) {
bool DBOps::LoadOptions( idInstrument_cref sInstrumentName, boost::uint16_t nYear, boost::uint16_t nMonth, boost::uint16_t nDay ) {

  bool bFound = false;
  OptionsQueryParameters query( sInstrumentName, nYear, nMonth, nDay );

  ou::db::QueryFields<OptionsQueryParameters>::pQueryFields_t pQuery
    = SQL<OptionsQueryParameters>(
      "select * from instruments", query ).Where( "instrumentid=? and type=? and year=? and month=? and day=?" ).OrderBy( "strike, optionside" ).NoExecute();

  ou::tf::Instrument::TableRowDef instrument;  // can we put stuff directly into object?
  ou::tf::Instrument::pInstrument_t pInstrument;
  Bind<OptionsQueryParameters>( pQuery );
  if ( Execute( pQuery ) ) {
    bFound = true;
    if ( NULL != OnNewInstrument ) {
      do {
        Columns<OptionsQueryParameters, ou::tf::Instrument::TableRowDef>( pQuery, instrument );
        pInstrument.reset( new ou::tf::Instrument( instrument ) );
        OnNewInstrument( pInstrument );
      }
      while ( Execute( pQuery ) );
    }
  }

  return bFound;

}

} // namespace tf
} // namespace ou

