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

DB::DB(void): ou::db::Session() {
  OnInitializeManagers.Add( &ou::tf::HandleInitializeManagers );
  OnPopulate.Add( MakeDelegate( this, &DB::HandlePopulateTables ) );
  OnDenitializeManagers.Add( &ou::tf::HandleDenitializeManagers );
}

DB::~DB(void) {
  this->OnPopulate.Remove( MakeDelegate( this, &DB::HandlePopulateTables ) );
  OnInitializeManagers.Remove( &ou::tf::HandleInitializeManagers );
  OnDenitializeManagers.Remove( &ou::tf::HandleDenitializeManagers );
}

void DB::HandlePopulateTables( ou::db::Session& session ) {
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

void DB::LoadUnderlying( const ou::tf::keytypes::idInstrument_t& id, ou::tf::Instrument::pInstrument_t& pInstrument ) {
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

bool DB::LoadOptions( ou::tf::CInstrumentManager::pInstrument_t& pUnderlying, boost::uint16_t nYear, boost::uint16_t nMonth, boost::uint16_t nDay ) {

  bool bFound = false;
  OptionsQueryParameters query( pUnderlying->GetInstrumentName(), nYear, nMonth, nDay );

  ou::db::QueryFields<OptionsQueryParameters>::pQueryFields_t pQuery 
    = SQL<OptionsQueryParameters>( 
      "select * from instruments", query ).Where( "underlyingid=? and type=? and year=? and month=? and day=?" ).OrderBy( "strike, optionside" ).NoExecute();

  ou::tf::Instrument::TableRowDef instrument;  // can we put stuff directly into object?
  ou::tf::Instrument::pInstrument_t pInstrument;
  Bind<OptionsQueryParameters>( pQuery );
  if ( Execute( pQuery ) ) {
    bFound = true;
    if ( NULL != OnNewInstrument ) {
      do {
        Columns<OptionsQueryParameters, ou::tf::Instrument::TableRowDef>( pQuery, instrument );
        pInstrument.reset( new ou::tf::Instrument( instrument, pUnderlying ) );
        OnNewInstrument( pInstrument );
      }
      while ( Execute( pQuery ) );
    }
  }

  return bFound;

}

