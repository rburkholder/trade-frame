/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

#include <OUCommon/Delegate.h>

#include "TimeSeriesSlidingWindow.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

// calculation from Intro HF Finance, pg 41
// page 43 has another version of realized volatility as the standard deviation fo the returns about the sample mean
//  which is not implemented here, due to extra computational complexity, but may have a viable solution based upon subsequent formulae
// the book indicates that Realized Volatility should be computed from a homogenous time series (top of page 44)
// delta T should be 15min to 2hr

class TSSWRealizedVolatility: 
  public TimeSeriesSlidingWindow<TSSWRealizedVolatility, CPrice>,
  public CPrices
{
  friend TimeSeriesSlidingWindow<TSSWRealizedVolatility, CPrice>;
public:
  TSSWRealizedVolatility( CPrices& prices, time_duration tdWindowWidth, double p );
  ~TSSWRealizedVolatility( void );
  void SetScaleFactor( time_duration tdScaledWidth ) { m_tdScaledWidth = tdScaledWidth; CalcScaleFactor(); };
  time_duration GetScaleFactor( void  ) { return m_tdScaledWidth; };
protected:
  void Add( const CPrice& price );
  void Expire( const CPrice& price );
  void PostUpdate( void );
private:
  unsigned int m_n;
  double m_dblSum;
  double m_dblP;
  ptime m_dt;
  time_duration m_tdScaledWidth;
  double m_dblScaleFactor;
  void CalcScaleFactor( void );
};

} // namespace tf
} // namespace ou

