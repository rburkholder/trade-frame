/************************************************************************
 * Copyright(c) 2023, One Unified. All rights reserved.                 *
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
 * File:    Strategy.hpp
 * Author:  raymond@burkholder.net
 * Project: rdaf/l2
 * Created: March 11, 2034  10:58:03
 */

#pragma once

#include <boost/signals2.hpp>

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartEntryVolume.h>
#include <OUCharting/ChartEntryIndicator.h>

#include <TFTrading/Position.h>

#include "ConfigParser.hpp"

namespace ou {
namespace tf {
  class TreeItem;
}
}

namespace Strategy {

class Base {
public:

  using pPosition_t = ou::tf::Position::pPosition_t;

  enum class ESignal {
    rising, neutral, falling
  };

  Base(
    const ou::tf::config::symbol_t&
  , ou::tf::TreeItem* pTreeItem
  );
  virtual ~Base();

  const ou::tf::config::symbol_t& Choices() const { return m_config; }
  ou::tf::config::symbol_t::EFeed Feed() const { return m_config.eFeed; }

  virtual void SetPosition( pPosition_t );
  virtual void SaveWatch( const std::string& sPrefix );

  ou::ChartDataView& GetChartDataView() { return m_cdv; }

  boost::signals2::signal<void(ESignal)> m_signal;
  virtual void Signal( ESignal ) {}

protected:

  enum EChartSlot { Price, Volume, SD, MASlope, MA, ImbalanceMean, Stoch, FVS_v8_rel, PL1, PL2, ET, MarketDepth };

  const ou::tf::config::symbol_t& m_config;

  ou::tf::TreeItem* m_pTreeItemSymbol;

  ou::ChartDataView m_cdv;

  ou::ChartEntryIndicator m_ceQuoteAsk;
  ou::ChartEntryIndicator m_ceQuoteBid;

  ou::ChartEntryIndicator m_ceTrade;
  ou::ChartEntryVolume m_ceVolume;

  pPosition_t m_pPosition;

private:
};

} // namespace Strategy
