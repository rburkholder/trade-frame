#include "StdAfx.h"
#include "ChartEntryShape.h"

int CChartEntryShape::m_rShapes[] = { 
  Chart::DiamondShape,
  Chart::TriangleShape, Chart::InvertedTriangleShape, 
  Chart::PolygonShape( 5 ), Chart::PolygonShape( 5 ), 
  Chart::Polygon2Shape( 6 ), Chart::Polygon2Shape( 6 ) 
};

CChartEntryShape::CChartEntryShape( void )
: CChartEntryBaseWithTime(), m_eShape( EDefault )
{
}

CChartEntryShape::CChartEntryShape( enumShape eShape, Colour::enumColour colour ) 
: CChartEntryBaseWithTime(), m_eShape( eShape )
{
  CChartEntryBase::SetColour( colour );
}

CChartEntryShape::~CChartEntryShape(void) {
  if ( !m_vpChar.empty() ) {
    for ( std::vector<const char *>::iterator iter = m_vpChar.begin(); m_vpChar.end() != iter; ++iter ) {
      delete [] *iter;
    }
  }
}

void CChartEntryShape::AddLabel(const ptime &dt, double price, const std::string &sText ) {
  CChartEntryBaseWithTime::Add( dt, price );
  char *pszLabel = new char[ sText.size() + 1 ];
  strcpy( pszLabel, sText.c_str() );
  //m_vLabel.push_back( sText );
  //const std::string &s = m_vLabel.back();
  m_vpChar.push_back( pszLabel );
}

void CChartEntryShape::AddDataToChart(XYChart *pXY) {
  if ( 0 < m_vPrice.size() ) {
    ScatterLayer *layer 
      = pXY->addScatterLayer( 
        GetDateTime(), GetPrice(), NULL, m_rShapes[ m_eShape ], 5, m_eColour, m_eColour );
    layer->setXData( GetDateTime() );
    layer->addExtraField( GetLabels() );
    layer->setDataLabelFormat("{field0}");
    TextBox *textbox = layer->setDataLabelStyle("arialbd.ttf", 8);
    //textbox->setBackground(0xcc99ff, Chart::Transparent, 1);
    textbox->setAlignment(Chart::Left);
    //textbox->setPos(4, 0);
  }
}
