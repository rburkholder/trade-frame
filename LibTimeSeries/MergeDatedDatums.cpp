#include "StdAfx.h"
#include "MergeDatedDatums.h"


//
// CMergeDatedDatums
//

CMergeDatedDatums::CMergeDatedDatums(void) {
}

CMergeDatedDatums::~CMergeDatedDatums(void) {
  CMergeCarrierBase *pCarrier;
  while ( !m_lCarriers.empty() ) {
    pCarrier = m_lCarriers.back();
    delete pCarrier;
    m_lCarriers.pop_back();
  }
}

void CMergeDatedDatums::Add( CTimeSeries<CQuote> *pSeries, CMergeDatedDatums::OnDatumHandler function) {
  m_lCarriers.push_back( new CMergeCarrier<CQuote>( pSeries, function ) );
}

void CMergeDatedDatums::Add( CTimeSeries<CTrade> *pSeries, CMergeDatedDatums::OnDatumHandler function) {
  m_lCarriers.push_back( new CMergeCarrier<CTrade>( pSeries, function ) );
}

void CMergeDatedDatums::Add( CTimeSeries<CBar> *pSeries, CMergeDatedDatums::OnDatumHandler function) {
  m_lCarriers.push_back( new CMergeCarrier<CBar>( pSeries, function ) );
}

void CMergeDatedDatums::Add( CTimeSeries<CMarketDepth> *pSeries, CMergeDatedDatums::OnDatumHandler function) {
  m_lCarriers.push_back( new CMergeCarrier<CMarketDepth>( pSeries, function ) );
}

void CMergeDatedDatums::Run() {
  bool bFirstSet;
  bool bDone = false;
  CMergeCarrierBase *pmcToUse;
  while ( !bDone ) {
    bFirstSet = false;
    for each ( CMergeCarrierBase *i in m_lCarriers ) {
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