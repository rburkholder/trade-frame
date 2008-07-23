#include "StdAfx.h"
#include "ChartEntryMark.h"

CChartEntryMark::CChartEntryMark(void) 
: CChartEntryBase()
{
}

CChartEntryMark::~CChartEntryMark(void) {
}

void CChartEntryMark::AddMark(double price, Colour::enumColour colour, const std::string &name) {
  m_vPrice.push_back( price );
  m_vColour.push_back( colour );
  m_vName.push_back( name );
}

void CChartEntryMark::AddDataToChart( XYChart *pXY ) {
  if ( 0 < m_vPrice.size() ) {
    // may need to make an adjustment for using only marks within a certain price range
    for ( size_t ix = 0; ix < m_vPrice.size(); ++ix ) {
      pXY->yAxis()->addMark( m_vPrice[ ix ], m_vColour[ ix ], m_vName[ ix ].c_str() );
    }
  }
  
}
