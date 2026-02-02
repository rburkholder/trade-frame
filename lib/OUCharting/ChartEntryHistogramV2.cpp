/************************************************************************
 * Copyright(c) 2026, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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

/*
 * File:    ChartEntryHistogramV2.cpp
 * Author:  raymond@burkholder.net
 * Project: OUCharting
 * Created: February 1, 2026 13:14:16
 */

#include "ChartEntryHistogramV2.hpp"

namespace ou { // One Unified

ChartEntryHistogram_v2::ChartEntryHistogram_v2()
: ChartEntryTime()
, m_ixStart {}, m_cntElement {}
{}

ChartEntryHistogram_v2::ChartEntryHistogram_v2( ChartEntryHistogram_v2&& rhs )
: ChartEntryTime( std::move( rhs ) )
, m_queue( std::move( rhs.m_queue ) )
, m_vap_max( rhs.m_vap_max )
, m_ixStart( rhs.m_ixStart )
, m_cntElement( rhs.m_cntElement )
{
  assert( 0 == m_vVolumeAtPrice_raw.size() );
  assert( 0 == m_mapVolumeAtPrice.size() );
}

ChartEntryHistogram_v2::~ChartEntryHistogram_v2() {}

void ChartEntryHistogram_v2::Append( const Datum& datum ) {
  m_queue.Append( datum );
}

void ChartEntryHistogram_v2::Pop( const Datum& datum ) {

  const bool direction( datum.bDirection );
  //const auto price( std::round( q.trade.Price() * 100.0) / 100.0 );
  const auto price( datum.dblPrice );
  const double volume( datum.dblVolume ); // convert from volume_t to double
  const auto dt( datum.dt  );

  ChartEntryTime::AppendFg( dt );

  if ( direction ) {
    VolumeAtPrice_t v( 0.0, volume, price );
    //if ( m_vap_max.atAsk < v.atAsk ) m_volumes_max.atAsk = v.atAsk;
    m_vVolumeAtPrice_raw.push_back( v );
  }
  else {
    VolumeAtPrice_t v( volume, 0.0, price );
    //if ( m_volumes_max.atBid < v.atBid ) m_volumes_max.atBid = v.atBid;
    m_vVolumeAtPrice_raw.push_back( v );
  }
}

void ChartEntryHistogram_v2::ClearQueue() {
  ChartEntryTime::ClearQueue();
  namespace ph = std::placeholders;
  m_queue.Sync( std::bind( &ChartEntryHistogram_v2::Pop, this, ph::_1 ) );
}

bool ChartEntryHistogram_v2::AddEntryToChart( XYChart* pXY, structChartAttributes& attributes ) {

  bool bAdded( false );
  ClearQueue();

  attributes.fPostLayout =
    [this,pXY](){
      //DrawArea* da = pXY->getDrawArea();
      //auto da_h = da->getHeight();
      //auto da_w = da->getWidth();

      static const bool bAggregate( true );

      PlotArea* pa = pXY->getPlotArea();

      const auto pa_lx = pa->getLeftX();
      const auto real_lx = pXY->getXValue( pa_lx );

      const auto pa_rx = pa->getRightX();
      const auto real_rx = pXY->getXValue( pa_rx );

      const auto diff_x( real_rx - real_lx );

      const auto pa_ty = pa->getTopY();
      const auto price_hi = pXY->getYValue( pa_ty );

      const auto pa_ly = pa->getBottomY();
      const auto price_lo = pXY->getYValue( pa_ly );

      const size_t ixStart( IxStart() );
      size_t cntElement( CntElements() );

      if ( ( ixStart != m_ixStart ) || ( cntElement != m_cntElement ) ) {

        m_ixStart = ixStart;
        m_cntElement = cntElement;

        vVolumeAtPrice_t::const_iterator itervvap( &m_vVolumeAtPrice_raw[ ixStart ] );

        m_vap_max = VolumeAtPrice_t();

        m_mapVolumeAtPrice.clear();

        while ( 0 != cntElement ) {

          const VolumeAtPrice_t& vvap( *itervvap );

          mapVolumeAtPrice_t::iterator itermvap = m_mapVolumeAtPrice.find( vvap.price );
          if ( m_mapVolumeAtPrice.end() == itermvap ) {
            bool bResult;
            std::tie( itermvap, bResult ) = m_mapVolumeAtPrice.emplace( vvap.price, VolumeAtPrice_t( vvap.price ) );
            assert( bResult);
          }

          VolumeAtPrice_t& mvap( itermvap->second );
          mvap += vvap;
          m_vap_max.max( mvap );

          ++itervvap;
          --cntElement;
        }

      }

      for ( const mapVolumeAtPrice_t::value_type& map: m_mapVolumeAtPrice ) {
        const VolumeAtPrice_t& vap( map.second );
        if ( bAggregate ) {
          const double offset = 0.75 * diff_x * vap.sumVolume / m_vap_max.sumVolume;
          const auto offset_x = pXY->getXCoor( real_lx + offset );
          if ( pa_lx != offset_x ) {
            auto y = pXY->getYCoor( vap.price );
            Line* line = pXY->addLine( pa_lx, y, offset_x, y );
            line->setColor( ou::Colour::Green );
          }
        }
        else {
          {
            const double offset_ask = 0.75 * diff_x * vap.atAsk / m_vap_max.sumVolume;
            const auto offset_x_ask = pXY->getXCoor( real_lx + offset_ask );
            if ( pa_lx != offset_x_ask ) {
              auto y = pXY->getYCoor( vap.price + 0.003 );
              Line* line = pXY->addLine( pa_lx, y, offset_x_ask, y );
              line->setColor( ou::Colour::Red );
            }
          }
          {
            const double offset_bid = 0.75 * diff_x * vap.atBid / m_vap_max.sumVolume;
            const auto offset_x_bid = pXY->getXCoor( real_lx + offset_bid );
            if ( pa_lx != offset_x_bid ) {
              auto y = pXY->getYCoor( vap.price - 0.003 );
              Line* line = pXY->addLine( pa_lx, y, offset_x_bid, y );
              line->setColor( ou::Colour::Blue );
            }
          }
        }
      }

    };
  bAdded = true;
  return bAdded;
}

void ChartEntryHistogram_v2::Clear() {
  m_queue.Clear();
  //ChartEntryTime::Clear();
}

} // namespace ou