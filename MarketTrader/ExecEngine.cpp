/************************************************************************
 * Copyright(c) 2024, One Unified. All rights reserved.                 *
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
 * File:    ExecEngine.cpp
 * Author:  raymond@burkholder.net
 * Project: MarketTrader
 * Created: 2024/12/24 16:10:12
 */

#include "ExecEngine.hpp"

namespace ou {
namespace tf {
namespace engine {

Exec::Exec( pProvider_t p )
: m_pDataProvider( p )
{}

Exec::~Exec() {}

} // namespace engine
} // namespace tf
} // namespace ou