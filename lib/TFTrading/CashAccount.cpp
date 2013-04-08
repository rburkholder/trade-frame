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

#include "StdAfx.h"

#include "CashAccount.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

CashAccount::CashAccount( idCashAccount_t idCashAccount, const idAccount_t& idAccount, money_t mnyBalance, const currency_t& sCurrency ) 
  : m_row( idCashAccount, idAccount, mnyBalance, sCurrency )
{
}

CashAccount::CashAccount( money_t mnyBalance, const currency_t& sCurrency ) 
  : m_row( 0, "", mnyBalance, sCurrency )
{
}

CashAccount::~CashAccount(void) {
}

} // namespace tf
} // namespace ou
