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
//  MoneyManager

#include <OUCommon/Singleton.h>

#include <TFTrading/Database.h>
//#include <OUSqlite/Session.h>

namespace ou {
namespace db { // Database

// T: CRTP base
template<class T>
class ManagerBase: public ou::Singleton<T> {
public:

  ManagerBase(): m_pSession( nullptr ) {};
  virtual ~ManagerBase() {};

  virtual void AttachToSession( ou::db::Session* pSession ) { m_pSession = pSession; };
  virtual void DetachFromSession( ou::db::Session* pSession ) { m_pSession = nullptr; };

protected:

  // if session has been assigned, then persist records, if not, don't
  ou::db::Session* m_pSession;

  template<class K, class M, class Q> // K:key, M:map, Q:query
  void DeleteRecord( const K& key, M& map, const std::string& sWhere );

  template<class K, class Q> // K:key, Q:query
  void DeleteRecord( const K& key, const std::string& sWhere );

  template<class K, class R, class M, class Q>
  void UpdateRecord( const K& key, const R& row, const M& map, const std::string& sWhere );

  template<class K, class R, class Q>
  void UpdateRecord( const K& key, const R& row, const std::string& sWhere );

private:
};

template<class T>
template<class K, class R, class Q>
void ManagerBase<T>::UpdateRecord( const K& key, const R& row, const std::string& sWhere ) {

  if ( nullptr != m_pSession ) {
    Q q( const_cast<R&>( row ), key );
    typename ou::db::QueryFields<Q>::pQueryFields_t pQueryUpdate = m_pSession->Update<Q>( q ).Where( sWhere );
  }

}

template<class T>
template<class K, class R, class M, class Q>
void ManagerBase<T>::UpdateRecord( const K& key, const R& row, const M& map, const std::string& sWhere ) {

  typename M::const_iterator iter = map.find( key );
  if ( map.end() == iter ) {
    std::string s(  typeid(T).name() );
    s += "::UpdateRecord: could not find key ";
    s += boost::lexical_cast<std::string>( key );
    throw std::runtime_error( s );
  }

  if ( nullptr != m_pSession ) {
    Q q( const_cast<R&>( row ), key );
    typename ou::db::QueryFields<Q>::pQueryFields_t pQueryUpdate = m_pSession->Update<Q>( q ).Where( sWhere );
  }

}

template<class T>
template<class K, class Q> // K:key, Q:query
void ManagerBase<T>::DeleteRecord( const K& key, const std::string& sWhere ) {

  if ( nullptr != m_pSession ) {
    Q q( key );
    typename ou::db::QueryFields<Q>::pQueryFields_t pQueryDelete = m_pSession->Delete<Q>( q ).Where( sWhere );
  }

}

template<class T>
template<class K, class M, class Q> // K:key, M:map, Q:query
void ManagerBase<T>::DeleteRecord( const K& key, M& map, const std::string& sWhere ) {

  typename M::const_iterator iter = map.find( key );
  if ( map.end() == iter ) {
    std::string s(  typeid(T).name() );
    s += "::DeleteRecord: could not find key ";
    s += boost::lexical_cast<std::string>( key );
    throw std::runtime_error( s );
  }

  if ( nullptr != m_pSession ) {
    Q q( key );
    typename ou::db::QueryFields<Q>::pQueryFields_t pQueryDelete = m_pSession->Delete<Q>( q ).Where( sWhere );
  }
  map.erase( iter );

}

} // namespace db
} // ou
