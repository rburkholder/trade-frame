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

#include <boost/shared_ptr.hpp>

#include <LibSqlite/sqlite3.h>

#include "Functions.h"

namespace ou {
namespace db {

//
// various Objects used for processing the TableDef
//

//
// ActionBase
//

// Action_CreateTable

class Action_CreateTable {
public:

  void Key( const std::string& sKey, const char* szDbKeyType  );
  void Field( const std::string& sField, const char* szDbFieldType );
  void Constraint( const std::string& sLocalField, const std::string& sRemoteTable, const std::string& sRemoteField );

  void ComposeCreationStatement( const std::string& sTableName, std::string& sStatement );

protected:

private:

  // definition of fields

  struct structFieldDef {
    std::string sFieldName;
    std::string sFieldType;
    structFieldDef( void ) {};
    structFieldDef(const std::string& sFieldName_, const std::string& sFieldType_ ) 
      : sFieldName( sFieldName_ ), sFieldType( sFieldType_ ) {};
  };

  typedef std::vector<structFieldDef> vFields_t;
  typedef vFields_t::iterator vFields_iter_t;
  vFields_t m_vKeys;
  vFields_t m_vFields;

  struct structConstraint {
    std::string sLocalField;
    std::string sRemoteTable;
    std::string sRemoteField;
    structConstraint( const std::string& sLocalField_, const std::string& sRemoteTable_, const std::string& sRemoteField_ ):
      sLocalField( sLocalField_ ), sRemoteTable( sRemoteTable_ ), sRemoteField( sRemoteField_ ) {};
  };

  typedef std::vector<structConstraint> vConstraints_t;
  typedef vConstraints_t::iterator vConstraints_iter_t;
  vConstraints_t m_vConstraints;

};

//
// pass through function specializations for Action_CreateTable
//

template<typename Var>
void Key( Action_CreateTable& action, const std::string& sKeyName, Var& var ) {
  action.Key( sName, KeyType( var ) );
}

template<typename Var>
void Field( Action_CreateTable& action, const std::string& sFieldName, Var& var ) {
  action.Field( sName, FieldType( var ) );
}

// TableDef_BuildStatement

// TableDef_BindForWrite

// TableDef_ColumnForRead

//
// CTableDefBase
//

class CTableDefBase {
public:

  typedef boost::shared_ptr<CTableDefBase> pCTableDefBase_t;

  CTableDefBase( void ) {};
  virtual ~CTableDefBase( void ) {};

  //virtual void CreateTable( sqlite3* pDb, const std::string& sTableName ) = 0;
  virtual void ComposeCreationStatement( const std::string& sTableName, std::string& sStatement ) = 0;

protected:

  // also need to keep table of active records?

private:
};

//
// CTableDef
//

template<class TD>  // TD: TableDef
class CTableDef: public CTableDefBase {
public:

  CTableDef( void ): CTableDefBase() {};
  ~CTableDef( void ) {};

  //void CreateTable( sqlite3* pDb, const std::string& sTableName );
  void ComposeCreationStatement( const std::string& sTableName, std::string& sStatement );

protected:
private:
};

template<class TD>
void CTableDef<TD>::ComposeCreationStatement( const std::string& sTableName, std::string& sStatement ) {

  Action_CreateTable ct;  // action structure maintenance

  TD::TableDef( ct );  // build structure from source definitions

  ct.ComposeCreationStatement( sTableName, sStatement );

}

} // db
} // ou
