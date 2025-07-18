/************************************************************************
 * Copyright(c) 2021, One Unified. All rights reserved.                 *
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
 * File:    BuildInstrument.hpp
 * Author:  raymond@burkholder.net
 * Project: TFTrading
 * Created: Sept 20, 2021, 21:52
 */

#pragma once

#include <set>
#include <map>
#include <functional>

#include <TFIQFeed/Provider.h>

#include <TFInteractiveBrokers/IBTWS.h>

#include "AcquireFundamentals.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

// can be run as shared_ptr, queues are thread safe

class BuildInstrument {
public:

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;

  using fInstrument_t = std::function<void(pInstrument_t, bool bConstructed )>;

  using pProviderIBTWS_t = ou::tf::ib::TWS::pProvider_t;
  using pProviderIQFeed_t = ou::tf::iqfeed::Provider::pProvider_t;

  BuildInstrument( pProviderIQFeed_t );
  BuildInstrument( pProviderIQFeed_t, pProviderIBTWS_t );

  void Queue( const std::string& sIQFeedSymbol, fInstrument_t&& );
  void Clear();

  bool Active();

protected:
private:

  using pAcquireFundamentals_t = AcquireFundamentals::pAcquireFundamentals_t;

  using setSymbol_t = std::set<std::string>;

  using mapSymbol_t = std::map<std::string,fInstrument_t>;

  // TODO: need a completion function?
  struct InProgress {
    pAcquireFundamentals_t pAcquireFundamentals;
    fInstrument_t fInstrument;
    InProgress( fInstrument_t&& fInstrument_ )
    : fInstrument( std::move( fInstrument_ ) ) {}
  };

  using mapInProgress_t = std::map<std::string,InProgress>;

  using fInstrumentDone_t = std::function<void(const std::string&)>;

  std::mutex m_mutexMap;

  mapSymbol_t m_mapSymbol;  // contains symbols waiting to be built
  mapInProgress_t m_mapInProgress; // waiting for fundamentals, contract

  bool m_bDeleteIterator;
  mapInProgress_t::iterator m_iterToDelete; // used to break the recursive problem when only iqf is available

  pProviderIQFeed_t m_pIQ;
  pProviderIBTWS_t m_pIB;

  void Update();
  void Update( mapInProgress_t::iterator );

  void Build( mapInProgress_t::iterator );
};

} // namespace tf
} // namespace ou
