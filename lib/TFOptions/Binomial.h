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

// Started 2013/07/15

// Page 288 Option Pricing Formulas, 2e

#pragma once

#include <cassert>

#include <TFTrading/TradingEnumerations.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options
namespace binomial { // binomial

struct structInput {
  ou::tf::OptionSide::EOptionSide optionSide;
  ou::tf::OptionStyle::EOptionStyle optionStyle;
  double S; // price of underlying
  double X; // strike price
  double T; // time to expiry
  double r; // risk free interest rate
  double b; // make same as r, carry rate
  double v; // volatility
  long n;  // number of time steps
  structInput():
    optionStyle( ou::tf::OptionStyle::American ),
    optionSide( ou::tf::OptionSide::Unknown ),
    n( 91 ), // binomial steps
    S( 0.0 ), X( 0.0 ), r( 0.0 ), b( 0.0 ), v( 0.0 ),
    T( 0.0 ) {}
  void Check() {
    assert( 0.0 != v );
    assert( ou::tf::OptionSide::Unknown != optionSide );
    assert( 0.0 != S );
    assert( 0.0 != T );
    assert( 0.0 != r );
    assert( 0.0 != b );
  }
};

struct structOutput {
  double option;
  double iv;
  double delta;
  double gamma;
  double theta;
  double vega;
  double rho;
  structOutput( void ) : option( 0 ), iv( 0 ), delta( 0 ), gamma( 0 ), theta( 0 ), vega( 0 ), rho( 0 ) {};
};

// Cox Ross Rubinstein American Binomial Tree
// pg 284 Option Pricing Formulas, 2e
void CRR( const structInput& input, structOutput& output );
double CalcImpliedVolatility( const structInput& input, double option, structOutput& output, double epsilon = 0.0001 );

} // namespace binomial
} // namespace option
} // namespace tf
} // namespace ou

