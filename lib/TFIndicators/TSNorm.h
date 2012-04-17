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

#include "TSMA.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace hf { // high frequency

class TSNorm: public CPrices {
public:
  TSNorm( CPrices& series, time_duration dt, unsigned int n, double p );
  ~TSNorm(void);
protected:
private:
  time_duration m_dtTimeRange;
  unsigned int m_n;
  double m_p;
  CPrices& m_seriesSource;
  TSMA m_ma;
  void HandleUpdate( const CPrice& );
  void HandleMAUpdate( const CPrice& );
};

} // namespace hf
} // namespace tf
} // namespace ou
