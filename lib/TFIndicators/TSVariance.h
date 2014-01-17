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

// p1 => power
// p2 => 1 / p for Standard Deviation

// 2013/10/03  Variance appears workable, but doesn't seem to be as 'tight' as the regular 
//   statistics case.  Could still be a bug somewhere.

class TSVariance: public Prices {
public:
  TSVariance( Prices& series, time_duration td, unsigned int n, double p1, double p2 = 1.0 );
  TSVariance( const TSVariance& );
  virtual ~TSVariance( void );

protected:
private:
  time_duration m_tdTimeRange;
  unsigned int m_n;
  double m_p1;
  double m_p2;
  double m_z;
  Prices& m_seriesSource;
  Prices m_dummy;
  TSMA* m_pma1; // this way in order to get events in proper order
  TSMA m_ma2;
  void Init( void );
  void HandleUpdate( const Price& );
  void HandleMA1Update( const Price& );
  void HandleMA2Update( const Price& );
};

} // namespace hf
} // namespace tf
} // namespace ou
