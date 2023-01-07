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
 * Project: lib/TFBitsNPieces
 * Created: 2022/11/29 11:57:41
 */

#pragma once

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartEntryIndicator.h>

#include <TFIndicators/TSEMA.h>
#include <TFIndicators/TSSWStats.h>

namespace ou {
namespace tf {

// ====

class MovingAverage {
public:

  MovingAverage( Quotes& quotes, size_t nPeriods, time_duration tdPeriod, ou::Colour::EColour colour, const std::string& sName );
  MovingAverage( MovingAverage&& rhs );
  virtual ~MovingAverage();

  void AddToView( ou::ChartDataView& cdv, size_t slot );

  double EMA() const { return m_ema.GetEMA(); }
  const std::string& Name() const { return m_ceMA.GetName(); }

protected:
  ou::tf::hf::TSEMA<ou::tf::Quote> m_ema;
private:

  ou::ChartEntryIndicator m_ceMA;

  void HandleUpdate( const ou::tf::Price& );

};

// ====

class MovingAverageSlope: public MovingAverage {
public:

  MovingAverageSlope( Quotes& quotes, size_t nPeriods, time_duration tdPeriod, ou::Colour::EColour colour, const std::string& sName );
  MovingAverageSlope( MovingAverageSlope&& rhs );
  virtual ~MovingAverageSlope();

  void AddToView( ou::ChartDataView& cdv, size_t slotMA, size_t slotSlope );

  double Slope() const { return m_dblLast; }

protected:
private:

  double m_dblLast;

  ou::tf::TSSWStatsPrice m_stats;
  ou::ChartEntryIndicator m_ceSlope;

  void HandleUpdate( const ou::tf::TSSWStatsPrice::Results& );
};

// ====

} // namespace ou
} // namespace tf
