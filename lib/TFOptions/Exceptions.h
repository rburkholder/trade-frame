/************************************************************************
 * Copyright(c) 2019, One Unified. All rights reserved.                 *
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

/*
 * File:    Exceptions.h
 * Author:  raymond@burkholder.net
 * Project: TFOptions
 * Created on July 4, 2019, 6:52 PM
 */

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

struct exception_strike_range_exceeded: public std::runtime_error {
  exception_strike_range_exceeded( const char* ch ): std::runtime_error( ch ) {}
};

struct exception_chain_not_found: public std::runtime_error {
  exception_chain_not_found( const char* ch ): std::runtime_error( ch ) {}
};

} // namespace option
} // namespace tf
} // namespace ou

#endif /* EXCEPTIONS_H */
