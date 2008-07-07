#include "StdAfx.h"
#include "PersistedOrderId.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPersistedOrderId::CPersistedOrderId(void) 
: CKeyValuePair()
{
}

CPersistedOrderId::~CPersistedOrderId(void) {
}

unsigned long CPersistedOrderId::GetNextOrderId() {
  static const string key = "OrderId";
  unsigned long nOrderId;
  try {
    nOrderId = GetUnsignedLong( key );
    ++nOrderId;
    Save( key, nOrderId );
  }
  catch ( std::out_of_range e ) {
    Save( key, m_nStartingOrderId );
    nOrderId = m_nStartingOrderId;
  }
  return nOrderId;
}
