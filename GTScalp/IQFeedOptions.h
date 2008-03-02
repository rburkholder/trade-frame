#pragma once
#include "iqfeedretrieval.h"
#include <vector>
#include <string>

using namespace std;

#include "FastDelegate.h"
using namespace fastdelegate;

class CIQFeedOptions :
  public CIQFeedRetrieval {
public:

  vector<string *> m_vOptionSymbols;

  CIQFeedOptions(const char *);
  virtual ~CIQFeedOptions(void);

  typedef FastDelegate0<void> OnSymbolListReceivedHandler;
  void SetOnSymbolListRecieved( OnSymbolListReceivedHandler function ) {
    OnSymbolListReceived = function;
  }

protected:
  virtual void OnNewResponse( const char *szLine );
  void AddOptionSymbol( const char *s, unsigned short cnt );
private:
  OnSymbolListReceivedHandler OnSymbolListReceived;
};
