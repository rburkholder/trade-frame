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
class CAccount
{
public:
  CAccount(void);
  ~CAccount(void);
protected:

  typedef unsigned long account_key_t;
  typedef unsigned long accountadvisor_key_t;
  typedef unsigned long string_key_t;

  struct structAccount {
    account_key_t pk;
    accountadvisor_key_t fkAccountAdvisor;
    string_key_t fkAccountName;
  };
private:
};

