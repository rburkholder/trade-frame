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

double BSM_Euro_NonDiv_Call( double S, double K, double r, double vol, double tue );
double BSM_Euro_NonDiv_Put( double S, double K, double r, double vol, double tue );

class BSM_Euro_NonDiv {
public:
  BSM_Euro_NonDiv( double r, double vol, double tue );
  ~BSM_Euro_NonDiv( void ) {};
  void Set( double S, double K );
  double Call( void );
  double Call( double S, double K );
  double Put( void );
  double Put( double S, double K );
protected:
private:
  double m_S;  // stock price
  double m_K;  // strike price
  double m_r;  // risk-free rate of interest per annum
  double m_vol; // volatility of stock undergeometric Brownian motion model
  double m_tue; // time until expiration, some fraction of a year
  double m_lsk;  // log( S / K )
  double m_VolXVolBy2; // vol * vol / 2
  double m_SqrtTUE; // sqrt( tue )
  double m_VolSqrtTUE; // vol * sqrt( tue )
  double m_d1; 
  double m_d2;
  double m_EToRateAndTime;
  boost::math::normal norm; //RealType mean = 0, RealType sd = 1
};

} // namespace option
} // namespace tf
} // namespace ou

