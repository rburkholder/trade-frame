/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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
 * File:    ChartEntryHistogram.cpp
 * Author:  raymond@burkholder.net
 * Project: SP500
 * Created: October 25, 2025 09:44:03
 */

#include "ChartEntryHistogram.hpp"

namespace ou { // One Unified

ChartEntryHistogram::ChartEntryHistogram()
: ChartEntryTime()
{}

ChartEntryHistogram::ChartEntryHistogram( ChartEntryHistogram&& rhs )
: ChartEntryTime( std::move( rhs ) )
, m_queue( std::move( rhs.m_queue ) )
, m_mapVolumeAtPrice( std::move( rhs.m_mapVolumeAtPrice ) )
, m_volumes_max( rhs.m_volumes_max )
{
}

ChartEntryHistogram::~ChartEntryHistogram() {}

void ChartEntryHistogram::Add( bool direction, const ou::tf::Trade& trade ) {
  m_queue.Append( queued_trade_t( direction, trade ) );
}

/*
  rounding to arbitrary digit count, not very fast
  double round_up(double value, int decimal_places) {
    const double multiplier = std::pow(10.0, decimal_places);
    return std::ceil(value * multiplier) / multiplier;
  }
*/

void ChartEntryHistogram::Pop( const queued_trade_t& q ) {

  const bool direction( q.bDirection );
  const auto price( std::round( q.trade.Price() * 100.0) / 100.0 );
  const auto volume( q.trade.Volume() );

  mapVolumeAtPrice_t::iterator iterVolumeAtPrice = m_mapVolumeAtPrice.find( price );
  if ( m_mapVolumeAtPrice.end() == iterVolumeAtPrice ) {
    bool bResult;
    std::tie( iterVolumeAtPrice, bResult ) = m_mapVolumeAtPrice.emplace( price, volumes_t() );
    assert( bResult);
  }

  volumes_t& v( iterVolumeAtPrice->second );

  if ( direction ) {
    v.at_ask += volume;
    if ( m_volumes_max.at_ask < v.at_ask ) m_volumes_max.at_ask = v.at_ask;
  }
  else {
    v.at_bid += volume;
    if ( m_volumes_max.at_bid < v.at_bid ) m_volumes_max.at_bid = v.at_bid;
  }
  const auto sum( v.at_ask + v.at_bid );
}

void ChartEntryHistogram::ClearQueue() {
  ChartEntryTime::ClearQueue();
  namespace ph = std::placeholders;
  m_queue.Sync( std::bind( &ChartEntryHistogram::Pop, this, ph::_1 ) );
}

bool ChartEntryHistogram::AddEntryToChart( XYChart* pXY, structChartAttributes& attributes ) {

  bool bAdded( false );
  ClearQueue();

  // ToDo:
  //   stage 1: generic historgram of full time range
  //   stage 2: histogram of prices within visible chart

  attributes.fPostLayout =
    [this,pXY](){
      //DrawArea* da = pXY->getDrawArea();
      //auto da_h = da->getHeight();
      //auto da_w = da->getWidth();

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

      const auto iterB = m_mapVolumeAtPrice.lower_bound( price_lo );
      const auto iterE = m_mapVolumeAtPrice.lower_bound( price_hi );

      struct volume_at_price {
        double volume;
        double volume_ask;
        double volume_bid;
        double price;

        volume_at_price( double volume_, double price_ )
        : volume( volume_ ), price( price_ )
        , volume_bid {}, volume_ask {}
        {}

        volume_at_price( double vol_bid, double vol_ask, double price_ )
        : volume_bid( vol_bid ), volume_ask( vol_ask ), price( price_ )
        , volume {}
        {}
      };

      std::vector<volume_at_price> vVAP;

      double volume_max {};

      static const bool bAggregate( true );

      for ( auto iter = iterB; iter != iterE; iter++ ) {
        const auto& [ key, value ] = *iter;
        const auto y = pXY->getYCoor( key );
        if ( bAggregate ) {
          const double volumes = (double) ( value.at_ask + value.at_bid );
          if ( volume_max < volumes ) volume_max = volumes;
          vVAP.push_back( volume_at_price( volumes, key ) );
        }
        else {
          const double ask( value.at_ask );
          if ( volume_max < ask ) volume_max = ask;
          const double bid( value.at_bid );
          if ( volume_max < bid ) volume_max = bid;
          vVAP.push_back( volume_at_price( bid, ask, key ) );
        }
      }

      for ( const volume_at_price& vap: vVAP ) {
        if ( bAggregate ) {
          const double offset = 0.75 * diff_x * vap.volume / volume_max;
          const auto offset_x = pXY->getXCoor( real_lx + offset );
          if ( pa_lx != offset_x ) {
            auto y = pXY->getYCoor( vap.price );
            Line* line = pXY->addLine( pa_lx, y, offset_x, y );
            line->setColor( ou::Colour::Green );
          }
        }
        else {
          {
            const double offset_ask = 0.75 * diff_x * vap.volume_ask / volume_max;
            const auto offset_x_ask = pXY->getXCoor( real_lx + offset_ask );
            if ( pa_lx != offset_x_ask ) {
              auto y = pXY->getYCoor( vap.price + 0.003 );
              Line* line = pXY->addLine( pa_lx, y, offset_x_ask, y );
              line->setColor( ou::Colour::Red );
            }
          }
          {
            const double offset_bid = 0.75 * diff_x * vap.volume_bid / volume_max;
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

void ChartEntryHistogram::Clear() {
  m_queue.Clear();
  m_mapVolumeAtPrice.clear();
  //ChartEntryTime::Clear();
}

} // namespace ou