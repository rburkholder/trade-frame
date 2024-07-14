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
 * File:    Ind_SuprSmth.hpp
 * Author:  raymond@burkholder.net
 * Project: CurrencyTrader
 * Created: July 14, 2024 13:59:27
 */

#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartEntryIndicator.h>

namespace ou {
namespace tf {
namespace indicator {

// super smoother, 2024/04, technical analysis of stocks & commodities, page 10
//   recommend period to be >8
//   https://easylanguagemastery.com/indicators/predictive-indicators/ (why 8)

class SuperSmoother {
public:

  SuperSmoother( unsigned int period, ou::ChartDataView& cdv, unsigned int ixSlot_ );
  ~SuperSmoother();

  void Set( ou::Colour::EColour colour, const std::string& sName );
  double Update( boost::posix_time::ptime dt, double value );
  double SS() const { return ss0; }

protected:
private:

  unsigned int n0;

  const double pi;
  const double root_2;
  const double a1, b1;
  const double c2;
  const double c3;
  const double c1;

  double val0, val1;
  double ss0, ss1, ss2;

  unsigned int ixSlot;

  ou::ChartEntryIndicator m_ce;
  ou::ChartDataView& m_cdv;

};

} // namespace indicator
} // namespace tf
} // namespace ou

