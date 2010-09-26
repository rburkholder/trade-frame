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

#include "KeyValuePairs.h"

// CBerkeleyDb =====

// S=struct for storage, T=tuple for processing, K=key
template <typename S, typename T, typename K> 
class CBerkeleyDb: public CKeyValuePairs<K>
{
public:

  CBerkeleyDb( const std::string& sDbFileName, const std::string& sDbName );
  ~CBerkeleyDb(void) {};

  void SetDbFileName( const std::string& sDbFileName ) { m_sDbFileName = sDbFileName; };
  void SetDbDName( const std::string& sDbName ) { m_sDbName = sDbName; };

  void Insert( S& record ); // use the key in the structure
  void Append( S& record ); // use the key in the structure
  void Update( S& record ); // use the key in the structure  
  void Delete( K& key );  // use this key

protected:
private:

  DbEnv& m_dbEnv;

};

template <typename S, typename T, typename K>
CBerkeleyDb<S,T,K>::CBerkeleyDb(  const std::string& sDbFileName, const std::string& sDbName )
: CKeyValuePairs<K>( sDbFileName, sDbName ), 
  m_dbEnv( *(CBerkeleyDBEnvManager::Instance().GetDbEnv() ) )
{
}

template <typename S, typename T, typename K>
void CBerkeleyDb<S,T,K>::Insert( S& record ) {
}

template <typename S, typename T, typename K>
void CBerkeleyDb<S,T,K>::Append( S& record ) {
  DbTxn* pDbTxn;
  m_dbEnv.txn_begin( NULL, &pDbTxn, 0 );
  pDbTxn->commit();
}

template <typename S, typename T, typename K>
void CBerkeleyDb<S,T,K>::Update( S& record ) {
}

template <typename S, typename T, typename K>
void CBerkeleyDb<S,T,K>::Delete( K& key ) {
}
