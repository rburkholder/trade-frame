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

#include <map>
#include <vector>
#include <memory>

#include <TFIQFeed/Provider.h>

#include "Config.hpp"

namespace ou {
namespace tf {
  class AcquireFundamentals;
}
}

class Process {
public:

  struct dividend_t {
    const std::string sSymbol;
    std::string sCompanyName;
    std::string sExchange;
    double yield;
    double yield_calculated; // implemented as DTN not supplying yield
    double rate;
    double amount;
    double trade; // last trade
    int nAverageVolume;
    double nSharesOutstanding;
    boost::gregorian::date datePayed;
    boost::gregorian::date dateExDividend;
    std::string sState;
    std::string sOptionRoots;

    dividend_t( const std::string& sSymbol_ )
    : sSymbol( sSymbol_ ), yield {}, rate {}, trade {} {}
  };
  using vSymbols_t = std::vector<dividend_t>;

  Process( const config::Choices& choices, vSymbols_t& );
  ~Process();

  void Wait();

protected:
private:

  const config::Choices& m_choices;

  using pIQFeed_t = ou::tf::iqfeed::Provider::pProvider_t;

  pIQFeed_t m_piqfeed;

  using vSymbols_iter = vSymbols_t::iterator;
  vSymbols_t& m_vSymbols;
  vSymbols_iter m_iterSymbols;

  using pAcquireFundamentals_t = std::shared_ptr<ou::tf::AcquireFundamentals>;

  struct InProgress {
    vSymbols_iter iterSymbols;
    pAcquireFundamentals_t pAcquireFundamentals;
  };

  using mapInProgress_t = std::map<std::string,InProgress>;
  mapInProgress_t m_mapInProgress;

  using mapIgnoreName_t = std::map<std::string, bool>;
  mapIgnoreName_t m_mapIgnoreName; // bool, true when found in a match

  using mapSecurityState_t = std::map<std::string, std::string>;
  mapSecurityState_t m_mapSecurityState;

  // assumes single thread
  pAcquireFundamentals_t m_pAcquireFundamentals_burial;

  bool m_bDone;
  std::mutex m_mutexWait;
  std::condition_variable m_cvWait;

  void HandleConnected( int );
  void Lookup();

};

