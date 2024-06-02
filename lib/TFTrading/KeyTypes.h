/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#pragma once

#include <string>

#include "TradingEnumerations.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace keytypes { // KeyTypes

// InstrumentManager
using idInstrument_t = std::string;
using idExchange_t = std::string;
// AccountManager
using idAccountAdvisor_t = std::string;
using idAccountOwner_t = std::string;
using idAccount_t = std::string;
// CashManager
//typedef Currency::type idCashAccount_t;
using idCashTransaction_t = std::int64_t;
using idCurrency_t = std::string;  // three character designation
// ProviderManager
//typedef boost::uint16_t idProvider_t;  // identifies instance of a provider
using idProvider_t = idAccount_t;
enum eidProvider_t {
  EProviderUnknown=0, EProviderSimulator=100, EProviderIQF, EProviderIB, EProviderGNDT, EProviderCalc, EProviderAlpaca, EProviderPhemex
, EProviderUserBase=900/*, _EProviderCount*/ };
// PortfolioManager
using idPortfolio_t = std::string;
using idPosition_t = std::int64_t;  // auto increment
// OrderManager
using idOrder_t = std::int64_t;  // supplied/sync'd with execution provider
using idExecution_t = std::int64_t; // auto increment

} // namespace keytypes

namespace tablenames { // tablenames

extern const std::string sAccountAdvisor;
extern const std::string sAccountOwner;
extern const std::string sCashAccount;
extern const std::string sCashTransaction;
extern const std::string sAccount;
extern const std::string sPortfolio;
extern const std::string sPosition;
extern const std::string sOrder;
extern const std::string sExecution;
extern const std::string sExchange;
extern const std::string sInstrument;
extern const std::string sAltInstrumentName;

} // namespace tablenames

} // namespace tf
} // namespace ou
