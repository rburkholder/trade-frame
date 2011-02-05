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
    const idAccount_t& idAccount,
    const idAccountOwner_t& idAccountOwner,
    const std::string& sAccountName,
    const std::string& sProviderName,
    const std::string& sBrokerName,
    const std::string& sBrokerAccountId,
    const std::string& sLogin, 
    const std::string& sPassword
    ) 
: m_row( idAccount, idAccountOwner, sAccountName,
  sProviderName, sBrokerName, sBrokerAccountId, sLogin, sPassword )
{
}

CAccount::~CAccount(void) {
}

} // namespace tf
} // namespace ou
