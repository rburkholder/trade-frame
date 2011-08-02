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

#include <stdexcept>

#include <TFTrading/Managers.h>

#include "DB.h"

CDB::CDB(void): ou::db::CSession() {
  OnInitializeManagers.Add( &ou::tf::HandleInitializeManagers );
  OnPopulate.Add( MakeDelegate( this, &CDB::HandlePopulateTables ) );
  OnDenitializeManagers.Add( &ou::tf::HandleDenitializeManagers );
}

CDB::~CDB(void) {
  this->OnPopulate.Remove( MakeDelegate( this, &CDB::HandlePopulateTables ) );
  OnInitializeManagers.Remove( &ou::tf::HandleInitializeManagers );
  OnDenitializeManagers.Remove( &ou::tf::HandleDenitializeManagers );
}

void CDB::HandlePopulateTables( ou::db::CSession& session ) {
  if ( 0 != OnPopulateDatabaseHandler ) OnPopulateDatabaseHandler();
}

struct UnderlyingQueryParameter {  // can this be simplified like PorfolioQuery?
  template<class A>
  void Fields( A& a ) {
    ou::db::Field( a, "instrumentid", idInstrument );
  }
  const ou::tf::keytypes::idInstrument_t& idInstrument;
  //UnderlyingQuery( void ) {};
  UnderlyingQueryParameter( const ou::tf::keytypes::idInstrument_t& idInstrument_ ) : idInstrument( idInstrument_ ) {};
};

void CDB::LoadUnderlying( const ou::tf::keytypes::idInstrument_t& id, ou::tf::CInstrument::pInstrument_t& pInstrument ) {
  pInstrument = ou::tf::CInstrumentManager::Instance().Get( id );
}

struct OptionsQueryParameters {
  template<class A>
  void Fields( A& a ) {
    ou::db::Field( a, "underlyingid", idUnderlying );
    ou::db::Field( a, "type", eType );
    ou::db::Field( a, "year", nYear );
    ou::db::Field( a, "month", nMonth );
    ou::db::Field( a, "day", nDay );
  }
  const ou::tf::keytypes::idInstrument_t& idUnderlying;
  boost::uint16_t nYear, nMonth, nDay;
  ou::tf::InstrumentType::enumInstrumentTypes eType;
  OptionsQueryParameters( const ou::tf::keytypes::idInstrument_t& id, boost::uint16_t nYear_, boost::uint16_t nMonth_, boost::uint16_t nDay_ )
    : idUnderlying( id ), nYear( nYear_ ), nMonth( nMonth_ ), nDay( nDay_ ), eType( ou::tf::InstrumentType::Option ) {};
};

bool CDB::LoadOptions( ou::tf::CInstrumentManager::pInstrument_t& pUnderlying, boost::uint16_t nYear, boost::uint16_t nMonth, boost::uint16_t nDay ) {

  bool bFound = false;
  OptionsQueryParameters query( pUnderlying->GetInstrumentName(), nYear, nMonth, nDay );

  ou::db::QueryFields<OptionsQueryParameters>::pQueryFields_t pQuery 
    = SQL<OptionsQueryParameters>( 
      "select * from instruments", query ).Where( "underlyingid=? and type=? and year=? and month=? and day=?" ).OrderBy( "strike, optionside" ).NoExecute();

  ou::tf::CInstrument::TableRowDef instrument;  // can we put stuff directly into object?
  ou::tf::CInstrument::pInstrument_t pInstrument;
  Bind<OptionsQueryParameters>( pQuery );
  if ( Execute( pQuery ) ) {
    bFound = true;
    if ( NULL != OnNewInstrument ) {
      do {
        Columns<OptionsQueryParameters, ou::tf::CInstrument::TableRowDef>( pQuery, instrument );
        pInstrument.reset( new ou::tf::CInstrument( instrument, pUnderlying ) );
        OnNewInstrument( pInstrument );
      }
      while ( Execute( pQuery ) );
    }
  }

  return bFound;

}

