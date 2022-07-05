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

#include <functional>

#include "RunningMinMax.h"
#include "TimeSeriesSlidingWindow.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

// 14,3,1 is standard  14 periods, 3 slow average, 1 fast average
// TODO: implement the averaging
// TODO: implement CRTP?

class TSSWStochastic:
  public RunningMinMax<TSSWStochastic,double>,
  public TimeSeriesSlidingWindow<TSSWStochastic, Quote>
{
  friend RunningMinMax<TSSWStochastic,double>;
  friend TimeSeriesSlidingWindow<TSSWStochastic, Quote>;
public:

  using fK_t = std::function<void( ptime, double ,double, double)>; // ptime, indicator, min, max

  TSSWStochastic( Quotes& quotes, time_duration tdWindowWidth );
  TSSWStochastic( Quotes& quotes, size_t nPeriods, time_duration tdPeriodWidth, fK_t&& );
  virtual ~TSSWStochastic();

  double K() const { return m_k; };
  double Size() const;
  void Reset();

protected:
  void Add( const Quote& quote );
  void Expire( const Quote& quote );
  //void PostUpdate();
private:
  bool m_bAvailable;
  double m_lastAdd;
  double m_lastExpire;
  double m_k;
  ptime m_dtLatest;
  fK_t m_fK;

  void UpdateOnAdd( double min, double max );
};

} // namespace tf
} // namespace ou
