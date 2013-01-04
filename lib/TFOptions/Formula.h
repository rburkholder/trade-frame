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

#include <boost/math/distributions/normal.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

// Put-Call Parity forEuropean Options

// pg 40, Black Scholes and Beyond

inline double DiffOfPutCall( double Stock, double InterestRate, double FractionOfYear, double Strike );
inline double PutFromCall( double Call, double Stock, double InterestRate, double FractionOfYear, double Strike );
inline double CallFromPut( double Put, double Stock, double InterestRate, double FractionOfYear, double Strike );

// pg 152 Black Scholes, vanilla options on non-dividend paying stock
// pg 160 has method for dealing with stocks paying lumpy dividends
// pg 161 has formula changes for  stock with annual continous dividend yield
// pg 180 has formula for greeks for BSM with continuous dividends

// delta: the rate of change of the value of the option with respect ot changes in the stock price
// gamma: the rate of change of the delta with respect to changes in the stock price
// theta: the rate of change of the value of an option with respect to time
// rho:   the rate of change of the value of an option with respect ot the risk-free rate of interest
// vega:  the reate of chagne of the value of an option with respect to volatility

double BSM_Euro_Call( double S, double K, double r, double vol, double tue );
double BSM_Euro_Put( double S, double K, double r, double vol, double tue );

class BSM_Euro {
public:
  BSM_Euro( double r, double vol, double tue );
  BSM_Euro( double r, double vol, double tue, double q );
  ~BSM_Euro( void ) {};
  void Set( double vol );
  void Set( double S, double K );
  void Set( double S, double K, double vol );
  double Call( void );
  double Call( double S, double K );
  double Put( void );
  double Put( double S, double K );
  double CallDelta( void );
  double PutDelta( void );
  double Gamma( void );
  double Vega( void );
  double CallTheta( void );
  double PutTheta( void );
  double CallRho( void );
  double PutRho( void );
  double ImpliedVolatility( double C, double epsilon = 0.0001 );  // market price for Call
  double SeedForRegular( void );
  double SeedForFutures( void );
protected:
private:
  double m_vol; // volatility
  double m_r;
  double m_q;
  double m_tue;
  double m_EToRateAndTime;
  double m_EToQAndTime;
  double m_SqrtTUE; // sqrt( tue )
  double m_VolSqrtTUE; // vol * sqrt( tue )
  double m_a;  
  double m_b; // recip of sqrt two pi
  double m_S;  // stock price
  double m_K;  // strike price
  double m_Nd1C; // probability call option expires in the money pg 114
  double m_Nd1P; // probability put option expires in the money
  double m_Nd2C;
  double m_Nd2P;
  double m_NPd1;  // NPrime of d1
  boost::math::normal norm; //RealType mean = 0, RealType sd = 1
  void Calc( void );
  void CalcVolStuff( void );
  double NPrime( double x );
};

} // namespace option
} // namespace tf
} // namespace ou

