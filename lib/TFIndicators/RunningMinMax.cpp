/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

#include "StdAfx.h"

#include <math.h>

#include "RunningMinMax.h"

// look up Tirone Levels, page 335, in Tech Anal, A-Z

namespace ou { // One Unified
namespace tf { // TradeFrame

RunningMinMax::RunningMinMax(void) 
: m_dblMax( 0 ), m_dblMin( 0 )
{
}

RunningMinMax::RunningMinMax( const RunningMinMax& rmm ) 
  : m_dblMax( rmm.m_dblMax ), m_dblMin( rmm.m_dblMin ),
  m_mapPointStats( rmm.m_mapPointStats )
{
}

RunningMinMax::~RunningMinMax(void) {
  m_mapPointStats.clear();
}

void RunningMinMax::Add(double val) {
  map_t::iterator iter;
  
  iter = m_mapPointStats.find( val );
  if ( m_mapPointStats.end() == iter ) {
    m_mapPointStats.insert( m_mapPointStats_pair_t( val, 1 ) );
    m_dblMin = m_mapPointStats.begin()->first;
    m_dblMax = m_mapPointStats.rbegin()->first;

  }
  else {
    ++(iter->second);
  }
}

void RunningMinMax::Remove(double val) {
  map_t::iterator iter;
  
  iter = m_mapPointStats.find( val );
  if ( (m_mapPointStats.end() == iter) ) {
    int i = 1;
  }
  else {
    --(iter->second);
    if ( 0 == iter->second ) {
      m_mapPointStats.erase( iter );
      if ( !m_mapPointStats.empty() ) {
        m_dblMin = m_mapPointStats.begin()->first;
        m_dblMax = m_mapPointStats.rbegin()->first;
      }
    }
  }
}

} // namespace tf
} // namespace ou
