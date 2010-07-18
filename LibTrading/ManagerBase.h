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

// class T:  target class
// class K:  type of key for record lookup
// class R:  type of record being looked up

// Manager base class for 
//  ExecutionManager, 
//  InstrumentManager, 
//  OrderManager, 
//  PortfolioManager
//  ProviderManager, 

#include <map>

#include "boost/shared_ptr.hpp"

#include "LibCommon/Singleton.h"

template<class T, class K, class R> class ManagerBase: public CSingleton<T> {
public:

  ManagerBase<T,K,R>( void );
  virtual ~ManagerBase<T,K,R>( void );
  typedef boost::shared_ptr<R> pR_t;

protected:

  typedef std::map<K, pR_t> map_t;
  typedef std::pair<K, pR_t> mappair_t;
  map_t m_records; // where records of type R with key of type K are stored

private:
};

template<class T, class K, class R> ManagerBase<T,K,R>::ManagerBase() {
}

template<class T, class K, class R> ManagerBase<T,K,R>::~ManagerBase( void ) {
}