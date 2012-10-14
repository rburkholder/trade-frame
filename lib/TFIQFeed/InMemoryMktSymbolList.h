/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

#pragma once

// Started 2012/10/14

#include <string>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>

#include "MarketSymbol.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

struct InMemoryMktSymbolList {

  struct ixSymbol{};
  struct ixExchange{};
  struct ixSymbolClass{};
  struct ixSic{};
  struct ixNaics{};
  struct ixUnderlying{};

  typedef ou::tf::iqfeed::MarketSymbol::TableRowDef trd_t;

  typedef boost::multi_index_container<
    trd_t,
    boost::multi_index::indexed_by<
      boost::multi_index::ordered_unique<
        boost::multi_index::tag<ixSymbol>, BOOST_MULTI_INDEX_MEMBER(trd_t,std::string,sSymbol)>,
      boost::multi_index::ordered_non_unique<
        boost::multi_index::tag<ixExchange>, BOOST_MULTI_INDEX_MEMBER(trd_t,std::string,sExchange)>,
      boost::multi_index::ordered_non_unique<
        boost::multi_index::tag<ixSymbolClass>, BOOST_MULTI_INDEX_MEMBER(trd_t,MarketSymbol::enumSymbolClassifier,sc)>,
      boost::multi_index::ordered_non_unique<
        boost::multi_index::tag<ixSic>, BOOST_MULTI_INDEX_MEMBER(trd_t,boost::uint32_t,nSIC)>,
      boost::multi_index::ordered_non_unique<
        boost::multi_index::tag<ixNaics>, BOOST_MULTI_INDEX_MEMBER(trd_t,boost::uint32_t,nNAICS)>,
      boost::multi_index::ordered_non_unique<
        boost::multi_index::tag<ixUnderlying>, BOOST_MULTI_INDEX_MEMBER(trd_t,std::string,sUnderlying)> >
  > symbols_t;

  symbols_t m_symbols;

  void HandleParsedStructure( trd_t& trd ) {
    m_symbols.insert( trd );
  }

  bool HandleSymbolHasOption( const std::string& s ) {
    typedef ou::tf::iqfeed::InMemoryMktSymbolList::ixSymbol ixSymbol;
    typedef symbols_t::index<ixSymbol>::type SymbolsByName_t;
    SymbolsByName_t::iterator iter = m_symbols.get<ixSymbol>().find( s );
    bool bReturn = false;
    if ( m_symbols.get<ixSymbol>().end() != iter ) {
      const_cast<bool&>( iter->bHasOptions ) = true;
      bReturn = true;
    }
    return bReturn;
  }

  void HandleUpdateOptionUnderlying( const std::string& sSymbol, const std::string& sUnderlying ) {
    typedef ou::tf::iqfeed::InMemoryMktSymbolList::ixSymbol ixSymbol;
    typedef symbols_t::index<ixSymbol>::type SymbolsByName_t;
    SymbolsByName_t::iterator iter = m_symbols.get<ixSymbol>().find( sSymbol );
    bool bReturn = false;
    if ( m_symbols.get<ixSymbol>().end() != iter ) {
      const_cast<std::string&>( iter->sUnderlying ) = sUnderlying;
    }
  }

}; 

} // namespace iqfeed
} // namespace tf
} // namespace ou
