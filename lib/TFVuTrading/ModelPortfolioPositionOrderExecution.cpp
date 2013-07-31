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

#include "StdAfx.h"

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/home/phoenix/bind.hpp> 
#include <boost/spirit/home/phoenix/bind/bind_member_function.hpp>

#include "ModelPortfolioPositionOrderExecution.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

ModelPortfolioPositionOrderExecution::ModelPortfolioPositionOrderExecution(void) 
  : m_PortfolioManager( ou::tf::PortfolioManager::Instance() ), m_OrderManager( ou::tf::OrderManager::Instance() )
{
  m_PortfolioManager.LoadActivePortfolios();
}

ModelPortfolioPositionOrderExecution::~ModelPortfolioPositionOrderExecution(void) {
}

void ModelPortfolioPositionOrderExecution::LoadMasterPortfolio( void ) {
  // load the portfolio with "" as id
  m_PortfolioManager.ScanPortfolios( 
    boost::phoenix::bind( &ModelPortfolioPositionOrderExecution::ScanMasterPortfolioResults, this, boost::phoenix::arg_names::arg1 ) );
}

void ModelPortfolioPositionOrderExecution::ScanMasterPortfolioResults( const idPortfolio_t& portfolio ) {
}

} // namespace tf
} // namespace ou
