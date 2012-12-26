/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#pragma once

#include "Option.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

class Strike {
public:

  typedef ou::tf::ProviderInterfaceBase::pProvider_t pProvider_t;

  Strike( void );  // for construction in std::Map
  Strike( double dblStrike );
  Strike( const Strike& rhs );
  ~Strike( void );

  Strike& operator=( const Strike& rhs );

  bool operator< ( const Strike& rhs ) const { return m_dblStrike <  rhs.m_dblStrike; };
  bool operator<=( const Strike& rhs ) const { return m_dblStrike <= rhs.m_dblStrike; };

  double GetStrike( void ) const { return m_dblStrike; };

  void AssignCall( Instrument::pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider ) { 
    assert( 0 == m_call.use_count() ); 
    m_call.reset( new ou::tf::option::Call( pInstrument, pDataProvider, pGreekProvider ) ); 
  };
  void AssignPut( Instrument::pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider )  { 
    assert( 0 == m_put.use_count() );  
    m_put.reset( new ou::tf::option::Put( pInstrument, pDataProvider, pGreekProvider ) ); 
  };

  Call* Call( void ) { return m_call.get(); };
  Put*  Put( void )  { return m_put.get(); };

protected:

  boost::shared_ptr<ou::tf::option::Call> m_call;
  boost::shared_ptr<ou::tf::option::Put>  m_put;

private:
//  std::stringstream m_ss;
  bool m_bWatching;  // this needs to be implemented.
  double m_dblStrike;
};

} // namespace option
} // namespace tf
} // namespace ou

