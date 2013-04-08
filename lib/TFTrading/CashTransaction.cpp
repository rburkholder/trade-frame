/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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

#include "CashTransaction.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

CashTransaction::CashTransaction(
    idCashTransaction_t idCashTransaction, idCashAccount_t idCashAccount,
    const money_t& mnyCredit, const money_t& mnyDebit,
    const std::string& sDescription   ) 
    : m_row( idCashTransaction, idCashAccount, mnyCredit, mnyDebit, sDescription )
{
}

CashTransaction::CashTransaction(
    idCashTransaction_t idCashTransaction,
    const money_t& mnyCredit, const money_t& mnyDebit,
    const std::string& sDescription   ) 
    : m_row( idCashTransaction, 0, mnyCredit, mnyDebit, sDescription )
{
}

} // namespace tf
} // namespace ou
