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

#include <boost/utility.hpp>
#include <boost/thread/tss.hpp>

// other classes to fix:
//   TimeSource

// http://www.oneunified.net/blog/Personal/SoftwareDevelopment/CPP/Singleton.article

namespace ou {

//
// Singleton
//

// look in patterns book for protected version of creation:  double check lock pattern
// but may need two versions:  1 singleton common across threads, 1 singleton per thread

class SingletonBase {
public:
  enum ELocalCommonInstanceSource_t {
    Global, // useful in simple program environments
    Assigned  // required for multi-threaded simultaneous simulations
  };
  static void SetLocalCommonInstanceSource( ELocalCommonInstanceSource_t source ) { m_source = source; };
  static ELocalCommonInstanceSource_t GetLocalCommonInstanceSource( void ) { return m_source; };
protected:
  static ELocalCommonInstanceSource_t m_source;
private:
};

template<typename T>
class Singleton: boost::noncopyable, public SingletonBase {
public:
  static T& GlobalInstance() {  // global to whole program
    static T _instance;
    return _instance;
  }

  static T& LocalUniqueInstance() {  // unique to this thread instance
    T* t( 0 );
    if ( 0 == m_pT.get() ) {
      t = new T;
      m_pT.reset( t );
      m_nLUI++;
    }
    else {
      t = m_pT.get();
    }
    return *t;
  }

  static T& LocalCommonInstance() { // unique to a number of thread instances, set with SetLocalCommonInstance
    T* t;
    switch ( m_source ) {
    case Global:
      return GlobalInstance();
      break;
    case Assigned:
      t = m_pT.get();
      if ( 0 == t )
        throw std::runtime_error( "LocalCommonInstance not available" );
      return *t;
      break;
    default:
      throw std::runtime_error( "LocalCommonInstance mis-assigned" );
      break;
    }
  }

  static void SetLocalCommonInstance( T* t ) {
    m_pT.reset( t );
  }

  static void ClearLocalCommonInstance() {
    m_pT.reset();
  }

protected:
  Singleton() {};          // ctor hidden
  virtual ~Singleton() {}; // dtor hidden
private:
  static std::size_t m_nLUI;
  static boost::thread_specific_ptr<T> m_pT;
};

template<typename T>
std::size_t Singleton<T>::m_nLUI( 0 );

template<typename T>
boost::thread_specific_ptr<T> Singleton<T>::m_pT;

} // ou
