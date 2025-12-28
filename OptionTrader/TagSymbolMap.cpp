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
 * File:    TagSymbolMap.cpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: July 27, 2025 11:24:02
 */

#include <cassert>

#include "TagSymbolMap.hpp"

TagSymbolMap::TagSymbolMap() {}

TagSymbolMap::~TagSymbolMap() {}

void TagSymbolMap::AddTag( const sTag_t& sTag, const sSymbol_t& sSymbol, fTag_t&& fTag ) {

  using setTag_t = mmapTagSymbol_t::index<ixTag>::type;

  const setTag_t& index( m_mmapTagSymbol.get<ixTag>() );
  const setTag_t::iterator iterTag = index.find( sTag );

  using setTagSymbol_t = mmapTagSymbol_t::index<ixTagSymbol>::type;
  setTagSymbol_t::iterator iterTagSymbol = m_mmapTagSymbol.get<ixTagSymbol>().find( std::make_tuple( sTag, sSymbol ) );
  if ( m_mmapTagSymbol.get<ixTagSymbol>().end() == iterTagSymbol ) {
    auto result = m_mmapTagSymbol.emplace( TagSymbol( sTag, sSymbol ) );
    assert( result.second );
  }

  if ( index.end() == iterTag ) {
    fTag( sTag );
  }

}

void TagSymbolMap::DelTag( const sTag_t& sTag, const sSymbol_t& sSymbol, fTag_t&& fTag ) {

  using setTagSymbol_t = mmapTagSymbol_t::index<ixTagSymbol>::type;
  setTagSymbol_t::iterator iterTagSymbol = m_mmapTagSymbol.get<ixTagSymbol>().find( std::make_tuple( sTag, sSymbol ) );
  if ( m_mmapTagSymbol.get<ixTagSymbol>().end() == iterTagSymbol ) {}
  else {
    m_mmapTagSymbol.get<ixTagSymbol>().erase( iterTagSymbol );

    using setTag_t = mmapTagSymbol_t::index<ixTag>::type;

    const setTag_t& index( m_mmapTagSymbol.get<ixTag>() );
    const setTag_t::iterator iterTag = index.find( sTag );

    if ( index.end() == iterTag ) {
      fTag( sTag );
    }
  }

}

void TagSymbolMap::SymbolListByTag( const sTag_t& sTag, fSymbol_t&& fSymbol) const {

  using setTag_t = mmapTagSymbol_t::index<ixTag>::type;

  const setTag_t& index( m_mmapTagSymbol.get<ixTag>() );
  setTag_t::const_iterator iterTag = index.find( sTag );

  while ( index.end() != iterTag ) {
    if ( sTag != iterTag->sTag ) break;
    fSymbol( iterTag->sSymbol );
    ++iterTag;
  }

}

void TagSymbolMap::TagListBySymbol( const sSymbol_t& sSymbol, fTag_t&& fTag ) const {

  using setSymbol_t = mmapTagSymbol_t::index<ixSymbol>::type;

  const setSymbol_t& index( m_mmapTagSymbol.get<ixSymbol>() );
  setSymbol_t::const_iterator iterSymbol = index.find( sSymbol );

  while ( index.end() != iterSymbol ) {
    if ( sSymbol != iterSymbol->sSymbol ) break;
    fTag( iterSymbol->sTag );
    ++iterSymbol;
  }

}

void TagSymbolMap::SymbolListByTagSet( const setTag_t& setTag, fSymbol_t&& fSymbol ) const {

    using setSymbol_t = std::set<sSymbol_t>;
    setSymbol_t setSymbol;

    using msetTag_t = mmapTagSymbol_t::index<ixTag>::type;

    for ( const setTag_t::value_type sTag: setTag ) {
      msetTag_t::const_iterator iterTag = m_mmapTagSymbol.get<ixTag>().find( sTag );
      while ( m_mmapTagSymbol.get<ixTag>().end() != iterTag ) {
        if ( sTag != iterTag->sTag ) break;
        setSymbol.insert( iterTag->sSymbol );
        ++iterTag;
      }
    }

    for ( const setSymbol_t::value_type& sSymbol: setSymbol ) {
      fSymbol( sSymbol );
    }

}

void TagSymbolMap::TagList( fTag_t&& fTag ) {
  sTag_t sTag;

  using setTagSymbol_t = mmapTagSymbol_t::index<ixTagSymbol>::type;
  const setTagSymbol_t& index( m_mmapTagSymbol.get<ixTagSymbol>() );
  setTagSymbol_t::iterator iterTagSymbol = index.begin();
  while ( index.end() != iterTagSymbol ) {
    if ( sTag != iterTagSymbol->sTag ) {
      sTag = iterTagSymbol->sTag;
      fTag( sTag );
    }
    ++iterTagSymbol;
  }

}

void TagSymbolMap::DelTagsForSymbol( const sSymbol_t& sSymbol, fTag_t&& fTag_ ) {

  using setSymbol_t = mmapTagSymbol_t::index<ixSymbol>::type;



  setSymbol_t& setSymbol( m_mmapTagSymbol.get<ixSymbol>() );
  while ( true ) {
    setSymbol_t::iterator iterSymbol = setSymbol.find( sSymbol );
    if ( setSymbol.end() == iterSymbol ) break;
    else {
      if ( sSymbol != iterSymbol->sSymbol ) break;
      else {
        fTag_t fTag = fTag_; // make a copy for repeated use
        const std::string sTag( iterSymbol->sTag );
        DelTag( sTag, sSymbol, std::move( fTag ) );
      }
    }
  }
}