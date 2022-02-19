/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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

/*
 * File:    db.cpp
 * Author:  raymond@burkholder.net
 * Project: AutoTrade
 * Created: February 19, 2022 13:04
 */

 #include <TFTrading/AccountManager.h>

#include "db.h"

namespace {
  const std::string sDbName = "AutoTrade.db";
}

db::db() {

  // TODO: need to test if this works properly, and is created fresh on a day by day basis
  m_sPortfolioStrategyAggregate = "started-" + boost::gregorian::to_iso_string( boost::gregorian::day_clock::local_day() );

  ou::tf::PortfolioManager& pm( ou::tf::PortfolioManager::GlobalInstance() );
  pm.OnPortfolioLoaded.Add( MakeDelegate( this, &db::HandlePortfolioLoad ) );
  pm.OnPositionLoaded.Add( MakeDelegate( this, &db::HandlePositionLoad ) );

  try {
    // try for day to day continuity
    //if ( boost::filesystem::exists( m_sDbName ) ) {
    //  boost::filesystem::remove( m_sDbName );
    //}

    m_db.OnLoad.Add( MakeDelegate( this, &db::HandleDbOnLoad ) );
    m_db.OnPopulate.Add( MakeDelegate( this, &db::HandleDbOnPopulate ) );
    m_db.OnRegisterTables.Add( MakeDelegate( this, &db::HandleRegisterTables ) );
    m_db.OnRegisterRows.Add( MakeDelegate( this, &db::HandleRegisterRows ) );
    m_db.SetOnPopulateDatabaseHandler( MakeDelegate( this, &db::HandlePopulateDatabase ) );
    m_db.SetOnLoadDatabaseHandler( MakeDelegate( this, &db::HandleLoadDatabase ) );

    m_db.Open( sDbName );
  }
  catch(...) {
    std::cout << "database fault on " << sDbName << std::endl;
  }

}

db::~db() {
  m_db.OnLoad.Remove( MakeDelegate( this, &db::HandleDbOnLoad ) );
  m_db.OnPopulate.Remove( MakeDelegate( this, &db::HandleDbOnPopulate ) );
  m_db.OnRegisterTables.Remove( MakeDelegate( this, &db::HandleRegisterTables ) );
  m_db.OnRegisterRows.Remove( MakeDelegate( this, &db::HandleRegisterRows ) );
  m_db.SetOnPopulateDatabaseHandler( nullptr );
  m_db.SetOnLoadDatabaseHandler( nullptr );

  if ( m_db.IsOpen() ) m_db.Close();

  ou::tf::PortfolioManager& pm( ou::tf::PortfolioManager::GlobalInstance() );
  pm.OnPortfolioLoaded.Remove( MakeDelegate( this, &db::HandlePortfolioLoad ) );
  pm.OnPositionLoaded.Remove( MakeDelegate( this, &db::HandlePositionLoad ) );
}

void db::HandleRegisterTables( ou::db::Session& session ) {
  // called when db created
  //std::cout << "db::HandleRegisterTables placeholder" << std::endl;
}

void db::HandleRegisterRows( ou::db::Session& session ) {
  // called when db created and when exists
  //std::cout << "db::HandleRegisterRows placeholder" << std::endl;
}

void db::HandlePopulateDatabase() {

  std::cout << "db::HandlePopulateDatabase" << std::endl;

  ou::tf::AccountManager::pAccountAdvisor_t pAccountAdvisor
    = ou::tf::AccountManager::Instance().ConstructAccountAdvisor( "aaTF", "Primary User", "TradeFrame" );

  ou::tf::AccountManager::pAccountOwner_t pAccountOwner
    = ou::tf::AccountManager::Instance().ConstructAccountOwner( "aoTF", "aaTF", "Primary", "User" );

  ou::tf::AccountManager::pAccount_t pAccountIB
    = ou::tf::AccountManager::Instance().ConstructAccount( "ib01", "aoTF", "Primary User", ou::tf::keytypes::EProviderIB, "Interactive Brokers", "acctid", "login", "password" );

  ou::tf::AccountManager::pAccount_t pAccountIQFeed
    = ou::tf::AccountManager::Instance().ConstructAccount( "iq01", "aoTF", "Primary User", ou::tf::keytypes::EProviderIQF, "IQFeed", "acctid", "login", "password" );

  ou::tf::AccountManager::pAccount_t pAccountSimulator
    = ou::tf::AccountManager::Instance().ConstructAccount( "sim01", "aoTF", "Primary User", ou::tf::keytypes::EProviderSimulator, "Sim", "acctid", "login", "password" );

  m_pPortfolioMaster  // owning portfolio: -none-
    = ou::tf::PortfolioManager::Instance().ConstructPortfolio(
    "Master", "aoTF", "", ou::tf::Portfolio::Master, ou::tf::Currency::Name[ ou::tf::Currency::USD ], "Master of all Portfolios" );

  m_pPortfolioCurrencyUSD  // owning portfolio: Master
    = ou::tf::PortfolioManager::Instance().ConstructPortfolio(
    "USD", "aoTF", "Master", ou::tf::Portfolio::CurrencySummary, ou::tf::Currency::Name[ ou::tf::Currency::USD ], "CurrencySummary of USD Portfolios" );

  m_pPortfolioStrategyAggregate  // owning portfolio: USD
    = ou::tf::PortfolioManager::Instance().ConstructPortfolio(
    m_sPortfolioStrategyAggregate, "aoTF", "USD", ou::tf::Portfolio::Basket, ou::tf::Currency::Name[ ou::tf::Currency::USD ], "Basket of Underlying Instances" );

}

void db::HandleDbOnPopulate(  ou::db::Session& session ) {
  // called when db created, after HandlePopulateDatabase
  //std::cout << "AppBasketTrading::HandleDbOnPopulate placeholder" << std::endl;
}

void db::HandleLoadDatabase( void ) {
  std::cout << "db::HandleLoadDatabase ..." << std::endl;
  ou::tf::PortfolioManager& pm( ou::tf::PortfolioManager::GlobalInstance() );
  pm.LoadActivePortfolios();
}

void db::HandleDbOnLoad(  ou::db::Session& session ) {
  // called when db exists, after HandleLoadDatabase
   //std::cout << "db::HandleDbOnLoad placeholder" << std::endl;
}

void db::HandlePortfolioLoad( pPortfolio_t& pPortfolio ) {
  switch ( pPortfolio->GetRow().ePortfolioType ) {
    case ou::tf::Portfolio::EPortfolioType::Basket:
      m_pPortfolioStrategyAggregate = pPortfolio;
      //BuildMasterPortfolio();
      break;
    case ou::tf::Portfolio::EPortfolioType::Standard: // not used
      //assert( m_pMasterPortfolio );
      //m_pMasterPortfolio->Add( pPortfolio );
      break;
    case ou::tf::Portfolio::EPortfolioType::Aggregate:
      //assert( m_pMasterPortfolio );
      //m_pMasterPortfolio->Add( pPortfolio );
      break;
    case ou::tf::Portfolio::EPortfolioType::MultiLeggedPosition:
      //assert( m_pMasterPortfolio );
      //m_pMasterPortfolio->Add( pPortfolio );
      break;
  }
}

void db::HandlePositionLoad( pPosition_t& pPosition ) {
  //m_pMasterPortfolio->Add( pPosition );
}

