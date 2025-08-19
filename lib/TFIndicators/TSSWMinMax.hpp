/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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

/*
 * File:    TSSWMinMax.hpp
 * Author:  raymond@burkholder.net
 * Project: TFIndicators
 * Created: August 18, 2025 19:36:44
 */

#pragma once

#include "RunningMinMax.h"
#include "TimeSeriesSlidingWindow.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class TSSWMinMax:
  public RunningMinMax<TSSWMinMax,double>,
  public TimeSeriesSlidingWindow<TSSWMinMax, ou::tf::Price>
{
  friend RunningMinMax<TSSWMinMax,double>;
  friend TimeSeriesSlidingWindow<TSSWMinMax, ou::tf::Price>;
public:

  TSSWMinMax( Prices&, time_duration tdWindowWidth );
  TSSWMinMax( Prices&, size_t nPeriods, time_duration tdPeriodWidth );
  virtual ~TSSWMinMax();

  double Diff() const; // returns max - min

  void Reset();

protected:
private:

  void Add( const Price& ); // CRTP sliding window callback
  void Expire( const Price& ); // CRTP sliding window callback

  void UpdateOnAdd( double min, double max ); // CRTP minmax callback

};

} // namespace tf
} // namespace ou
