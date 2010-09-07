/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

#include "StdAfx.h"

//#include "LibCommon/Log.h"

#include "MergeDatedDatums.h"

//
// CMergeDatedDatums
//

CMergeDatedDatums::CMergeDatedDatums(void) 
: m_state( eInit ), m_request( eUnknown )
{
}

CMergeDatedDatums::~CMergeDatedDatums(void) {
  while ( !m_mhCarriers.Empty() ) {
    CMergeCarrierBase *p = m_mhCarriers.RemoveEnd();
    delete p;
  }
}

void CMergeDatedDatums::Add( CTimeSeries<CQuote>* pSeries, CMergeDatedDatums::OnDatumHandler function) {
  m_mhCarriers.Append( new CMergeCarrier<CQuote>( pSeries, function ) );
}

void CMergeDatedDatums::Add( CTimeSeries<CTrade>* pSeries, CMergeDatedDatums::OnDatumHandler function) {
  m_mhCarriers.Append( new CMergeCarrier<CTrade>( pSeries, function ) );
}

void CMergeDatedDatums::Add( CTimeSeries<CBar>* pSeries, CMergeDatedDatums::OnDatumHandler function) {
  m_mhCarriers.Append( new CMergeCarrier<CBar>( pSeries, function ) );
}

void CMergeDatedDatums::Add( CTimeSeries<CGreek>* pSeries, CMergeDatedDatums::OnDatumHandler function) {
  m_mhCarriers.Append( new CMergeCarrier<CGreek>( pSeries, function ) );
}

void CMergeDatedDatums::Add( CTimeSeries<CMarketDepth>* pSeries, CMergeDatedDatums::OnDatumHandler function) {
  m_mhCarriers.Append( new CMergeCarrier<CMarketDepth>( pSeries, function ) );
}

// http://www.codeguru.com/forum/archive/index.php/t-344661.html

/*
struct SortByMergeCarrier {
public:
  SortByMergeCarrier( std::vector<CMergeCarrierBase *> *v ): m_v( v ) {};
  bool operator() ( size_t lhs, size_t rhs ) { return (*m_v)[lhs]->GetDateTime() < (*m_v)[rhs]->GetDateTime(); };
protected:
  std::vector<CMergeCarrierBase *> *m_v;
};
*/

// be aware that this maybe running in alternate thread
// the thread is not created in this class 
// for example, see CSimulationProvider
void CMergeDatedDatums::Run() {
  m_request = eRun;
  size_t cntCarriers = m_mhCarriers.Size();
//  LOG << "#carriers: " << cntCarriers;  // need cross thread writing 
  CMergeCarrierBase *pCarrier;
  m_cntProcessedDatums = 0;
  m_state = eRunning;
  while ( ( 0 != cntCarriers ) && ( eRun == m_request ) ) {  // once all series have been depleted, end of run
    pCarrier = m_mhCarriers.GetRoot();
    pCarrier->ProcessDatum();  // automatically loads next datum when done
    ++m_cntProcessedDatums;
    if ( NULL == pCarrier->GetDatedDatum() ) {
      // retire the consumed carrier
      m_mhCarriers.ArchiveRoot();
      --cntCarriers;
    }
    else {
      // reorder the carriers
      m_mhCarriers.SiftDown();
    }
  }
  m_state = eStopped;
//  LOG << "Merge stats: " << m_cntProcessedDatums << ", " << m_cntReorders;
}

void CMergeDatedDatums::Stop( void ) {
  m_request = eStop;
}

