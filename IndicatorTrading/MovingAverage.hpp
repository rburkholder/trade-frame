/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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
 * File:    MovingAverage.hpp
 * Author:  raymond@burkholder.net
 * Project: IndicatorTrading
 * Created: 2022/11/29 11:57:41
 */

#pragma once

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartEntryIndicator.h>

#include <TFIndicators/TSEMA.h>

class MovingAverage {
public:

  MovingAverage( ou::tf::Quotes& quotes, size_t nPeriods, time_duration tdPeriod, ou::Colour::EColour colour, const std::string& sName );
  MovingAverage( MovingAverage&& rhs );

  void AddToView( ou::ChartDataView& cdv, size_t slot );

  void Update( ptime dt );

  double Latest() const { return m_ema.GetEMA(); }
  const std::string& Name() const { return m_ceMA.GetName(); }

protected:
private:

  ou::ChartEntryIndicator m_ceMA;

  ou::tf::hf::TSEMA<ou::tf::Quote> m_ema;

};
