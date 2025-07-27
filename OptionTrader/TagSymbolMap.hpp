/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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
 * File:    TagSymbolMap.hpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: July 27, 2025 11:24:02
 */

// inspired by AppBarChart

#pragma once

#include <set>

#include <boost/multi_index/member.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/composite_key.hpp>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

class TagSymbolMap {
  friend class boost::serialization::access;
public:

  TagSymbolMap();
  ~TagSymbolMap();

  using sTag_t = std::string;
  using sSymbol_t = std::string;

  using fTag_t = std::function<void( const sTag_t& )>;
  using fSymbol_t = std::function<void( const sSymbol_t& )>;

  void AddTag( const sTag_t&, const sSymbol_t&, fTag_t&& );
  void DelTag( const sTag_t&, const sSymbol_t&, fTag_t&& );
  void FilterByTag();

  void SymbolListByTag( const sTag_t&, fSymbol_t&& ) const;
  void TagListBySymbol( const sSymbol_t&, fTag_t&& ) const;

  using setTag_t = std::set<sTag_t>;
  void SymbolListByTagSet( const setTag_t&, fSymbol_t&& ) const;

protected:
private:

  struct ixTag{};
  struct ixSymbol{};
  struct ixTagSymbol{};

  struct TagSymbol {
    std::string sTag;
    std::string sSymbol;
    TagSymbol(){}
    TagSymbol( sTag_t sTag_, sSymbol_t sSymbol_ )
    : sTag( sTag_ ), sSymbol( sSymbol_ )
    {}
    TagSymbol( TagSymbol&& rhs )
    : sTag( std::move( rhs.sTag ) ), sSymbol( std::move( rhs.sSymbol ) )
    {}
  };

  using mmapTagSymbol_t = boost::multi_index_container<
    TagSymbol,
    boost::multi_index::indexed_by<
      boost::multi_index::ordered_non_unique<
        boost::multi_index::tag<ixTag>,
        boost::multi_index::member<TagSymbol,sTag_t,&TagSymbol::sTag>
      >,
      boost::multi_index::ordered_non_unique<
        boost::multi_index::tag<ixSymbol>,
        boost::multi_index::member<TagSymbol,sSymbol_t,&TagSymbol::sSymbol>
      >,
      boost::multi_index::ordered_unique<
        boost::multi_index::tag<ixTagSymbol>,
        boost::multi_index::composite_key<
          TagSymbol,
          boost::multi_index::member<TagSymbol,sTag_t,&TagSymbol::sTag>,
          boost::multi_index::member<TagSymbol,sSymbol_t,&TagSymbol::sSymbol>
        >
      >
    >
  >;
  mmapTagSymbol_t m_mmapTagSymbol;

  // TODO: use database
  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {

    ar & m_mmapTagSymbol.size();
    for ( const mmapTagSymbol_t::value_type& vt: m_mmapTagSymbol ) {
      ar & vt.sTag;
      ar & vt.sSymbol;
    }

  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {

    mmapTagSymbol_t::size_type nTagSymbol;
    ar & nTagSymbol;
    while ( 0 != nTagSymbol ) {
      sTag_t sTag;
      ar & sTag;
      sSymbol_t sSymbol;
      ar & sSymbol;
      AddTag( sTag, sSymbol, []( const sTag_t& ){} );
      --nTagSymbol;
    }
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(TagSymbolMap, 1)
