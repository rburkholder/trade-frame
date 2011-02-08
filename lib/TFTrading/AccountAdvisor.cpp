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

#include "StdAfx.h"

#include "AccountAdvisor.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

const std::string CAccountAdvisor::m_sTableName = "accountadvisors";

CAccountAdvisor::CAccountAdvisor( const idAccountAdvisor_t& sAdvisorId, 
  const std::string& sAdvisorName, const std::string& sCompanyName ) 
: m_row( sAdvisorId, sAdvisorName, sCompanyName )
{
}

CAccountAdvisor::CAccountAdvisor( const TableRowDef& row ) : m_row( row ) {};

CAccountAdvisor::~CAccountAdvisor(void) {
}

} // namespace tf
} // namespace ou
