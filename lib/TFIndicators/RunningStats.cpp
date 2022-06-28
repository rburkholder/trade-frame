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

#include "math.h"

#include "RunningStats.h"

// TODO: incorporate:
// https://stats.stackexchange.com/questions/111851/standard-deviation-of-an-exponentially-weighted-mean
// http://people.ds.cam.ac.uk/fanf2/hermes/doc/antiforgery/stats.pdf - no longer available
// https://fanf2.user.srcf.net/hermes/doc/antiforgery/stats.pdf - available here
// implemented in BasketTrading::ManageStrategy - to be refactored

namespace ou { // One Unified
namespace tf { // TradeFrame

RunningStats::RunningStats()
: SumXX( 0 ), SumX( 0 ), SumXY( 0 ), SumY( 0 ), SumYY( 0 )
, nX( 0 ), m_BBMultiplier( 2.0 )
{}

RunningStats::RunningStats( double BBMultiplier )
: SumXX( 0 ), SumX( 0 ), SumXY( 0 ), SumY( 0 ), SumYY( 0 )
, nX( 0 ), m_BBMultiplier( BBMultiplier )
{}

RunningStats::RunningStats( const RunningStats& rhs )
:
  m_stats( rhs.m_stats )
, SumXX( rhs.SumXX ), SumX( rhs.SumX )
, SumYY( rhs.SumYY ), SumY( rhs.SumY )
, SumXY( rhs.SumXY )
, nX( rhs.nX ), nY( rhs.nY )
, m_BBMultiplier( rhs.m_BBMultiplier )
{}


RunningStats::RunningStats( const RunningStats&& rhs )
:
  m_stats( rhs.m_stats )
, SumXX( rhs.SumXX ), SumX( rhs.SumX )
, SumYY( rhs.SumYY ), SumY( rhs.SumY )
, SumXY( rhs.SumXY )
, nX( rhs.nX ), nY( rhs.nY )
, m_BBMultiplier( rhs.m_BBMultiplier )
{}


RunningStats::~RunningStats() {
}

void RunningStats::Reset() {
  /*b2 = */
    m_stats.Reset();
    nX = nY = 0;
    SumXX = SumX = SumXY = SumY = SumYY = 0.0;
}

void RunningStats::Add( double x, double y ) {
  SumXX += x * x;
  SumX += x;
  SumXY += x * y;
  SumY += y;
  SumYY += y * y;
  nX++;
}

void RunningStats::Remove( double x, double y ) {
  SumXX -= x * x;
  SumX -= x;
  SumXY -= x * y;
  SumY -= y;
  SumYY -= y * y;
  nX--;
}

void RunningStats::CalcStats() {

  if ( 0 == nX ) {
    SumXX = SumX = 0;
    SumXY = 0;
    SumY = SumYY = 0;
    m_stats.Reset();
  }
  else {

    const double nX_( nX );

    const double Sxx = SumXX - ( SumX * SumX ) / nX_;
    const double Sxy = SumXY - ( SumX * SumY ) / nX_;
    const double Syy = SumYY - ( SumY * SumY ) / nX_;

    const double SST = Syy;
    const double SSR = ( Sxy * Sxy ) / Sxx;
    const double SSE = SST - SSR;

    m_stats.rr = SSR / SST;
    m_stats.r = Sxy / sqrt( Sxx * Syy );

    m_stats.sd = sqrt( Syy / nX_ );

    m_stats.meanY = SumY / nX_;

//    double BBOffset = m_BBMultiplier * sd;
//    bbUpper = meanY + BBOffset;
//    bbLower = meanY - BBOffset;

    m_stats.b1 = ( nX > 1 ) ? Sxy / Sxx : 0.0;
    m_stats.b0 = ( 1.0 / nX_ ) * ( SumY - m_stats.b1 * SumX );
//    b2 = b1 - oldb1;  // *** do this differently
  }
}

const RunningStats::Stats& RunningStats::CalcStats_v2() {
  CalcStats();
  return m_stats;
}

} // namespace tf
} // namespace ou
