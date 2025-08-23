/************************************************************************
 * Copyright(c) 2018, One Unified. All rights reserved.                 *
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
 * File:   Engine.hpp
 * Author: raymond@burkholder.net
 *
 * Created on July 19, 2018, 9:02 PM
 */

#ifndef ENGINE_H
#define ENGINE_H

#include <mutex>
#include <functional>
#include <unordered_map>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/asio/steady_timer.hpp>

#include <TFTimeSeries/DatedDatum.h>

#include <TFTrading/Instrument.h>
#include <TFTrading/Watch.h>

#include <TFOptions/Option.h>

#include <TFTrading/ProviderManager.h>

#include "NoRiskInterestRateSeries.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

// ================ OptionEntry =================

class OptionEntry {
public:
  using size_type = size_t;
  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pOption_t = Option::pOption_t;
  using fCallbackWithGreek_t = Option::fCallbackWithGreek_t;
  using fCalc_t = std::function<void(pOption_t, const ou::tf::Quote&, fCallbackWithGreek_t&)>; // underlying quote

private:
  size_type m_cntInstances; // when pOption and pUnderlying are added in
  //bool m_bStartedWatch; // needs to be based upon cntInstances
  pOption_t m_pOption;
  pWatch_t m_pUnderlying;
  fCallbackWithGreek_t m_fGreek;

  ou::tf::Quote m_quoteLastUnderlying;
  //ou::tf::Quote m_quoteLastOption;  // is this actually needed?
  //double m_dblLastUnderlyingQuote;  // should these be atomic as well?  can doubles be atomic?
  //double m_dblLastOptionQuote;

public:

  OptionEntry(): m_cntInstances( 0 ) {};
  OptionEntry( const OptionEntry& rhs ) = delete;
  OptionEntry( OptionEntry&& rhs );

  OptionEntry( pWatch_t pUnderlying_, pOption_t pOption_, fCallbackWithGreek_t&& fGreek_ );  // unused for now
  OptionEntry( pWatch_t pUnderlying_, pOption_t pOption_ );
  virtual ~OptionEntry();

  const std::string& OptionName() { return m_pOption->GetInstrument()->GetInstrumentName(); }
  const std::string& UnderlyingName() { return m_pUnderlying->GetInstrument()->GetInstrumentName(); }

  void Inc();
  size_t Dec();

  void Calc( const fCalc_t& );  // supply underlying and option quotes

  pWatch_t GetUnderlying() { return m_pUnderlying; }
  pOption_t GetOption() { return m_pOption; }

private:

  void HandleUnderlyingQuote( const ou::tf::Quote& );
  void PrintState( const std::string id );

};

// ================ Engine =================

class Engine {
public:

  // locking on the polling, rather than the callback from the quote
  // allows full speed on the callbacks, and not time critical on the polling and locking
  // continuous scanning?  1/10 sec scanning? once a second scanning?
  // run with futures?
  // need to loop through map on each scan
  // need to lock the scan from add/deletions
  // meed to queue additions and deletions with each scan (something like Delegate)
  // prevents map iterators from being invalidated (check invalidation - invalidation does not occur on insertion )
  // interleave add/delete  chronologically, so enum the operation.

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pOption_t = Option::pOption_t;
  using fCallbackWithGreek_t = OptionEntry::fCallbackWithGreek_t;

  //Engine( const ou::tf::LiborFromIQFeed& );
  //Engine( const ou::tf::FedRateFromIQFeed& );
  Engine( const ou::tf::NoRiskInterestRateSeries& );
  virtual ~Engine( );

  // start the calculation process, the option and underlying need to be pre-registered
  void Add( pOption_t pOption, pWatch_t pUnderlying );  // the option already has a delegate for callback
  void Remove( pOption_t pOption, pWatch_t pUnderlying ); // part of the reference counting, will change reference count on associated underlying and auto remove

private:

  enum Action { Unknown, Option_Add, Option_Remove };

  using idInstrument_t = ou::tf::Instrument::idInstrument_t;

  using mapOptionEntry_t  = std::unordered_map<idInstrument_t, OptionEntry>;

  //std::atomic<size_t> m_cntOptionEntryOperationQueueCount;
  std::mutex m_mutexOptionEntryOperationQueue;

  boost::asio::io_context m_srvc;
  boost::thread_group m_threads;
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type> m_srvcWork;
  boost::asio::steady_timer m_timerScan;

  //const LiborFromIQFeed& m_InterestRateFeed;
  //const FedRateFromIQFeed& m_InterestRateFeed;
  const NoRiskInterestRateSeries& m_InterestRateFeed;

  struct OptionEntryOperation {
    Action m_action;
    OptionEntry m_oe;
    OptionEntryOperation(): m_action( Action::Unknown ) {}
    OptionEntryOperation( Action action, OptionEntry&& oe ): m_action( action ), m_oe( std::move( oe ) ) {}
    OptionEntryOperation( const OptionEntryOperation& rhs ) = delete;
    OptionEntryOperation( OptionEntryOperation&& rhs ): m_action( rhs.m_action ), m_oe( std::move( rhs.m_oe ) ) {}
    virtual ~OptionEntryOperation() {}
  };

  using dequeOptionEntryOperation_t = std::deque<OptionEntryOperation>;

  dequeOptionEntryOperation_t m_dequeOptionEntryOperation;

  mapOptionEntry_t m_mapOptionEntry;

  void HandleTimerScan( const boost::system::error_code &ec );
  void ProcessOptionEntryOperationQueue();
  void ScanOptionEntryQueue();

};

} // namespace option
} // namespace tf
} // namespace ou

#endif /* ENGINE_H */

