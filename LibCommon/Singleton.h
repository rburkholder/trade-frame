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

#include <stdexcept>

// other classes to fix:
//   IQFeedProviderSingleton
//   TimeSource
//   GeneratePeriodicRefresh

// http://www.oneunified.net/blog/Personal/SoftwareDevelopment/CPP/Singleton.article

template<class T> class CSingleton {
public:
  static T& Instance() {
    static T _instance;
    return _instance;
  }
protected:
  CSingleton() {
    ++m_ref;
    if ( 1 != m_ref ) throw std::runtime_error( "too many instances of CSingleton class T" );
  };          // ctor hidden
  virtual ~CSingleton() {};          // dtor hidden
private:
  CSingleton(CSingleton const&);    // copy ctor hidden
  CSingleton& operator=(CSingleton const&);  // assign op hidden

  static int m_ref;  // validation that only one instance has been created
};

template<class T> int CSingleton<T>::m_ref = 0;