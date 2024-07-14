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
 * File:    Ind_Ema.hpp
 * Author:  raymond@burkholder.net
 * Project: CurrencyTrader
 * Created: July 14, 2024 11:35:57
 */

#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartEntryIndicator.h>

namespace ou {
namespace tf {
namespace indicator {

class Ema {
public:

  Ema( unsigned int n, ou::ChartDataView& cdv, unsigned int ixSlot_ );
  ~Ema();

  void Set( ou::Colour::EColour colour, const std::string& sName );
  double Update( boost::posix_time::ptime dt, double value );

protected:
private:

  bool bBootStrapped;

  const double c1;
  const double c2;

  double dblLatest;
  unsigned int ixSlot;

  ou::ChartEntryIndicator m_ce;
  ou::ChartDataView& m_cdv;

};

} // namespace indicator
} // namespace tf
} // namespace ou

