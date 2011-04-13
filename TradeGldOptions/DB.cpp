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

#include "StdAfx.h"

#include <stdexcept>

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

#include <TFTrading/Managers.h>

#include "DB.h"

CDB::CDB(void): m_bOpened( false ) {
  ou::tf::Initialize();
}

CDB::~CDB(void) {
  Close();
  ou::tf::Denitialize();
}

void CDB::Open( const std::string& sDbName ) {

  if ( !m_bOpened ) {
    m_pSession.reset( new ou::db::CSession );
    if ( boost::filesystem::exists( sDbName ) ) {
      // open already created and loaded database
      m_pSession->Open( sDbName );
      ou::tf::InitializeManagersDb( m_pSession );
      ou::tf::RegisterRowDefinitions();
    }
    else {
      // create and build new database
      m_pSession->Open( sDbName, ou::db::EOpenFlagsAutoCreate );
      ou::tf::InitializeManagersDb( m_pSession );
      ou::tf::RegisterTablesForCreation();
      m_pSession->CreateTables();
      ou::tf::RegisterRowDefinitions();
      ou::tf::PopulateTables();
      if ( 0 != OnPopulateDatabaseHandler ) OnPopulateDatabaseHandler();
    }
    m_bOpened = true;
  }

}

void CDB::Close( void ) {
  if ( m_bOpened ) {
    m_bOpened = false;
    m_pSession->Close();
  }
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
    = m_pSession->SQL<OptionsQueryParameters>( 
    "select * from instruments", query ).Where( "underlyingid=? and type=? and year=? and month=? and day=?" ).OrderBy( "strike, optionside" ).NoExecute();

  ou::tf::CInstrument::TableRowDef instrument;  // can we put stuff directly into object?
  ou::tf::CInstrument::pInstrument_t pInstrument;
  m_pSession->Bind<OptionsQueryParameters>( pQuery );
  if ( m_pSession->Execute( pQuery ) ) {
    bFound = true;
    if ( NULL != OnNewInstrument ) {
      do {
        m_pSession->Columns<OptionsQueryParameters, ou::tf::CInstrument::TableRowDef>( pQuery, instrument );
        pInstrument.reset( new ou::tf::CInstrument( instrument, pUnderlying ) );
        OnNewInstrument( pInstrument );
      }
      while ( m_pSession->Execute( pQuery ) );
    }
  }

  return bFound;

}

