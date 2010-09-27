/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#include <boost/fusion/sequence/intrinsic/front.hpp>
#include <boost/fusion/include/front.hpp>

#include <LibBerkeleyDb/BerkeleyDb.h>

#include "ManagerBase.h"
#include "Account.h"
#include "AccountAdvisor.h"

class CAccountManager: public ManagerBase<CAccountManager, std::string, CAccountAdvisor> {
public:

  CAccountManager( const std::string& sDbFileName );
  ~CAccountManager(void);

  void AddAccountAdvisor( const std::string& sAccountAdvisorId, const std::string& sAccountAdvisorName );

protected:
private:

  typedef CAccountAdvisor::structAccountAdvisor structAccountAdvisor;
  typedef CAccountAdvisor::tplAccountAdvisor_t tplAccountAdvisor_t;
  typedef boost::fusion::result_of::front<tplAccountAdvisor_t>::element::type::fldStored_t AccountAdvisor_fldStored_t;
  typedef CBerkeleyDb<tplAccountAdvisor_t,structAccountAdvisor,AccountAdvisor_fldStored_t> tblAccountAdvisor_t;

  typedef CAccount::structAccount structAccount;
  typedef CAccount::tplAccount_t tplAccount_t;
  typedef boost::fusion::result_of::front<tplAccount_t>::element::type::fldStored_t Account_fldStored_t;
  typedef CBerkeleyDb<tplAccount_t,structAccount,Account_fldStored_t> tblAccount_t;

  std::string m_sDbFileName;
  static const std::string m_sAccountAdvisorDbName;
  static const std::string m_sAccountDbName;

  tblAccountAdvisor_t m_tblAccountAdvisor;
  tblAccount_t m_tblAccount;;

};
