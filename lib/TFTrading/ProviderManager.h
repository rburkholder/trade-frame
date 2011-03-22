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

#include "KeyTypes.h"

#include "ManagerBase.h"
#include "ProviderInterface.h"

// key might be account or other similar globally known identifier

// ProviderManager probably won't be used much at the moment
// it should be converted over to boost::fusion for storing full types and values for the 
// various providers, and then have specialized algorithms for processing the tuples of providers.

namespace ou { // One Unified
namespace tf { // TradeFrame

class CProviderManager: public ManagerBase<CProviderManager> {
public:

  typedef CProviderInterfaceBase::pProvider_t pProvider_t;
  typedef keytypes::idProvider_t idProvider_t;

  CProviderManager(void) {};
  ~CProviderManager(void) {};

  void Register( const idProvider_t& key,  pProvider_t pProvider );
  void Release( const idProvider_t& key );  // should we check for close or anything?
  pProvider_t Get( const idProvider_t& key );

protected:

private:

  typedef std::map<idProvider_t, pProvider_t> mapProviders_t;
  typedef std::pair<idProvider_t, pProvider_t> mapProviders_pair_t;
  typedef mapProviders_t::iterator iterProviders_t;

  mapProviders_t m_mapProviders;

};

} // namespace tf
} // namespace ou
