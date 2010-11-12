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

// http://www.oneunified.net/blog/Personal/SoftwareDevelopment/CPP/Singleton.article

namespace ou {

//
// CSingleton
//

template<typename T> 
class CSingleton {
public:
  static T& Instance() {
    static T _instance;
    return _instance;
  }
protected:
  CSingleton() {};          // ctor hidden
  ~CSingleton() {};          // dtor hidden
private:
  CSingleton(CSingleton const&);    // copy ctor hidden
  CSingleton& operator=(CSingleton const&);  // assign op hidden

};

//
// CMultipleInstanceTest
//

// a CRTP class to ensure Singleton'd class isn't multiply defined
// see CBerkeleyDBEnvManager as an example
template<typename T>
class CMultipleInstanceTest {
public:
  CMultipleInstanceTest( void ) {
#ifdef _DEBUG
    ++m_ref;
    // this may be changed to handle multi thread stuff, Dr. Dobbs has a solution
    if ( 1 != m_ref ) throw std::runtime_error( "too many instances of CSingleton typename T" );
#endif
  }
protected:
private:
#ifdef _DEBUG
  static int m_ref;  // validation that only one instance has been created
#endif
};

#ifdef _DEBUG
template<typename T> int CMultipleInstanceTest<T>::m_ref = 0;
#endif

} // ou