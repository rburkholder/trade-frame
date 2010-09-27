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

CAccountAdvisor::CAccountAdvisor( void )
: OU_DB_INITIALIZE_STRUCTURES(AccountAdvisor, OU_TABLE_ACCOUNTADVISOR_RECORD_FIELDS)
{
  m_recAccountAdvisor.nVersion = 1010101; // version 1.1.1.1
}

CAccountAdvisor::CAccountAdvisor( const std::string& sAdvisorId, const std::string& sAdvisorName )
: OU_DB_INITIALIZE_STRUCTURES(AccountAdvisor, OU_TABLE_ACCOUNTADVISOR_RECORD_FIELDS)
{
  m_recAccountAdvisor.nVersion = 1010101; // version 1.1.1.1
  SetAdvisorId( sAdvisorId );
  SetAdvisorName( sAdvisorName );
}

CAccountAdvisor::~CAccountAdvisor(void) {
}
