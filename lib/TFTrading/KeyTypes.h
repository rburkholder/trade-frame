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

#include <boost/cstdint.hpp>

#include "TradingEnumerations.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace keytypes { // KeyTypes

// InstrumentManager
typedef std::string idInstrument_t;
typedef std::string idExchange_t;
// AccountManager
typedef std::string idAccountAdvisor_t;
typedef std::string idAccountOwner_t;
typedef std::string idAccount_t;
// CashManager
//typedef Currency::type idCashAccount_t;
typedef boost::int64_t idCashTransaction_t;
typedef std::string idCurrency_t;  // three character designation
// ProviderManager
//typedef boost::uint16_t idProvider_t;  // identifies instance of a provider
typedef idAccount_t idProvider_t;
enum eidProvider_t {
  EProviderUnknown=0, EProviderSimulator=100, EProviderIQF, EProviderIB, EProviderGNDT, EProviderCalc,
  EProviderUserBase=900/*, _EProviderCount*/ };
// PortfolioManager
typedef std::string idPortfolio_t;
typedef boost::int64_t idPosition_t;  // auto increment
// OrderManager
typedef boost::int64_t idOrder_t;  // supplied/sync'd with execution provider
typedef boost::int64_t idExecution_t; // auto increment

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
