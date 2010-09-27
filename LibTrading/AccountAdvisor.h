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

#include <LibBerkeleyDb/Record.h>

#define OU_TABLE_ACCOUNTADVISOR_RECORD_FIELDS \
  ((OU_TABLE_ACCOUNTADVISOR_ADVISORID,    ProcessFieldSk, sAdvisorId  )) \
  ((OU_TABLE_ACCOUNTADVISOR_ADVISORNAME,  ProcessFieldSk, sAdvisorName  )) \
  ((OU_TABLE_ACCOUNTADVISOR_VERSION,      ProcessFieldPod<unsigned long>, nVersion )) \
  /**/

class CAccountAdvisor
{
  friend class CAccountManager;

  OU_DB_DECLARE_STRUCTURES(AccountAdvisor, OU_TABLE_ACCOUNTADVISOR_RECORD_FIELDS)

public:

  CAccountAdvisor( void );
  CAccountAdvisor( const std::string& sAdvisorId, const std::string& sAdvisorName );
  ~CAccountAdvisor(void);

  void SetAdvisorId( const std::string& sAdvisorId ) { 
    boost::fusion::at_c<OU_TABLE_ACCOUNTADVISOR_ADVISORID>( m_tplAccountAdvisor ) = sAdvisorId; 
  }
  const std::string& GetAdvisorId( void ) { 
    return boost::fusion::at_c<OU_TABLE_ACCOUNTADVISOR_ADVISORID>( m_tplAccountAdvisor ).value(); 
  }

  void SetAdvisorName( const std::string& sAdvisorName ) { 
    boost::fusion::at_c<OU_TABLE_ACCOUNTADVISOR_ADVISORNAME>( m_tplAccountAdvisor ) = sAdvisorName; 
  }
  const std::string& GetAdvisorName( void ) { 
    return boost::fusion::at_c<OU_TABLE_ACCOUNTADVISOR_ADVISORNAME>( m_tplAccountAdvisor ).value(); 
  }

protected:

private:

};

