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

// code could be optimized to be calculated at specific intervals such as 1 second
// value could then be used for all options expirying on same date
// rather than calculating each and every time a request is made.

#pragma once

#include <vector>
#include <string>
#include <ostream>

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;

#include <TFTrading/Watch.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

class NoRiskInterestRateSeries {
public:

  friend std::ostream& operator<<( std::ostream& os, const NoRiskInterestRateSeries& nrirs );

  typedef ou::tf::Watch::pProvider_t pProvider_t;

  struct structSymbol { // used for importing symbols into the object
    time_duration td;
    std::string Symbol;
    structSymbol( void ) {};
    structSymbol( const time_duration& td_, const std::string& Symbol_ )
      : td( td_ ), Symbol( Symbol_ ) {};
  };

  typedef std::vector<structSymbol> vSymbol_t;

  NoRiskInterestRateSeries( void );
  virtual ~NoRiskInterestRateSeries( void );

  void SetWatchOn( pProvider_t pProvider );
  void SetWatchOff( void );

  double ValueAt( time_duration td );  // index to determine appropriate interest rate

//  void EmitYieldCurve( void );

protected:
  void Initialize( const vSymbol_t& vSymbol );  // called from inheritor's constructor
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

std::ostream& operator<<( std::ostream& os, const NoRiskInterestRateSeries& nrirs );

class LiborFromIQFeed: public NoRiskInterestRateSeries {
public:
  LiborFromIQFeed( void );
  ~LiborFromIQFeed( void );
protected:
private:
};

// http://www.treasury.gov/resource-center/data-chart-center/interest-rates/Pages/TextView.aspx?data=yield
// http://www.federalreserve.gov/releases/H15/update/

class FedRateFromIQFeed: public NoRiskInterestRateSeries {
public:
  FedRateFromIQFeed( void );
  ~FedRateFromIQFeed( void );
protected:
private:
};

} // namespace tf
} // namespace ou
