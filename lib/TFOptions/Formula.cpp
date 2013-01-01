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

BSM_Euro_NonDiv::BSM_Euro_NonDiv( double r, double vol, double tue )
  : m_r( r ), m_vol( vol ), m_tue( tue ), 
  m_VolXVolBy2( vol * vol * 0.5 ),
  m_SqrtTUE( sqrt( tue ) ), m_EToRateAndTime( exp( -r * tue ) )
{
  m_VolSqrtTUE = m_vol * m_SqrtTUE;
}

void BSM_Euro_NonDiv::Set( double S, double K ) {
  m_S = S;
  m_K = K;
  m_lsk = log( S / K );
  double d1 = ( m_lsk + ( m_r + m_VolXVolBy2 ) * m_tue ) / m_VolSqrtTUE;
  //double d2 = ( m_lsk + ( m_r - m_VolXVolBy2 ) * m_tue ) / m_VolSqrtTUE;
  double d2 = d1 - m_VolSqrtTUE;
}

double BSM_Euro_NonDiv::Call( void ) {
  double Nd1 = boost::math::cdf( norm, m_d1 );
  double Nd2 = boost::math::cdf( norm, m_d2 );
  return Nd1 * m_S - m_EToRateAndTime * m_K * Nd2;
}

double BSM_Euro_NonDiv::Put( void ) {
  double Nd1 = boost::math::cdf( norm, -m_d1 );
  double Nd2 = boost::math::cdf( norm, -m_d2 );
  return Nd1 * m_S + m_EToRateAndTime * m_K * Nd2;
}

double BSM_Euro_NonDiv::Call( double S, double K ) {
  Set( S, K );
  double Nd1 = boost::math::cdf( norm, m_d1 );
  double Nd2 = boost::math::cdf( norm, m_d2 );
  return Nd1 * m_S - m_EToRateAndTime * m_K * Nd2;
}

double BSM_Euro_NonDiv::Put( double S, double K ) {
  Set( S, K );
  double Nd1 = boost::math::cdf( norm, -m_d1 );
  double Nd2 = boost::math::cdf( norm, -m_d2 );
  return Nd1 * m_S + m_EToRateAndTime * m_K * Nd2;
}


} // namespace option
} // namespace tf
} // namespace ou

