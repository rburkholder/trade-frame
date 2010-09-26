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

#include "Account.h"

CAccount::CAccount(void)
: OU_DB_INITIALIZE_STRUCTURES(Account, OU_TABLE_ACCOUNT_RECORD_FIELDS)
{
  m_recAccount.nVersion = 01010101;
}

CAccount::~CAccount(void) {
}
