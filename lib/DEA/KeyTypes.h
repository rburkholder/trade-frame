/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

// Started 20130518
#pragma once

#include <string>

#include <boost/cstdint.hpp>

#include <OUCommon/Decimal.h>

namespace ou { // One Unified
namespace dea { // double entry accounting
namespace keytypes { // KeyTypes

typedef dec::decimal6 money_t;

// User
typedef boost::uint32_t idUser_t;

// Privilege
typedef boost::uint32_t idPrivilege_t;

// ChartOfAccounts
typedef std::string idAccount_t;
typedef std::string idCurrency_t;

// GeneralJournal
typedef boost::uint64_t idTransaction_t;
typedef boost::uint64_t idReference_t;

} // keytypes

namespace tablenames { // tablenames

extern const std::string sUser;
extern const std::string sPrivilege;
extern const std::string sChartOfAccounts;
extern const std::string sGeneralJournal;

} // tablenames

} // dea
} // ou

