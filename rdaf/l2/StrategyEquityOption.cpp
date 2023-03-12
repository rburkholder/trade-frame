/************************************************************************
 * Copyright(c) 2023, One Unified. All rights reserved.                 *
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
 * File:    StrategyEquityOption.cpp
 * Author:  raymond@burkholder.net
 * Project: rdaf/l2
 * Created: March 11, 2034  10:58:03
 */

#include "StrategyEquityOption.hpp"

namespace Strategy {

EquityOption::EquityOption(
  const ou::tf::config::symbol_t& config
)
: Base( config )
{}

EquityOption::~EquityOption() {
  if ( m_pPosition ) {
    pWatch_t pWatch = m_pPosition->GetWatch();
    pWatch->OnQuote.Remove( MakeDelegate( this, &EquityOption::HandleQuote ) );
    pWatch->OnTrade.Remove( MakeDelegate( this, &EquityOption::HandleTrade ) );
  }
}

void EquityOption::SetPosition( pPosition_t pPosition ) {

  Base::SetPosition( pPosition );

  pWatch_t pWatch = m_pPosition->GetWatch();
  pWatch->OnQuote.Add( MakeDelegate( this, &EquityOption::HandleQuote ) );
  pWatch->OnTrade.Add( MakeDelegate( this, &EquityOption::HandleTrade ) );
}

void EquityOption::HandleQuote( const ou::tf::Quote& quote ) {}

void EquityOption::HandleTrade( const ou::tf::Trade& trade ) {}

} // namespace Strategy
