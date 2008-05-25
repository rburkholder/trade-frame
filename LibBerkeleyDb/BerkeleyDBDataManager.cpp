#include "StdAfx.h"
#include "BerkeleyDBDataManager.h"

const char CBerkeleyDBDataManager::m_BDBFileName[] = "TradeFrame.bdb";
DbEnv CBerkeleyDBDataManager::m_DbEnv( 0 );
unsigned int CBerkeleyDBDataManager::m_RefCount = 0;

CBerkeleyDBDataManager BDBDM;

CBerkeleyDBDataManager::CBerkeleyDBDataManager(void) {
  ++m_RefCount;
  if ( 1 == m_RefCount ) {
    m_DbEnv.open( ".", DB_INIT_MPOOL | DB_CREATE | DB_THREAD | DB_INIT_CDB, 0 );
  }
}

CBerkeleyDBDataManager::~CBerkeleyDBDataManager(void) {
  --m_RefCount;
  if ( 0 == m_RefCount ) {
    m_DbEnv.close( 0 );
  }
}
