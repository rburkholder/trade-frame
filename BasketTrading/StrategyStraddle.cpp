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
 * File:    StrategyStraddle.cpp
 * Author:  raymond@burkholder.net
 * Project: BasketTrading
 * Created on July 4, 2019, 11:09 AM
 */

#include "StrategyStraddle.h"

namespace Strategy {

Straddle::Straddle( )
: Base<Straddle,ou::tf::option::Straddle>()
{
}

Straddle::Straddle( const Straddle&& rhs )
: Base<Straddle,ou::tf::option::Straddle>( std::move( rhs ) )
{
}

Straddle::~Straddle( ) { }

} // namespace Strategy