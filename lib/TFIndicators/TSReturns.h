/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

#pragma once

#include <TFTimeSeries/DatedDatum.h>
#include <TFTimeSeries/TimeSeries.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

// create a new time series from the updates of another time series
// need to use the OnAppend delegate
// change sometime, so intermediary handles the Price calc, and only Price is passed in

class TSReturns: public Prices {
public:

  typedef Prices::size_type size_type;
  typedef Price::price_t price_t;

  TSReturns( void );
  TSReturns( size_type );
  virtual ~TSReturns(void);

  void Append( const Bar& bar );
  void Append( const Quote& quote );
  void Append( const Trade& trade );
  void Append( const Price& price );

protected:
private:

  bool m_bFirstAppend;
  price_t m_priceLast;

};

} // namespace tf
} // namespace ou
