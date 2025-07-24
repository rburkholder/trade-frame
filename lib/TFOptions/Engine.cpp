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

#include <boost/bind/bind.hpp>
#include <boost/log/trivial.hpp>

#include <OUCommon/TimeSource.h>

#include "Engine.h"
#include "Binomial.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

OptionEntry::OptionEntry( OptionEntry&& rhs ) {
  if ( 0 < rhs.m_cntInstances ) {
    rhs.m_pUnderlying->OnQuote.Remove( MakeDelegate( &rhs, &OptionEntry::HandleUnderlyingQuote ) );
  }
  m_cntInstances = rhs.m_cntInstances;
  m_pOption = std::move( rhs.m_pOption );
  m_pUnderlying = std::move( rhs.m_pUnderlying );
  m_fGreek = std::move( rhs.m_fGreek );
  //m_bStartedWatch = rhs.m_bStartedWatch;
  //rhs.m_bStartedWatch = false;
  rhs.m_cntInstances = 0; // can this be set, what happens on delete?  what happens when tied to m_bStartedWatch?
  //if ( m_bStartedWatch ) {
  if ( 0 < m_cntInstances ) {
    m_pUnderlying->OnQuote.Add( MakeDelegate( this, &OptionEntry::HandleUnderlyingQuote) );
  }
  //PrintState( "OptionEntry::OptionEntry(0)" );
}

//OptionEntry::OptionEntry( pOption_t pOption_): m_pOption( pOption_ ), /*m_bStartedWatch( false ),*/ m_cntInstances( 0 ) {
//  std::cout << "*** should not be here, does nothing ***" << std::endl;
  //PrintState( "OptionEntry::OptionEntry(1)" );
//}  // can't start with out an underlying

OptionEntry::OptionEntry( pWatch_t pUnderlying_, pOption_t pOption_, fCallbackWithGreek_t&& fGreek_ ):
  m_pUnderlying( pUnderlying_ ), m_pOption( pOption_ ), m_fGreek( std::move( fGreek_ ) ),
  //m_bStartedWatch( false ),
  m_cntInstances( 0 ) // handled by Inc, Dec
{
  //m_pUnderlying->OnQuote.Add( MakeDelegate( this, &OptionEntry::HandleUnderlyingQuote) );
  //m_pUnderlying->StartWatch();
  //pOption->OnQuote.Add( MakeDelegate( this, &OptionEntry::HandleOptionQuote ) );
  //m_pOption->StartWatch();
  //m_bStartedWatch = true;
  //PrintState( "OptionEntry::OptionEntry(2)" );
}

OptionEntry::OptionEntry( pWatch_t pUnderlying_, pOption_t pOption_ ):
  m_pUnderlying( pUnderlying_ ), m_pOption( pOption_ ),
  //m_bStartedWatch( false ),
  m_cntInstances( 0 )
{
  //m_pUnderlying->OnQuote.Add( MakeDelegate( this, &OptionEntry::HandleUnderlyingQuote) );
  //m_pUnderlying->StartWatch();
  //pOption->OnQuote.Add( MakeDelegate( this, &OptionEntry::HandleOptionQuote ) );
  //m_pOption->StartWatch();
  //m_bStartedWatch = true;
  //PrintState( "OptionEntry::OptionEntry(3)" );
}

// need to properly work with m_bStartedWatch and cntInstances
OptionEntry::~OptionEntry() {
  //pOption->OnQuote.Remove( MakeDelegate( this, &OptionEntry::HandleOptionQuote ) );
  //PrintState( "OptionEntry::~OptionEntry" );
  //if ( m_bStartedWatch ) {

  if ( 0 < m_cntInstances ) {
    std::cout << "OptionEntry::~OptionEntry m_cntInstances was not zero: " << m_cntInstances << "," << m_pOption->GetInstrument()->GetInstrumentName() << std::endl;
    m_cntInstances = 1;
    Dec();
    //m_pUnderlying->StopWatch();
    //m_pOption->StopWatch();
    //m_pUnderlying->OnQuote.Remove( MakeDelegate( this, &OptionEntry::HandleUnderlyingQuote ) );
    //m_cntInstances = 0;
    //m_bStartedWatch = false;
  }
}

void OptionEntry::PrintState( const std::string id ) {
  std::string sUnderlying;
  std::string sOption;
  if ( 0 < m_pUnderlying.use_count() ) {
    sUnderlying = m_pUnderlying->GetInstrument()->GetInstrumentName();
  }
  if ( 0 < m_pOption.use_count() ) {
    sOption = m_pOption->GetInstrument()->GetInstrumentName();
  }
  std::cout << id << ": U(" << sUnderlying << "),O(" << sOption /* << "), B(" << m_bStartedWatch */ << ")" << std::endl;
}

void OptionEntry::Inc() {
  if ( 0 == m_cntInstances ) {
    m_pUnderlying->OnQuote.Add( MakeDelegate( this, &OptionEntry::HandleUnderlyingQuote ) );
    m_pUnderlying->StartWatch();
    m_pOption->StartWatch();  }
  m_cntInstances++;
}
size_t OptionEntry::Dec() {
  assert( 0 < m_cntInstances );
  m_cntInstances--;
  if ( 0 == m_cntInstances ) {
    m_pUnderlying->StopWatch();
    m_pOption->StopWatch();
    m_pUnderlying->OnQuote.Remove( MakeDelegate( this, &OptionEntry::HandleUnderlyingQuote ) );
  }
  return m_cntInstances;
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
  fCalc( m_pOption, m_quoteLastUnderlying, m_fGreek );
}

// ====================

Engine::Engine( const ou::tf::NoRiskInterestRateSeries& feed ):
  m_InterestRateFeed( feed ),
  m_srvcWork(boost::asio::make_work_guard( m_srvc )),
  m_timerScan( m_srvc )
{

  for ( std::size_t ix = 0; ix < 1; ix++ ) {  // change the final value to add threads, and fix the TODO further down
    m_threads.create_thread( boost::bind( &boost::asio::io_context::run, &m_srvc ) ); // add handlers
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

  // TODO: perform a deque of all operations?

  if ( !m_dequeOptionEntryOperation.empty() ) {
    BOOST_LOG_TRIVIAL(warning) << "Engine::~Engine: operations still remaining in the queue";
  }

  {
    std::scoped_lock<std::mutex> lock(m_mutexOptionEntryOperationQueue);
    m_dequeOptionEntryOperation.clear();
  }

  m_srvcWork.reset();
  m_threads.join_all();

  m_mapOptionEntry.clear();

  m_mapKnownOptions.clear();
  m_mapKnownWatches.clear();
}

void Engine::RegisterUnderlying( const pWatch_t& pWatch ) {
  assert( pWatch );
  std::scoped_lock<std::mutex> lock(m_mutexOptionEntryOperationQueue);
  const std::string& sInstrumentName( pWatch->GetInstrument()->GetInstrumentName() );
  mapKnownWatches_t::iterator iter = m_mapKnownWatches.find( sInstrumentName );
  if ( m_mapKnownWatches.end() == iter ) {
    m_mapKnownWatches.insert( mapKnownWatches_t::value_type( sInstrumentName, pWatch ) );
  }
  else {
    throw std::runtime_error( "Engine::Register Underlying: already exists - " + sInstrumentName );
  }
}

void Engine::RegisterOption( const pOption_t& pOption) {
  assert( pOption );
  const std::string& sInstrumentName( pOption->GetInstrument()->GetInstrumentName() );
  //BOOST_LOG_TRIVIAL(trace) << "Engine queue Option_Register " << sInstrumentName;

  OptionEntry oe( nullptr, pOption );
  std::scoped_lock<std::mutex> lock(m_mutexOptionEntryOperationQueue);
  m_dequeOptionEntryOperation.push_back( OptionEntryOperation( Action::Option_Register, std::move( oe ) ) );
}

void Engine::DeRegisterOption( const pOption_t& pOption) {
  assert( pOption );
  const std::string& sInstrumentName( pOption->GetInstrumentName() );
  //BOOST_LOG_TRIVIAL(trace) << "Engine queue Option_DeRegister " << sInstrumentName;

  OptionEntry oe( nullptr, pOption );
  std::scoped_lock<std::mutex> lock(m_mutexOptionEntryOperationQueue);
  m_dequeOptionEntryOperation.push_back( OptionEntryOperation( Action::Option_DeRegister, std::move( oe ) ) );
}

// if used, then need to perform a lookup on the underlying first to prevent duplicated effort
//void Engine::Register( pOption_t& pOption, pWatch_t& pWatch) {
//  Register( pWatch );
//  Register( pOption );
//}

// needs to be used to load up underlying watch
ou::tf::Watch::pWatch_t Engine::FindWatch( const pInstrument_t pInstrument ) {
  //std::cout << "Engine::Find Watch: " << pWatch->GetInstrument()->GetInstrumentName() << std::endl;
  //std::cout << "Engine::Find Watch: " << pInstrument->GetInstrumentName() << std::endl;
  pWatch_t pWatch;
  std::scoped_lock<std::mutex> lock(m_mutexOptionEntryOperationQueue);
  mapKnownWatches_t::iterator iter = m_mapKnownWatches.find( pInstrument->GetInstrumentName() );
  if ( m_mapKnownWatches.end() == iter ) {
    if ( nullptr != m_fBuildWatch ) {
      pWatch = m_fBuildWatch( pInstrument );
      assert( 0 != pWatch.get() );
      m_mapKnownWatches.insert( mapKnownWatches_t::value_type( pInstrument->GetInstrumentName(), pWatch ) );
    }
    else {
      throw std::runtime_error( "Engine::m_fBuildWatch is nullptr" );
    }
  }
  else {
    pWatch = iter->second;
  }
  assert( pWatch );
  return pWatch;
}

// needs to be used to load up options
Option::pOption_t Engine::FindOption( const pInstrument_t pInstrument ) {
  //std::cout << "Engine::Find Option: " << pOption->GetInstrument()->GetInstrumentName() << std::endl;
  //std::cout << "Engine::Find Option: " << pInstrument->GetInstrumentName() << std::endl;
  pOption_t pOption;
  std::scoped_lock<std::mutex> lock(m_mutexOptionEntryOperationQueue);
  mapKnownOptions_t::iterator iter = m_mapKnownOptions.find( pInstrument->GetInstrumentName() );
  if ( m_mapKnownOptions.end() == iter ) {
    if ( nullptr != m_fBuildOption ) {
      pOption = m_fBuildOption( pInstrument );
      assert( 0 != pOption.get() );
      m_mapKnownOptions.insert( mapKnownOptions_t::value_type( pInstrument->GetInstrumentName(), pOption ) );
    }
    else {
      throw std::runtime_error( "Engine::m_fBuildOption is nullptr" );
    }
  }
  else {
    pOption = iter->second;
  }
  assert( 0 != pOption.get() );
  return pOption;
}

void Engine::Add( pOption_t pOption, pWatch_t pUnderlying ) {
//  if ( m_srvcWork.owns_work() ) {
    assert( ( 0 != pOption.use_count() ) && ( 0 != pUnderlying.use_count() ) );
    OptionEntry oe( pUnderlying, pOption );
    //BOOST_LOG_TRIVIAL(trace) << "Engine queue Option_Add " << pOption->GetInstrumentName();
    std::scoped_lock<std::mutex> lock(m_mutexOptionEntryOperationQueue);
    m_dequeOptionEntryOperation.push_back( OptionEntryOperation( Action::Option_Add, std::move(oe) ) );
//  }
}

void Engine::Remove( pOption_t pOption, pWatch_t pUnderlying ) {
//  if ( m_srvcWork.owns_work() ) {
    assert( ( 0 != pOption.use_count() ) && ( 0 != pUnderlying.use_count() ) );
    OptionEntry oe( pUnderlying, pOption );
    //BOOST_LOG_TRIVIAL(trace) << "Engine queue Option_Remove " << pOption->GetInstrumentName();
    std::scoped_lock<std::mutex> lock(m_mutexOptionEntryOperationQueue);
    m_dequeOptionEntryOperation.push_back( OptionEntryOperation( Action::Option_Remove, std::move(oe) ) );
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
      //m_timerScan.expires_after( boost::asio::chrono::milliseconds(250) );
      m_timerScan.expires_after( boost::asio::chrono::milliseconds( 495 ) );
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
  std::scoped_lock<std::mutex> lock(m_mutexOptionEntryOperationQueue);
  if ( !m_dequeOptionEntryOperation.empty() ) {
    OptionEntryOperation& oe( m_dequeOptionEntryOperation.front() );

    switch( oe.m_action ) {
      case Action::Option_Add: {
          const std::string& sUnderlying( oe.m_oe.UnderlyingName() );
          const std::string& sOption( oe.m_oe.OptionName() );
          std::string MapKey( sUnderlying + "_" + sOption );
          //BOOST_LOG_TRIVIAL(trace) << "Engine action AddOption " << MapKey;

          mapKnownWatches_t::iterator iterWatches = m_mapKnownWatches.find( sUnderlying );
          if ( m_mapKnownWatches.end() == iterWatches ) {
            //m_mapKnownWatches.insert( mapKnownWatches_t::value_type( sUnderlying, iterOption->second.GetUnderlying() ) );
            throw  std::runtime_error( "Engine::ProcessOptionEntryOperationQueue doesn't find known watch: " + sUnderlying );
          }

          mapKnownOptions_t::iterator iterOptions = m_mapKnownOptions.find( sOption );
          if ( m_mapKnownOptions.end() == iterOptions ) {
            //m_mapKnownOptions.insert( mapKnownOptions_t::value_type( sOption, iterOption->second.GetOption() ) );
            throw  std::runtime_error( "Engine::ProcessOptionEntryOperationQueue doesn't find known option " + sOption );
          }

          mapOptionEntry_t::iterator iterOption = m_mapOptionEntry.find( MapKey );
          if ( m_mapOptionEntry.end() == iterOption ) {
            iterOption = m_mapOptionEntry.insert( m_mapOptionEntry.begin(), mapOptionEntry_t::value_type(MapKey, std::move( oe.m_oe ) ) );
            //std::cout << "Engine::AddOption: " << MapKey << " added" << std::endl;
          }
          else {
            //std::cout << "Engine::AddOption: " << MapKey << " dropped" << std::endl;
          }
          iterOption->second.Inc();
        }
        break;
      case Action::Option_Remove: {
          const std::string& sUnderlying( oe.m_oe.UnderlyingName() );
          const std::string& sOption( oe.m_oe.OptionName() );
          std::string MapKey( sUnderlying + "_" + sOption );
          //BOOST_LOG_TRIVIAL(trace) << "Engine action RemoveOption " << MapKey;

          // should option and instrument be removed from m_mapKnownWatches, m_mapKnownOptions?
          // if so, then maps require counters, or use the pOption_t use_count?
          mapOptionEntry_t::iterator iterOption = m_mapOptionEntry.find( MapKey );
          if ( m_mapOptionEntry.end() == iterOption ) {
            throw std::runtime_error( "Engine::Remove: can't find option" + MapKey );
          }

          OptionEntry::size_type cnt = iterOption->second.Dec();
          if ( 0 == cnt ) {
            m_mapOptionEntry.erase( iterOption );
            //std::cout << "Engine::RemoveOption: " << MapKey << " erased" << std::endl;
          }
          else {
            //std::cout << "Engine::RemoveOption: " << MapKey << " count " << cnt << std::endl;
          }
        }
        break;
      case Action::Option_Register: {
          const std::string& sInstrumentName( oe.m_oe.GetOption()->GetInstrumentName() );
          //BOOST_LOG_TRIVIAL(trace) << "Engine action Option_Register " << sInstrumentName;
          mapKnownOptions_t::iterator iter = m_mapKnownOptions.find( sInstrumentName );
          if ( m_mapKnownOptions.end() == iter ) {
            m_mapKnownOptions.insert( mapKnownOptions_t::value_type( sInstrumentName, oe.m_oe.GetOption() ) );
          }
          else {
            //throw std::runtime_error( "Engine::Register Option: already exists - " + sInstrumentName );
            BOOST_LOG_TRIVIAL(error) << "Engine action Option_Register " << sInstrumentName << " already exists";
          }
        }
        break;
      case Action::Option_DeRegister: {
          const std::string& sInstrumentName( oe.m_oe.GetOption()->GetInstrumentName() );
          //BOOST_LOG_TRIVIAL(trace) << "Engine action Option_DeRegister " << sInstrumentName;
          mapKnownOptions_t::iterator iter = m_mapKnownOptions.find( sInstrumentName );
          if ( m_mapKnownOptions.end() != iter ) {
            // todo: check that there has been a remove action on the option
            // todo: check that does not come too soon after Remove( option )
            m_mapKnownOptions.erase( iter );
          }
          else {
            //throw std::runtime_error( "Engine::DeRegister Option: does not exist - " + sInstrumentName );
            BOOST_LOG_TRIVIAL(error) << "Engine action Option_DeRegister " << sInstrumentName << " does not exist";
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
  std::for_each(
    m_mapOptionEntry.begin(), m_mapOptionEntry.end(),
    [this, dtUtcNow](mapOptionEntry_t::value_type& vt){
      //std::cout << "for each " << vt.second.GetUnderlying()->GetInstrument()->GetInstrumentName() << std::endl;
      //std::cout << "         " << vt.second.GetOption()->GetInstrument()->GetInstrumentName() << std::endl;
      vt.second.Calc(
        [this, dtUtcNow](OptionEntry::pOption_t pOption, const ou::tf::Quote& quoteUnderlying, fCallbackWithGreek_t& fCallbackWithGreek ){
          if ( !quoteUnderlying.IsNonZero() ) {
            // underlying is unstable
          }
          else {
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
                    pOption->CalcGreeks( input, dtUtcNow, true ); // TODO, don't proceed if option quote is bad (test on exit)
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
          }
      });
    });
}

} // namespace option
} // namespace tf
} // namespace ou

