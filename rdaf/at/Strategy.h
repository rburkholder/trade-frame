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
 * Project: rdaf/at
 * Created: March 7, 2022 14:35
 */

#include "TFTimeSeries/DatedDatum.h"
#include <vector>
#include <thread>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <OUCharting/ChartEntryBars.h>
#include <OUCharting/ChartEntryMark.h>
#include <OUCharting/ChartEntryShape.h>
#include <OUCharting/ChartEntryVolume.h>
#include <OUCharting/ChartEntryIndicator.h>

#include <TFTimeSeries/BarFactory.h>

#include <TFTrading/Order.h>
#include <TFTrading/Position.h>
#include <TFTrading/DailyTradeTimeFrames.h>

class TH3D;
class TRint;
class TFile;
class TTree;
class TMacro;

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

  using pOrder_t = ou::tf::Order::pOrder_t;
  using pPosition_t = ou::tf::Position::pPosition_t;

  Strategy( const std::string& filename, ou::ChartDataView&, const config::Options& );
  virtual ~Strategy();

  void SetPosition( pPosition_t );

  void SaveWatch( const std::string& );

  void CloseAndDone();

protected:
private:

  enum EChartSlot { Price, Volume, PL }; // IndMA = moving averate indicator
  enum class ETradeState {
    Init,  // initiaize state in current market
    Search,  // looking for long or short enter
    LongSubmitted, // order has been submitted, waiting for confirmation
    LongExit,  // position exists, looking for exit
    ShortSubmitted,  // order has been submitted, waiting for confirmtaion
    ShortExit,  // position exists, looking for exit
    ExitSubmitted, // wait for exit to complete
    Done // no more action
    };

  ETradeState m_stateTrade;

  const config::Options& m_options;

  const std::string& m_sFilePrefix;

  pPosition_t m_pPosition;

  ou::tf::Quote m_quote;

  pOrder_t m_pOrder;

  ou::ChartDataView& m_cdv;

  ou::ChartEntryIndicator m_ceQuoteAsk;
  ou::ChartEntryIndicator m_ceQuoteBid;

  ou::ChartEntryIndicator m_ceTrade;
  ou::ChartEntryVolume m_ceVolume;

  ou::ChartEntryShape m_ceLongEntry;
  ou::ChartEntryShape m_ceLongFill;
  ou::ChartEntryShape m_ceLongExit;
  ou::ChartEntryShape m_ceShortEntry;
  ou::ChartEntryShape m_ceShortFill;
  ou::ChartEntryShape m_ceShortExit;

  ou::ChartEntryIndicator m_ceProfitLoss;

  ou::tf::BarFactory m_bfQuotes01Sec;

  // ==
  struct QuoteForBranch {
    double time;
    double ask;
    uint64_t askvol;
    double bid;
    uint64_t bidvol;
  } m_branchQuote;

  struct TradeForBranch {
    double time;
    double price;
    uint64_t vol;
    int64_t direction;
  } m_branchTrade;

  std::thread m_threadRdaf;
  std::unique_ptr<TRint> m_prdafApp;

  std::unique_ptr<TFile> m_pFile;

  // https://root.cern/doc/master/classTTree.html
  using pTTree_t = std::shared_ptr<TTree>;
  pTTree_t m_pTreeQuote;
  pTTree_t m_pTreeTrade;

  using pTH3D_t = std::shared_ptr<TH3D>;
  pTH3D_t m_pHistVolume;

  void StartRdaf( const std::string& sFilePrefix );
  static void ThreadRdaf( Strategy* p, const std::string& sFilePrefix );

  // ==

  void HandleQuote( const ou::tf::Quote& );
  void HandleTrade( const ou::tf::Trade& );

  void HandleBarQuotes01Sec( const ou::tf::Bar& bar );

  void HandleRHTrading( const ou::tf::Bar& bar );
  void HandleCancel( boost::gregorian::date, boost::posix_time::time_duration );
  void HandleGoNeutral( boost::gregorian::date, boost::posix_time::time_duration );

  void EnterLong( const ou::tf::Bar& );
  void EnterShort( const ou::tf::Bar& );

  void HandleOrderCancelled( const ou::tf::Order& );
  void HandleOrderFilled( const ou::tf::Order& );

  void Clear();
  void SetupChart();

};