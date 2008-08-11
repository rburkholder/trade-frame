#include "StdAfx.h"
#include "MergeDatedDatums.h"

//
// CMergeDatedDatums
//

CMergeDatedDatums::CMergeDatedDatums(void) 
: m_state( eInit ), m_request( eUnknown )
{
}

CMergeDatedDatums::~CMergeDatedDatums(void) {
  for each ( CMergeCarrierBase *i in m_vCarriers ) {
    delete i;
  }
}

void CMergeDatedDatums::Add( CTimeSeries<CQuote> *pSeries, CMergeDatedDatums::OnDatumHandler function) {
  m_vCarriers.push_back( new CMergeCarrier<CQuote>( pSeries, function ) );
}

void CMergeDatedDatums::Add( CTimeSeries<CTrade> *pSeries, CMergeDatedDatums::OnDatumHandler function) {
  m_vCarriers.push_back( new CMergeCarrier<CTrade>( pSeries, function ) );
}

void CMergeDatedDatums::Add( CTimeSeries<CBar> *pSeries, CMergeDatedDatums::OnDatumHandler function) {
  m_vCarriers.push_back( new CMergeCarrier<CBar>( pSeries, function ) );
}

void CMergeDatedDatums::Add( CTimeSeries<CMarketDepth> *pSeries, CMergeDatedDatums::OnDatumHandler function) {
  m_vCarriers.push_back( new CMergeCarrier<CMarketDepth>( pSeries, function ) );
}

// http://www.codeguru.com/forum/archive/index.php/t-344661.html

struct SortByMergeCarrier {
public:
  SortByMergeCarrier( std::vector<CMergeCarrierBase *> *v ): m_v( v ) {};
  bool operator() ( size_t lhs, size_t rhs ) { return (*m_v)[lhs]->GetDateTime() < (*m_v)[rhs]->GetDateTime(); };
protected:
  std::vector<CMergeCarrierBase *> *m_v;
};

// be aware that this maybe running in alternate thread
void CMergeDatedDatums::Run() {
  m_request = eRun;
  std::vector<size_t> vIx;  // ordered by most recent values from CMergeCarrierBase
  vIx.resize( m_vCarriers.size() );  // vIx provides mechanism to extract datums in datetime order from carriers.
  for ( size_t ix = 0; ix < vIx.size(); ++ix ) vIx[ ix ] = ix;  // preset each entry for each carrier
  std::sort( vIx.begin(), vIx.end(), SortByMergeCarrier( &m_vCarriers ) );
  size_t cntNulls = 0; // as timeseries depleted, move to end, and keep count
  size_t cntCarriers = vIx.size();
  CMergeCarrierBase *pCarrier;
  m_state = eRunning;
  while ( ( 0 != cntCarriers ) && ( eRun == m_request ) ) {  // once all series have been depleted, end of run
    pCarrier = m_vCarriers[vIx[0]];
    pCarrier->ProcessDatum();
    if ( NULL == pCarrier->GetDatedDatum() ) {
      // retire the consumed carrier
      ++cntNulls;
      --cntCarriers;
      size_t retired = vIx[ 0 ];
      for ( size_t ix = 0; ix < cntCarriers; ++ix ) {
        vIx[ ix ] = vIx[ ix + 1 ];  // move the carriers up to fill vacated spot at front
      }
      vIx[ cntCarriers ] = retired;  // used up time series
    }
    else {
      // reorder the carriers
      size_t ix = 1;
      size_t carrier = vIx[ 0 ];
      while ( ix < cntCarriers ) {
        if ( pCarrier->GetDateTime() <= m_vCarriers[vIx[ix]]->GetDateTime() ) break;
        vIx[ ix - 1 ] = vIx[ ix ];
        ++ix;
      }
      vIx[ ix - 1 ] = carrier;
    }
  }
  m_state = eStopped;
}

void CMergeDatedDatums::Stop( void ) {
  m_request = eStop;
}

