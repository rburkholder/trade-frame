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

#pragma once

// useful for determining trending vs mean reverting

#include "TimeSeriesSlidingWindow.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class TSSWEfficiencyRatio: public TimeSeriesSlidingWindow<TSSWEfficiencyRatio, CTrade> {
  friend TimeSeriesSlidingWindow<TSSWEfficiencyRatio, CTrade>;
public:

  TSSWEfficiencyRatio( CTrades*, long WindowSizeSeconds );
  TSSWEfficiencyRatio( const TSSWEfficiencyRatio& );
  ~TSSWEfficiencyRatio( void );

  double Ratio( void ) const { return m_ratio; };
  double Total( void ) const { return m_total; };

protected:
  void Add( const CTrade& );
  void Expire( const CTrade&e );
  void PostUpdate( void );
private:
  double m_lastAdd;
  double m_lastExpire;
  double m_sum;  // moving sum
  double m_total;  // over complete time series
  double m_ratio;
};

} // namespace tf
} // namespace ou

