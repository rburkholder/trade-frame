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
 * File:   Engine.cpp
 * Author: raymond@burkholder.net
 * 
 * Created on July 19, 2018, 9:02 PM
 */

/* 
 * set watch on option, set watch on underlying
 * as each changes, recalculate option greeks
 * but set a limit of once a second?  or a when a large move hits
 * 
 */

// old way: https://www.boost.org/doc/libs/1_67_0/doc/html/boost_asio/reference/io_service.html
// new way: https://www.boost.org/doc/libs/1_67_0/doc/html/boost_asio/reference/io_context.html

#include <algorithm>

#include <boost/bind.hpp>

#include <OUCommon/TimeSource.h>

#include "Engine.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

OptionEntry::OptionEntry( const OptionEntry&& rhs ) {
  cntInstances = rhs.cntInstances;
  pUnderlying = std::move( rhs.pUnderlying );
  pOption = std::move( rhs.pOption );
  fGreek = std::move( rhs.fGreek );
//  m_dblLastUnderlyingQuote = rhs.m_dblLastUnderlyingQuote;
//  m_dblLastOptionQuote = rhs.m_dblLastOptionQuote;
  m_quoteLastUnderlying = rhs.m_quoteLastUnderlying;
//  m_quoteLastOption = rhs.m_quoteLastOption;
  m_bChanged = rhs.m_bChanged;
}

OptionEntry::OptionEntry( pOption_t pOption_): pOption( pOption_ ) {}

OptionEntry::OptionEntry( pWatch_t pUnderlying_, pOption_t pOption_, fGreekResultCallback_t&& fGreek_ ):
  pUnderlying( pUnderlying_ ), pOption( pOption_ ), fGreek( std::move( fGreek_ ) ), 
  m_bChanged( false ),
  cntInstances {} {
    pUnderlying->OnQuote.Add( MakeDelegate( this, &OptionEntry::HandleUnderlyingQuote) );
    pUnderlying->StartWatch();
    //pOption->OnQuote.Add( MakeDelegate( this, &OptionEntry::HandleOptionQuote ) );
    pOption->StartWatch();
}
  
OptionEntry::~OptionEntry() {
  pOption->StopWatch();
  //pOption->OnQuote.Remove( MakeDelegate( this, &OptionEntry::HandleOptionQuote ) );
  pUnderlying->StopWatch();
  pUnderlying->OnQuote.Remove( MakeDelegate( this, &OptionEntry::HandleUnderlyingQuote ) );
}

void OptionEntry::HandleUnderlyingQuote(const ou::tf::Quote& quote_) {
  if ( ! m_quoteLastUnderlying.SameBidAsk( quote_ ) ) {
    m_quoteLastUnderlying = quote_;
    m_bChanged = true;
  }
}

//void OptionEntry::HandleOptionQuote(const ou::tf::Quote& quote_) { // should this be kept?
//  if ( ! m_quoteLastOption.SameBidAsk( quote_ ) ) {
//    m_quoteLastOption = quote_;
//    m_bChanged = true;
//  }
//}

void OptionEntry::Calc( const fCalc_t& fCalc ) {
  if ( m_bChanged ) {
    fCalc( pOption, m_quoteLastUnderlying, fGreek );
    m_bChanged = false;
  }
}

// ====================

Engine::Engine( const ou::tf::LiborFromIQFeed& feed ): 
  m_InterestRateFeed( feed ), 
  m_srvcWork(boost::asio::make_work_guard( m_srvc )),
  m_timerScan( m_srvc )
{
  
  for ( std::size_t ix = 0; ix < 1; ix++ ) {
    m_threads.create_thread( boost::bind( &boost::asio::io_context::run, &m_srvc ) ); // add handlers
    
    m_fCalc =
      [](ou::tf::option::Option::pOption_t pOption, const ou::tf::Quote& quoteUnderlying, fGreekResultCallback_t& fGreek){
    };
  }
  
  m_timerScan.expires_after( boost::asio::chrono::milliseconds(1000) );
  m_timerScan.async_wait(
    boost::bind( 
      &Engine::HandleTimerScan, this, 
      boost::asio::placeholders::error 
      ) 
    );
}

Engine::~Engine( ) {
  m_srvcWork.reset();
  m_threads.join_all();
}

void Engine::Add( pWatch_t pUnderlying, pOption_t pOption, fGreekResultCallback_t&& fGreek ) {
  if ( m_srvcWork.owns_work() ) {
    OptionEntry oe( pUnderlying, pOption, std::move( fGreek ) );
    std::lock_guard<std::mutex> lock(m_mutexOptionEntryOperationQueue);
    m_dequeOptionEntryOperation.push_back( OptionEntryOperation( Action::AddOption, std::move(oe) ) );
  }
}

void Engine::Delete( pOption_t pOption ) {
  if ( m_srvcWork.owns_work() ) {
    OptionEntry oe( pOption );
    std::lock_guard<std::mutex> lock(m_mutexOptionEntryOperationQueue);
    m_dequeOptionEntryOperation.push_back( OptionEntryOperation( Action::RemoveOption, std::move(oe) ) );
  }
}

void Engine::HandleTimerScan( const boost::system::error_code &ec ) {
  if ( boost::asio::error::operation_aborted == ec ) {
  }
  else {
    if ( m_srvcWork.owns_work() ) {
      ScanOptionEntryQueue();
      
    // other ways:
      //timer_.expires_at(timer_.expiry() + boost::asio::chrono::seconds(1));
      m_timerScan.expires_after( boost::asio::chrono::milliseconds(250) );
      m_timerScan.async_wait(
        boost::bind( 
          &Engine::HandleTimerScan, this, 
          boost::asio::placeholders::error
          )
        );
    }
  }
}

void Engine::ProcessOptionEntryOperationQueue() {
  std::lock_guard<std::mutex> lock(m_mutexOptionEntryOperationQueue);
  if ( !m_dequeOptionEntryOperation.empty() ) {
    OptionEntryOperation& oe( m_dequeOptionEntryOperation.front() );
    switch( oe.m_action ) {
      case Action::AddOption: {

        mapOptionEntry_t::iterator iterOption = m_mapOptionEntry.find( oe.m_oe.OptionName() );
        if ( m_mapOptionEntry.end() == iterOption ) {
          iterOption = m_mapOptionEntry.insert( m_mapOptionEntry.begin(), mapOptionEntry_t::value_type( oe.m_oe.OptionName(), std::move( oe.m_oe ) ) );
        }
        iterOption->second.Inc();
      }
        break;
      case Action::RemoveOption: {
        mapOptionEntry_t::iterator iterOption = m_mapOptionEntry.find( oe.m_oe.OptionName() );
        if ( m_mapOptionEntry.end() == iterOption ) {
          throw std::runtime_error( "Engine::Delete: can't find option" + oe.m_oe.OptionName() );
        }

        OptionEntry::size_type cnt = iterOption->second.Dec();
        if ( 0 == cnt ) {
          m_mapOptionEntry.erase( iterOption );
        }
      }
        break;
      case Action::Unknown:
        break;
    }
    m_dequeOptionEntryOperation.pop_front();
  }
}

// TODO: sort map by expiry?  // then Option::CalcRate is required less often
void Engine::ScanOptionEntryQueue() {

  ProcessOptionEntryOperationQueue();
  
  // dtUtcNow needs to be passed by value
  boost::posix_time::ptime dtUtcNow = ou::TimeSource::GlobalInstance().External();
  
  // three step lambda call:
  //  1) lambda for each active mapOptionEntry
  //  2) capture private values from the OptionEntry
  //  3) use the values in a background thread for calculations via post to io_service
  std::for_each( m_mapOptionEntry.begin(), m_mapOptionEntry.end(), 
                [this, dtUtcNow](mapOptionEntry_t::value_type& vt){
                  vt.second.Calc( 
                    [this, dtUtcNow](OptionEntry::pOption_t pOption, const ou::tf::Quote& quoteUnderlying, fGreekResultCallback_t& fGreekResultCallback ){
                      double midpointUnderlying( quoteUnderlying.Midpoint() );
                      boost::asio::post( m_srvc, 
                        [this, dtUtcNow, pOption, midpointUnderlying, fGreekResultCallback](){
                          ou::tf::option::binomial::structInput input;
                          input.S = midpointUnderlying;
                          pOption->CalcRate( input, dtUtcNow, m_InterestRateFeed );
                          pOption->CalcGreeks( input, dtUtcNow, true );
                          if ( nullptr != fGreekResultCallback ) {
                            fGreekResultCallback( pOption->LastGreek() ); // need to create the method
                          }
                      });
                  });
                });
}

} // namespace option
} // namespace tf
} // namespace ou

