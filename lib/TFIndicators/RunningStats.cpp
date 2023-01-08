/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

#include <cmath>

#include "RunningStats.h"

// TODO: incorporate:
// https://stats.stackexchange.com/questions/111851/standard-deviation-of-an-exponentially-weighted-mean
// http://people.ds.cam.ac.uk/fanf2/hermes/doc/antiforgery/stats.pdf - no longer available
// https://fanf2.user.srcf.net/hermes/doc/antiforgery/stats.pdf - available here
// implemented in BasketTrading::ManageStrategy - to be refactored

namespace ou { // One Unified
namespace tf { // TradeFrame

RunningStats::RunningStats()
: m_SumXX {}, m_SumX {}, m_SumXY {}, m_SumY {}, m_SumYY {}
, m_nX( 0 ), m_BBMultiplier( 2.0 )
{}

RunningStats::RunningStats( double BBMultiplier )
: m_SumXX {}, m_SumX {}, m_SumXY {}, m_SumY {}, m_SumYY {}
, m_nX{}, m_BBMultiplier( BBMultiplier )
{}

RunningStats::RunningStats( const RunningStats& rhs )
:
  m_stats( rhs.m_stats )
, m_SumXX( rhs.m_SumXX ), m_SumX( rhs.m_SumX )
, m_SumYY( rhs.m_SumYY ), m_SumY( rhs.m_SumY )
, m_SumXY( rhs.m_SumXY )
, m_nX( rhs.m_nX )
, m_BBMultiplier( rhs.m_BBMultiplier )
{}


RunningStats::RunningStats( const RunningStats&& rhs )
:
  m_stats( rhs.m_stats )
, m_SumXX( rhs.m_SumXX ), m_SumX( rhs.m_SumX )
, m_SumYY( rhs.m_SumYY ), m_SumY( rhs.m_SumY )
, m_SumXY( rhs.m_SumXY )
, m_nX( rhs.m_nX )
, m_BBMultiplier( rhs.m_BBMultiplier )
{}


RunningStats::~RunningStats() {
}

void RunningStats::Reset() {
  /*b2 = */
    m_stats.Reset();
    m_nX = 0;
    m_SumXX = m_SumX = m_SumXY = m_SumY = m_SumYY = 0.0;
}

void RunningStats::Add( double x, double y ) {
  m_SumXX += x * x;
  m_SumX += x;
  m_SumXY += x * y;
  m_SumY += y;
  m_SumYY += y * y;
  m_nX++;
}

void RunningStats::Remove( double x, double y ) {
  m_SumXX -= x * x;
  m_SumX -= x;
  m_SumXY -= x * y;
  m_SumY -= y;
  m_SumYY -= y * y;
  m_nX--;
}

void RunningStats::CalcStats() {
  CalcStats( m_stats );
}

void RunningStats::CalcStats( Stats& stats ) {

  if ( 0 == m_nX ) {
    m_SumXX = m_SumX = 0;
    m_SumXY = 0;
    m_SumY = m_SumYY = 0;
    stats.Reset();
  }
  else {

    const double nX( m_nX );

    const double Sxx = m_SumXX - ( m_SumX * m_SumX ) / nX;
    const double Sxy = m_SumXY - ( m_SumX * m_SumY ) / nX;
    const double Syy = m_SumYY - ( m_SumY * m_SumY ) / nX;

    const double SST = Syy;
    const double SSR = ( Sxy * Sxy ) / Sxx;
    const double SSE = SST - SSR;

    stats.rr = SSR / SST;
    stats.r = Sxy / std::sqrt( Sxx * Syy );

    stats.sd = std::sqrt( Syy / nX );

    stats.meanY = m_SumY / nX;

//    double BBOffset = m_BBMultiplier * sd;
//    bbUpper = meanY + BBOffset;
//    bbLower = meanY - BBOffset;

    stats.b1 = ( nX > 1 ) ? Sxy / Sxx : 0.0;
    stats.b0 = ( m_SumY - stats.b1 * m_SumX ) / nX;
//    b2 = b1 - oldb1;  // TODO: do this differently
  }
}

} // namespace tf
} // namespace ou
