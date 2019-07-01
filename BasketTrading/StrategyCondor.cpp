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
 * File:    StrategyCondor.cpp
 * Author:  raymond@burkholder.net
 * Project: BasketTrading
 * Created on June 23, 2019, 9:34 PM
 */

// utility has std::move
#include <utility>

#include "StrategyCondor.h"

namespace Strategy {

StrategyCondor::StrategyCondor( )
: Base<StrategyCondor,Condor>()
{
}

StrategyCondor::StrategyCondor( const StrategyCondor&& rhs )
: Base<StrategyCondor,Condor>( std::move( rhs ) )
{
}

StrategyCondor::~StrategyCondor( ) {
}

} // namespace Strategy