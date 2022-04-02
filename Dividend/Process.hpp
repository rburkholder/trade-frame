/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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
 * File:    Process.hpp
 * Author:  raymond@burkholder.net
 * Project: Dividend
 * Created: April 1, 2022  21:48
 */

#include <vector>
#include <memory>
#include <thread>

#include <TFIQFeed/Provider.h>

namespace ou {
namespace tf {
  class AcquireFundamentals;
}
}

class Process {
public:

  struct dividend_t {
    const std::string sSymbol;
    std::string sExchange;
    double yield;
    double rate;
    double amount;
    int nAverageVolume;
    boost::gregorian::date dateExDividend;
    dividend_t( const std::string& sSymbol_ )
    : sSymbol( sSymbol_ ), yield {}, rate {} {}
  };
  using vSymbols_t = std::vector<dividend_t>;

  Process( vSymbols_t& );

  void Wait();

protected:
private:

  using pIQFeed_t = ou::tf::iqfeed::IQFeedProvider::pProvider_t;

  pIQFeed_t m_piqfeed;

  std::shared_ptr<ou::tf::AcquireFundamentals> m_pAcquireFundamentals_live;
  std::shared_ptr<ou::tf::AcquireFundamentals> m_pAcquireFundamentals_dead;

  vSymbols_t& m_vSymbols;
  vSymbols_t::iterator m_iterSymbols;

  bool m_bDone;
  std::mutex m_mutexWait;
  std::condition_variable m_cvWait;

  void HandleConnected( int );
  void Lookup();

};

