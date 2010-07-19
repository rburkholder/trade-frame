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

// when a provider is created, it should be registered with the provider manager

#pragma once

#include <string>

#include "ManagerBase.h"
#include "ProviderInterface.h"

class CProviderManager: public ManagerBase<CProviderManager, std::string, CProviderInterfaceBase> {
public:
  CProviderManager(void);
  ~CProviderManager(void);

  void Register( CProviderInterfaceBase::pProvider_ref pProvider ) {};
protected:
private:
};
