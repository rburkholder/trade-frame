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

#include "RunningMinMax.h"

// look up Tirone Levels, page 335, in Tech Anal, A-Z

namespace ou { // One Unified
namespace tf { // TradeFrame

CRunningMinMax::CRunningMinMax(void) 
: m_dblMax( 0 ), m_dblMin( 0 )
{
}

CRunningMinMax::~CRunningMinMax(void) {
  std::map<double, CRMMPointStat*>::iterator iter;
  CRMMPointStat* ps;
  iter = m_mapPointStats.begin();
  while ( m_mapPointStats.end() != iter ) {
    ps = iter->second;
    iter->second = NULL;
    delete ps;
    ++iter;
  }
  m_mapPointStats.clear();
}

void CRunningMinMax::Add(double val) {
  std::map<double, CRMMPointStat*>::iterator iter;
  
  iter = m_mapPointStats.find( val );
  if ( m_mapPointStats.end() == iter ) {
    m_mapPointStats.insert( m_mapPointStats_pair( val, new CRMMPointStat() ) );
    iter = m_mapPointStats.begin();
    m_dblMin = iter->first;
    iter = m_mapPointStats.end();
    --iter;
    m_dblMax = iter->first;
  }
  else {
    ++(iter->second->m_cntObjectsAtPrice);
  }
}

void CRunningMinMax::Remove(double val) {
  std::map<double, CRMMPointStat*>::iterator iter;
  
  iter = m_mapPointStats.find( val );
  if ( !(m_mapPointStats.end() == iter) ) {
    CRMMPointStat* ps = iter->second;
    --(ps->m_cntObjectsAtPrice);
    if ( 0 == ps->m_cntObjectsAtPrice ) {
      iter->second = NULL;
      delete ps;
      m_mapPointStats.erase( iter );
      if ( !m_mapPointStats.empty() ) {
        iter = m_mapPointStats.begin();
        m_dblMin = iter->first;
        iter = m_mapPointStats.end();
        --iter;
        m_dblMax = iter->first;
      }
    }
  }
}

} // namespace tf
} // namespace ou
