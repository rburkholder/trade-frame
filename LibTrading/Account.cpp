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

namespace ou { // One Unified
namespace tf { // TradeFrame

const std::string CAccount::m_sTableName = "accounts";

CAccount::CAccount(
    const keyAccountId_t& sAccountId,
    const keyAccountOwnerId_t& sAccountOwnerId,
    const std::string& sAccountName,
    const std::string& sProviderName,
    const std::string& sBrokerName,
    const std::string& sBrokerAccountId,
    const std::string& sLogin, 
    const std::string& sPassword
    ) 
: m_row( sAccountId, sAccountOwnerId, sAccountName,
  sProviderName, sBrokerName, sBrokerAccountId, sLogin, sPassword )
{
}

CAccount::~CAccount(void) {
}

} // namespace tf
} // namespace ou
