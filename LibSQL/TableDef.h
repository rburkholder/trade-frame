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


#include "FieldDef.h"
#include "Functions.h"
#include "Sql.h"

namespace ou {
namespace db {

// Action_CreateTable

class Action_CreateTable: public Action_AddFields {
public:

  Action_CreateTable( void );
  ~Action_CreateTable( void );

  void registerConstraint( const std::string& sLocalField, const std::string& sRemoteTable, const std::string& sRemoteField );
  void setKey( const std::string& sLocalField );

  void ComposeStatement( const std::string& sTableName, std::string& sStatement );

protected:

private:

  unsigned int m_cntKeys;

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
// CTableDef
//

template<class T>  // T: Table Class with TableDef member function
class CTableDef: public CSqlNamedTable<T> {
public:

  typedef boost::shared_ptr<CTableDef<T> > pCTableDef_t;

  CTableDef( IDatabase& db, const std::string& sTableName );
  ~CTableDef( void ) {};

protected:

  void ComposeStatement( std::string& sStatement );

private:
  CTableDef( void );  // no default constructor
  CTableDef( const CTableDef& );  // no default copy constructor
};

template<class T>
CTableDef<T>::CTableDef( IDatabase& db, const std::string& sTableName )
: CSqlNamedTable<T>( db, sTableName )
{
  PrepareStatement();
}

template<class T>
void CTableDef<T>::ComposeStatement( std::string& sStatement ) {

  Action_CreateTable ct;  // action structure maintenance

  T t;

  t.Fields( ct );  // build structure from source definitions

  ct.ComposeStatement( TableName(), sStatement );  // build statement from structures

}

} // db
} // ou
