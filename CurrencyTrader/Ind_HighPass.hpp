/************************************************************************
 * Copyright(c) 2024, One Unified. All rights reserved.                 *
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
 * File:    Ind_HighPass.hpp
 * Author:  raymond@burkholder.net
 * Project: CurrencyTrader
 * Created: July 14, 2024 13:19:59
 */

#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartEntryIndicator.h>

namespace ou {
namespace tf {
namespace indicator {

// high-pass filter, 2024/04, technical analysis of stocks & commodities, page 11

// create a band-pass filter with:
//  HP = $HighPass( Close, LowerPeriod )
//  BP = $SuperSmoother( HP, UpperPeriod )

class HighPass {
public:

  HighPass( unsigned int period, ou::ChartDataView& cdv, unsigned int ixSlot_ );
  ~HighPass();

  void Set( ou::Colour::EColour colour, const std::string& sName );
  double Update( boost::posix_time::ptime dt, double value );
  double HP() const { return hp0; }

protected:
private:

  unsigned int n0;

  const double pi;
  const double root_2;
  const double a1, b1;
  const double c2;
  const double c3;
  const double c1;

  double val0, val1, val2;
  double hp0, hp1, hp2;

  unsigned int ixSlot;

  ou::ChartEntryIndicator m_ce;
  ou::ChartDataView& m_cdv;

};

} // namespace indicator
} // namespace tf
} // namespace ou

