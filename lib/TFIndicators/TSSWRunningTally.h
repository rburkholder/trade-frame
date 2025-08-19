/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

// operation:
//     Add: m_net += price.Value();
//  Expire: m_net -= price.Value();

#pragma once

#include "TimeSeriesSlidingWindow.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class TSSWRunningTally: public TimeSeriesSlidingWindow<TSSWRunningTally, Price> {
  friend TimeSeriesSlidingWindow<TSSWRunningTally, Price>;
public:

  TSSWRunningTally( Prices&, time_duration tdWindowWidth );
  TSSWRunningTally( const TSSWRunningTally& );
  ~TSSWRunningTally();

  double Net() const { return m_net; };

protected:
  void Add( const Price& );
  void Expire( const Price& );
  void PostUpdate( void );
private:
  double m_net;
};

} // namespace tf
} // namespace ou

