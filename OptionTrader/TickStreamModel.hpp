/************************************************************************
 * Copyright(c) 2026, One Unified. All rights reserved.                 *
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
 * File:    TickStreamModel.hpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: February 1, 2026 10:42:46
 */

#pragma once

#include <OUCharting/ChartDataView.h>

#include <OUCharting/ChartEntryVolume.h>
#include <OUCharting/ChartEntryIndicator.h>
#include <OUCharting/ChartEntryHistogramV2.hpp>

#include <TFTrading/Watch.h>

class TickStreamModel {
public:

  using pWatch_t = ou::tf::Watch::pWatch_t;

  TickStreamModel( pWatch_t& );
  ~TickStreamModel();

  ou::ChartDataView* GetChartDataView() { return &m_dvChart; }

protected:
private:

  enum EChartSlot { Price, Volume };

  double m_sizeTick;

  pWatch_t m_pWatch;

  ou::ChartDataView m_dvChart; // the data

  ou::ChartEntryIndicator m_ceAsk;
  ou::ChartEntryIndicator m_ceBid;
  ou::ChartEntryIndicator m_ceTrade;
  ou::ChartEntryVolume m_ceVolume;
  ou::ChartEntryHistogram_v2 m_ceVolumeAtPrice;

  ou::tf::Quote m_quote;
  ou::tf::Trade m_trade;

  void HandleTrade( const ou::tf::Trade& );
  void HandleQuote( const ou::tf::Quote& );

};
