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

#include "StdAfx.h"

#include <stdexcept>

#include "OUCommon\TimeSource.h"

#include "Execution.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

Execution::Execution( 
  idExecution_t idExecution, idOrder_t idOrder,
  double dblPrice, boost::uint32_t nQuantity, OrderSide::enumOrderSide eOrderSide,
  const std::string& sExchange, const std::string& sExchangeExecutionId )
: 
  m_row( idExecution, idOrder, nQuantity, dblPrice, eOrderSide, sExchange, sExchangeExecutionId )
{
  assert( 0 < dblPrice );
  assert( 0 < nQuantity );
  m_row.dtExecutionTimeStamp = ou::TimeSource::LocalCommonInstance().Internal();
}

Execution::Execution( 
/*  idExecution_t idExecution, idOrder_t idOrder, */
  double dblPrice, boost::uint32_t nQuantity, OrderSide::enumOrderSide eOrderSide,
  const std::string& sExchange, const std::string& sExchangeExecutionId )
: 
  m_row( nQuantity, dblPrice, eOrderSide, sExchange, sExchangeExecutionId )
{
  assert( 0 < dblPrice );
  assert( 0 < nQuantity );
  m_row.dtExecutionTimeStamp = ou::TimeSource::LocalCommonInstance().Internal();
}

Execution::~Execution(void) {
}

} // namespace tf
} // namespace ou

