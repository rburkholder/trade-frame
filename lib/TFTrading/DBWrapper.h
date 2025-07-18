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
 * File:    DBWrapper.h
 * Author:  raymond@burkholder.net
 * Project: lib/TFTrading
 * Created: February 19, 2022 13:04
 */

#pragma once

// may need to inherit and add more functionality to the class:
#include <TFTrading/DBOps.h>
#include <TFTrading/PortfolioManager.h>

namespace ou {
namespace tf {

class db {
public:

  db( const std::string& sFileName );
  ~db();

protected:
private:

  using pPortfolio_t = ou::tf::PortfolioManager::pPortfolio_t;
  using pPosition_t  = ou::tf::PortfolioManager::pPosition_t;

  ou::tf::DBOps m_db;
  std::string m_sPortfolioStrategyAggregate;

  pPortfolio_t m_pPortfolioMaster;
  pPortfolio_t m_pPortfolioCurrencyUSD;

  void HandlePopulateDatabase();
  void HandleLoadDatabase();

  void HandlePortfolioLoad( pPortfolio_t& pPortfolio );
  void HandlePositionLoad( pPosition_t& pPosition );

  void HandleDbOnLoad( ou::db::Session& session );
  void HandleDbOnPopulate( ou::db::Session& session );
  void HandleRegisterTables( ou::db::Session& session );
  void HandleRegisterRows( ou::db::Session& session );
};

} // namespace tf
} // namespace ou
