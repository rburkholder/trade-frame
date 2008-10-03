#include "StdAfx.h"
#include "TimeSource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool CTimeSource::m_bInSimulation = false;
ptime CTimeSource::m_dtSimulationTime = boost::date_time::not_a_date_time;

CTimeSource::CTimeSource(void) {
}

CTimeSource::~CTimeSource(void) {
}
