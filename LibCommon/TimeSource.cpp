#include "StdAfx.h"
#include "TimeSource.h"

bool CTimeSource::m_bInSimulation = false;
ptime CTimeSource::m_dtSimulationTime = boost::date_time::not_a_date_time;

CTimeSource::CTimeSource(void) {
}

CTimeSource::~CTimeSource(void) {
}
