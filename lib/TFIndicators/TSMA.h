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

#include <vector>

#include "TSEMA.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace hf { // high frequency

class TSMA: public Prices {
public:
  TSMA( Prices& series, time_duration dt, unsigned int nInf, unsigned int nSup ); // pg 63
  TSMA( Prices& series, time_duration dt, unsigned int n );  // eq 3.56, pg 61, n typically 2, 3, 4
  ~TSMA(void);
  double GetMA( void ) { return m_dblRecentMA; };
protected:
private:
  time_duration m_dtTimeRange;
  unsigned int m_nInf;
  unsigned int m_nSup;
  Prices& m_seriesSource;
  std::vector<TSEMA<Price>*> m_vEMA;
  double m_dblRecentMA;
  void Initialize( void );
  void HandleUpdate( const Price& );
};

} // namespace hf
} // namespace tf
} // namespace ou
