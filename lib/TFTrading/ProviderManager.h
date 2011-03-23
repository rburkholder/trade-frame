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

  typedef keytypes::idProvider_t idProvider_t;
  typedef CProviderInterfaceBase::pProvider_t pProvider_t;

  CProviderManager(void) {};
  ~CProviderManager(void) {};

  pProvider_t Construct( const idProvider_t& key, keytypes::eidProvider_t type ); // construct given an enum
  template<class P> pProvider_t Construct( const idProvider_t& key ); // construct given a provider 'P' type
  void Release( const idProvider_t& key );  // should we check for close or anything?
  pProvider_t Get( const idProvider_t& key );

protected:

  void Register( const idProvider_t& key,  pProvider_t& pProvider );

private:

  typedef std::map<idProvider_t, pProvider_t> mapProviders_t;
  typedef std::pair<idProvider_t, pProvider_t> mapProviders_pair_t;
  typedef mapProviders_t::iterator iterProviders_t;

  mapProviders_t m_mapProviders;

};

template<class P> CProviderManager::pProvider_t CProviderManager::Construct( const idProvider_t& key ) {
  // need to perform some construction asssertions to ensure P is of a valid type
  pProvider_t pProvider;
  pProvider.reset( new P() );
  Register( key, pProvider );
  return pProvider;
}

} // namespace tf
} // namespace ou
