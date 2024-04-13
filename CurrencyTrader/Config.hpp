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
#include <ostream>

#include <boost/date_time/posix_time/posix_time.hpp>

namespace config {

struct Choices {

  std::string m_sExchange;
  std::string m_sHdf5File; // optional
  std::string m_sHdf5SimSet; // run simulation if present

  struct PairSettings {

    std::string m_sName;
    boost::posix_time::time_duration m_tdStartTime;
    boost::posix_time::time_duration m_tdStopTime;
    std::string m_sTimeZone; // Europe/London GMT

    PairSettings()
    : m_tdStartTime( boost::posix_time::not_a_date_time )
    , m_tdStopTime( boost::posix_time::not_a_date_time )
    {}

    PairSettings( std::string&& sName
                , boost::posix_time::time_duration tdStartTime
                , boost::posix_time::time_duration tdStopTime
                , std::string&& sTimeZone
                  )
    : m_sName( std::move( sName ) )
    , m_tdStartTime( tdStartTime ), m_tdStopTime( tdStopTime )
    , m_sTimeZone( std::move( sTimeZone ) )
    {}

    PairSettings( const std::string& sName
                , const boost::posix_time::time_duration tdStartTime
                , const boost::posix_time::time_duration tdStopTime
                , const std::string& sTimeZone
                  )
    : m_sName( sName )
    , m_tdStartTime( tdStartTime ), m_tdStopTime( tdStopTime )
    , m_sTimeZone( sTimeZone )
    {}

    PairSettings( PairSettings&& ps )
    : m_sName( std::move( ps.m_sName ) )
    , m_tdStartTime( ps.m_tdStartTime )
    , m_tdStopTime( ps.m_tdStopTime )
    , m_sTimeZone( std::move( ps.m_sTimeZone ) )
    {}

    PairSettings( const PairSettings& ps )
    : m_sName( ps.m_sName )
    , m_tdStartTime( ps.m_tdStartTime )
    , m_tdStopTime( ps.m_tdStopTime )
    , m_sTimeZone( ps.m_sTimeZone )
    {}

    const PairSettings& operator=( const PairSettings& ps ) {
      if ( this != &ps ) {
        m_sName = ps.m_sName;
        m_tdStartTime = ps.m_tdStartTime;
        m_tdStopTime = ps.m_tdStopTime;
        m_sTimeZone = ps.m_sTimeZone;
      }
      return *this;
    }

    void Parse( const std::string& ); // perform the parser here?

    // non member function - output - not used
    friend std::ostream& operator<<( std::ostream& os, PairSettings const& ps ) {
      //return os << ps._i;
      return os;
    }

    // non member function - input
    friend std::istream& operator>>( std::istream& is, PairSettings& ps ){
      //return os >> ps._i;
      std::string input;
      is >> input;
      ps.Parse( input );
      return is;
    }

  };

  using vPairSettings_t = std::vector<PairSettings>;
  vPairSettings_t m_vPairSettings;

  int m_nIbInstance; // Interactive Brokers api instance

  std::string m_sMaxTradeLifeTime; // minutes
  boost::posix_time::time_duration m_tdMaxTradeLifeTime;

  unsigned int m_nPipProfit;
  unsigned int m_nPipStopLoss;
  unsigned int m_nPipTrailingStop;
  unsigned int m_nLotSize;

  unsigned int m_nBarSeconds;

};

bool Load( const std::string& sFileName, Choices& );

} // namespace config
