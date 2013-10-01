/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

// Started 2013/10/01

#pragma once

#include "TimeSeries.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

template<typename TS> // TS -> TimeSeries
class PriceAdapter: public Prices {
public:
  PriceAdapter( TS& ts ) : m_ts( ts ) { 
    m_ts.OnAppend.Add( MakeDelegate( this, &PriceAdapter::HandleUpdate ) );
  }
  ~PriceAdapter( void ) {
    m_ts.OnAppend.Remove( MakeDelegate( this, &PriceAdapter::HandleUpdate ) );
  }
protected:
private:
  TS& m_ts;
  void HandleUpdate( const typename TS::datum_t& datum ) {
    Prices::Append( Price( datum.DateTime(), GetPrice( datum ) ) );
  }
  double GetPrice( const Quote& quote ) const { return quote.Midpoint(); };
  double GetPrice( const Trade& trade ) const { return trade.Price(); };
};


} // namespace tf
} // namespace ou
