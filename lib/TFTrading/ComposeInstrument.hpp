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
 * File:    ComposeInstrument.h
 * Author:  raymond@burkholder.net
 * Project: TFTrading
 * Created: 2022/11/14 10:37:56
 */

// distinguish between futures & equities
// turn a continuous future into the appropriate front month

#pragma once

#include <unordered_map>

#include "BuildInstrument.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed
  class OptionChainQuery;
} // namespace iqfeed

class ComposeInstrument {
public:

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  using fInstrument_t = std::function<void( pInstrument_t )>;

  using pProviderIBTWS_t = ou::tf::ib::TWS::pProvider_t;
  using pProviderIQFeed_t = ou::tf::iqfeed::IQFeedProvider::pProvider_t;

  using fInitDone_t = std::function<void()>;

  ComposeInstrument( pProviderIQFeed_t, fInitDone_t&& );
  ComposeInstrument( pProviderIQFeed_t, pProviderIBTWS_t, fInitDone_t&& );
  ~ComposeInstrument();

  void Compose( const std::string& sIQFeedSymbol, fInstrument_t&& );

protected:
private:

  struct Query {
    fInstrument_t fInstrument;
    pInstrument_t pInstrument;
    size_t cntInstrumentsProcessed;

    Query( fInstrument_t&& fInstrument_ )
    : fInstrument( std::move( fInstrument_ ) )
    , cntInstrumentsProcessed {}
    {}

    Query( Query&& query )
    : fInstrument( std::move( query.fInstrument ) )
    , pInstrument( std::move( query.pInstrument ) )
    , cntInstrumentsProcessed( query.cntInstrumentsProcessed )
    {}
  };

  std::mutex m_mutexMap;

  using pMapQuery_t = std::unordered_map<std::string,Query>;
  pMapQuery_t m_pMapQuery;

  using pBuildInstrument_t = std::unique_ptr<BuildInstrument>;
  using pOptionChainQuery_t = std::unique_ptr<ou::tf::iqfeed::OptionChainQuery>;

  pProviderIQFeed_t m_pProviderIQFeed;
  pProviderIBTWS_t  m_pProviderIBTWS;

  fInitDone_t m_fInitDone;

  pBuildInstrument_t m_pBuildInstrumentIQFeed;
  pBuildInstrument_t m_pBuildInstrumentBoth;

  pOptionChainQuery_t m_pOptionChainQuery; // need to disconnect

  void Initialize();
  void StartChainQuery();
  void Finish( pMapQuery_t::iterator );
};

} // namespace tf
} // namespace ou