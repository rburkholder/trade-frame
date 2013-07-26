/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

// Started 2013/07/26

// Uses IQFeed for series source
// base class for Libor and for US Fed rate

#pragma once

#include <vector>
#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;

#include <TFTrading/Watch.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

class NoRiskInterestRateSeries {
public:

  typedef ou::tf::Watch::pProvider_t pProvider_t;

  struct structSymbol {
    time_duration td;
    std::string Symbol;
    structSymbol( void ) {};
    structSymbol( const time_duration& td_, const std::string& Symbol_ )
      : td( td_ ), Symbol( Symbol_ ) {};
  };

  typedef std::vector<structSymbol> vSymbol_t;

  explicit NoRiskInterestRateSeries( const vSymbol_t& vSymbol );
  virtual ~NoRiskInterestRateSeries(void);

  void SetWatchOn( pProvider_t pProvider );
  void SetWatchOff( void );

  double ValueAt( time_duration td );  // index to determine appropriate interest rate

  void EmitYieldCurve( void );

protected:
private:

  typedef ou::tf::Watch::pWatch_t pWatch_t;

  struct structInterestRate {
    time_duration td;
    std::string Symbol;
    pWatch_t pWatch;
    structInterestRate( void ) {};
    structInterestRate( const time_duration& td_, const std::string& Symbol_ )
      : td( td_ ), Symbol( Symbol_ ) {};
    structInterestRate( const structSymbol& symbol )
      : td( symbol.td ), Symbol( symbol.Symbol ) {};
  };

  typedef std::vector<structInterestRate> vInterestRate_t;
  typedef vInterestRate_t::iterator vInterestRate_iter_t;
  vInterestRate_t m_vInterestRate;

  struct compareInterestRate {
    bool operator()( const structInterestRate& lhs, const structInterestRate& rhs ) const { return lhs.td < rhs.td; };
  };

  bool m_bInitialized;
  bool m_bWatching;

  pProvider_t m_pProvider;

};

} // namespace tf
} // namespace ou
