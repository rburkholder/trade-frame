/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

// 2010/09/12
// eventually, this will supercede what is in KeyValuePair.h.
// for the time being, both are used 
// database for incrementing keys
// databases for various record types

// may need to worry about multi-threading at some point in time

#include <string>

#include "EnvManager.h"

// 
// CKeyValuePairsBase
//

class CKeyValuePairsBase {
public:

  CKeyValuePairsBase( const std::string& sDbFileName, const std::string& sDbName );
  virtual ~CKeyValuePairsBase( void );

  void Set( Dbt *pKey, Dbt *pValue, DbTxn* pTxn = NULL );
  void Get( Dbt *pKey, Dbt *pValue, DbTxn* pTxn = NULL );
  void Truncate( DbTxn* pTxn = NULL );

//  void OpenCursor( void );
//  void CloseCursor( void );
//  void ResetCursor( void );

protected:

  Db *m_pdb; // database
  //Dbc *mpdbc;  // cursor

private:
};

//
// CKeyValuePairs
//

// K=type of key, V=type of value;  used for size calcuations, and for type checking inbound/outbound
template<typename K>
class CKeyValuePairs: public CKeyValuePairsBase
{
public:

  CKeyValuePairs( const std::string& sDbFileName, const std::string& sDbName )
    : CKeyValuePairsBase( sDbFileName, sDbName ) {};
  ~CKeyValuePairs(void) {};

  // Sets
  template<typename V>
  void Set( const K& key, const V& value ) {
    Dbt k( &key, sizeof( K ) );
    Dbt v( &value, sizeof( V ) );
    CKeyValuePairsBase::Set( &k, &v );
  }
  
  template<typename V>
  void Set( const std::string& key, V& value ) {
    Dbt k( reinterpret_cast<void*>( const_cast<char*>( key.c_str() ) ), key.length() );
    Dbt v( reinterpret_cast<void*>( &value ), sizeof( V ) );
    CKeyValuePairsBase::Set( &k, &v );
  }

  void Set( const K& key, const std::string& value ) {
    Dbt k( &key, sizeof( K ) );
    Dbt v( reinterpret_cast<void*>( value.c_str() ), value.length() );
    CKeyValuePairsBase::Set( &k, &v );
  }

  void Set( const std::string& key, const std::string& value ) {
    Dbt k( reinterpret_cast<void*>( const_cast<char*>( key.c_str() ) ), key.length() );
    Dbt v( value.c_str(), value.length() );
    CKeyValuePairsBase::Set( &k, &v );
  }

  // gets
  template<typename V>
  void Get( const K& key, V* value ) {
    Dbt k( &key, sizeof( K ) );
    Dbt v;
    v.set_flags( DB_DBT_USERMEM );
    v.set_ulen( sizeof( V ) );
    v.set_size( sizeof( V ) );
    v.set_data( value );  
    CKeyValuePairsBase::Get( &k, &v );
  }

  template<typename V>
  void Get( const std::string& key, V* value ) {
    Dbt k( reinterpret_cast<void*>( const_cast<char*>( key.c_str() ) ), key.length() );
    Dbt v;
    v.set_flags( DB_DBT_USERMEM );
    v.set_ulen( sizeof( V ) );
    v.set_size( sizeof( V ) );
    v.set_data( value );  
    CKeyValuePairsBase::Get( &k, &v );
  }

  void Get( const K& key, std::string* value ) {
    Dbt k( &key, sizeof( K ) );
    Dbt v; // db engine allocates the memory
    CKeyValuePairsBase::Get( &k, &v );
    value->assign( v.get_data(), v.get_size() );
  }

  template<typename V>
  void Get( const std::string& key, std::string* value ) {
    Dbt k( reinterpret_cast<void*>( const_cast<char*>( key.c_str() ) ), key.length() );
    Dbt v; // db engine allocates the memory
    CKeyValuePairsBase::Get( &k, &v );
    value->assign( v.get_data(), v.get_size() );
  }

protected:
private:
  CKeyValuePairs( void ) {};
};

