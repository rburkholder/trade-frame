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

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>

#include <fstream>

#include "MarketSymbol.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

class InMemoryMktSymbolList {
public:

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
//      boost::multi_index::ordered_non_unique<
//        boost::multi_index::tag<ixSymbolClass>, BOOST_MULTI_INDEX_MEMBER(trd_t,MarketSymbol::enumSymbolClassifier,sc)>,
//      boost::multi_index::ordered_non_unique<
//        boost::multi_index::tag<ixSic>, BOOST_MULTI_INDEX_MEMBER(trd_t,boost::uint32_t,nSIC)>,
//      boost::multi_index::ordered_non_unique<
//        boost::multi_index::tag<ixNaics>, BOOST_MULTI_INDEX_MEMBER(trd_t,boost::uint32_t,nNAICS)>,
      boost::multi_index::ordered_non_unique<  // IQFeed file doesn't provide good info, and option symbology sucks
        boost::multi_index::tag<ixUnderlying>, BOOST_MULTI_INDEX_MEMBER(trd_t,std::string,sUnderlying)> 
    >
  > symbols_t;

  typedef symbols_t::iterator iterator;

  iterator begin(){return m_symbols.begin();}
  iterator end(){return m_symbols.end();}

  symbols_t::size_type Size( void ) const { return m_symbols.size(); }

  bool HandleSymbolHasOption( const std::string& s ) {
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
    typedef symbols_t::index<ixSymbol>::type SymbolsByName_t;
    SymbolsByName_t::iterator iter = m_symbols.get<ixSymbol>().find( sSymbol );
    bool bReturn = false;
    if ( m_symbols.get<ixSymbol>().end() != iter ) {
      const_cast<std::string&>( iter->sUnderlying ) = sUnderlying;
    }
  }

  void SaveToFile(const std::string& sFilename) const {
    std::ofstream ofs( sFilename, std::ios::binary );
    boost::archive::binary_oarchive oa( ofs );
    oa << boost::serialization::make_nvp( "symbols", *this );
  }

  void LoadFromFile( const std::string& sFilename ) {
    m_symbols.clear();
    std::ifstream ifs( sFilename, std::ios::binary );
    if ( ifs ) {
      boost::archive::binary_iarchive ia(ifs);
      ia >> boost::serialization::make_nvp( "symbols", *this );
    }
  }

  const trd_t& GetTrd( const std::string& sName ) {
    typedef symbols_t::index<ixSymbol>::type ixSymbol_t;
    ixSymbol_t::const_iterator endSymbols = m_symbols.get<ixSymbol>().end();
    ixSymbol_t::const_iterator iter = m_symbols.get<ixSymbol>().find( sName );
    if ( endSymbols == iter ) {
      throw std::runtime_error( "GetTrd can't find " + sName );
    }
    return *iter;
  }

  template<typename Function>  // not sure if functions correctly, particularily if option list has other symbols interspersed
  void SelectOptionsBySymbol( const std::string& sUnderlying, Function& f ) {
    typedef symbols_t::index<ixSymbol>::type ixSymbol_t;
    ixSymbol_t::const_iterator endSymbols = m_symbols.get<ixSymbol>().end();
    for ( ixSymbol_t::const_iterator iter = m_symbols.get<ixSymbol>().find( sUnderlying ); endSymbols != iter; ++iter ) {
      if ( ou::tf::iqfeed::MarketSymbol::IEOption == iter->sc ) {
        if ( iter->sUnderlying != sUnderlying ) break;
        f( *iter );
      }
    }
  }

  // requires index by underlying, which may be taking up mucho room, actually doesn't
  template<typename Function>
  void SelectOptionsByUnderlying( const std::string& sUnderlying, Function& f ) {
    typedef symbols_t::index<ixUnderlying>::type SymbolsByUnderlying_t;
    SymbolsByUnderlying_t::const_iterator endSymbols = m_symbols.get<ixUnderlying>().end();
    for ( SymbolsByUnderlying_t::const_iterator iter = m_symbols.get<ixUnderlying>().find( sUnderlying ); endSymbols != iter; ++iter ) {
      if ( iter->sUnderlying != sUnderlying ) break;
      f( *iter );
    }
  }
  
  template<typename ExchangeIterator, typename Function>
  void SelectSymbolsByExchange( ExchangeIterator beginExchange, ExchangeIterator endExchange, Function& f ) {
    typedef symbols_t::index<ixExchange>::type SymbolsByExchange_t;
    SymbolsByExchange_t::const_iterator endSymbols = m_symbols.get<ixExchange>().end();
    while ( beginExchange != endExchange ) {
      SymbolsByExchange_t::const_iterator iterSymbols = m_symbols.get<ixExchange>().find( *beginExchange );
      while ( endSymbols != iterSymbols ) {
        if ( *beginExchange != iterSymbols->sExchange ) break;
        f( *iterSymbols );
        iterSymbols++;
      }
      beginExchange++;
    }
  }

  void HandleParsedStructure( const trd_t& trd ) {
    m_symbols.insert( trd );
  }

  void operator()( const trd_t& trd ) {
    m_symbols.insert( trd );
  }

  void Clear( void ) { m_symbols.clear(); };

protected:
private:

  symbols_t m_symbols;

  void insert( trd_t& trd ) { m_symbols.insert( trd ); };

  /* serialization support */

  friend class boost::serialization::access;
    
  template<class Archive>
  void serialize(Archive& ar,const unsigned int) {
    ar&BOOST_SERIALIZATION_NVP(m_symbols);
  }
}; 

} // namespace iqfeed
} // namespace tf
} // namespace ou
