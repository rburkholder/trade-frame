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

#include "sqlite3.h"

class CCountryCode
{
public:

  CCountryCode( const std::string& sAlpha2, const std::string& sName );
  CCountryCode( const std::string& sAlpha2, sqlite3_stmt* pStmt );
  ~CCountryCode(void);

  static void CreateDbTable( sqlite3* pDb );
  int BindDbKey( sqlite3_stmt* pStmt );
  int BindDbVariables( sqlite3_stmt* pStmt );
  static const std::string& GetSqlSelect( void ) { return m_sSqlSelect; };
  static const std::string& GetSqlInsert( void ) { return m_sSqlInsert; };
  static const std::string& GetSqlUpdate( void ) { return m_sSqlUpdate; };
  static const std::string& GetSqlDelete( void ) { return m_sSqlDelete; };

protected:
private:

  std::string m_sAlpha2;
  std::string m_sName;

  static const std::string m_sSqlCreate;
  static const std::string m_sSqlSelect;
  static const std::string m_sSqlInsert;
  static const std::string m_sSqlUpdate;
  static const std::string m_sSqlDelete;

};

