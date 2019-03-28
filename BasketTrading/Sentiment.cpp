/*** *********************************************************************
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
 * File:   Sentiment.cpp
 * Author:    raymond@burkholder.net
 * copyright: 2019 Raymond Burkholder
 *
 * Created on March 26, 2019, 10:05 PM
 */

#include "Sentiment.h"

Sentiment::Sentiment()
  : nUp {}, nDown {}, dtCurrent( boost::date_time::special_values::not_a_date_time )
  {}

void Sentiment::Reset( ptime dtNew ) { // will probably need a lock
  nUp = 0;
  nDown = 0;
  dtCurrent = dtNew;
}

void Sentiment::Update( const ou::tf::Bar& bar ) {
  if ( dtCurrent.is_not_a_date_time() ) dtCurrent = bar.DateTime();
  if ( bar.DateTime() > dtCurrent ) Reset( bar.DateTime() );
  if ( bar.DateTime() == dtCurrent ) { // excludes late (low liquidity) bars
    if ( bar.Open() < bar.Close() ) nUp++;
    if ( bar.Open() > bar.Close() ) nDown++;
  }
}

void Sentiment::Get( size_t& nUp_, size_t& nDown_ ) const { // will probably need a lock
  nUp_ = nUp;
  nDown_ = nDown;
}
