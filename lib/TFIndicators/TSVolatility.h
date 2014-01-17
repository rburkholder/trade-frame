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

#include "TSNorm.h"
#include "TSDifferential.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace hf { // high frequency

// page 69
// tauprime - return interval
// tau - length of moving sample
// z - irregular time series
// equation 3.68

class TSVolatility: public Prices {
public:
  TSVolatility( Prices& series, time_duration dtTau, time_duration dtTauPrime, double p, unsigned int n = 4 );
  TSVolatility( const TSVolatility& );
  ~TSVolatility(void);
protected:
private:
  time_duration m_dtTau;
  time_duration m_dtTauByTwo;
  time_duration m_dtTauPrime;
  double m_p;
  unsigned int m_n;
  Prices& m_seriesSource;
  TSDifferential m_tsDif;  // needs to be before norm
  TSNorm m_tsNorm;
  void HandleUpdate( const Price& price );
};

} // namespace hf
} // namespace tf
} // namespace ou
