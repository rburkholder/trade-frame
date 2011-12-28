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

#include "TimeSeriesSlidingWindow.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
  
class TSSWRateOfChange: public TimeSeriesSlidingWindow<TSSWRateOfChange, CPrice> {
  friend TimeSeriesSlidingWindow<TSSWRateOfChange, CPrice>;
public:

  TSSWRateOfChange( CPrices*, long WindowSizeSeconds );
  ~TSSWRateOfChange(void);

  double RateOfChange( void ) const { return m_head - m_tail; };
  double RateOfChangePct( void ) const { return ( 0 == m_tail ) ? 0.0 : ( ( m_head - m_tail ) / m_tail ); };

protected:
  void Add( const CPrice& );
  void Expire( const CPrice& );
  void PostUpdate( void );
private:
  double m_tail;
  double m_head;
};

} // namespace tf
} // namespace ou

