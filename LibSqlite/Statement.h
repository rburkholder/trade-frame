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

#include <string>
#include <vector>

#include <boost/cstdint.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "sqlite3.h"

template<typename A> // A=Action
void Table( A& a, const std::string& sName ) {
  a.registerTable( sName );
};

template<typename A, typename T> // A=Action, T=Type
void Key( A& a, const std::string& sName, T& var, const std::string& sDbType ) {
  a.registerKey( sName, var, sDbType );
};

template<typename A, typename T> // A=Action, T=Type
void Key( A& a, const std::string& sName, T& var ) {
  a.registerKey( sName, var );
};

template<typename A, typename T> // A=Action, T=Type
void Field( A& a, const std::string& sName, T& var, const std::string& sDbType ) {
  a.registerField( sName, var, sDbType );
};

template<typename A, typename T> // A=Action, T=Type
void Field( A& a, const std::string& sName, T& var ) {
  a.registerField( sName, var );
};

template<typename A, typename T> // A=Action, T=Type
void Where( A& a, const std::string& sName, T& var ) {  // expand functionality at a later date, defaults to '=' currently
  a.registerWhere( sName, var );
};

template<typename A> // A=Action
void OrderBy( A& a, const std::string& sName ) {
  a.registerOrderBy( sName );
};

template<typename A> // A=Action
void Constraint( A& a, const std::string& sLocalKey, const std::string& sRemoteTable, const std::string& sRemoteKey ) {
  a.registerConstraint( sLocalKey, sRemoteTable, sRemoteKey );
};

// use some template magic to merge KeyType and FieldType so don't have redundancy in the lists
const char* KeyType( boost::int64_t key );
const char* KeyType( std::string& key );

const char* FieldType( int key );
const char* FieldType( char key );
const char* FieldType( bool key );
const char* FieldType( boost::posix_time::ptime& key );
const char* FieldType( boost::int64_t key );
const char* FieldType( boost::int32_t key );
const char* FieldType( boost::int16_t key );
const char* FieldType( boost::int8_t key );
const char* FieldType( std::string& key );
const char* FieldType( double key );

class CDbSession;

// 
// CStatementBase
//

class CStatementBase {
public:
  CStatementBase( void ) : m_pStmt( 0 ) {};
  virtual ~CStatementBase( void ) {
    if ( 0 != m_pStmt ) {
      int rtn = sqlite3_finalize( m_pStmt );
      m_pStmt = 0;
    }
  };
protected:
  std::string m_sStatement;
  sqlite3_stmt* m_pStmt;
private:
};

// 
// CStatement
//

template<typename T>
class CStatement: public CStatementBase {
  friend class CDbSession;
public:

  CStatement(void);
  ~CStatement(void);

  void registerTable( const std::string& sTableName );

  template<typename V>
  void registerKey( const std::string& sKey, V& var ) {
    {
      structFieldDef def;
      m_vKeyDefs.push_back( def );
    }
    structFieldDef& def = m_vKeyDefs.back();
    def.sFieldName = sKey;
    def.sFieldType = KeyType( var );

    m_bRegistrationStarted = true;
  };

  template<typename V>
  void registerKey( const std::string& sKey, V& var, const std::string& sDbType ) {
    {
      structFieldDef def;
      m_vKeyDefs.push_back( def );
    }
    structFieldDef& def = m_vKeyDefs.back();
    def.sFieldName = sKey;
    if ( sDbType.empty() ) {
      throw std::runtime_error( "registerKey requires non empty DbType" );
    }
    def.sFieldType = sDbType;

    m_bRegistrationStarted = true;
  };

  template<typename V>
  void registerField( const std::string& sKey, V& var ) {
    {
      structFieldDef def;
      m_vFieldDefs.push_back( def );
    }
    structFieldDef& def = m_vFieldDefs.back();
    def.sFieldName = sKey;
    def.sFieldType = FieldType( var );

    m_bRegistrationStarted = true;
  };

  template<typename V>
  void registerField( const std::string& sKey, V& var, const std::string& sDbType ) {
    {
      structFieldDef def;
      m_vFieldDefs.push_back( def );
    }
    structFieldDef& def = m_vFieldDefs.back();
    def.sFieldName = sKey;
    if ( sDbType.empty() ) {
      throw std::runtime_error( "registerField requires non empty DbType" );
    }
    def.sFieldType = sDbType;

    m_bRegistrationStarted = true;
  };

  template<typename V>
  void registerWhere( const std::string& sKey, V& var ) {
    m_vWhereNames.push_back( sKey );
    m_bRegistrationStarted = true;
  };

  void registerOrderBy( const std::string& sName ) {
    m_vWhereNames.push_back( sName );
    m_bRegistrationStarted = true;
  }

  void registerConstraint( const std::string& sLocalKey, const std::string& sRemoteTable, const std::string& sRemoteKey );

  template<typename TD> // TableDef from persisted class
  void RegisterDefinitions( TD& td ) {
  }

protected:

  struct structConstraint {
    std::string sLocalKey;
    std::string sRemoteTable;
    std::string sRemoteKey;
  };

  struct structFieldDef {
    std::string sFieldName;
    std::string sFieldType;
  };

  typedef typename std::vector<structFieldDef>::iterator iteratorFieldDef_t;
  typedef typename std::vector<structConstraint>::iterator iteratorConstraint_t;

  std::string m_sTableName;
  std::vector<structFieldDef> m_vKeyDefs;
  std::vector<structFieldDef> m_vFieldDefs;
  std::vector<std::string> m_vWhereNames;
  std::vector<std::string> m_vOrderByNames;
  std::vector<structConstraint> m_vConstraints;


private:

  bool m_bRegistrationStarted;  // so we can tell if a new prepare is required.

  void Prepare( sqlite3* db );  // used by CDbSession only
  void ComposeStatement( void );  // used from Prepare only

};

template<typename T>
CStatement<T>::CStatement( void ) 
: CStatementBase(), m_bRegistrationStarted( true )
{
}

template<typename T>
CStatement<T>::~CStatement( void ) {
}

template<typename T>
void CStatement<T>::registerTable( const std::string& sTableName ) {
  m_sTableName = sTableName;
  m_bRegistrationStarted = true;
}

template<typename T>
void CStatement<T>::registerConstraint( const std::string& sLocalKey, const std::string& sRemoteTable, const std::string& sRemoteKey ) {

  {
    structConstraint constraint;
    m_vConstraints.push_back( constraint );
  }

  structConstraint& constraint m_vConstraints.back();
  constraint.sLocalKey = sLocalKey;
  constraint.sRemoteTable = sRemoteTable;
  constraint.sRemoteKey = sRemoteKey;
  
  m_bRegistrationStarted = true;
}

template<typename T>
void CStatement<T>::ComposeStatement( void ) {

  if ( m_sTableName.empty() ) throw std::runtime_error( "CStatement<T>::ComposeStatement: no TableName" );
  if ( m_vKeyDefs.empty() ) throw std::runtime_error( "CStatement<T>::ComposeStatement: no Keys" );
  if ( m_vFieldDefs.empty() ) throw std::runtime_error( "CStatement<T>::ComposeStatement: no fields" );

  static_cast<T*>(this)->ComposeStatement();  // override has to exist;

}

template<typename T>
void CStatement<T>::Prepare( sqlite3* db ) {

  int rtn;

  if ( m_bRegistrationStarted ) { // redo prepare
    if ( 0 != m_pStmt ) {  // already a statement prepared so finalize it and make ready for the next one
      int rtn = sqlite3_finalize( m_pStmt );
      m_pStmt = 0;
      if ( SQLITE_OK != rtn ) {
        std::string sErr( "CStatement::Prepare: " );
        sErr += ":  error in finalize(";
        sErr += boost::lexical_cast<std::string>( rtn );
        sErr += ")";
        throw std::runtime_error( sErr );
      }
    }

    m_sStatement.clear();

    ComposeStatement();

    if ( m_sStatement.empty() ) {
      throw std::runtime_error( "CStatement::Prepare: statement is empty" );
    }

    rtn = sqlite3_prepare_v2( db, m_sStatement.c_str(), -1, &m_pStmt, NULL );
    if ( SQLITE_OK != rtn ) {
      std::string sErr( "CStatement::Prepare: " );
      sErr += " error in prepare(";
      sErr += boost::lexical_cast<std::string>( rtn );
      sErr += ")";
      throw std::runtime_error( sErr );
    }
  }
  else { // m_bRegistrationStarted is false, so reset and unbind
    rtn = sqlite3_reset( m_pStmt );
    if ( SQLITE_OK != rtn ) {
      std::string sErr( "CStatement::Prepare: " );
      sErr += ":  error in reset(";
      sErr += boost::lexical_cast<std::string>( rtn );
      sErr += ")";
      throw std::runtime_error( sErr );
    }
    rtn = sqlite3_clear_bindings( m_pStmt );
    if ( SQLITE_OK != rtn ) {
      std::string sErr( "CStatement::Prepare: " );
      sErr += ":  error in clear bindings(";
      sErr += boost::lexical_cast<std::string>( rtn );
      sErr += ")";
      throw std::runtime_error( sErr );
    }
  }

}

// 
// CStatementCreateTable
//

class CStatementCreateTable: public CStatement<CStatementCreateTable> {
public:
  CStatementCreateTable( void ) : CStatement<CStatementCreateTable>() {};
  ~CStatementCreateTable( void ) {};
  void ComposeStatement( void );
protected:
private:
};

// 
// CStatementInsert
//

class CStatementInsert: public CStatement<CStatementInsert> {
public:
  CStatementInsert( void ) : CStatement<CStatementInsert>() {};
  ~CStatementInsert( void ) {};
  void ComposeStatement( void );
protected:
private:
};

// 
// CStatementUpdate
//

class CStatementUpdate: public CStatement<CStatementUpdate> {
public:
  CStatementUpdate( void ) : CStatement<CStatementUpdate>() {};
  ~CStatementUpdate( void ) {};
  void ComposeStatement( void );
protected:
private:
};

// 
// CStatementSelect
//

class CStatementSelect: public CStatement<CStatementSelect> {
public:
  CStatementSelect( void ) : CStatement<CStatementSelect>() {};
  ~CStatementSelect( void ) {};
  void ComposeStatement( void );
protected:
private:
};

// 
// CStatementDelete
//

class CStatementDelete: public CStatement<CStatementDelete> {
public:
  CStatementDelete( void ) : CStatement<CStatementDelete>() {};
  ~CStatementDelete( void ) {};
  void ComposeStatement( void );
protected:
private:
};