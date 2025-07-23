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

//#include <math.h>

#include <cmath>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <algorithm>
#include <stdexcept>

#include <boost/lexical_cast.hpp>

#include "Binomial.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options
namespace binomial { // binomial

void CRR( const structInput& input, structOutput& output ) {

  std::vector<double> v; v.resize( input.n + 1 );
  double u, d, p;
  double dt;
  double df;
  double z;

  switch ( input.optionSide ) {
  case ou::tf::OptionSide::Call:
    z = 1;
    break;
  case ou::tf::OptionSide::Put:
    z = -1;
    break;
  }

  dt = input.T / input.n;
  u = exp( input.v * sqrt( dt ) );
  d = 1.0 / u;
  p = ( exp( input.b * dt ) - d ) / ( u - d );
  df = exp( -input.r * dt );

  for ( int ix = 0; ix <= input.n; ++ix ) {
    v[ ix ] = std::max<double>( 0.0, z * ( input.S * pow( u, ix ) * pow( d, input.n - ix ) - input.X ) );
  }
  for ( int j = input.n - 1; j >= 0; --j ) {
    for ( int i = 0; i <= j; ++i ) {
      double europrice = df * ( p * v[ i + 1 ] + ( 1.0 - p ) * v[ i ] );
      double exerciseprice;
      switch ( input.optionStyle ) {
      case ou::tf::OptionStyle::American:
        exerciseprice = z * ( input.S * pow( u, i ) * pow( d, j - i ) - input.X );
        v[ i ] = std::max<double>( exerciseprice, europrice );
        break;
      case ou::tf::OptionStyle::European:
        v[ i ] = europrice;
        break;
      }
      if ( 2 == j ) {
        output.gamma = ( ( v[ 2 ] - v[ 1 ] ) / ( input.S * u * u - input.S )
          - ( v[ 1 ] - v[ 0 ] ) / ( input.S - input.S * d * d ) )
          / ( 0.5 * ( input.S * u * u - input.S * d * d ) );
        output.theta = v[ 1 ];
      }
      if ( 1 == j ) {
        output.delta = ( v[ 1 ] - v[ 0 ] ) / ( input.S * ( u - d ) );
      }
    }
  }
  output.theta = ( output.theta - v[ 0 ] ) / ( 2.0 * dt ) / 365.0;
  output.option = v[ 0 ];
}

double CalcImpliedVolatility( const structInput& input_, double option, structOutput& output, double epsilon ) {
  // Black Scholes and Beyond, page 336  -- not sure if this is correct model used.  I didn't document model used
  // Option Pricing Formulas, page 453  -- or might have been this one
  // New vega portion taken from top of page 288 (Option Pricing Formulas) ,
  // and uses the 1% description from pg 166 of Black Scholes and Beyond

  // page 163 - 164 provides vega calc
  // page 337 provides newton-raphson method of iv calc
  size_t cnt = 12;
  structInput input( input_ );  // copy rather than reference to keep local copy of parameters

  ou::tf::option::binomial::CRR( input, output );
  double option1 = output.option;

  //std::cout << "CRRp basic: P=" << output.option << ",D=" << output.delta << ",G=" << output.gamma << ",T=" << output.theta << std::endl;

  static double pct = 0.01;  // 1% change in volatility

  double diff = 2 * epsilon; // set > epsilon
  while ( epsilon < diff ) { // epsilon

    double vol = input.v;
    double deltaVol = pct * vol;  // do we use pct * vol or just pct?
    double volInput1 = input.v = vol + deltaVol;  // adjust by 1% to calc vega

    ou::tf::option::binomial::CRR( input, output );
    double option2 = output.option;

    output.vega = ( option2 - option1 ) / ( deltaVol );
    double volInput2 = output.iv = input.v = vol - ( ( option1 - option ) / output.vega ); // new volatility value

    ou::tf::option::binomial::CRR( input, output );  // calc new option values with new IV
    option1 = output.option;  // keep for next go around if needed
    diff = std::fabs( (double) ( output.option - option ) );

    output.vega = ( ( output.option - option2 ) / ( volInput2 - volInput1 ) ) * 0.01;  // see if this works, if so then can remove one CRR calc below (not sure why need the 1/100 factor (maybe to undo pct variable)

    --cnt;
    if ( 0 == cnt ) {
      const std::string sError(
        "IVp in CRR: "
        + boost::lexical_cast<std::string>( epsilon)
        + "," + boost::lexical_cast<std::string>( diff )
      );
      throw std::runtime_error( sError );
    }
  }

//  std::cout << "IV1=" << output.iv << ",O=" << output.option << ",D=" << output.delta << ",G=" << output.gamma << ",T=" << output.theta << ",V=" << output.vega << "," << cnt << std::endl;

  // calculate CRR deltaVol to obtain new vega, as current calculated vega is for iteration n-1.
  // close enough so don't bother
  structOutput outputTmp;
//  double vol = input.v;  // keep old value
//  input.v += pct * vol;  // add a delta
//  ou::tf::option::binomial::CRR( input, outputTmp );
//  output.vega = ( outputTmp.option - output.option ) / ( pct * vol );

//  std::cout << "IV2=" << output.iv << ",O=" << output.option << ",D=" << output.delta << ",G=" << output.gamma << ",T=" << output.theta << ",V=" << output.vega << "," << cnt << std::endl;

  // need one more calc to do rho.  formulas on page 313 of black scholes and beyond
//  input.v = vol;  // reset vol
  double r = input.r; // keep old r
  input.r += pct * r;  // add a delta
  ou::tf::option::binomial::CRR( input, outputTmp );
  output.rho = ( outputTmp.option - output.option ) / ( pct * r );

//  std::cout << "IV3=" << output.iv << ",O=" << output.option << ",D=" << output.delta << ",G=" << output.gamma << ",T=" << output.theta << ",V=" << output.vega << "," << output.rho << "," << cnt << std::endl;

  return output.iv;
}

} // namespace binomial
} // namespace option
} // namespace tf
} // namespace ou

