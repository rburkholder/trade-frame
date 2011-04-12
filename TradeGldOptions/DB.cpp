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

#include <TFTrading/AccountManager.h>
#include <TFTrading/ProviderManager.h>

#include <TFTrading/Managers.h>

#include "DB.h"

//using namespace ou::tf;

std::string CDB::m_sPortfolioId( "dn01" );

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
      Populate();
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

void CDB::Populate( void ) {

  ou::tf::CAccountManager::pAccountAdvisor_t pAccountAdvisor 
    = ou::tf::CAccountManager::Instance().ConstructAccountAdvisor( "ray", "Raymond Burkholder", "One Unified" );

  ou::tf::CAccountManager::pAccountOwner_t pAccountOwner
    = ou::tf::CAccountManager::Instance().ConstructAccountOwner( "ray", "ray", "Raymond", "Burkholder" );

  ou::tf::CAccountManager::pAccount_t pAccountIB
    = ou::tf::CAccountManager::Instance().ConstructAccount( "ib01", "ray", "Raymond Burkholder", ou::tf::keytypes::EProviderIB, "Interactive Brokers", "acctid", "login", "password" );

  ou::tf::CAccountManager::pAccount_t pAccountIQFeed
    = ou::tf::CAccountManager::Instance().ConstructAccount( "iq01", "ray", "Raymond Burkholder", ou::tf::keytypes::EProviderIQF, "IQFeed", "acctid", "login", "password" );

//  CPortfolio::TableRowDef portfolio( PortfolioId(), "ray", "Delta Neutral 01 - long und, long put" );
//  ou::db::QueryFields<CPortfolio::TableRowDef>::pQueryFields_t pPortfolio = m_session.Insert<CPortfolio::TableRowDef>( portfolio );

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

bool CDB::LoadOptions( const ou::tf::keytypes::idInstrument_t& idUnderlying, boost::uint16_t nYear, boost::uint16_t nMonth, boost::uint16_t nDay ) {

  bool bFound = false;
  OptionsQueryParameters query( idUnderlying, nYear, nMonth, nDay );

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
        pInstrument.reset( new ou::tf::CInstrument( instrument ) );
        OnNewInstrument( pInstrument );
      }
      while ( m_pSession->Execute( pQuery ) );
    }
  }

  return bFound;

}

