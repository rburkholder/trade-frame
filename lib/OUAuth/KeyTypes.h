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

// started 2013-05-19

#pragma once

#include <string>

#include <boost/cstdint.hpp>

namespace ou { // One Unified
namespace auth { // double entry accounting
namespace keytypes { // KeyTypes

// User
typedef boost::uint64_t idUser_t;

// Privilege
typedef boost::uint64_t idPrivilege_t;

} // keytypes

namespace tablenames { // tablenames

extern const std::string sUser;
extern const std::string sPrivilege;

} // tablenames

} // auth
} // ou

