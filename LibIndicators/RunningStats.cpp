#include "StdAfx.h"

#include "math.h"
#include "RunningStats.h"

CRunningStats::CRunningStats(void) : 
  b2( 0 ), b1( 0 ), b0( 0 ), 
  SumXX( 0 ), SumX( 0 ), SumXY( 0 ), SumY( 0 ), SumYY( 0 ),
  nX( 0 ), m_BBMultiplier( 2.0 )
{
}

CRunningStats::CRunningStats( double BBMultiplier ) : 
  b2( 0 ), b1( 0 ), b0( 0 ), 
  SumXX( 0 ), SumX( 0 ), SumXY( 0 ), SumY( 0 ), SumYY( 0 ),
  nX( 0 ), m_BBMultiplier( BBMultiplier )
{
}

CRunningStats::~CRunningStats(void) {
}

void CRunningStats::Add(double x, double y) {
  SumXX += x * x;
  SumX += x;
  SumXY += x * y;
  SumY += y;
  SumYY += y * y;
  nX++;
}

void CRunningStats::Remove(double x, double y) {
  SumXX -= x * x;
  SumX -= x;
  SumXY -= x * y;
  SumY -= y;
  SumYY -= y * y;
  nX--;
}

void CRunningStats::CalcStats() {

  if ( nX > 1 ) {

    double Sxx, Sxy, Syy;
    double SST, SSR, SSE;

    double oldb1 = b1;

    Sxx = SumXX - ( SumX * SumX ) / nX;
    Sxy = SumXY - ( SumX * SumY ) / nX;
    Syy = SumYY - ( SumY * SumY ) / nX;

    SST = Syy;
    SSR = ( Sxy * Sxy ) / Sxx;
    SSE = SST - SSR;

    RR = SSR / SST;
    R = Sxy / sqrt(Sxx * Syy);

    SD = sqrt(Syy / nX);

    meanY = SumY / nX;

    double BBOffset = m_BBMultiplier * SD;
    BBUpper = meanY + BBOffset;
    BBLower = meanY - BBOffset;

    b1 = ( nX > 1 ) ? Sxy / Sxx : 0;
    b0 = (1 / nX) * ( SumY - b1 * SumX );
    b2 = b1 - oldb1;  // *** do this differently
  }
}

