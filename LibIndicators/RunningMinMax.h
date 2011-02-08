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

#pragma once

#include <map>

namespace ou { // One Unified
namespace tf { // TradeFrame

class CRMMPointStat {
public:
  CRMMPointStat(void) { m_cntObjectsAtPrice = 1; };
  virtual ~CRMMPointStat(void) {};

  unsigned int m_cntObjectsAtPrice;
};

class CRunningMinMax {
public:
  CRunningMinMax(void);
  virtual ~CRunningMinMax(void);

  virtual void Add( double );
  virtual void Remove( double );

  double m_dblMax;
  double m_dblMin;

protected:
  std::map<double, CRMMPointStat*> m_mapPointStats;
  typedef std::pair<double, CRMMPointStat*> m_mapPointStats_pair;
private:
};

} // namespace tf
} // namespace ou
