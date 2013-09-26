/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

// Started 2013/09/23

// started after MultiExpiryBundle has been populated

#include <TFTimeSeries/TimeSeries.h>
#include <TFTrading/DailyTradeTimeFrames.h>
#include <TFOptions/Bundle.h>
#include <TFIndicators/TSSWStats.h>

class Trader: public ou::tf::DailyTradeTimeFrame<Trader> {
  friend ou::tf::DailyTradeTimeFrame<Trader>; 
public:
  Trader( ou::tf::option::MultiExpiryBundle* meb );
  ~Trader(void);
protected:
private:

  ou::tf::option::MultiExpiryBundle* m_pBundle;

  ou::tf::TSSWStatsMidQuote* m_pIndStats1;
  ou::tf::TSSWStatsMidQuote* m_pIndStats2;
  ou::tf::TSSWStatsMidQuote* m_pIndStats3;

  void HandleQuoteUnderlying( const ou::tf::Quote& quote );
  void HandleTradeUnderlying( const ou::tf::Trade& trade );

  void HandleRHTrading( const ou::tf::Quote& quote );

};

