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
#include <stdexcept>

#include <boost/math/constants/constants.hpp>

#include "Formula.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

double DiffOfPutCall( double Stock, double InterestRate, double FractionOfYear, double Strike ) {
  return Stock - exp( -InterestRate * FractionOfYear ) * Strike;
}

double PutFromCall( double Call, double Stock, double InterestRate, double FractionOfYear, double Strike ) {
  return Call - DiffOfPutCall( Stock, InterestRate, FractionOfYear, Strike );
}

double CallFromPut( double Put, double Stock, double InterestRate, double FractionOfYear, double Strike ) {
  return Put + DiffOfPutCall( Stock, InterestRate, FractionOfYear, Strike );
}

/*
S: price of stock per share at time t
K: strike price
lsk: log(Ssubt/K)
r: annual risk free rate of interest
vol: volatility of stock under geometric Brownian motion model
tue: T - t, time until expiration
rtue: sqrt( tue )
q: continuous dividend yield pg 180
*/

double BSM_Euro_NonDiv_Call( double S, double K, double r, double vol, double tue ) {
  double lsk = log( S / K );
  double VolXVolBy2 = 0.5 * vol * vol;
  double RootTUE = sqrt( tue );
  double VolRootTUE = vol * RootTUE;
  double d1 = ( lsk + ( r + VolXVolBy2 ) * tue ) / VolRootTUE;
  //double d2 = ( lsk + ( r - VolXVolBy2 ) * tue ) / VolRootTUE;
  double d2 = d1 - VolRootTUE;
  boost::math::normal norm; //RealType mean = 0, RealType sd = 1
  double Nd1 = boost::math::cdf( norm, d1 );
  double Nd2 = boost::math::cdf( norm, d2 );
  return Nd1 * S - exp( -r * tue ) * K * Nd2;
}

double BSM_Euro_NonDiv_Put( double S, double K, double r, double vol, double tue ) {
  double lsk = log( S / K );
  double VolXVolBy2 = 0.5 * vol * vol;
  double RootTUE = sqrt( tue );
  double VolRootTUE = vol * RootTUE;
  double d1 = ( lsk + ( r + VolXVolBy2 ) * tue ) / VolRootTUE;
  //double d2 = ( lsk + ( r - VolXVolBy2 ) * tue ) / VolRootTUE;
  double d2 = d1 - VolRootTUE;
  boost::math::normal norm; //RealType mean = 0, RealType sd = 1
  double Nd1 = boost::math::cdf( norm, -d1 );
  double Nd2 = boost::math::cdf( norm, -d2 );
  return -Nd1 * S + exp( -r * tue ) * K * Nd2;
}

BSM_Euro::BSM_Euro( double r, double vol, double tue )
  : m_vol( vol ), m_r( r ), m_q( 0.0 ), m_tue( tue ),
  m_K( 1.0 ), m_S( 1.0 ),
  m_SqrtTUE( sqrt( tue ) ), m_EToRateAndTime( exp( -r * tue ) ),
  m_EToQAndTime( 1.0 /* exp( 0.0 ) */ ),
  m_b( 1.0 / sqrt( 2.0 * boost::math::double_constants::pi ) )
{
  CalcVolStuff();
}

BSM_Euro::BSM_Euro( double r, double vol, double tue, double q )
  : m_vol( vol ), m_r( r ), m_q( q ), m_tue( tue ),
  m_K( 1.0 ), m_S( 1.0 ),
  m_SqrtTUE( sqrt( tue ) ), m_EToRateAndTime( exp( -r * tue ) ),
  m_EToQAndTime( exp( -q * tue ) ),
  m_b( 1.0 / sqrt( 2.0 * boost::math::double_constants::pi ) )
{
  CalcVolStuff();
}

void BSM_Euro::CalcVolStuff( void ) {
  m_VolSqrtTUE = m_vol * m_SqrtTUE;
  double VolXVolBy2( m_vol * m_vol * 0.5 );
  m_a = ( m_r + VolXVolBy2 ) * m_tue;
}

void BSM_Euro::Calc( void ) {
  double lsk( log( m_S / m_K ) );
  double d1( ( lsk + m_a ) / m_VolSqrtTUE );
  //double d2 = ( m_lsk + ( m_r - m_VolXVolBy2 ) * m_tue ) / m_VolSqrtTUE;
  double d2( d1 - m_VolSqrtTUE );
  m_Nd1C = boost::math::cdf( norm, d1 );
  m_Nd2C = boost::math::cdf( norm, d2 );
  m_Nd1P = boost::math::cdf( norm, -d1 );
  m_Nd2P = boost::math::cdf( norm, -d2 );
  m_NPd1 = NPrime( d1 );
}

void BSM_Euro::Set( double vol ) {
  m_vol = vol;
  CalcVolStuff();
  Calc();
}

void BSM_Euro::Set( double S, double K ) {
  if ( 0.0 == S ) throw std::runtime_error( "S can't be 0.0" );
  if ( 0.0 == K ) throw std::runtime_error( "K can't be 0.0" );
  m_S = S;
  m_K = K;
  Calc();
}

void BSM_Euro::Set( double S, double K, double vol ) {
  m_vol = vol;
  CalcVolStuff();
  Set( S, K );
}

double BSM_Euro::Call( void ) {
  return m_Nd1C * m_EToQAndTime * m_S - m_EToRateAndTime * m_K * m_Nd2C;
}

double BSM_Euro::Put( void ) {
  return -m_Nd1P * m_EToQAndTime * m_S + m_EToRateAndTime * m_K * m_Nd2P;
}

double BSM_Euro::Call( double S, double K ) {
  Set( S, K );
  return Call();
}

double BSM_Euro::Put( double S, double K ) {
  Set( S, K );
  return Put();
}

double BSM_Euro::NPrime( double x ) {
  return m_b * exp( -0.5 * ( x * x ) );
}

double BSM_Euro::CallDelta( void ) {
  return m_EToQAndTime * m_Nd1C;
}

double BSM_Euro::PutDelta( void ) {
  return m_EToQAndTime * ( m_Nd1C - 1.0 );
}

double BSM_Euro::Gamma( void )  {
  return m_NPd1 * m_EToQAndTime / ( m_S * m_VolSqrtTUE );
}

double BSM_Euro::Vega( void ) {
  return m_S * m_SqrtTUE * m_NPd1 * m_EToQAndTime;
}

double BSM_Euro::CallTheta( void ) {
  double a = m_S * m_NPd1 * m_vol * m_EToQAndTime / ( 2.0 * m_SqrtTUE );
  double b = m_r * m_K * m_EToRateAndTime * m_Nd2C;
  double c = m_q * m_S * m_Nd1C * m_EToQAndTime;
  return -a - b + c;
}

double BSM_Euro::PutTheta( void ) {
  double a = m_S * m_NPd1 * m_vol * m_EToQAndTime / ( 2.0 * m_SqrtTUE );
  double b = m_r * m_K * m_EToRateAndTime * m_Nd2P;
  double c = m_q * m_S * m_Nd1P * m_EToQAndTime;
  return -a + b - c;
}

double BSM_Euro::CallRho( void ) {
  return m_K * m_SqrtTUE * m_SqrtTUE * m_EToRateAndTime * m_Nd2C;
}

double BSM_Euro::PutRho( void ) {
  return -m_K * m_SqrtTUE * m_SqrtTUE * m_EToRateAndTime * m_Nd2P;
}

double BSM_Euro::ImpliedVolatilityCall( double C, double epsilon ) {
  // pg 339 Black Scholes and beyond
  size_t cnt = 20;  // maximum iterations
  double diff( abs( Call() - C ) );
  while ( epsilon < diff ) {
    double slope = diff / Vega();
    Set( m_vol - slope );
    diff = abs( Call() - C );
    cnt--;
    if ( 0 == cnt ) {
      std::cout <<  "Implied Volatility calc did not converge" << std::endl;
      //throw std::runtime_error( "Implied Volatility calc did not converge" );
      break;
    }
  }
  return m_vol;
}

double BSM_Euro::ImpliedVolatilityPut( double P, double epsilon ) {
  // pg 339 Black Scholes and beyond
  size_t cnt = 20;  // maximum iterations
  double diff( abs( Put() - P ) );
  while ( epsilon < diff ) {
    double slope = diff / Vega();
    Set( m_vol + slope );
    diff = abs( Put() - P );
    cnt--;
    if ( 0 == cnt ) {
      std::cout <<  "Implied Volatility calc did not converge" << std::endl;
      //throw std::runtime_error( "Implied Volatility calc did not converge" );
      break;
    }
  }
  return m_vol;
}

double BSM_Euro::SeedForRegular( void ) {
  // pg 454 Option Pricing Formulas
  return sqrt( abs( log( m_S / m_K ) + m_r * m_tue ) * 2.0 / m_tue );
}

double BSM_Euro::SeedForFutures( void ) {
  // pg 454 Option Pricing Formulas
  return sqrt( abs( log( m_S / m_K ) ) * 2.0 / m_tue );
}

} // namespace option
} // namespace tf
} // namespace ou

