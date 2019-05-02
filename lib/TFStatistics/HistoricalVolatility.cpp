/************************************************************************
 * Copyright(c) 2019, One Unified. All rights reserved.                 *
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
 * File:   HistoricalVolatility.cpp
 * Author: rpb
 * 
 * Created on May 1, 2019, 10:07 PM
 */

#include "HistoricalVolatility.h"

namespace ou {

HistoricalVolatility::HistoricalVolatility()
: bFirstFound( false ), dblPrevious {}, nPrices {}, dblSumNatLogReturns {}
{}

void HistoricalVolatility::operator()( const ou::tf::Bar& bar ) {
  if ( bFirstFound ) {
    double dblPrice = bar.Close();
    double dblReturn = std::log( dblPrice / dblPrevious );
    dblPrevious = dblPrice;
    dblSumNatLogReturns += dblReturn;
    vReturns.push_back( dblReturn );
  }
  else {
    dblPrevious = bar.Close();
    bFirstFound = true;
  }
  nPrices++;
}

 HistoricalVolatility::operator double() {
  double dblSums {};
  double dblAverage = dblSumNatLogReturns / nPrices;
  std::for_each(
    vReturns.begin(), vReturns.end(),
    [&dblSums,dblAverage](double dblReturn){
      double dblDiff = dblReturn - dblAverage;
      dblSums += dblDiff * dblDiff;
    });
  double dblVariance = dblSums / ( nPrices - 1 );
  return std::sqrt( dblVariance );
}

} // namespace ou
