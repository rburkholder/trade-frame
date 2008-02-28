#include "StdAfx.h"
#include "math.h"
#include "RunningStats.h"

CRunningStats::CRunningStats(void) {
  b2 = b1 = b0 = 0;
  SumXX = SumX = SumXY = SumY = SumYY = 0;
  nX = 0;
  BBMultiplier = 2.0;
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

    double oldb1 = b1;

    Sxx = SumXX - SumX * SumX / nX;
    Sxy = SumXY - SumX * SumY / nX;
    Syy = SumYY - SumY * SumY / nX;

    SST = Syy;
    SSR = Sxy * Sxy / Sxx;
    SSE = SST - SSR;

    RR = SSR / SST;
    R = Sxy / sqrt(Sxx * Syy);

    //SD = Math.Sqrt(Syy / (Xcnt - 1));
    SD = sqrt(Syy / nX);

    meanY = SumY / nX;

    double BBOffset = BBMultiplier * SD;
    BBUpper = meanY + BBOffset;
    BBLower = meanY - BBOffset;

    b1 = ( nX > 1 ) ? Sxy / Sxx : 0;
    b0 = (1 / nX) * (SumY - b1 * SumX);
    b2 = b1 - oldb1;  // *** do this differently
  }
}

