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



#include "FastDelegate.h"
// http://www.codeproject.com/cpp/FastDelegate.asp
using namespace fastdelegate;

#include <vector>
#include <algorithm>

#include <assert.h>

namespace ou {

  template<class RO> class Delegate {
  // RO: Return Object in call

public:
  Delegate<RO>(void);
  ~Delegate<RO>(void);
  typedef FastDelegate1<RO> OnMessageHandler;
  typedef typename std::vector<OnMessageHandler>::size_type vsize_t;
  void Add( OnMessageHandler function );
  void Remove( OnMessageHandler function );
  void operator()( RO );
  bool IsEmpty() { return ( 0 == m_size ); };
  vsize_t Size( void ) const { return m_size; };
protected:
private:
  std::vector<OnMessageHandler> rOnFD;
  std::vector<OnMessageHandler> rToBeRemoved;
  std::vector<OnMessageHandler> rToBeAdded;
  bool m_bIterating;
  typename std::vector<OnMessageHandler>::size_type m_size;
};

template<class RO> Delegate<RO>::Delegate(void) 
: m_bIterating( false ), m_size( 0 )
{
}

template<class RO> Delegate<RO>::~Delegate(void) {
  assert( !m_bIterating );
  rOnFD.clear();
  rToBeRemoved.clear();
  rToBeAdded.clear();
  m_size = 0;
}

template<class RO> void Delegate<RO>::operator()( RO ro ) {

  std::vector<OnMessageHandler>::iterator iter;

  m_bIterating = true;
  iter = rOnFD.begin();
  while ( rOnFD.end() != iter ) {
    (*iter)( ro );
    ++iter;
  }
  m_bIterating = false;

  if ( !rToBeAdded.empty() ) {
    iter = rToBeAdded.begin();
    while ( rToBeAdded.end() != iter ) {
      rOnFD.push_back( *iter );
      ++iter;
    }
    rToBeAdded.clear();
  }

  if ( !rToBeRemoved.empty() ) {
    iter = rToBeRemoved.begin();
    while ( rToBeRemoved.end() != iter ) {
      Remove( *iter );
      ++iter;
    }
    rToBeRemoved.clear();
  }
}

template<class RO> void Delegate<RO>::Add( OnMessageHandler function ) {
  if ( m_bIterating ) {
    rToBeAdded.push_back( function );
  }
  else {
    rOnFD.push_back( function );
  }
  ++m_size;
}

template<class RO> void Delegate<RO>::Remove( OnMessageHandler function ) {

  if ( m_bIterating ) {
    rToBeRemoved.push_back( function );
  }
  else {
    std::vector<OnMessageHandler>::iterator rOnFD_Iter = rOnFD.begin();

    while ( rOnFD.end() != rOnFD_Iter ) {
      if ( function == *rOnFD_Iter ) {
        rOnFD.erase( rOnFD_Iter );
        break;  // allow only one deletion
      }
      ++rOnFD_Iter;
    }
  }
  --m_size;
}

} // ou