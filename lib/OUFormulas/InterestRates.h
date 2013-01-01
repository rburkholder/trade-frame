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

#include <math.h>

namespace ou { // One Unified

// pg 63 Black Scholes and Beyond
double DiscreteFutureValue( double PresentValue, // present value
                            double InterestRate, // annual interest rate 
                            double N, // compounding periods per year
                            double Y  // number of annual periods
                            );

// used for pricing a zero coupon bond 
double DiscretePresentValue( double FutureValue, // future value
                             double InterestRate, // annual interest rate
                             double N, // compounding periods per year
                             double Y // number of annual periods
                             );

// pg 63 Black Scholes and Beyond, future value based upon continously compounded interest rate
double ContinuousFutureValue( double PresentValue,  
                             double InterestRate,
                             double Y // number of annual periods
                             );

// pg 63 Black Scholes and Beyond, Conversion between discrete/continuous interest rates
double ContinuousInterestRate( double DiscreteInterestRate, double N ); // N is compounding periods per annum

double DiscreteInterestRate( double ContinuousInterestRate, double N ); // N is compounding periods per annum

double DiscreteOrdinaryAnnuityFutureValue( double AnnualPayment, // payment at end of year
                                   double InterestRate,
                                   size_t N // number of annual periods
                                   );

double DiscreteAnnuityDueFutureValue( double AnnualPayment, // payment made at beginning of year
                                      double InterestRate,
                                      size_t N // number of annual periods
                                      );

double AnnualizedContinuouslyCompoundedReturn( double S1, double S2, double DeltaT ); 

} // namespace ou