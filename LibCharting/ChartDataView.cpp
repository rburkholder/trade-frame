#include "StdAfx.h"
#include "ChartDataView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
// CChartDataViewCarrier
//

CChartDataViewCarrier::CChartDataViewCarrier( void ) 
: m_nChart( 0 ), m_pChartEntry( NULL )
{
}

CChartDataViewCarrier::CChartDataViewCarrier( size_t nChart, CChartEntryBaseWithTime *pChartEntry ) 
: m_nChart( nChart ), m_pChartEntry( pChartEntry )
{
}

CChartDataViewCarrier::CChartDataViewCarrier( const CChartDataViewCarrier &carrier ) 
: m_nChart( carrier.m_nChart ), m_pChartEntry( carrier.m_pChartEntry )
{
}

CChartDataViewCarrier::~CChartDataViewCarrier() {
  m_nChart = 0;
  m_pChartEntry = NULL;
}


// 
// CChartDataView
//

CChartDataView::CChartDataView( const std::string &sStrategy, const std::string &sName )
: m_bClosed( false ), m_sStrategy( sStrategy ), m_sName( sName )
{
  m_vChartDataViewEntry.reserve( 10 );
}

CChartDataView::~CChartDataView(void) {
  assert( m_bClosed );
  m_vChartDataViewEntry.clear();
}

void CChartDataView::Add(size_t nChart, CChartEntryBaseWithTime *pChartEntry) {
  CChartDataViewCarrier carrier( nChart, pChartEntry );
  m_vChartDataViewEntry.push_back( carrier );
}

void CChartDataView::Close() {
  assert( !m_bClosed );
  OnClosing( this );
  m_bClosed = true;
}

