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

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace keytypes { // KeyTypes

typedef std::string idAccountAdvisor_t;
typedef std::string idAccountOwner_t;
typedef std::string idAccount_t;
typedef std::string idPortfolio_t;
typedef boost::int64_t idPosition_t;
typedef boost::int64_t idOrder_t;
typedef boost::int64_t idExecution_t;
typedef std::string idInstrument_t;
typedef std::string idExchange_t;

typedef boost::uint16_t idProvider_t;  // used in HDF5Attribute.h
enum eidProvider_t: idProvider_t {EProviderUnknown=0, EProviderSimulator=100, EProviderIQF, EProviderIB, EProviderGNDT, EProviderUserBase=900/*, _EProviderCount*/ };

} // namespace keytypes

namespace tablenames { // tablenames

extern std::string sAccountAdvisor;
extern std::string sAccountOwner;
extern std::string sAccount;
extern std::string sPortfolio;
extern std::string sPosition;
extern std::string sOrder;
extern std::string sExecution;
extern std::string sExchange;
extern std::string sInstrument;
extern std::string sAltInstrumentName;

} // namespace tablenames

} // namespace tf
} // namespace ou
