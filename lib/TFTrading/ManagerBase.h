/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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
#include <typeinfo>
#include <boost/lexical_cast.hpp>

// class T:  target class

// Manager base class for 
//  AccountManager,
//  PortfolioManager
//  OrderManager, 
//  InstrumentManager, 
//  ProviderManager, 

#include <OUCommon/Singleton.h>

#include "Database.h"

namespace ou {
namespace tf { // TradeFrame

// T: CRTP base
template<class T> 
class ManagerBase: public ou::CSingleton<T> {
public:

  ManagerBase( void ) {};
  virtual ~ManagerBase( void ) {};

  void SetDbSession( ou::db::CSession::pSession_t& pDbSession ) {
    m_pDbSession = pDbSession;
  }

  virtual void RegisterTablesForCreation( void ) {};
  virtual void RegisterRowDefinitions( void ) {};
  virtual void PopulateTables( void ) {};


protected:
  // if session has been assigned, then persist records, if not, don't
  ou::db::CSession::pSession_t m_pDbSession;

  template<class K, class M, class Q> // K:key, M:map, Q:query
  void DeleteRecord( const K& key, M& map, const std::string& sWhere );

  template<class K, class R, class M, class Q>
  void UpdateRecord( const K& key, const R& row, const M& map, const std::string& sWhere );

private:
};

template<class T>
template<class K, class R, class M, class Q>
void ManagerBase<T>::UpdateRecord( const K& key, const R& row, const M& map, const std::string& sWhere ) {

  M::const_iterator iter = map.find( key );
  if ( map.end() == iter ) {
    std::string s(  typeid(T).name() );
    s += "::UpdateRecord: could not find key ";
    s += boost::lexical_cast<std::string>( key );
    throw std::runtime_error( s );
  }

  if ( 0 != m_pDbSession ) {
    Q q( const_cast<R&>( row ), key );
    ou::db::QueryFields<Q>::pQueryFields_t pQueryUpdate = m_pDbSession->Update<Q>( q ).Where( sWhere );
  }

}

template<class T>
template<class K, class M, class Q> // K:key, M:map, Q:query
void ManagerBase<T>::DeleteRecord( const K& key, M& map, const std::string& sWhere ) {
     
  M::const_iterator iter = map.find( key );
  if ( map.end() == iter ) {
    std::string s(  typeid(T).name() );
    s += "::DeleteRecord: could not find key ";
    s += boost::lexical_cast<std::string>( key );
    throw std::runtime_error( s );
  }

  if ( 0 != m_pDbSession ) {
    Q q( key );
    ou::db::QueryFields<Q>::pQueryFields_t pQueryDelete = m_pDbSession->Delete<Q>( q ).Where( sWhere );
  }
  map.erase( iter );

}



} // namespace tf
} // ou