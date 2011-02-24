/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#pragma once

#include <string>
#include <map>

#include "ManagerBase.h"
#include "Portfolio.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class CPortfolioManager: public ManagerBase<CPortfolioManager, std::string, CPortfolio> {
public:

  typedef CPortfolio::pPortfolio_t pPortfolio_t;

  CPortfolioManager(void);
  ~CPortfolioManager(void);

  pPortfolio_t Create( const std::string& sName );
  pPortfolio_t GetPortfolio( const std::string &sName, bool bCreate = false );
  void Delete( const std::string& sName );

protected:

  typedef boost::uint32_t porfolio_key_t;
  typedef boost::uint32_t account_key_t;
  typedef boost::uint32_t string_key_t;
  
  struct structPorfolio {
    porfolio_key_t pk;
    account_key_t fkAccount;
    string_key_t fkPorfolioName;
  };

private:

  typedef std::pair<std::string, pPortfolio_t> m_mapPortfolios_pair;
  typedef std::map<std::string, pPortfolio_t> map_t;
  typedef map_t::iterator iterator;
  map_t m_mapPortfolios;

};

} // namespace tf
} // namespace ou
