#include "StdAfx.h"
#include "Log.h"

int CLog::m_cntInstances = 0;

CLog::CLog(void) {
  if ( 0 == m_cntInstances ) {
  }
  ++m_cntInstances;
}

CLog::~CLog(void) {
  --m_cntInstances;
  if ( 0 == m_cntInstances ) {
  }
}
