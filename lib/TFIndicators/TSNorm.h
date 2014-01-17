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

class TSNorm: public Prices {
public:
  TSNorm( Prices& series, time_duration dt, unsigned int n, double p );
  TSNorm( const TSNorm& rhs );
  ~TSNorm(void);
protected:
private:
  time_duration m_dtTimeRange;
  unsigned int m_n;
  double m_p;
  Prices& m_seriesSource;
  TSMA m_ma;  // this needs to be at end of list for proper initialization
  void HandleUpdate( const Price& );
  void HandleMAUpdate( const Price& );
};

} // namespace hf
} // namespace tf
} // namespace ou
