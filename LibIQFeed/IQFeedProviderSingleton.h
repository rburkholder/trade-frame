#pragma once

#include "IQFeedProvider.h"

class CIQFeedProviderSingleton {
public:
  CIQFeedProviderSingleton(void);
  ~CIQFeedProviderSingleton(void);
  CIQFeedProvider *GetIQFeedProvider( void ) { return m_pProvider; };
protected:
private:
  static size_t m_cntConstructionCount;
  static CIQFeedProvider *m_pProvider;
};
