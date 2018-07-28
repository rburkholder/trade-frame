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

#include <algorithm>

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
  m_quoteLastOption = rhs.m_quoteLastOption;
  m_bChanged = rhs.m_bChanged;
}

OptionEntry::OptionEntry( pOption_t pOption_): pOption( pOption_ ) {}

OptionEntry::OptionEntry( pWatch_t pUnderlying_, pOption_t pOption_, fGreekResultCallback_t&& fGreek_ ):
  pUnderlying( pUnderlying_ ), pOption( pOption_ ), fGreek( std::move( fGreek_ ) ), 
  m_bChanged( false ),
  cntInstances {} {
    pUnderlying->OnQuote.Add( MakeDelegate( this, &OptionEntry::HandleUnderlyingQuote) );
    pUnderlying->StartWatch();
    pOption->OnQuote.Add( MakeDelegate( this, &OptionEntry::HandleOptionQuote ) );
    pOption->StartWatch();
}
  
OptionEntry::~OptionEntry() {
  pOption->StopWatch();
  pOption->OnQuote.Remove( MakeDelegate( this, &OptionEntry::HandleOptionQuote ) );
  pUnderlying->StopWatch();
  pUnderlying->OnQuote.Remove( MakeDelegate( this, &OptionEntry::HandleUnderlyingQuote ) );
}

void OptionEntry::HandleUnderlyingQuote(const ou::tf::Quote& quote_) {
  if ( ! m_quoteLastUnderlying.SameBidAsk( quote_ ) ) {
    m_quoteLastUnderlying = quote_;
    m_bChanged = true;
  }
}

void OptionEntry::HandleOptionQuote(const ou::tf::Quote& quote_) {
  if ( ! m_quoteLastOption.SameBidAsk( quote_ ) ) {
    m_quoteLastOption = quote_;
    m_bChanged = true;
  }
}

void OptionEntry::Calc( const fCalc_t& fCalc ) {
  if ( m_bChanged ) {
    fCalc( pOption, m_quoteLastUnderlying, m_quoteLastOption, fGreek );
    m_bChanged = false;
  }
}

// ====================

Engine::Engine( ou::tf::NoRiskInterestRateSeries& feed ): m_InterestRateFeed( feed ) {
  boost::asio::io_service::work work( m_srvc );  // keep things running while real work arrives
  for ( std::size_t ix = 0; ix < 1; ix++ ) {
    //m_threads.create_thread( boost::bind( &boost::asio::io_service::run, &m_srvc ) ); // add handlers
    m_threads.create_thread( boost::bind( &boost::asio::io_service::run, &m_srvc ) ); // add handlers
    
    m_fCalc = // also will need current date, expiry date
      [](ou::tf::option::Option::pOption_t pOption, const ou::tf::Quote& quoteUnderlying, const ou::tf::Quote& quoteOption, fGreekResultCallback_t& fGreek){
    };
  }
}

Engine::~Engine( ) {
}

void Engine::Add( pWatch_t pUnderlying, pOption_t pOption, fGreekResultCallback_t&& fGreek ) {
  OptionEntry oe( pUnderlying, pOption, std::move( fGreek ) );
  std::lock_guard<std::mutex> lock(m_mutexOptionEntryOperationQueue);
  m_dequeOptionEntryOperation.push_back( OptionEntryOperation( Action::AddOption, std::move(oe) ) );
}

void Engine::Delete( pOption_t pOption ) {
  OptionEntry oe( pOption );
  std::lock_guard<std::mutex> lock(m_mutexOptionEntryOperationQueue);
  m_dequeOptionEntryOperation.push_back( OptionEntryOperation( Action::RemoveOption, std::move(oe) ) );
}

void Engine::ProcessOptionEntryOperationQueue() {
  std::lock_guard<std::mutex> lock(m_mutexOptionEntryOperationQueue);
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

// TODO: sort map by expiry?  // then Option::CalcRate is required less often
void Engine::Scan() {

  ProcessOptionEntryOperationQueue();
  std::for_each( m_mapOptionEntry.begin(), m_mapOptionEntry.end(), 
                [this](mapOptionEntry_t::value_type& vt){
                  // TODO: do the calc and in the 
                  //m_srvc.post( boost::bind( &OptionEntry::Calc, &vt.second, m_fCalc ) );  
                  vt.second.Calc( [this](OptionEntry::pOption_t, const ou::tf::Quote&, const ou::tf::Quote&, fGreekResultCallback_t&){
                    m_srvc.post( [](){});
                  });
                });
}

} // namespace option
} // namespace tf
} // namespace ou

