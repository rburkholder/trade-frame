#include "StdAfx.h"
#include "MergeDatedDatums.h"

//
// CMergeCarrier
//

CMergeCarrier::CMergeCarrier( CTimeSeries<CDatedDatum> *pSeries, OnDatumHandler function ) {
  m_pSeries = pSeries;
  OnDatum = function;
  m_pDatum = m_pSeries->First();  // preload with first datum so we have it's time available for comparison
  m_dt = ( NULL == m_pDatum ) 
    ? boost::date_time::special_values::not_a_date_time 
    : m_pDatum->m_dt;
}

CMergeCarrier::~CMergeCarrier() {
}

void CMergeCarrier::ProcessDatum(void) {
  if ( NULL != OnDatum ) OnDatum( *m_pDatum );
  m_pDatum = m_pSeries->Next();
  m_dt = ( NULL == m_pDatum ) 
    ? boost::date_time::special_values::not_a_date_time 
    : m_pDatum->m_dt;
}

//
// CMergeDatedDatums
//

CMergeDatedDatums::CMergeDatedDatums(void) {
}

CMergeDatedDatums::~CMergeDatedDatums(void) {
  CMergeCarrier *pCarrier;
  while ( !m_lCarriers.empty() ) {
    pCarrier = m_lCarriers.back();
    delete pCarrier;
    m_lCarriers.pop_back();
  }
}

void CMergeDatedDatums::Add(CTimeSeries<CDatedDatum> *pSeries, CMergeDatedDatums::OnDatumHandler function) {
  m_lCarriers.push_back( new CMergeCarrier( pSeries, function ) );
}

void CMergeDatedDatums::Run() {
  bool bFirstSet;
  bool bDone = false;
  CMergeCarrier *pmcToUse;
  while ( !bDone ) {
    bFirstSet = false;
    for each ( CMergeCarrier *i in m_lCarriers ) {
      // goal is to find earliest time, that will be the one queued
      if ( NULL != i->m_pDatum ) {
        if ( !bFirstSet ) {
          pmcToUse = i;
          bFirstSet = true;
        }
        else {
          if ( i->m_dt < pmcToUse->m_dt ) {
            pmcToUse = i;
          }
        }
      }
    }
    if ( bFirstSet ) { // if at least one has been set, then use it
      pmcToUse->ProcessDatum();
    }
    else { // we havn't set anything so nothing to use
      bDone = true;
    }
  }
}

/*
  bool bFirstSet;
  bool bDone = false;
  CMergeCarrier *pmcToCompare;
  CMergeCarrier *pmcToUse;
  while ( !bDone ) {
    bFirstSet = false;
    for each ( CMergeCarrier *i in m_lCarriers ) {
      if ( !bFirstSet ) {
        pmcToCompare = i;
        if ( NULL == pmcToCompare->pDatum ) {
        }
        else {
          pmcToUse = i;
          bFirstSet = true;
        }
      }
      else {
        if ( i->m_dt < pmcToCompare->m_dt ) {
          pmcToUse = i;
        }
        pmcToCompare = i;
      }
    }
    if ( bFirstSet ) {
      pmcToUse->ProcessDatum();
    }
    else {
      bDone = true;
    }
  }
*/