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


#include "StrategyStrangle.h"

namespace Strategy {

Strangle::Strangle( )
: Base<Strangle,ou::tf::option::Strangle>()
{
}

Strangle::Strangle( const Strangle&& rhs )
: Base<Strangle,ou::tf::option::Strangle>( std::move( rhs ) )
{
}

Strangle::~Strangle( ) {
}

//void Strangle::ChooseStrikes( vLegSelected_t&, const mapChains_t&, boost::gregorian::date, double lower, double upper ) {
//}

//void StrategyStrangle::ChooseStrikes( const double lower, const double upper, boost::gregorian::date date, const mapChains_t& mapChains ) {
//  citerChain_t citerChain = SelectChain( mapChains, date, nDaysToExpiry );
//  ou::tf::option::Strangle::strike_pair_t pair = m_combo.ChooseStrikes( citerChain->second, lower, upper );
//}

} // namespace Strategy
