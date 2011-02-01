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
#include <map>

#include "ManagerBase.h"
#include "ProviderInterface.h"

// key might be account or other similar globally known identifier

// ProviderManager probably won't be used much at the moment
// it hould be converted over to boost::fusion for storing full types and values for the 
// various providers, and then have specialized algorithms for processing the tuples of providers.

namespace ou { // One Unified
namespace tf { // TradeFrame

class CProviderManager: public ManagerBase<CProviderManager, std::string, CProviderInterfaceBase> {
public:

  typedef CProviderInterfaceBase::pProvider_t pProvider_t;

  CProviderManager(void);
  ~CProviderManager(void);

  void Register( const std::string& sKey,  pProvider_t pProvider );

protected:

  typedef std::map<std::string, pProvider_t> providers_t;
  typedef std::pair<std::string, pProvider_t> providers_pair_t;

  providers_t m_mapProviders;

private:
};

} // namespace tf
} // namespace ou
