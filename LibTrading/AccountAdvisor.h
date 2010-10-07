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

#include <LibSqlite/sqlite3.h>

class CAccountAdvisor
{
  friend class CAccountManager;

public:

  CAccountAdvisor( void );
  CAccountAdvisor( const std::string& sAdvisorId, const std::string& sAdvisorName );
  ~CAccountAdvisor(void);

  void SetAdvisorId( const std::string& sAdvisorId ) { 
    m_sAdvisorId = sAdvisorId; 
  }
  const std::string& GetAdvisorId( void ) const { 
    return m_sAdvisorId; 
  }

  void SetAdvisorName( const std::string& sAdvisorName ) { 
    m_sAdvisorName = sAdvisorName; 
  }
  const std::string& GetAdvisorName( void ) const { 
    return m_sAdvisorName; 
  }

  static void CreateDbTable( sqlite3* pDb );

protected:

private:

  std::string m_sAdvisorId;
  std::string m_sAdvisorName;

};


