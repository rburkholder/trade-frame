/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

#include "stdafx.h"

#include "AccountAdvisor.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

AccountAdvisor::AccountAdvisor( const idAccountAdvisor_t& sAdvisorId, 
  const std::string& sAdvisorName, const std::string& sCompanyName ) 
: m_row( sAdvisorId, sAdvisorName, sCompanyName )
{
}

AccountAdvisor::AccountAdvisor( const TableRowDef& row ) : m_row( row ) {};

AccountAdvisor::~AccountAdvisor(void) {
}

} // namespace tf
} // namespace ou
