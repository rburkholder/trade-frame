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
 * File:    LegSelected.cpp
 * Author:  raymond@burkholder.net
 * Project: BasketTrading
 * Created on July 4, 2019, 10:08 PM
 */

#include "LegSelected.h"

LegSelected::LegSelected()
: m_bChanged( false ),
  m_dblStrike {}
{}

LegSelected::LegSelected( const LegSelected& rhs )
:  m_bChanged( rhs.m_bChanged ),
  m_dblStrike( rhs.m_dblStrike ),
  m_dateExpiry( rhs.m_dateExpiry ),
  m_sIQFeedName( rhs.m_sIQFeedName ),
  m_pOption( rhs.m_pOption )
{
}

LegSelected::LegSelected( const LegSelected&& rhs )
: m_bChanged( rhs.m_bChanged ),
  m_dblStrike( rhs.m_dblStrike ),
  m_dateExpiry( rhs.m_dateExpiry ),
  m_sIQFeedName( std::move( rhs.m_sIQFeedName ) ),
  m_pOption( std::move( rhs.m_pOption ) )
{
}

void LegSelected::Update( double strike, boost::gregorian::date dateExpiry, const std::string& sIQFeedName ) {
  m_bChanged = ( ( strike != m_dblStrike ) || ( dateExpiry != m_dateExpiry ) );
  if ( m_bChanged ) {
    m_dblStrike = strike;
    m_dateExpiry = dateExpiry;
    m_sIQFeedName = sIQFeedName;
  }
}

void LegSelected::Clear() {
  m_bChanged = false;
  m_dblStrike = 0.0;
  m_sIQFeedName.clear();
  m_pOption.reset();
}
