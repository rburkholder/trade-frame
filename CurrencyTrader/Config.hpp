/************************************************************************
 * Copyright(c) 2024, One Unified. All rights reserved.                 *
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
 * File:    Config.hpp
 * Author:  raymond@burkholder.net
 * Project: CurrencyTrader
 * Created: March 09, 2024 19:58:27
 */

#pragma once

#include <string>
#include <vector>
#include <ostream>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "Common.hpp"

namespace config {

struct CurrencyPair {

  std::string m_sName;
  boost::posix_time::time_duration m_tdStartTime;
  boost::posix_time::time_duration m_tdStopTime;
  std::string m_sTimeZone; // reference: Europe/London is GMT
  uint32_t m_nTradingAmount;

  CurrencyPair()
  : m_tdStartTime( boost::posix_time::not_a_date_time )
  , m_tdStopTime( boost::posix_time::not_a_date_time )
  , m_nTradingAmount {}
  {}

  CurrencyPair( std::string&& sName
              , boost::posix_time::time_duration tdStartTime
              , boost::posix_time::time_duration tdStopTime
              , std::string&& sTimeZone
              , uint32_t dblTradingAmount
                )
  : m_sName( std::move( sName ) )
  , m_tdStartTime( tdStartTime ), m_tdStopTime( tdStopTime )
  , m_sTimeZone( std::move( sTimeZone ) )
  , m_nTradingAmount( dblTradingAmount )
  {}

  CurrencyPair( const std::string& sName
              , const boost::posix_time::time_duration tdStartTime
              , const boost::posix_time::time_duration tdStopTime
              , const std::string& sTimeZone
              , const uint32_t m_nTradingAmount
                )
  : m_sName( sName )
  , m_tdStartTime( tdStartTime ), m_tdStopTime( tdStopTime )
  , m_sTimeZone( sTimeZone )
  , m_nTradingAmount( m_nTradingAmount )
  {}

  CurrencyPair( CurrencyPair&& ps )
  : m_sName( std::move( ps.m_sName ) )
  , m_tdStartTime( ps.m_tdStartTime )
  , m_tdStopTime( ps.m_tdStopTime )
  , m_sTimeZone( std::move( ps.m_sTimeZone ) )
  , m_nTradingAmount( ps.m_nTradingAmount )
  {}

  CurrencyPair( const CurrencyPair& ps )
  : m_sName( ps.m_sName )
  , m_tdStartTime( ps.m_tdStartTime )
  , m_tdStopTime( ps.m_tdStopTime )
  , m_sTimeZone( ps.m_sTimeZone )
  , m_nTradingAmount( ps.m_nTradingAmount )
  {}

  const CurrencyPair& operator=( const CurrencyPair& ps ) {
    if ( this != &ps ) {
      m_sName = ps.m_sName;
      m_tdStartTime = ps.m_tdStartTime;
      m_tdStopTime = ps.m_tdStopTime;
      m_sTimeZone = ps.m_sTimeZone;
      m_nTradingAmount = ps.m_nTradingAmount;
    }
    return *this;
  }

  void Parse( const std::string& ); // perform the parser here?

  // non member function - output - not used
  friend std::ostream& operator<<( std::ostream& os, CurrencyPair const& ps ) {
    //return os << ps._i;
    return os;
  }

  // non member function - input
  friend std::istream& operator>>( std::istream& is, CurrencyPair& ps ){
    //return os >> ps._i;
    std::string input;
    is >> input;
    ps.Parse( input );
    return is;
  }

};

// ===

struct Choices {

  std::string m_sBaseCurrency;
  double m_dblBaseCurrencyTopUp;

  std::string m_sExchange;
  int m_nIbInstance; // Interactive Brokers api instance

  std::string m_sHdf5File; // optional
  std::string m_sHdf5SimSet; // run simulation if present

  Strategy m_strategy;

  using vCurrencyPair_t = std::vector<CurrencyPair>;
  vCurrencyPair_t m_vCurrencyPair;

};

bool Load( const std::string& sFileName, Choices& );

} // namespace config
