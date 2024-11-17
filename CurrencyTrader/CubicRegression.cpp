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

#include <iostream>

#include <eigen3/Eigen/Eigen>

#include "CubicRegression.hpp"

// example:
// https://www.omnicalculator.com/statistics/cubic-regression


namespace Regression {
namespace Cubic {

void Calc() {

  //static const size_t rows= 10;
  static const size_t rows= 5;

  using matData_t = Eigen::Matrix<double, rows, 2>;
/*
  const matData_t data(
    { //  X    Y
      {  1.0, 2.0 },
      {  3.0, 3.0 },
      {  7.0, 4.0 },
      { 13.0, 3.0 },
      { 18.0, 2.0 },
      { 23.0, 1.0 },
      { 24.0, 2.0 },
      { 27.0, 3.0 },
      { 29.0, 4.0 },
      { 31.0, 5.0 }
    }
  );
*/

  const matData_t data
    { //  X    Y
      {  0.0, 1.0 },
      {  2.0, 0.0 },
      {  3.0, 3.0 },
      {  4.0, 5.0 },
      {  5.0, 4.0 },
    }
  ;

  using matX_t = Eigen::Matrix<double, rows, 4>;
  matX_t X;

  using vecY_t = Eigen::Matrix<double, rows, 1>;
  vecY_t Y;

  using vecCoef_t = Eigen::Matrix<double, 4, 1>;
  vecCoef_t coef;

  for ( size_t ix_row = 0; ix_row < rows; ix_row++ ) {

    X( ix_row, 0 ) = 1;

    const double x = data( ix_row, 0 );
    X( ix_row, 1 ) = x;

    const double xx = x * x;
    X( ix_row, 2 ) = xx;

    const double xxx = xx * x;
    X( ix_row, 3 ) = xxx;

    Y( ix_row ) = data( ix_row, 1 );
  }

  coef = ( X.transpose() * X ).inverse() * X.transpose() * Y;

  std::cout << coef << std::endl;
}

}
}

