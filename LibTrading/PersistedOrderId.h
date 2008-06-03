#pragma once
#include "k:\data\projects\tradingsoftware\libberkeleydb\keyvaluepair.h"

class CPersistedOrderId :
  public CKeyValuePair {
public:
  CPersistedOrderId(void);
  virtual ~CPersistedOrderId(void);
  unsigned long GetNextOrderId( void ); // may need to worry about multi-threading at some point in time
protected:
  const static unsigned long m_nStartingOrderId = 1000;
private:
};
