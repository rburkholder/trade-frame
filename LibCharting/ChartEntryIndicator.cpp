#include "StdAfx.h"
#include "ChartEntryIndicator.h"

CChartEntryIndicator::CChartEntryIndicator(void) 
: CChartEntryBaseWithTime() 
{
}

CChartEntryIndicator::CChartEntryIndicator( unsigned int nSize ) 
: CChartEntryBaseWithTime( nSize )
{
}

CChartEntryIndicator::~CChartEntryIndicator(void) {
}

void CChartEntryIndicator::Reserve( unsigned int nSize ) {
  CChartEntryBaseWithTime::Reserve( nSize );
}

void CChartEntryIndicator::AddDataToChart(XYChart *pXY) {
  if ( 0 != this->m_vDateTime.size() ) {
    LineLayer *ll = pXY->addLineLayer( this->GetPrice() );
    // LineLayer *lltrade = xy->addLineLayer( m_pModel->Asks()->GetPrice(), Green, _T( "Trade" ) );
    ll->setXData( this->GetDateTime() );
  }
}
