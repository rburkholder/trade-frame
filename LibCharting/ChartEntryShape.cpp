#include "StdAfx.h"
#include "ChartEntryShape.h"

int CChartEntryShape::m_rShapes[] = { 
  Chart::TriangleShape, Chart::InvertedTriangleShape, 
  Chart::Polygon2Shape( 6 ), Chart::Polygon2Shape( 6 ),
  Chart::Polygon2Shape( 5 ), Chart::PolygonShape( 5 ) };


CChartEntryShape::CChartEntryShape( EShape eShape, Colour::enumColour colour ) 
: CChartEntryBaseWithTime(), m_eShape( eShape ), m_colour( colour )
{
}

CChartEntryShape::~CChartEntryShape(void) {
}

void CChartEntryShape::AddLabel(const ptime &dt, double price, const std::string &sText ) {
  CChartEntryBaseWithTime::Add( dt, price );
  m_vLabel.push_back( sText );
  const std::string &s = m_vLabel.back();
  m_vpChar.push_back( s.c_str() );
}

void CChartEntryShape::AddDataToChart(XYChart *pXY) {
  if ( 0 < m_vPrice.size() ) {
    ScatterLayer *layer 
      = pXY->addScatterLayer( 
        GetPrice(), GetDateTime(), NULL, m_rShapes[ m_eShape ], 5, m_colour, m_colour );
    layer->addExtraField( GetLabels() );
    layer->setDataLabelFormat("{field0}");
    TextBox *textbox = layer->setDataLabelStyle("arialbd.ttf", 8);
    //textbox->setBackground(0xcc99ff, Chart::Transparent, 1);
    textbox->setAlignment(Chart::Left);
    textbox->setPos(4, 0);
  }
}
