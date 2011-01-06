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

// LibSql/Actions.h

#pragma once

#include <string>
#include <vector>

namespace ou {
namespace db {

// Action_Compose

class Action_Compose {
public:

  Action_Compose( void );
  virtual ~Action_Compose( void );

  // inheritor will need one of two:
  // void ComposeStatement( const std::string& sTableName, std::string& sStatement );
  // void ComposeStatement( std::string& sStatement );

protected:

  // definition of fields
  struct structFieldDef {
    std::string sFieldName;
    std::string sFieldType;
    bool bIsKeyPart;
    structFieldDef( void ): bIsKeyPart( false ) {};
    structFieldDef(const std::string& sFieldName_, const std::string& sFieldType_ ) 
      : bIsKeyPart( false ), sFieldName( sFieldName_ ), sFieldType( sFieldType_ ) {};
  };

  typedef std::vector<structFieldDef> vFields_t;
  typedef vFields_t::iterator vFields_iter_t;
  vFields_t m_vFields;

  // called by inheritor
  void addField( const std::string& sField, const char* szDbFieldType );

private:
};

// Action_Compose_CreateTable

class Action_Compose_CreateTable: public Action_Compose {
public:

  Action_Compose_CreateTable( const std::string& sTableName );
  ~Action_Compose_CreateTable( void );

  void registerConstraint( const std::string& sLocalField, const std::string& sRemoteTable, const std::string& sRemoteField );
  void setKey( const std::string& sLocalField );

  void ComposeStatement( std::string& sStatement );

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

  std::string m_sTableName;

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
