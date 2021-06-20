/************************************************************************
 * Copyright(c) 2021, One Unified. All rights reserved.                 *
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
 * File:    Underlying.h
 * Author:  raymond@burkholder.net
 * Project: BasketTrading
 * Created on 2021/06/19 19:41
 */

#pragma once

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartEntryMark.h>

#include <TFOptions/Aggregate.h>

#include <TFTrading/Watch.h>

class Underlying { // contains: 1) to contain active strategies, 2) gex calcs
public:

  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pChartDataView_t = ou::ChartDataView::pChartDataView_t;

  Underlying(
    pWatch_t pWatchUnderlying_,
    const std::string& sDailyBarPath
  );

  // TODO: will need two mapChain types:
  //   1) basic for passing to strategy
  //   2) gex calcs

protected:
private:

  using gex_t = ou::tf::option::Aggregate;

  pWatch_t pWatchUnderlying;
  gex_t GexCalc;
  std::string m_sDailyBarPath;

  pChartDataView_t m_pChartDataView;

  ou::ChartEntryMark m_cePivots;

  void BuildUnderlyingChains( gex_t& );
};
