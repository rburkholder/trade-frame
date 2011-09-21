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

#include "Strike.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

Strike::Strike( void ) 
: m_dblStrike( 0 ),
  m_bWatching( false )
{
  //assert( false );  // see if it actaully gets called
}

Strike::Strike( double dblStrike ) 
: m_dblStrike( dblStrike ),
  m_bWatching( false )
{
}

Strike::Strike( const Strike& rhs ) 
: m_dblStrike( rhs.m_dblStrike ),
  m_call( rhs.m_call ), m_put( rhs.m_put ),
  m_bWatching( false )
{ 
  assert( !rhs.m_bWatching );
}

Strike::~Strike( void ) {
}

Strike& Strike::operator=( const Strike& rhs ) {
  assert( !rhs.m_bWatching );
  assert( !m_bWatching );
  m_dblStrike = rhs.m_dblStrike;
  m_call = rhs.m_call;
  m_put = rhs.m_put;
  return *this;
};

} // namespace option
} // namespace tf
} // namespace ou

