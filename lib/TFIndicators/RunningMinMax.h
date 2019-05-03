/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

#include <map>

namespace ou { // One Unified
namespace tf { // TradeFrame

template<typename CRTP, typename value_t>
class RunningMinMax {
public:

  RunningMinMax();
  RunningMinMax( const RunningMinMax& );
  RunningMinMax( const RunningMinMax&& );
  virtual ~RunningMinMax();

  void Add( const value_t& );
  void Remove( const value_t& );

  value_t Min() const { return m_mapPointStats.begin()->first; };
  value_t Max() const { return m_mapPointStats.rbegin()->first; };

  void Reset();

  void UpdateMax( const value_t& ) {} // CRTP callback
  void UpdateMin( const value_t& ) {} // CRTP callback

protected:
private:

  using mapPointStats_t = std::map<value_t,unsigned int>;
  mapPointStats_t m_mapPointStats;
};

template<typename CRTP, typename value_t>
RunningMinMax<CRTP,value_t>::RunningMinMax() {}

template<typename CRTP, typename value_t>
RunningMinMax<CRTP,value_t>::RunningMinMax( const RunningMinMax& rhs )
  : m_mapPointStats( rhs.m_mapPointStats )
{
}

template<typename CRTP, typename value_t>
RunningMinMax<CRTP,value_t>::RunningMinMax( const RunningMinMax&& rhs )
  : m_mapPointStats( std::move( rhs.m_mapPointStats ) )
{
}

template<typename CRTP, typename value_t>
RunningMinMax<CRTP,value_t>::~RunningMinMax() {
  m_mapPointStats.clear();
}

template<typename CRTP, typename value_t>
void RunningMinMax<CRTP,value_t>::Add(const value_t& value) {

  typename mapPointStats_t::iterator iter = m_mapPointStats.find( value );
  if ( m_mapPointStats.end() == iter ) {
    m_mapPointStats.insert( typename mapPointStats_t::value_type( value, 1 ) );
    static_cast<CRTP*>(this)->UpdateMax( m_mapPointStats.rbegin()->first );
    static_cast<CRTP*>(this)->UpdateMin( m_mapPointStats.begin()->first );
  }
  else {
    (iter->second)++;
  }
}

template<typename CRTP, typename value_t>
void RunningMinMax<CRTP,value_t>::Remove( const value_t& value ) {

  typename mapPointStats_t::iterator iter = m_mapPointStats.find( value );
  if ( (m_mapPointStats.end() == iter) ) {
    int i = 1;  // shouldn't land here, a bug if we do
  }
  else {
    (iter->second)--;
    if ( 0 == iter->second ) {
      m_mapPointStats.erase( iter );
      if ( !m_mapPointStats.empty() ) {
        static_cast<CRTP*>(this)->UpdateMax( m_mapPointStats.rbegin()->first );
        static_cast<CRTP*>(this)->UpdateMin( m_mapPointStats.begin()->first );
      }
    }
  }
}

template<typename CRTP, typename value_t>
void RunningMinMax<CRTP,value_t>::Reset() {
  m_mapPointStats.clear();
}

} // namespace tf
} // namespace ou
