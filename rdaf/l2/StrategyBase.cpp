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
 * File:    Strategy.cpp
 * Author:  raymond@burkholder.net
 * Project: rdaf/l2
 * Created: March 11, 2034  10:58:03
 */

#include <boost/log/trivial.hpp>

#include "StrategyBase.hpp"

namespace Strategy {

Base::Base( const ou::tf::config::symbol_t& config )
: m_config( config )
 {}

Base::~Base() {}

void Base::SetPosition( pPosition_t pPosition ) {

  assert( pPosition );
  m_pPosition = pPosition;

  m_cdv.SetNames( "AutoTrade", pPosition->GetInstrument()->GetInstrumentName() );

  BOOST_LOG_TRIVIAL(info)
    << "Strategy::SetPosition " << m_config.sSymbol_IQFeed
    //<< ": algorithm='" << m_config.sAlgorithm
    << " signal_from='" <<m_config.sSignalFrom
    << "'"
    ;
}

void Base::SaveWatch( const std::string& sPrefix ) {
  if ( m_pPosition ) {
    m_pPosition->GetWatch()->SaveSeries( sPrefix );
  }
}

} // namespace Strategy
