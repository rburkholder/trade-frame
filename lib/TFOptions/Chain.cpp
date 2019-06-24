/************************************************************************
 * Copyright(c) 2019, One Unified. All rights reserved.                 *
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
 * File:    Chains.cpp
 * Author:  raymond@burkholder.net
 * Project: TFOptions
 * Created on June 23, 2019, 9:49 PM
 */

#include <cassert>
#include <algorithm>
#include <iostream>

#include "Chain.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

Chain::Chain( ) { }

Chain::Chain( const Chain&& rhs ) { }

Chain::~Chain( ) { }

double Chain::Put_Itm( double value ) const { // price < strike
  mapChain_t::const_iterator iter = std::upper_bound(
    m_mapChain.begin(), m_mapChain.end(), value,
    [](double value, const mapChain_t::value_type& vt)->bool{ return value < vt.first; } );
  if ( m_mapChain.end() == iter ) throw exception_strike_not_found( "Put_Itm not found" );
  return iter->first;
}

double Chain::Put_ItmAtm( double value ) const { // price <= strike
  mapChain_t::const_iterator iter = std::lower_bound(
    m_mapChain.begin(), m_mapChain.end(), value,
    [](const mapChain_t::value_type& vt, double value)->bool{ return vt.first < value; } );
  if ( m_mapChain.end() == iter ) throw exception_strike_not_found( "Put_ItmAtm not found" );
  return iter->first;
}

double Chain::Put_Atm( double value ) const { // closest strike (use itm vs otm)
  double atm {};
  mapChain_t::const_iterator iter1 = std::lower_bound(
    m_mapChain.begin(), m_mapChain.end(), value,
    [](const mapChain_t::value_type& vt, double value)->bool{ return vt.first < value; } );
  if ( m_mapChain.end() == iter1 ) throw exception_strike_not_found( "Put_Atm not found" );
  if ( value == iter1->first ) {
    atm = value;
  }
  else {
    if ( m_mapChain.begin() == iter1 ) {
      atm = value;
    }
    else {
      mapChain_t::const_iterator iter2 = iter1;
      iter2--;
      if ( ( iter1->first - value ) < ( value - iter2->first ) ) {
        atm = iter1->first;
      }
      else {
        atm = iter2->first;
      }
    }
  }
  assert( 0.0 != atm );
  return atm;
}

double Chain::Put_OtmAtm( double value ) const { // price >= strike
  mapChain_t::const_iterator iter = std::lower_bound(
    m_mapChain.begin(), m_mapChain.end(), value,
    [](const mapChain_t::value_type& vt, double value)->bool{ return vt.first < value; } );
  if ( m_mapChain.end() == iter ) throw exception_strike_not_found( "Put_OtmAtm not found" );
  if ( value == iter->first ) {
    // atm
  }
  else {
    if ( m_mapChain.begin() == iter ) {
      throw exception_at_start_of_chain( "Put_OtmAtm at begin of chain" );
    }
    else {
      iter--; // strike will be OTM
    }
  }
  return iter->first;
}

double Chain::Put_Otm( double value ) const { // price > strike
  mapChain_t::const_iterator iter = std::lower_bound(
    m_mapChain.begin(), m_mapChain.end(), value,
    [](const mapChain_t::value_type& vt, double value)->bool{ return vt.first < value; } );
  if ( m_mapChain.end() == iter ) throw exception_strike_not_found( "Put_Otm not found" );
  if ( m_mapChain.begin() == iter ) {
    throw exception_at_start_of_chain( "Put_Otm at begin of chain" );
  }
  else {
    iter--; // strike will be OTM
  }
  return iter->first;
}

double Chain::Call_Itm( double value ) const { // price > strike
  mapChain_t::const_iterator iter = std::lower_bound(
    m_mapChain.begin(), m_mapChain.end(), value,
    [](const mapChain_t::value_type& vt, double value)->bool{ return vt.first < value; } );
  if ( m_mapChain.end() == iter ) throw exception_strike_not_found( "Call_Itm not found" );
    if ( m_mapChain.begin() == iter ) {
      throw exception_at_start_of_chain( "Call_Itm at begin of chain" );
    }
    else {
      iter--;
    }
  return iter->first;
}

double Chain::Call_ItmAtm( double value ) const { // price >= strike
  mapChain_t::const_iterator iter = std::lower_bound(
    m_mapChain.begin(), m_mapChain.end(), value,
    [](const mapChain_t::value_type& vt, double value)->bool{ return vt.first < value; } );
  if ( m_mapChain.end() == iter ) throw exception_strike_not_found( "Call_ItmAtm not found" );
  if ( value == iter->first ) {
    // atm
  }
  else {
    if ( m_mapChain.begin() == iter ) {
      throw exception_at_start_of_chain( "Call_ItmAtm at begin of chain" );
    }
    else {
      iter--; // strike will be Itm
    }
  }
  return iter->first;
}

double Chain::Call_Atm( double value ) const { // closest strike (use itm vs otm)
  double atm {};
  mapChain_t::const_iterator iter1 = std::lower_bound(
    m_mapChain.begin(), m_mapChain.end(), value,
    [](const mapChain_t::value_type& vt, double value)->bool{ return vt.first < value; } );
  if ( m_mapChain.end() == iter1 ) throw exception_strike_not_found( "Call_Atm not found" );
  if ( value == iter1->first ) {
    atm = value;
  }
  else {
    if ( m_mapChain.begin() == iter1 ) {
      atm = value;
    }
    else {
      mapChain_t::const_iterator iter2 = iter1;
      iter2--;
      if ( ( iter1->first - value ) < ( value - iter2->first ) ) {
        atm = iter1->first;
      }
      else {
        atm = iter2->first;
      }
    }
  }
  assert( 0.0 != atm );
  return atm;
}

double Chain::Call_OtmAtm( double value ) const { // price <= strike
  mapChain_t::const_iterator iter = std::lower_bound(
    m_mapChain.begin(), m_mapChain.end(), value,
    [](const mapChain_t::value_type& vt, double value)->bool{ return vt.first < value; } );
  if ( m_mapChain.end() == iter ) throw exception_strike_not_found( "Call_OtmAtm not found" );
  return iter->first;
}

double Chain::Call_Otm( double value ) const { // price < strike
  mapChain_t::const_iterator iter = std::upper_bound(
    m_mapChain.begin(), m_mapChain.end(), value,
    [](double value, const mapChain_t::value_type& vt)->bool{ return value < vt.first; } );
  if ( m_mapChain.end() == iter ) throw exception_strike_not_found( "Call_Otm not found" );
  return iter->first;
}

int Chain::AdjacentStrikes( double strikeSource, double& strikeLower, double& strikeUpper ) const {
  strikeLower = strikeUpper = 0.0;
  int nReturn {};
  mapChain_t::const_iterator iterSource = m_mapChain.find( strikeSource );
  if ( m_mapChain.end() != iterSource ) {
    mapChain_t::const_iterator iterLower = iterSource;
    if ( m_mapChain.begin() != iterLower ) {
      iterLower--;
      strikeLower = iterLower->first;
      nReturn++;
    }
    mapChain_t::const_iterator iterUpper = iterSource;
    iterUpper++;
    if ( m_mapChain.end() != iterUpper ) {
      strikeUpper = iterUpper->first;
      nReturn++;
    }
  }
  return nReturn;
}

void Chain::SetIQFeedNameCall( double dblStrike, const std::string& sIQFeedSymbolName ) {
  mapChain_t::iterator iter = m_mapChain.find( dblStrike );
  if ( m_mapChain.end() == iter ) {
    iter = m_mapChain.insert( m_mapChain.begin(), mapChain_t::value_type( dblStrike, OptionsAtStrike() ) );
  }
  assert( 0 == iter->second.sCall.size() );
  iter->second.sCall = sIQFeedSymbolName;
}

void Chain::SetIQFeedNamePut( double dblStrike, const std::string& sIQFeedSymbolName ) {
  mapChain_t::iterator iter = m_mapChain.find( dblStrike );
  if ( m_mapChain.end() == iter ) {
    iter = m_mapChain.insert( m_mapChain.begin(), mapChain_t::value_type( dblStrike, OptionsAtStrike() ) );
  }
  assert( 0 == iter->second.sPut.size() );
  iter->second.sPut = sIQFeedSymbolName;
}

const std::string Chain::GetIQFeedNameCall( double dblStrike ) const {
  mapChain_t::const_iterator iter = FindStrike( dblStrike );
  return iter->second.sCall;
}

const std::string Chain::GetIQFeedNamePut( double dblStrike ) const {
  mapChain_t::const_iterator iter = FindStrike( dblStrike );
  return iter->second.sPut;
}

// const iterator
Chain::mapChain_t::const_iterator Chain::FindStrike( const double strike ) const {
  mapChain_t::const_iterator iter = m_mapChain.find( strike );
  if ( m_mapChain.end() == iter ) {
    throw exception_strike_not_found( "Chain::FindStrike const: can't find strike" );
  }
  return iter;
}

// regular iterator
Chain::mapChain_t::iterator Chain::FindStrike( const double strike ) {
  mapChain_t::iterator iter = m_mapChain.find( strike );
  if ( m_mapChain.end() == iter ) {
    throw exception_strike_not_found( "Chain::FindStrike: can't find strike" );
  }
  return iter;
}

void Chain::EmitValues( void ) const { // TODO: supply output stream
  std::for_each( m_mapChain.begin(), m_mapChain.end(), [](const mapChain_t::value_type& vt){
    std::cout << vt.first << ": " << vt.second.sCall << ", " << vt.second.sPut << std::endl;
  });
}

void Chain::Test( double price ) { // TODO: supply output stream
  try {
    std::cout << "Put_Itm: "     << Put_Itm( price ) << std::endl;
    std::cout << "Put_ItmAtm: "  << Put_ItmAtm( price ) << std::endl;
    std::cout << "Put_Atm: "     << Put_Atm( price ) << std::endl;
    std::cout << "Put_OtmAtm: "  << Put_OtmAtm( price ) << std::endl;
    std::cout << "Put_Otm: "     << Put_Otm( price ) << std::endl;

    std::cout << "Call_Itm: "    << Call_Itm( price ) << std::endl;
    std::cout << "Call_ItmAtm: " << Call_ItmAtm( price ) << std::endl;
    std::cout << "Call_Atm: "    << Call_Atm( price ) << std::endl;
    std::cout << "Call_OtmAtm: " << Call_OtmAtm( price ) << std::endl;
    std::cout << "Call_Otm: "    << Call_Otm( price ) << std::endl;
  }
  catch ( std::runtime_error& e ) {
    std::cout << "Chain::Test runtime error: " << e.what() << std::endl;
  }
}

} // namespace option
} // namespace tf
} // namespace ou
