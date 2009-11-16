#include "StdAfx.h"

#include "BerkeleyDb.h"

// CBerkeleyDb =====

CBerkeleyDb::CBerkeleyDb(void) {
}

CBerkeleyDb::~CBerkeleyDb(void) {
}

// CBerkeleyDbEnv =====

unsigned int CBerkeleyDbEnv::m_RefCount = 0;
CBerkeleyDbEnv::mapDbEnv_t CBerkeleyDbEnv::m_mapDbEnv;

CBerkeleyDbEnv::CBerkeleyDbEnv() {
  ++m_RefCount;
  if ( 1 == m_RefCount ) {
    // need to do something here to ensure only one instance
  }
}

CBerkeleyDbEnv::~CBerkeleyDbEnv() {
  --m_RefCount;
  if ( 0 == m_RefCount ) {
    try {
      mapDbEnv_t::iterator m_iter_mapDbEnv = m_mapDbEnv.begin();
      DbEnv *pDbEnv;
      while ( m_mapDbEnv.end() != m_iter_mapDbEnv ) {
        pDbEnv = m_iter_mapDbEnv->second;
        pDbEnv->close( 0 );  // flags are undefined and need to be zero
        // delete pDbEnv;  // don't do this, the routine gets rid of its own stuff// but we do have a memory leak
        ++m_iter_mapDbEnv;
      }
      m_mapDbEnv.clear();
    }
    catch (...) {
      printf( "junk\n" );
    }
  }
}

DbEnv *CBerkeleyDbEnv::GetDbEnv( void ) {
  return GetDbEnv( "." );
}

DbEnv *CBerkeleyDbEnv::GetDbEnv( const char *szHome ) {
  // should expand szHome to full path so can tell if pointing to same location.
  DbEnv *pDbEnv;
  mapDbEnv_t::iterator m_mapDbEnv_iter = m_mapDbEnv.find( szHome );
  if ( m_mapDbEnv.end() == m_mapDbEnv_iter ) {
    pDbEnv = new DbEnv( 0 );
    pDbEnv->open( szHome, DB_INIT_MPOOL | DB_CREATE | DB_THREAD | DB_INIT_CDB, 0 );
    m_mapDbEnv.insert( mapDbEnv_pair_t( szHome, pDbEnv ) );
  }
  else {
    pDbEnv = m_mapDbEnv_iter->second;
  }
  return pDbEnv;
}
