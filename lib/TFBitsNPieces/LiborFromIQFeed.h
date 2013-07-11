/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

// Started 2013/07/11

#pragma once

#include <vector>
#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;

#include <TFTrading/Watch.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace libor { // libor

typedef ou::tf::Watch::pProvider_t pProvider_t;

namespace local {

  typedef ou::tf::Watch::pWatch_t pWatch_t;

  struct structLibor {
    time_duration td;
    std::string Symbol;
    pWatch_t pWatch;
    structLibor( void ) {};
    structLibor( const time_duration& td_, const std::string& Symbol_ )
      : td( td_ ), Symbol( Symbol_ ) {};
  };

  struct compLibor {
    bool operator()( const structLibor& lhs, const structLibor& rhs ) const { return lhs.td < rhs.td; };
  };

  typedef std::vector<structLibor> vLibor_t;
  typedef vLibor_t::iterator vLibor_iter_t;
  extern vLibor_t vLibor;

  extern bool bInitialized;
  extern bool bWatching;

  extern pProvider_t pProvider;

  void Initialize( pProvider_t pProvider );
} 


void SetWatchOn( pProvider_t pProvider );
void SetWatchOff( void );

double Value( time_duration td );  // index to determine appropriate interest rate

} // namespace libor
} // namespace tf
} // namespace ou
