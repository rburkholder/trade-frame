/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

// Started 2013/07/30

#pragma once

#include <TFTrading/PortfolioManager.h>
#include <TFTrading/OrderManager.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

class ModelPortfolioPositionOrderExecution {
public:

  ModelPortfolioPositionOrderExecution(void);
  ~ModelPortfolioPositionOrderExecution(void);

  void LoadMasterPortfolio( void );

protected:
private:

  typedef Portfolio::idPortfolio_t idPortfolio_t;

  ou::tf::PortfolioManager& m_PortfolioManager;
  ou::tf::OrderManager& m_OrderManager;

  void ScanMasterPortfolioResults( const idPortfolio_t& );

};

} // namespace tf
} // namespace ou
