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
 * File:    Strategy.h
 * Author:  raymond@burkholder.net
 * Project: AutoTrade
 * Created: February 14, 2022 10:59
 */

#include <vector>

#include "TFTrading/Position.h"
#include <memory>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <OUCharting/ChartEntryBars.h>
#include <OUCharting/ChartEntryMark.h>
#include <OUCharting/ChartEntryShape.h>
#include <OUCharting/ChartEntryVolume.h>
#include <OUCharting/ChartEntryIndicator.h>

#include <OUCharting/ChartDataView.h>

#include <TFIndicators/TSSWStats.h>

#include <TFTrading/DailyTradeTimeFrames.h>
#include <TFTrading/Position.h>

namespace config {
  class Options;
}

namespace ou {
  class ChartDataView;
}

class Strategy:
  public ou::tf::DailyTradeTimeFrame<Strategy>
{
  friend class boost::serialization::access;
  friend ou::tf::DailyTradeTimeFrame<Strategy>;
public:

  using pPosition_t = ou::tf::Position::pPosition_t;
  using pChartDataView_t = ou::ChartDataView::pChartDataView_t;

  Strategy( pPosition_t, pChartDataView_t, const config::Options& );
  virtual ~Strategy();

protected:
private:

  enum EChartSlot { Price, Volume, PL }; // IndMA = moving averate indicator

  pPosition_t m_pPosition;
  pChartDataView_t m_pChartDataView;

  struct MA {

    ou::tf::TSSWStatsMidQuote m_statsMA;
    ou::ChartEntryIndicator m_ceMA;

    MA( ou::tf::Quotes& quotes, size_t nPeriods, time_duration tdPeriod, ou::Colour::enumColour colour, const std::string& sName )
    : m_statsMA( quotes, nPeriods, tdPeriod )
    {
      m_ceMA.SetName( sName );
      m_ceMA.SetColour( colour );
    }

    MA( MA&& rhs )
    : m_statsMA( std::move( rhs.m_statsMA ) ) {}

    void AddToView( pChartDataView_t pChartDataView ) {
      pChartDataView->Add( EChartSlot::Price, &m_ceMA );
    }
  };

  using vMA_t = std::vector<MA>;
  vMA_t m_vMA;

  ou::ChartEntryVolume m_ceVolume;

  ou::ChartEntryIndicator m_ceQuoteAsk;
  ou::ChartEntryIndicator m_ceQuoteBid;

  ou::ChartEntryShape m_ceShortEntries;
  ou::ChartEntryShape m_ceLongEntries;
  ou::ChartEntryShape m_ceShortFills;
  ou::ChartEntryShape m_ceLongFills;
  ou::ChartEntryShape m_ceShortExits;
  ou::ChartEntryShape m_ceLongExits;

  void HandleQuote( const ou::tf::Quote& );
  void HandleTrade( const ou::tf::Trade& );

  void HandleRHTrading( const ou::tf::Quote& );

};