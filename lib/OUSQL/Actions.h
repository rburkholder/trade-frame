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

// LibSql/Actions.h

#pragma once

#include <string>
#include <vector>

#include "Functions.h"

namespace ou {
namespace db {

// Action_Compose

class Action_Compose {
private:

  // definition of fields for table creation
  struct structField {
    std::string sFieldName;
    structField( const std::string& sFieldName_ ): sFieldName( sFieldName_ ) {};
  };

  typedef std::vector<structField> vField_t;

public:

  Action_Compose( void );
  virtual ~Action_Compose( void );

  template<typename T>
  void Field( const std::string& sFieldName, const T& var ) {
    addField( sFieldName );
  }

  template<typename T>
  void Field( const std::string& sFieldName, const T& var, const std::string& sFieldType ) {
    addField( sFieldName );
  }

  // inheritor will likely need something like:
  // void ComposeStatement( const std::string& sTableName, std::string& sStatement );
  // void ComposeStatement( std::string& sStatement );

  vField_t::size_type FieldCount( void ) { return m_vField.size(); };

protected:

  typedef vField_t::iterator vField_iter_t;
  vField_t m_vField;

  void addField( const std::string& sFieldName );

private:
};

// Action_Compose_CreateTable

class Action_Assemble_TableDef: public Action_Compose {
public:

  Action_Assemble_TableDef( const std::string& sTableName );
  ~Action_Assemble_TableDef( void );

//  template<typename T> // located in inheritor
//  void Field( const std::string& sFieldName, T& var ) { };

//  template<typename T> // located in inheritor
//  void Field( const std::string& sFieldName, T& var, const std::string& sFieldType ) { };

  void Key( const std::string& sLocalField );
  void Constraint( const std::string& sLocalField, const std::string& sRemoteTable, const std::string& sRemoteField );

  void ComposeCreateStatement( std::string& sStatement );

protected:

  // called by inheritor
  void addField( const std::string& sField, const char* szDbFieldType );

  // definition of fields for table creation
  struct structFieldDef {
    std::string sFieldName;
    std::string sFieldType;
    bool bIsKeyPart;
    structFieldDef( void ): bIsKeyPart( false ) {};
    structFieldDef(const std::string& sFieldName_, const std::string& sFieldType_ ) 
      : sFieldName( sFieldName_ ), sFieldType( sFieldType_ ), bIsKeyPart( false ) {};
  };

  typedef std::vector<structFieldDef> vFieldDef_t;
  typedef vFieldDef_t::iterator vFieldDef_iter_t;
  vFieldDef_t m_vFieldDef;

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

  std::string m_sTableName;

private:

};

// Action_Compose_Insert

class Action_Compose_Insert: public Action_Compose {
public:

  Action_Compose_Insert( const std::string& sTableName );
  ~Action_Compose_Insert( void );

  void ComposeStatement( std::string& sStatement );

protected:
private:
  std::string m_sTableName;
};

// Action_Compose_Update

class Action_Compose_Update: public Action_Compose {
public:

  Action_Compose_Update( const std::string& sTableName );
  ~Action_Compose_Update( void );

  void ComposeStatement( std::string& sStatement );

protected:
private:
  std::string m_sTableName;
};

// Action_Compose_Delete

class Action_Compose_Delete: public Action_Compose {
public:

  Action_Compose_Delete( const std::string& sTableName );
  ~Action_Compose_Delete( void );

  void ComposeStatement( std::string& sStatement );

protected:
private:
  std::string m_sTableName;
};


} // db
} // ou
