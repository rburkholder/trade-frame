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

#include "StdAfx.h"

#include <stdexcept>

#include "AccountOwner.h"

const std::string CAccountOwner::m_sSqlCreate( 
  "create table accountowners ( \
    accountownerid TEXT CONSTRAINT pk_accountowners PRIMARY KEY, \
    version SMALLINT DEFAULT 1, \
    accountadvisorid TEXT NOT NULL, \
    firstname TEXT NOT NULL, \
    lastname TEXT NOT NULL, \
    CONSTRAINT fk_accountowners_accountadvisorid \
      FOREIGN KEY(accountadvisorid) REFERENCES accountadvisors(accountadvisorid) \
        ON DELETE RESTRICT ON UPDATE CASCADE \
    );" );
const std::string CAccountOwner::m_sSqlSelect( "SELECT accountadvisorid, firstname, lastname FROM accountowners WHERE accountownerid = :id;" );
const std::string CAccountOwner::m_sSqlInsert( 
  "INSERT INTO accountowners (accountownderid, accountadvisorid, firstname, lastname) VALUES ( :id, :advisorid, :fn, :ln );" );
const std::string CAccountOwner::m_sSqlUpdate( "UPDATE accountowners SET firstname = :fn, lastname = :ln WHERE accountownerid = :id;" );
const std::string CAccountOwner::m_sSqlDelete( "DELETE FROM accountownders WHERE accountownderid = :id;" );

CAccountOwner::CAccountOwner( 
    const keyAccountOwnerId_t& sAccountOwnerId, 
    const keyAccountAdvisorId_t& sAccountAdvisorId,
    const std::string& sFirstName, const std::string& sLastName ) 
: m_sAccountOwnerId( sAccountOwnerId ),
  m_sAccountAdvisorId( sAccountAdvisorId ),
  m_sFirstName( sFirstName ), m_sLastName( sLastName )
{
}

CAccountOwner::CAccountOwner( const keyAccountOwnerId_t& sAccountOwnerId, sqlite3_stmt* pStmt ) 
: m_sAccountOwnerId( sAccountOwnerId ),
  m_sAccountAdvisorId( reinterpret_cast<const char*>( sqlite3_column_text( pStmt, 0 ) ) ),
  m_sFirstName( reinterpret_cast<const char*>( sqlite3_column_text( pStmt, 1 ) ) ),
  m_sLastName( reinterpret_cast<const char*>( sqlite3_column_text( pStmt, 2 ) ) )
{
}

CAccountOwner::~CAccountOwner(void) {
}

void CAccountOwner::CreateDbTable( sqlite3* pDb ) {

  char* pMsg;
  int rtn;

  rtn = sqlite3_exec( pDb, m_sSqlCreate.c_str(), 0, 0, &pMsg );

  if ( SQLITE_OK != rtn ) {
    std::string sErr( "Error creating table accountowners: " );
    sErr += pMsg;
    sqlite3_free( pMsg );
    throw std::runtime_error( sErr );
  }
}

int CAccountOwner::BindDbKey( sqlite3_stmt* pStmt ) {
  int rtn( 0 );
  rtn = sqlite3_bind_text( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":id" ), m_sAccountOwnerId.c_str(), -1, SQLITE_TRANSIENT );
  return rtn;
}

int CAccountOwner::BindDbVariables( sqlite3_stmt* pStmt ) {
  int rtn( 0 );
  rtn += sqlite3_bind_text( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":advisorid" ), m_sAccountAdvisorId.c_str(), -1, SQLITE_TRANSIENT );
  rtn += sqlite3_bind_text( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":fn" ), m_sFirstName.c_str(), -1, SQLITE_TRANSIENT );
  rtn += sqlite3_bind_text( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":ln" ), m_sLastName.c_str(), -1, SQLITE_TRANSIENT );
  return rtn;  // should be 0 if all goes well
}

