/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

// started 2013-04-06

#include "stdafx.h"

#include "CashAccount.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

CashAccount::CashAccount( const idAccount_t& idAccount, idCurrency_t idCurrency, money_t mnyBalance ) 
  : m_row( idAccount, idCurrency, mnyBalance )
{
}

CashAccount::CashAccount( idCurrency_t idCurrency, money_t mnyBalance ) 
  : m_row( "", idCurrency, mnyBalance )
{
}

CashAccount::~CashAccount(void) {
}

} // namespace tf
} // namespace ou
