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
 * File:    StrategyStrangle.cpp
 * Author:  raymond@burkholder.net
 * Project: BasketTrading
 * Created on June 23, 2019, 9:33 PM
 */

// utility has std::move
#include <utility>

#include "StrategyStrangle.h"

StrategyStrangle::StrategyStrangle( ) 
: StrategyBase()
{
}

StrategyStrangle::StrategyStrangle( const StrategyStrangle&& rhs )
: StrategyBase( std::move( rhs ) )
{
}

StrategyStrangle::~StrategyStrangle( ) {
}

