/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

#include <boost/phoenix/core.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>

#include "ChartEntryShape.h"

namespace ou { // One Unified

int ChartEntryShape::m_rShapes[] = {
  Chart::DiamondShape,
  Chart::TriangleShape, Chart::InvertedTriangleShape,
  Chart::PolygonShape( 5 ), Chart::PolygonShape( 5 ),
  Chart::Polygon2Shape( 6 ), Chart::Polygon2Shape( 6 )
};

ChartEntryShape::ChartEntryShape()
: ChartEntryPrice(), m_eShape( EShape::Default )
{
}

ChartEntryShape::ChartEntryShape( EShape eShape, ou::Colour::EColour colour )
: ChartEntryPrice(), m_eShape( eShape )
{
  ChartEntryBase::SetColour( colour );
}

// destructor needs to clear out queue first
ChartEntryShape::~ChartEntryShape() {
  // run ClearQueue() instead?
  //if ( !m_vpChar.empty() ) {
  //  for ( vpChar_t::iterator iter = m_vpChar.begin(); m_vpChar.end() != iter; ++iter ) {
  //    delete [] *iter;
  //  }
  //}
  ClearQueue(); // redundant?
}

// in background thread
void ChartEntryShape::AddLabel(const boost::posix_time::ptime &dt, double price, const std::string &sText ) {
  char *pszLabel = new char[ sText.size() + 1 ];
  strcpy( pszLabel, sText.c_str() );
  m_queueLabel.Append( Entry( ou::tf::Price( dt, price ), pszLabel ) );

  //ChartEntryPrice::Append( dt, price );
  //m_vLabel.push_back( sText );
  //const std::string &s = m_vLabel.back();
  //if ( m_bUseThreadSafety ) {
  //  while ( !m_lfShape.push( pszLabel ) ) {};
  //}
  //else {
  //  m_vpChar.push_back( pszLabel );
  //}
}

void ChartEntryShape::ClearQueue() {
  namespace args = boost::phoenix::placeholders;
  m_queueLabel.Sync( boost::phoenix::bind( &ChartEntryShape::Pop, this, args::arg1 ) );
}

void ChartEntryShape::Pop( const Entry& entry ) {
  ChartEntryPrice::Pop( entry.price );
  m_vpChar.push_back( entry.pLabel );
}

bool ChartEntryShape::AddEntryToChart( XYChart* pXY, structChartAttributes& attributes ) {

  bool bAdded( false );

  ClearQueue();

  if ( 0 < ChartEntryPrice::Size() ) {
    const DoubleArray daXData = ChartEntryPrice::GetViewPortDateTimes();
    if ( 0 != daXData.len ) {
      ScatterLayer *layer
        = pXY->addScatterLayer(
          daXData, GetPrices(), NULL, m_rShapes[ (int)m_eShape ], 15, m_eColour, m_eColour );

      layer->setXData( daXData );
      attributes.dblXMin = daXData[0];
      attributes.dblXMax = daXData[ daXData.len - 1 ];

      layer->addExtraField( GetLabels() );
      layer->setDataLabelFormat("{field0}");
      TextBox *textbox = layer->setDataLabelStyle("arialbd.ttf", 8);
      //textbox->setBackground(0xcc99ff, Chart::Transparent, 1);
      textbox->setAlignment(Chart::Left);
      //textbox->setPos(4, 0);
      bAdded = true;
    }
  }
  return bAdded;
}

void ChartEntryShape::Clear() {
  // run ClearQueue() instead?
  if ( !m_vpChar.empty() ) {
    for ( vpChar_t::iterator iter = m_vpChar.begin(); m_vpChar.end() != iter; ++iter ) {
      delete [] *iter;
    }
  }
  //ChartEntryPrice::Clear();
}

} // namespace ou
