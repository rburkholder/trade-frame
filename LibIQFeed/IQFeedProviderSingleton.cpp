#include "StdAfx.h"
#include "IQFeedProviderSingleton.h"

size_t CIQFeedProviderSingleton::m_cntConstructionCount = 0;
CIQFeedProvider *CIQFeedProviderSingleton::m_pProvider = NULL;

CIQFeedProviderSingleton::CIQFeedProviderSingleton(void) {
  ++m_cntConstructionCount;
  if ( 1 == m_cntConstructionCount ) {
    m_pProvider = new CIQFeedProvider();
    m_pProvider->Connect();
  }
}

CIQFeedProviderSingleton::~CIQFeedProviderSingleton(void) {
  assert( 0 < m_cntConstructionCount );
  --m_cntConstructionCount;
  if ( 0 == m_cntConstructionCount ) {
    m_pProvider->Disconnect();
    delete m_pProvider;
    m_pProvider = NULL;
  }
}
