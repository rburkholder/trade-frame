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

CAccountAdvisor::CAccountAdvisor( const std::string& sDbFileName, bool bPersist )
: OU_DB_INITIALIZE_STRUCTURES(AccountAdvisor, OU_TABLE_ACCOUNTADVISOR_RECORD_FIELDS),
  m_bPersist( bPersist )
{
  m_recAccountAdvisor.nVersion = 01010101;
}


CAccountAdvisor::~CAccountAdvisor(void) {
}
