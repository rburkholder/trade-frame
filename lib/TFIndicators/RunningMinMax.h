/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

#include <map>

namespace ou { // One Unified
namespace tf { // TradeFrame

class RunningMinMax {
public:

  RunningMinMax(void);
  RunningMinMax( const RunningMinMax& );
  virtual ~RunningMinMax(void);

  virtual void Add( double );
  virtual void Remove( double );

  double Min() const { return m_dblMin; };
  double Max() const { return m_dblMax; };

  void Reset( void );

protected:
  typedef std::map<double,unsigned int> map_t;
  map_t m_mapPointStats;
  typedef std::pair<double, unsigned int> m_mapPointStats_pair_t;
private:
  double m_dblMax;
  double m_dblMin;
};

} // namespace tf
} // namespace ou
