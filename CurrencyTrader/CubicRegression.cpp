/************************************************************************
 * Copyright(c) 2024, One Unified. All rights reserved.                 *
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

/*
 * File:    CubicRegression.cpp
 * Author:  raymond@burkholder.net
 * Project: CurrencyTrader
 * Created: November 15, 2024 22:58:52
 */

#include <eigen3/Eigen/Eigen>

#include "CubicRegression.hpp"

// example:
// https://www.omnicalculator.com/statistics/cubic-regression

namespace Regression {
namespace Cubic {

void Calc( const rInput_t& inX, const rInput_t& inY, rOutput_t& b ) {

  using matX_t = Eigen::Matrix<double, nRows, 4>;
  matX_t X;

  using vecY_t = Eigen::Matrix<double, nRows, 1>;
  vecY_t Y;

  using vecCoef_t = Eigen::Matrix<double, 4, 1>;
  vecCoef_t coef;

  for ( size_t ix_row = 0; ix_row < nRows; ix_row++ ) {

    X( ix_row, 0 ) = 1;

    const double x = inX[ ix_row ];
    X( ix_row, 1 ) = x;

    const double xx = x * x;
    X( ix_row, 2 ) = xx;

    const double xxx = xx * x;
    X( ix_row, 3 ) = xxx;

    Y( ix_row ) = inY[ ix_row ];
  }

  coef = ( X.transpose() * X ).inverse() * X.transpose() * Y;

  for ( size_t ix = 0; ix < nCoef; ix++ ) {
    b[ ix ] = coef[ ix ];
  }

}

}
}

