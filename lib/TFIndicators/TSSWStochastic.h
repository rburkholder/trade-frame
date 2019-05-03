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

#include "RunningMinMax.h"
#include "TimeSeriesSlidingWindow.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

// 14,3,1 is standard  14 periods, 3 slow average, 1 fast average

class TSSWStochastic:
  public RunningMinMax<TSSWStochastic,double>,
  public TimeSeriesSlidingWindow<TSSWStochastic, Quote>
{
  friend RunningMinMax<TSSWStochastic,double>;
  friend TimeSeriesSlidingWindow<TSSWStochastic, Quote>;
public:
  TSSWStochastic( Quotes& quotes, time_duration tdWindowWidth );
  TSSWStochastic( const TSSWStochastic& );
  ~TSSWStochastic(void);
  double K( void ) const { return m_k; };
  void Reset( void );
protected:
  void Add( const Quote& quote );
  void Expire( const Quote& quote );
  void PostUpdate( void );
private:
  double m_lastAdd;
  double m_lastExpire;
  double m_k;
};

} // namespace tf
} // namespace ou
