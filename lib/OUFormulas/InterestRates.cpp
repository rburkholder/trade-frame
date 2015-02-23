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

#include <cstdlib>  // used for size_t

#include "InterestRates.h"

namespace ou { // One Unified

// pg 63 Black Scholes and Beyond
double DiscreteFutureValue( double PresentValue, // present value
                            double InterestRate, // annual interest rate 
                            double N, // compounding periods per year
                            double Y  // number of annual periods
                            ) {
  return PresentValue * pow( 1.0 + InterestRate / N, N * Y );
}

// http://iris.nyit.edu/appack/pdf/FINC201_2.pdf
// http://iris.nyit.edu/appack/pdf/FINC201_5.pdf 
// used for pricing a zero coupon bond 
double DiscretePresentValue( double FutureValue, // future value
                             double InterestRate, // annual interest rate
                             double N, // compounding periods per year
                             double Y // number of annual periods
                             ) {
  return FutureValue * pow( 1.0 + InterestRate / N, -N * Y );
}

// pg 63 Black Scholes and Beyond, future value based upon continously compounded interest rate
double ContinuousFutureValue( double PresentValue,  
                             double InterestRate,
                             double Y // number of annual periods
                             ) {
  return PresentValue * exp( InterestRate * Y );
}

// pg 65 Black Scholes and Beyond
double AnnualizedContinuouslyCompoundedReturn( double S1,  // value at beginning of term
                                               double S2,  // value at end of term
                                               double DeltaT  // duration in years
                                               ) {
  return ( 1.0 / DeltaT ) * log( S2 / S1 );
}

// pg 63 Black Scholes and Beyond, Conversion between discrete/continuous interest rates
double ContinuousInterestRate( double DiscreteInterestRate, double N ) { // N is compounding periods per annum
  return N * log( 1.0 + DiscreteInterestRate / N );
}

double DiscreteInterestRate( double ContinuousInterestRate, double N ) { // N is compounding periods per annum
  return N * ( exp( ContinuousInterestRate / N ) - 1.0 );
}

// http://iris.nyit.edu/appack/pdf/FINC201_3.pdf
double DiscreteOrdinaryAnnuityFutureValue( double AnnualPayment, // payment at end of year
                                   double InterestRate,
                                   size_t N // number of annual periods
                                   ) {
  return AnnualPayment * ( pow( 1 + InterestRate, (int) N ) - 1.0 ) / InterestRate;
}

// http://iris.nyit.edu/appack/pdf/FINC201_3.pdf
double DiscreteAnnuityDueFutureValue( double AnnualPayment, // payment made at beginning of year
                                      double InterestRate,
                                      size_t N // number of annual periods
                                      ) {
  return ( AnnualPayment * ( 1.0 + InterestRate ) * ( pow( 1 + InterestRate, (int) N ) - 1.0 ) ) / InterestRate;
}

} // namespace ou