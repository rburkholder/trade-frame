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

OptionEntry::OptionEntry( OptionEntry&& rhs ) {
  if ( rhs.m_bStartedWatch ) {
    rhs.pUnderlying->OnQuote.Remove( MakeDelegate( &rhs, &OptionEntry::HandleUnderlyingQuote ) );
  }
  cntInstances = rhs.cntInstances;
  pUnderlying = std::move( rhs.pUnderlying );
  pOption = std::move( rhs.pOption );
  fGreek = std::move( rhs.fGreek );
  m_bStartedWatch = rhs.m_bStartedWatch;
  rhs.m_bStartedWatch = false;
  if ( m_bStartedWatch ) {
    pUnderlying->OnQuote.Add( MakeDelegate( this, &OptionEntry::HandleUnderlyingQuote) );
  }
  //PrintState( "OptionEntry::OptionEntry(0)" );
}

OptionEntry::OptionEntry( pOption_t pOption_): pOption( pOption_ ), m_bStartedWatch( false ), cntInstances( 0 ) {
  std::cout << "here we are" << std::endl;
  //PrintState( "OptionEntry::OptionEntry(1)" );
}  // can't start with out an underlying

OptionEntry::OptionEntry( pWatch_t pUnderlying_, pOption_t pOption_, fCallbackWithGreek_t&& fGreek_ ):
  pUnderlying( pUnderlying_ ), pOption( pOption_ ), fGreek( std::move( fGreek_ ) ), 
  m_bStartedWatch( false ),
  cntInstances( 0 )
{
  pUnderlying->OnQuote.Add( MakeDelegate( this, &OptionEntry::HandleUnderlyingQuote) );
  pUnderlying->StartWatch();
  //pOption->OnQuote.Add( MakeDelegate( this, &OptionEntry::HandleOptionQuote ) );
  pOption->StartWatch();
  m_bStartedWatch = true;
  //PrintState( "OptionEntry::OptionEntry(2)" );
}
  
OptionEntry::OptionEntry( pWatch_t pUnderlying_, pOption_t pOption_ ):
  pUnderlying( pUnderlying_ ), pOption( pOption_ ), 
  m_bStartedWatch( false ),
  cntInstances( 0 )
{
  pUnderlying->OnQuote.Add( MakeDelegate( this, &OptionEntry::HandleUnderlyingQuote) );
  pUnderlying->StartWatch();
  //pOption->OnQuote.Add( MakeDelegate( this, &OptionEntry::HandleOptionQuote ) );
  pOption->StartWatch();
  m_bStartedWatch = true;
  //PrintState( "OptionEntry::OptionEntry(3)" );
}
  
OptionEntry::~OptionEntry() {
  //pOption->OnQuote.Remove( MakeDelegate( this, &OptionEntry::HandleOptionQuote ) );
  //PrintState( "OptionEntry::~OptionEntry" );
  if ( m_bStartedWatch ) {
    pUnderlying->StopWatch();
    pOption->StopWatch();
    pUnderlying->OnQuote.Remove( MakeDelegate( this, &OptionEntry::HandleUnderlyingQuote ) );
    m_bStartedWatch = false;
  }
}

void OptionEntry::PrintState( const std::string id ) {
  std::string sUnderlying;
  std::string sOption;
  if ( 0 < pUnderlying.use_count() ) {
    sUnderlying = pUnderlying->GetInstrument()->GetInstrumentName();
  }
  if ( 0 < pOption.use_count() ) {
    sOption = pOption->GetInstrument()->GetInstrumentName();
  }
  std::cout << id << ": U(" << sUnderlying << "),O(" << sOption << "), B(" << m_bStartedWatch << ")" << std::endl;
}

void OptionEntry::HandleUnderlyingQuote(const ou::tf::Quote& quote_) {
  m_quoteLastUnderlying = quote_;
}

//void OptionEntry::HandleOptionQuote(const ou::tf::Quote& quote_) { // should this be kept?
//  if ( ! m_quoteLastOption.SameBidAsk( quote_ ) ) {
//    m_quoteLastOption = quote_;
//    m_bChanged = true;
//  }
//}

void OptionEntry::Calc( const fCalc_t& fCalc ) {
  fCalc( pOption, m_quoteLastUnderlying, fGreek );
}

// ====================

Engine::Engine( const ou::tf::LiborFromIQFeed& feed ): 
  m_InterestRateFeed( feed ), 
  m_srvcWork(boost::asio::make_work_guard( m_srvc )),
  m_timerScan( m_srvc )
{
  
  for ( std::size_t ix = 0; ix < 1; ix++ ) {  // change the final value to add threads, and fix the TODO further down
    m_threads.create_thread( boost::bind( &boost::asio::io_context::run, &m_srvc ) ); // add handlers
    
    m_fCalc =
      [](ou::tf::option::Option::pOption_t pOption, const ou::tf::Quote& quoteUnderlying, fCallbackWithGreek_t& fGreek){
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
  
  {
    std::lock_guard<std::mutex> lock(m_mutexOptionEntryOperationQueue);
    m_dequeOptionEntryOperation.clear();
  }
  
  m_srvcWork.reset();
  m_threads.join_all();
  
  m_mapOptionEntry.clear();
  
  m_mapKnownOptions.clear();
  m_mapKnownWatches.clear();
}

void Engine::Find( const pInstrument_t pInstrument, pWatch_t& pWatch ) {
  mapKnownWatches_t::iterator iter = m_mapKnownWatches.find( pInstrument->GetInstrumentName() );
  if ( m_mapKnownWatches.end() == iter ) {
    if ( nullptr != m_fBuildWatch ) {
      pWatch = m_fBuildWatch( pInstrument );
      m_mapKnownWatches.insert( mapKnownWatches_t::value_type( pInstrument->GetInstrumentName(), pWatch ) );
    }
    else {
      throw std::runtime_error( "Engine::m_fBuildWatch is nullptr" );
    }
  }
  else {
    pWatch = iter->second;
  }
}

void Engine::Find( const pInstrument_t pInstrument, pOption_t& pOption ) {
  mapKnownOptions_t::iterator iter = m_mapKnownOptions.find( pInstrument->GetInstrumentName() );
  if ( m_mapKnownOptions.end() == iter ) {
    if ( nullptr != m_fBuildOption ) {
      pOption = m_fBuildOption( pInstrument );
      m_mapKnownOptions.insert( mapKnownOptions_t::value_type( pInstrument->GetInstrumentName(), pOption ) );
    }
    else {
      throw std::runtime_error( "Engein::m_fBuildOption is nullptr" );
    }
  }
  else {
    pOption = iter->second;
  }
}

void Engine::Addv1( pOption_t pOption, pWatch_t pUnderlying, fCallbackWithGreek_t&& fGreek ) {
//  if ( m_srvcWork.owns_work() ) {
    OptionEntry oe( pUnderlying, pOption, std::move( fGreek ) );
    std::lock_guard<std::mutex> lock(m_mutexOptionEntryOperationQueue);
    m_dequeOptionEntryOperation.push_back( OptionEntryOperation( Action::AddOption, std::move(oe) ) );
//  }
}

void Engine::Add( pOption_t pOption, pWatch_t pUnderlying ) {
//  if ( m_srvcWork.owns_work() ) {
    OptionEntry oe( pUnderlying, pOption );
    std::lock_guard<std::mutex> lock(m_mutexOptionEntryOperationQueue);
    m_dequeOptionEntryOperation.push_back( OptionEntryOperation( Action::AddOption, std::move(oe) ) );
//  }
}

void Engine::Remove( pOption_t pOption ) {
//  if ( m_srvcWork.owns_work() ) {
    OptionEntry oe( pOption );
    std::lock_guard<std::mutex> lock(m_mutexOptionEntryOperationQueue);
    m_dequeOptionEntryOperation.push_back( OptionEntryOperation( Action::RemoveOption, std::move(oe) ) );
//  }
}

void Engine::HandleTimerScan( const boost::system::error_code &ec ) {
  if ( boost::asio::error::operation_aborted == ec ) {
    std::cout << "Engine::HandleTimerScan error: " << ec.message() << ", not starting new cycle" << std::endl;
  }
  else {
    if ( m_srvcWork.owns_work() ) {
      try {
        ScanOptionEntryQueue();
      }
      catch ( std::runtime_error& e ) {
        std::cout << "Engine::HandleTimerScan runtime: " << e.what() << std::endl;
      }
      catch (...) {
        std::cout << "Engine::HandleTimerScan exception: unknown" << std::endl;
      }
      
    // other ways:
      //timer_.expires_at(timer_.expiry() + boost::asio::chrono::seconds(1));
      m_timerScan.expires_after( boost::asio::chrono::milliseconds(250) );
      //m_timerScan.expires_after( boost::asio::chrono::milliseconds(750) );
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
          std::cout << "Engine::Add: " << oe.m_oe.OptionName() << std::endl;
          mapOptionEntry_t::iterator iterOption = m_mapOptionEntry.find( oe.m_oe.OptionName() );
          if ( m_mapOptionEntry.end() == iterOption ) {
            iterOption = m_mapOptionEntry.insert( m_mapOptionEntry.begin(), mapOptionEntry_t::value_type( oe.m_oe.OptionName(), std::move( oe.m_oe ) ) );
          }
          iterOption->second.Inc();
        }
        break;
      case Action::RemoveOption: {
          std::cout << "Engine::Remove: " << oe.m_oe.OptionName() << std::endl;
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
  
  // TODO:  process only those being watched
  
  // three step lambda call:
  //  1) lambda for each active mapOptionEntry
  //  2) capture private values from the OptionEntry
  //  3) use the values in a background thread for calculations via post to io_service
  std::for_each( m_mapOptionEntry.begin(), m_mapOptionEntry.end(), 
                [this, dtUtcNow](mapOptionEntry_t::value_type& vt){
                  //std::cout << "for each " << vt.second.GetUnderlying()->GetInstrument()->GetInstrumentName() << std::endl;
                  //std::cout << "         " << vt.second.GetOption()->GetInstrument()->GetInstrumentName() << std::endl;
                  vt.second.Calc( 
                    [this, dtUtcNow](OptionEntry::pOption_t pOption, const ou::tf::Quote& quoteUnderlying, fCallbackWithGreek_t& fCallbackWithGreek ){
                      double midpointUnderlying( quoteUnderlying.Midpoint() );
                      if ( 0.0 < midpointUnderlying ) {  // only start calculations once underlying has quotes
                        // TODO: add flag to start calculation only after previous calculation is complete
                        boost::asio::post( m_srvc, 
                          [this, dtUtcNow, pOption, midpointUnderlying, fCallbackWithGreek](){
                            try {
                              //boost::timer::auto_cpu_timer t;
                              ou::tf::option::binomial::structInput input;
                              input.S = midpointUnderlying;
                              pOption->CalcRate( input, dtUtcNow, m_InterestRateFeed );
                              pOption->CalcGreeks( input, dtUtcNow, true );
                              if ( nullptr != fCallbackWithGreek ) {
                                fCallbackWithGreek( pOption->LastGreek() ); // need to create the method
                              }
                            }
                            catch ( std::runtime_error& e ) {
                              std::cout << "Engine::ScanOptionEntryQueue runtime: " << e.what() << std::endl;
                            }
                            catch (...) {
                              std::cout << "Engine::ScanOptionEntryQueue exception: unknown" << std::endl;
                            }                            
                        });
                      }
                  });
                });
}

} // namespace option
} // namespace tf
} // namespace ou

