/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#pragma once

#include <vector>
#include <string>

#include <OUCommon/Delegate.h>

#include <TFTrading/Symbol.h>

#include "Fundamentals.h"
#include "IQFeedMessages.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

class IQFeedProvider;

class IQFeedSymbol : public Symbol<IQFeedSymbol> {
  friend class IQFeedProvider;
public:

  using inherited_t = Symbol<IQFeedSymbol>;
  using pInstrument_t = inherited_t::pInstrument_t;
  using symbol_id_t =  std::string;

  // Public for RowKeyValues.  Pass in a structure sometime.
  // Public for VuChartArmsIntraDay.  Pass in structure sometime.
  // Update/Summary
  struct Summary {
    ptime dtLastTrade;
    double dblTrade;
    double dblChange; // last - close
    int nTotalVolume;
    int nTradeSize;
    double dblHigh;
    double dblLow;
    double dblBid;
    double dblAsk;
    int nBidSize;
    int nAskSize;
    int nOpenInterest;
    double dblOpen;
    double dblClose;
    int cntTrades;
    bool bNewTrade;
    bool bNewQuote;
    bool bNewOpen;
    Summary()
    : dblTrade {}, dblChange {},
      nTotalVolume {}, nTradeSize {},
      dblHigh {}, dblLow {},
      dblBid {}, dblAsk {}, nBidSize( -1 ), nAskSize( -1 ),
      nOpenInterest {},
      dblOpen {}, dblClose {},
      cntTrades {}, bNewTrade( false ), bNewQuote( false ), bNewOpen( false )
      {}
  };

  IQFeedSymbol(const std::string &symbol, pInstrument_t pInstrument);
  virtual ~IQFeedSymbol(void);

  using pFundamentals_t = std::shared_ptr<Fundamentals>;
  using pSummary_t = std::shared_ptr<Summary>;

  ou::Delegate<pFundamentals_t> OnFundamentalMessage;
  ou::Delegate<pSummary_t> OnUpdateMessage;
  ou::Delegate<pSummary_t> OnSummaryMessage;

  ou::Delegate<IQFeedSymbol&> OnNewsMessage;

protected:

  unsigned short m_cnt;  // used for watch/unwatch
  enum enumQStatus { qUnknown, qFound, qNotFound } m_QStatus;

  enum WatchState {
    None, WSQuote, WSTrade, Both
  };

  void SetWatchState( WatchState state ) { m_stateWatch = state; };
  WatchState GetWatchState() const { return m_stateWatch; };

  void SetDepthWatchInProgress() { m_bDepthWatchInProgress = true; };
  void ResetDepthWatchInProgress() { m_bDepthWatchInProgress = false; };
  bool GetDepthWatchInProgress() const { return m_bDepthWatchInProgress; };
  bool m_bDepthWatchInProgress;

  void HandleFundamentalMessage( IQFFundamentalMessage* pMsg );
  void HandleUpdateMessage( IQFUpdateMessage* pMsg );
  void HandleSummaryMessage( IQFSummaryMessage* pMsg );
  void HandleDynamicFeedUpdateMessage( IQFDynamicFeedUpdateMessage* pMsg );
  void HandleDynamicFeedSummaryMessage( IQFDynamicFeedSummaryMessage* pMsg );
  void HandleNewsMessage( IQFNewsMessage* pMsg );

  template <typename T>
  void DecodePricingMessage( IQFPricingMessage<T>* pMsg );

  template <typename T>
  void DecodeDynamicFeedMessage( IQFDynamicFeedMessage<T>* pMsg );

private:

  WatchState m_stateWatch;

  bool m_bWaitForFirstQuote;

  pFundamentals_t m_pFundamentals;
  pSummary_t m_pSummary;
};

} // namespace iqfeed
} // namespace tf
} // namespace ou
