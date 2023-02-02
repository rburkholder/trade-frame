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
// value could then be used for all options expiring on same date
// rather than calculating each and every time a request is made.

#pragma once

#include <vector>
#include <string>
#include <ostream>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <TFTrading/Watch.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

class NoRiskInterestRateSeries {
public:

  friend std::ostream& operator<<( std::ostream& os, const NoRiskInterestRateSeries& nrirs );

  using pProvider_t = ou::tf::Watch::pProvider_t;

  struct structSymbol { // used for importing symbols into the object
    time_duration td;
    std::string Symbol;
    structSymbol() {};
    structSymbol( const boost::posix_time::time_duration& td_, const std::string& Symbol_ )
      : td( td_ ), Symbol( Symbol_ ) {};
  };

  NoRiskInterestRateSeries();
  virtual ~NoRiskInterestRateSeries();

  void SetWatchOn( pProvider_t pProvider );
  void SetWatchOff();

  double ValueAt( boost::posix_time::time_duration td ) const;  // index to determine appropriate interest rate

  bool Watching() const { return m_bWatching; }

  void SaveSeries( const std::string& sPrefix );

protected:

  using vSymbol_t = std::vector<structSymbol>;

  std::string m_sDescription;

  void AssignSymbols( const vSymbol_t& vSymbol );  // called from inheritor's constructor

private:

  using pWatch_t = ou::tf::Watch::pWatch_t;

  struct structInterestRate {
    boost::posix_time::time_duration td;
    std::string Symbol;
    pWatch_t pWatch;
    structInterestRate( void ) {};
    structInterestRate( const boost::posix_time::time_duration& td_, const std::string& Symbol_ )
      : td( td_ ), Symbol( Symbol_ ) {};
    structInterestRate( const structSymbol& symbol )
      : td( symbol.td ), Symbol( symbol.Symbol ) {};
  };

  using vInterestRate_t = std::vector<structInterestRate>;
  using vInterestRate_iter_t = vInterestRate_t::iterator;
  using vInterestRate_citer_t = vInterestRate_t::const_iterator;
  vInterestRate_t m_vInterestRate;

  struct compareInterestRate {
    bool operator()( const structInterestRate& lhs, const structInterestRate& rhs ) const { return lhs.td < rhs.td; };
  };

  bool m_bInitialized;
  bool m_bWatching;

  pProvider_t m_pProvider;

  void Initialize();

};

std::ostream& operator<<( std::ostream& os, const NoRiskInterestRateSeries& nrirs );

class LiborFromIQFeed: public NoRiskInterestRateSeries { // deprecated, use FedRate for now
public:
  LiborFromIQFeed();
  virtual ~LiborFromIQFeed();
protected:
private:
};

// http://www.treasury.gov/resource-center/data-chart-center/interest-rates/Pages/TextView.aspx?data=yield
// http://www.federalreserve.gov/releases/H15/update/

class FedRateFromIQFeed: public NoRiskInterestRateSeries {
public:
  FedRateFromIQFeed();
  virtual ~FedRateFromIQFeed();
protected:
private:
};

} // namespace tf
} // namespace ou
