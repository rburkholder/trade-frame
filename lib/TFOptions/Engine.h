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
 * File:   Engine.h
 * Author: raymond@burkholder.net
 *
 * Created on July 19, 2018, 9:02 PM
 */

#ifndef ENGINE_H
#define ENGINE_H

#include <map>
#include <queue>
#include <mutex>
#include <chrono>
#include <functional>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/asio/steady_timer.hpp>

#include <TFTimeSeries/DatedDatum.h>

#include <TFTrading/Instrument.h>
#include <TFTrading/Watch.h>

#include <TFOptions/Option.h>

#include <TFTrading/ProviderManager.h>
#include <TFTrading/NoRiskInterestRateSeries.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

class OptionEntry {
public:
  typedef size_t size_type;
  typedef ou::tf::Watch::pWatch_t pWatch_t;
  typedef Option::pOption_t pOption_t;
  typedef std::function<void(const ou::tf::Greek&)> fGreekResultCallback_t; // engine provides callback of greek calculation
  typedef std::function<void(pOption_t, const ou::tf::Quote&, fGreekResultCallback_t&)> fCalc_t; // underlying quote
  
private:
  size_type cntInstances; 
  pWatch_t pUnderlying;
  pOption_t pOption;
  fGreekResultCallback_t fGreek;

  bool m_bChanged;  // needs to be atomic (set in one thread, reset in the other)
  ou::tf::Quote m_quoteLastUnderlying;
  ou::tf::Quote m_quoteLastOption;  // is this actually needed?
  //double m_dblLastUnderlyingQuote;  // should these be atomic as well?  can doubles be atomic?
  //double m_dblLastOptionQuote;
  
public:
  
  //OptionEntry(): cntInstances {}, m_dblLastUnderlyingQuote {}, m_dblLastOptionQuote {}, m_bChanged( false ) {};
  OptionEntry(): cntInstances {}, m_bChanged( false ) {};
  OptionEntry( pOption_t pOption);  // used for storing deletion aspect
  OptionEntry( const OptionEntry& rhs ) = delete;
  OptionEntry( const OptionEntry&& rhs );
  
  OptionEntry( pWatch_t pUnderlying_, pOption_t pOption_, fGreekResultCallback_t&& fGreek_ );
  virtual ~OptionEntry();
  
  const std::string& OptionName() { return pOption->GetInstrument()->GetInstrumentName(); }
  
  void Inc() { cntInstances++; }
  size_t Dec() { assert( 0 < cntInstances ); cntInstances--; return cntInstances; }
  
  void Calc( const fCalc_t& );  // supply underlying and option quotes
private:
  
  void HandleUnderlyingQuote( const ou::tf::Quote& );
  void HandleOptionQuote( const ou::tf::Quote& );
};

class Engine {
public:
  
  // locking on the polling, rather than the callback from the quote
  // allows full speed on the callbacks, and not time critical on the polling and locking
  // continuous scanning?  1/10 sec scanning? once a second scanning?
  // run with futures?
  // need to loop through map on each scan
  // need to lock the scan from add/deletions
  // meed to queue additions and deletions with each scan (something like Delegate)
  // prevents map interators from being invalidated (check invalidation)
  // interleave add/delete  chronologically, so enum the operation.
  
  typedef ou::tf::Watch::pWatch_t pWatch_t;
  typedef Option::pOption_t pOption_t;
  typedef OptionEntry::fGreekResultCallback_t fGreekResultCallback_t;
  
  Engine( const ou::tf::LiborFromIQFeed& );
  virtual ~Engine( );
  
  void Add( pWatch_t pUnderlying, pOption_t pOption, fGreekResultCallback_t&& ); // reference counted
  void Delete( pOption_t pOption ); // part of the reference counting, will change reference count on associated underlying and auto remove
  
private:
  
  enum Action { Unknown, AddOption, RemoveOption };
  
  typedef ou::tf::Instrument::idInstrument_t idInstrument_t;
  
  typedef std::map<idInstrument_t, OptionEntry> mapOptionEntry_t;
  
  //std::atomic<size_t> m_cntOptionEntryOperationQueueCount;
  std::mutex m_mutexOptionEntryOperationQueue;
  
  boost::asio::io_context m_srvc;
  boost::thread_group m_threads;
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type> m_srvcWork;
  boost::asio::steady_timer m_timerScan;
  
  OptionEntry::fCalc_t m_fCalc;
  
  const LiborFromIQFeed& m_InterestRateFeed;
  
  struct OptionEntryOperation {
    Action m_action;
    OptionEntry m_oe;
    OptionEntryOperation(): m_action( Action::Unknown ) {}
    OptionEntryOperation( Action action, OptionEntry&& oe ): m_action( action ), m_oe( std::move( oe ) ) {}
    OptionEntryOperation( const OptionEntryOperation& rhs ) = delete;
    OptionEntryOperation( const OptionEntryOperation&& rhs ): m_action( rhs.m_action ), m_oe( std::move( rhs.m_oe ) ) {}
  };
  
  typedef std::deque<OptionEntryOperation> dequeOptionEntryOperation_t;
  
  mapOptionEntry_t m_mapOptionEntry;
  
  dequeOptionEntryOperation_t m_dequeOptionEntryOperation;
  
  void HandleTimerScan( const boost::system::error_code &ec );
  void ProcessOptionEntryOperationQueue();
  void ScanOptionEntryQueue();

};

} // namespace option
} // namespace tf
} // namespace ou

#endif /* ENGINE_H */

