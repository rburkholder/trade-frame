/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

// handles the table oriented stuff

#include <stdexcept>

#include "KeyValuePairs.h"

// CBerkeleyDb =====

// S=struct for storage, T=tuple for processing, K=key
template <typename T, typename S, typename K> 
class CBerkeleyDb: public CKeyValuePairs<K>
{
public:

  CBerkeleyDb( const std::string& sDbFileName, const std::string& sDbName );
  ~CBerkeleyDb(void) {};

  bool Exists( T& tuple ) const;
  void Fetch(  T& tuple, S& record );
  void Insert( T& tuple, S& record ); 
  void Append( T& tuple, S& record ); 
  void Update( T& tuple, S& record ); 
  void Delete( T& tuple );  

protected:
private:

  DbEnv& m_dbEnv;

};

template <typename T, typename S, typename K> 
CBerkeleyDb<T,S,K>::CBerkeleyDb(  const std::string& sDbFileName, const std::string& sDbName )
: CKeyValuePairs<K>( sDbFileName, sDbName ), 
  m_dbEnv( *(CBerkeleyDBEnvManager::Instance().GetDbEnv() ) )
{
}

template <typename T, typename S, typename K> 
bool CBerkeleyDb<T,S,K>::Exists( T& tuple ) const {
  Dbt key;
  boost::fusion::at_c<0>( tuple ).SetKey( key );
  int rtn = m_pdb->exists( NULL, &key, 0 );
  if ( DB_NOTFOUND == rtn ) return false;
  if ( DB_KEYEMPTY == rtn ) return false;
  if ( DB_KEYEXIST == rtn ) return true;
  throw std::runtime_error( "bad exist type" );
}

template <typename T, typename S, typename K> 
void CBerkeleyDb<T,S,K>::Fetch( T& tuple, S& record ) {
}

template <typename T, typename S, typename K> 
void CBerkeleyDb<T,S,K>::Insert( T& tuple, S& record ) {
  Dbt key;
  boost::fusion::at_c<0>( tuple ).SetKey( key );
}

template <typename T, typename S, typename K> 
void CBerkeleyDb<T,S,K>::Append( T& tuple, S& record ) {
  DbTxn* pDbTxn;
  m_dbEnv.txn_begin( NULL, &pDbTxn, 0 );
  pDbTxn->commit();
}

template <typename T, typename S, typename K> 
void CBerkeleyDb<T,S,K>::Update( T& tuple, S& record ) {
}

template <typename T, typename S, typename K> 
void CBerkeleyDb<T,S,K>::Delete( T& tuple ) {
}

