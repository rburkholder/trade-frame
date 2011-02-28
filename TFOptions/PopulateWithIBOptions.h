/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#pragma once

#include <string>

#include "boost/date_time/gregorian/gregorian.hpp"

#include <OUSQL/Session.h>

#include <TFInteractiveBrokers/IBTWS.h>
#include <TFTrading/Instrument.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

// returns number of options retrieved
template<class DB>
class PopulateOptions {
public:

  PopulateOptions( ou::db::CSession<DB>& session, CIBTWS& tws );
  ~PopulateOptions( void ) {};

  Populate( const std::string& sUnderlying, boost::gregorian::date expiry, bool bCall, bool bPut );
protected:
private:
  PopulateWithIBOptions( void );
};

template<class DB>
PopulateOptions::PopulateOptions( ou::db::CSession<DB>& session, CIBTWS& tws ) {
  // assert( session active? );
  assert( tws.Connected() );
}
  
template<class DB>
unsigned int PopulateOptions::Populate( const std::string& sUnderlying, boost::gregorian::date expiry, bool bCall, bool bPut ) {
  unsigned int n( 0 );  // start with no options retrieved
}

} // namespace tf
} // namespace ou

