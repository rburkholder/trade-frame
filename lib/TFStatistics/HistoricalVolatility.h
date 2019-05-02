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
 * File:   HistoricalVolatility.h
 * Author: rpb
 *
 * Created on May 1, 2019, 10:07 PM
 */

#ifndef HISTORICALVOLATILITY_H
#define HISTORICALVOLATILITY_H

// aka std dev?

#include <vector>

#include <TFTimeSeries/DatedDatum.h>

namespace ou {

class HistoricalVolatility { // may need to normalize to yearly historical volatility
public:
  HistoricalVolatility();
  void operator()( const ou::tf::Bar& bar );
  operator double();
protected:
private:
  bool bFirstFound;
  int nPrices;
  double dblPrevious;
  double dblSumNatLogReturns;
  std::vector<double> vReturns;
};

} // namespace ou

#endif /* HISTORICALVOLATILITY_H */

