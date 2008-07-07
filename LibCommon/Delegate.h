#pragma once

#include "FastDelegate.h"
// http://www.codeproject.com/cpp/FastDelegate.asp
using namespace fastdelegate;

#include <vector>
#include <algorithm>
using namespace std;

#include <assert.h>

template<class RO> class Delegate {
  // RO: Return Object in call

public:
  Delegate<RO>(void);
  ~Delegate<RO>(void);
  typedef FastDelegate1<RO> OnMessageHandler;
  typedef typename std::vector<OnMessageHandler>::size_type vsize_t;
  void Add( OnMessageHandler function );
  void Remove( OnMessageHandler function );
  bool IsEmpty();
  void operator()( RO );
  vsize_t Size( void ) const { return rOnFD.size(); };
protected:
private:
  std::vector<OnMessageHandler> rOnFD;
  std::vector<OnMessageHandler> rToBeRemoved;
  std::vector<OnMessageHandler> rToBeAdded;
  bool m_bIterating;
};

template<class RO> Delegate<RO>::Delegate(void) : m_bIterating( false ) {
}

template<class RO> Delegate<RO>::~Delegate(void) {
  assert( !m_bIterating );
  rOnFD.clear();
  rToBeRemoved.clear();
  rToBeAdded.clear();
}

template<class RO> void Delegate<RO>::operator()( RO ro ) {

  std::vector<OnMessageHandler>::iterator iter;

  m_bIterating = true;
  iter = rOnFD.begin();
  while ( rOnFD.end() != iter ) {
    (*iter)( ro );
    ++iter;
  }
  //}
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
        break;
      }
      ++rOnFD_Iter;
    }
  }
}

template<class RO> bool Delegate<RO>::IsEmpty() {
  return rOnFD.empty();
}

