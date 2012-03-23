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
  
class TSSWRunningTally: public TimeSeriesSlidingWindow<TSSWRunningTally, CPrice> {
  friend TimeSeriesSlidingWindow<TSSWRunningTally, CPrice>;
public:

  TSSWRunningTally( CPrices&, long WindowSizeSeconds );
//  TSSWRunningTally( const TSSWRunningTally& );
  ~TSSWRunningTally( void );

  double Net( void ) const { return m_net; };

protected:
  void Add( const CPrice& );
  void Expire( const CPrice& );
  void PostUpdate( void );
private:
  double m_net;
};

} // namespace tf
} // namespace ou

