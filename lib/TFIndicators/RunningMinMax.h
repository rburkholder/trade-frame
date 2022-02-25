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
  RunningMinMax( RunningMinMax&& );
  virtual ~RunningMinMax();

  void Add( const value_t& );
  void Remove( const value_t& );

  value_t Min() const { return m_mapValueCount.begin()->first; };
  value_t Max() const { return m_mapValueCount.rbegin()->first; };

  void Reset();

protected:
  void UpdateOnAdd( const value_t min, const value_t max ) {} // CRTP callback
  void UpdateOnDel( const value_t min, const value_t max ) {} // CRTP callback
private:
  using mapValueCount_t = std::map<value_t,unsigned int>;
  mapValueCount_t m_mapValueCount;
};

template<typename CRTP, typename value_t>
RunningMinMax<CRTP,value_t>::RunningMinMax() {}

template<typename CRTP, typename value_t>
RunningMinMax<CRTP,value_t>::RunningMinMax( const RunningMinMax& rhs )
  : m_mapValueCount( rhs.m_mapValueCount )
{
}

template<typename CRTP, typename value_t>
RunningMinMax<CRTP,value_t>::RunningMinMax( RunningMinMax&& rhs )
  : m_mapValueCount( std::move( rhs.m_mapValueCount ) )
{
}

template<typename CRTP, typename value_t>
RunningMinMax<CRTP,value_t>::~RunningMinMax() {
  m_mapValueCount.clear();
}

template<typename CRTP, typename value_t>
void RunningMinMax<CRTP,value_t>::Add(const value_t& value) {

  typename mapValueCount_t::iterator iter = m_mapValueCount.find( value );
  if ( m_mapValueCount.end() == iter ) {
    m_mapValueCount.insert( typename mapValueCount_t::value_type( value, 1 ) );
  }
  else {
    (iter->second)++;
  }
  if ( &RunningMinMax<CRTP,value_t>::UpdateOnAdd != &CRTP::UpdateOnAdd ) {
    static_cast<CRTP*>(this)->UpdateOnAdd( m_mapValueCount.begin()->first, m_mapValueCount.rbegin()->first );
  }

}

template<typename CRTP, typename value_t>
void RunningMinMax<CRTP,value_t>::Remove( const value_t& value ) {

  if ( &RunningMinMax<CRTP,value_t>::UpdateOnDel != &CRTP::UpdateOnDel ) {
    static_cast<CRTP*>(this)->UpdateOnDel( m_mapValueCount.begin()->first, m_mapValueCount.rbegin()->first );
  }

  typename mapValueCount_t::iterator iter = m_mapValueCount.find( value );
  if ( (m_mapValueCount.end() == iter) ) {
    int i = 1;  // shouldn't land here, a bug if we do
  }
  else {
    (iter->second)--;
    if ( 0 == iter->second ) {
      m_mapValueCount.erase( iter );
      if ( !m_mapValueCount.empty() ) {

      }
    }
  }
}

template<typename CRTP, typename value_t>
void RunningMinMax<CRTP,value_t>::Reset() {
  m_mapValueCount.clear();
}

} // namespace tf
} // namespace ou
