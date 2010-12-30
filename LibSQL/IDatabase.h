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

// inherited by ISqlite3 and IPostgresql for specialization
#pragma once

#include <string>
#include <list>

namespace ou {
namespace db {

enum enumOpenFlags { 
  EOpenFlagsZero = 0, 
  EOpenFlagsAutoCreate = 0x1 
};

class IDatabase {
public:

  struct structStatement {
    std::string sSqlStatement;  // where the query goes
  };

  IDatabase(void);
  virtual ~IDatabase(void);

  virtual void Open( const std::string& sDbFileName, enumOpenFlags = EOpenFlagsZero ) {};
  virtual void Close( void ) {};

  virtual structStatement& AllocateStatement( void ) = 0;
  virtual void PrepareStatement( structStatement& statement ) = 0;
  virtual void CloseStatement( structStatement& statement ) = 0;

  virtual void ExecuteStatement( structStatement& statement ) = 0;

protected:

  bool m_bDbOpened;

  std::string m_sDbFileName;

private:
};

// used for common linked list processing
template<class SS>  // StatementState is stuff for linked list from IDatabase specializations
class IDatabaseCommon: public IDatabase {
public:
  structStatement& AllocateStatement( void );
  //void PrepareStatement( structStatement& statement ) {};
protected:

  typedef IDatabase::structStatement structStatement;

  struct structStatementControl: public structStatement {  // facilitates dynamic_cast
    SS stateStatement;
    typename std::list<structStatementControl>::iterator iterStmt;  // self for referencing
  };

  // maintain list of statements submitted to database
  // caller holds an iterator as a handle
  typedef std::list<structStatementControl> lStmt_t;

  lStmt_t m_lStmt;
  typedef typename lStmt_t::iterator lStmt_iter_t;

private:
};

template<class SS>
IDatabase::structStatement& IDatabaseCommon<SS>::AllocateStatement( void ) {
  structStatementControl sc;  // dummy for pushing
  m_lStmt.push_front( sc );
  lStmt_iter_t iter = m_lStmt.begin();
  iter->iterStmt = iter;
  return m_lStmt.front();
}



} // db
} // ou
