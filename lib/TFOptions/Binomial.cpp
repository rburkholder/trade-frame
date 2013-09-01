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

#include <math.h>
#include <vector>

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

double ImpliedVolatility( const structInput& input_, double option, structOutput& output, double epsilon ) {
  // Black Scholes and Beyond, page 336  -- not sure if this is correct model used.  I didn't document model used
  // Option Pricing Formulas, page 453  -- or might have been this one
  // New vega portion taken from top of page 288 (Option Pricing Formulas) , 
  // and uses the 1% description from pg 166 of Black Scholes and Beyond
  size_t cnt = 10;
  structInput input( input_ );  // copy rather than reference to keep local copy of parameters
//  input.optionSide = ou::tf::OptionSide::Put;  // is 2013/08/31 was set, commented out, should it have been here?
  ou::tf::option::binomial::CRR( input, output );
//  std::cout << "CRRp basic: P=" << output.option << ",D=" << output.delta << ",G=" << output.gamma << ",T=" << output.theta << std::endl;
  double diff = abs( output.option - option );
  // may be able to speed algorithm by doing a +0.1 or a -0.1 rather than both.
  while ( epsilon < diff ) { // epsilon
    double deltaVol = 0.01;
    double vol = input.v;
    input.v = vol + deltaVol;
    ou::tf::option::binomial::CRR( input, output );
    double option1 = output.option;
    input.v = vol - deltaVol;
    ou::tf::option::binomial::CRR( input, output );
    double option2 = output.option;
    output.vega = ( option1 - option2 ) / ( 2 * deltaVol );
    output.iv = input.v = vol - ( diff / output.vega );  
    ou::tf::option::binomial::CRR( input, output );
    diff = abs( output.option - option );
    --cnt;
    if ( 0 == cnt ) {
//      std::cout << "problems with IVp in CRR" << std::endl;
      throw std::runtime_error( "problems with IVp in CRR" );
      break;
    }
  }
//  std::cout << "CRR IVp: " << input.v << ",P=" << output.option << ",D=" << output.delta << ",G=" << output.gamma << ",T=" << output.theta << "," << cnt << std::endl;
  return output.iv;
}

} // namespace binomial
} // namespace option
} // namespace tf
} // namespace ou

