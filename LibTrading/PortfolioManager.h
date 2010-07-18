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

class CPortfolioManager: public ManagerBase<CPortfolioManager, std::string, CPortfolio> {
public:

  CPortfolioManager(void);
  ~CPortfolioManager(void);

  void Add( const std::string &sName );
  void Delete( const std::string &sName );
  CPortfolio *GetPortfolio( const std::string &sName );

protected:
private:
};
