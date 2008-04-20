#pragma once

#include "FastDelegate.h"
// http://www.codeproject.com/cpp/FastDelegate.asp
using namespace fastdelegate;

#include <vector>
#include <algorithm>
using namespace std;

template<class RO> class Delegate {
  // RO: Return Object in call

public:
  Delegate<RO>(void);
  ~Delegate<RO>(void);
  typedef FastDelegate1<RO> OnMessageHandler;
  void Add( OnMessageHandler function );
  void Remove( OnMessageHandler function );
  bool IsEmpty();
  void operator()( RO );
protected:
private:
  std::vector<OnMessageHandler> rOnFD;
};

template<class RO> Delegate<RO>::Delegate(void) {
}

template<class RO> Delegate<RO>::~Delegate(void) {
  rOnFD.clear();
}

template<class RO> void Delegate<RO>::Add( OnMessageHandler function ) {
  rOnFD.push_back( function );
}

template<class RO> void Delegate<RO>::Remove( OnMessageHandler function ) {

  std::vector<OnMessageHandler>::iterator rOnFD_Iter = rOnFD.begin();;

  while ( rOnFD.end() != rOnFD_Iter ) {
    if ( function == *rOnFD_Iter ) {
      rOnFD.erase( rOnFD_Iter );
      break;
    }
    ++rOnFD_Iter;
  }
}

template<class RO> bool Delegate<RO>::IsEmpty() {
  return rOnFD.empty();
}

template<class RO> void Delegate<RO>::operator()( RO ro ) {

  std::vector<OnMessageHandler>::iterator rOnFD_Iter;

  rOnFD_Iter = rOnFD.begin();
  while ( rOnFD.end() != rOnFD_Iter ) {
    (*rOnFD_Iter)( ro );
    ++rOnFD_Iter;
  }
}

