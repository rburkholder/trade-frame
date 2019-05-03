/************************************************************************
 * Copyright(c) 2019, One Unified. All rights reserved.                 *
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
 * File:   TSSWDonchianChannel.h
 * Author: rpb
 *
 * Created on May 3, 2019, 1:25 PM
 */

#ifndef TSSWDONCHIANCHANNEL_H
#define TSSWDONCHIANCHANNEL_H

#include "RunningMinMax.h"
#include "TimeSeriesSlidingWindow.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class TSSWDonchianChannel:
  public RunningMinMax<TSSWDonchianChannel,double>,
  public TimeSeriesSlidingWindow<TSSWDonchianChannel,Price>
{
  friend RunningMinMax<TSSWDonchianChannel,double>;
  friend TimeSeriesSlidingWindow<TSSWDonchianChannel,Price>;
public:
  TSSWDonchianChannel( Prices& prices, time_duration tdWindowWidth, size_t nWindowWidth );
  //TSSWDonchianChannel( const TSSWDonchianChannel& orig );
  virtual ~TSSWDonchianChannel( );

  using minmax = RunningMinMax<TSSWDonchianChannel,double>;

  double Max() const { return minmax::Max(); }
  double Min() const { return minmax::Min(); }
protected:

  void Add( const Price& );    // sliding window crtp recipient
  void Expire( const Price& ); // sliding window crtp recipient
  //void PostUpdate();           // sliding window crtp recipient

  //void UpdateMax( const double& ); // CRTP runningminmax callback recipient
  //void UpdateMin( const double& ); // CRTP runningminmax callback recipient

private:

};

} // namespace tf
} // namespace ou

#endif /* TSSWDONCHIANCHANNEL_H */

