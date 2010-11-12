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

namespace ou {
namespace db {

//
// various Objects used for processing the TableDef
//

// TableDef_CreateTable

class Action_CreateTable {
public:
  template<typename Var>
  void Key( const std::string& sKey,  );

  void Field( const std::string& sField, const std::string& sDbKeyType );
  void Where( const std::string& sWhere );
  void OrderBy( const std::string& sOrderBy );
  void Constraint( const std::string& sLocalField, const std::string& sRemoteTable, const std::string& sRemoteField );
protected:
private:
};



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

  virtual void CreateTable( sqlite3* pDb, const std::string& sTableName ) = 0;

protected:

  // definition of fields

  struct structFieldDef {
    std::string m_sFieldName;
    std::string m_sFieldType;
    structFieldDef( void ) {};
    structFieldDef(const std::string& sFieldName, const std::string& sFieldType ) 
      : m_sFieldName( sFieldName ), m_sFieldType( sFieldType ) {};
  };

  typedef std::vector<structFieldDef> vFields_t;
  typedef vFields_t::iterator vFields_iter_t;
  vFields_t m_vFields;

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

  void CreateTable( sqlite3* pDb, const std::string& sTableName );

protected:
private:
};

template<class TD>
void CTableDef<TD>::CreateTable( sqlite3* pDb, const std::string& sTableName ) {
  TD::TableDef( TableDef_CreateTable );
}

} // db
} // ou
