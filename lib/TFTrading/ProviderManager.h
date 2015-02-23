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

#include <OUCommon/ManagerBase.h>

#include "KeyTypes.h"

#include "ProviderInterface.h"

// key might be account or other similar globally known identifier

// map could be converted over to boost::fusion for storing full types and values for the 
// various providers, and then have specialized algorithms for processing the tuples of providers.

namespace ou { // One Unified
namespace tf { // TradeFrame

class ProviderManager: public ou::db::ManagerBase<ProviderManager> {
public:

  typedef keytypes::idProvider_t idProvider_t;
  typedef ProviderInterfaceBase::pProvider_t pProvider_t;

  ProviderManager(void) {};
  ~ProviderManager(void) {};

  // when to use Construct and when to use Get?
  pProvider_t Construct( const idProvider_t& key, keytypes::eidProvider_t type ); // construct given an enum
  template<class P> pProvider_t Construct( const idProvider_t& key ); // construct given a provider 'P' type
  void Register( const idProvider_t& key,  pProvider_t pProvider );
  void Release( const idProvider_t& key );  // should we check for close or anything?  need to keep a lock count.
  pProvider_t Get( const idProvider_t& key );

protected:

private:

  typedef std::map<idProvider_t, pProvider_t> mapProviders_t;
  typedef std::pair<idProvider_t, pProvider_t> mapProviders_pair_t;
  typedef mapProviders_t::iterator iterProviders_t;

  mapProviders_t m_mapProviders;

};

template<class P> ProviderManager::pProvider_t ProviderManager::Construct( const idProvider_t& key ) {
  // need to perform some construction asssertions to ensure P is of a valid type
  pProvider_t pProvider;
  pProvider.reset( new P() );
  Register( key, pProvider );
  return pProvider;
}

} // namespace tf
} // namespace ou
