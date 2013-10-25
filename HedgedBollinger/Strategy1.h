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
#include <TFTimeSeries/BarFactory.h>

#include <TFTrading/DailyTradeTimeFrames.h>

#include <TFOptions/Bundle.h>

#include <OUCharting/ChartDataBase.h>

class Strategy: public ou::tf::DailyTradeTimeFrame<Strategy> {
  friend ou::tf::DailyTradeTimeFrame<Strategy>; 
public:
  Strategy( ou::tf::option::MultiExpiryBundle* meb );
  ~Strategy(void);
  ou::ChartDataView& GetChartDataView( void ) {return m_ChartDataUnderlying.GetChartDataView(); };
protected:
private:

  ou::tf::option::MultiExpiryBundle* m_pBundle;  // keep towards top of variable section

  ou::ChartDataBase m_ChartDataUnderlying;

  void HandleQuoteUnderlying( const ou::tf::Quote& quote );
  void HandleTradeUnderlying( const ou::tf::Trade& trade );

  void HandleCommon( const ou::tf::Quote& quote );
  void HandleRHTrading( const ou::tf::Quote& quote );

  void HandleCommon( const ou::tf::Trade& trade );
  void HandleRHTrading( const ou::tf::Trade& trade ) {};

};

