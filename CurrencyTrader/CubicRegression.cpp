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

#include <algorithm>

#include <eigen3/Eigen/Eigen>

#include <OUCharting/ChartEntryBase.cpp>

#include "CubicRegression.hpp"

// example:
// https://www.omnicalculator.com/statistics/cubic-regression

namespace ou {
namespace tf {
namespace Regression {

Cubic::Cubic()
: m_cnt {}
{
  assert( 4 <= nRows );
  assert( 4 == nCoef );
  m_coef[ 0 ] = m_coef[ 1 ] = m_coef[ 2 ] = m_coef[ 3 ] = 0.0;
}

void Cubic::Append( boost::posix_time::ptime dt, double y ) {
  Append( ou::ChartEntryTime::Convert( dt ), y );
}

void Cubic::Append( double x, double y ) {

  {
    rInput_t::iterator x1( m_X.begin() );
    x1++;
    std::move( x1, m_X.end(), m_X.begin() );
  }

  {
    rInput_t::iterator y1( m_Y.begin() );
    y1++;
    std::move( y1, m_Y.end(), m_Y.begin() );
  }

  *m_X.rbegin() = x;
  *m_Y.rbegin() = y;

  m_cnt++;

}

double Cubic::Terpolate( double x ) const {
  return m_coef[ 0 ] + x * ( m_coef[ 1 ] + x * ( m_coef[ 2 ] + m_coef[ 3 ] * x ) );
}

void Cubic::CalcCoef() {

  assert( m_cnt >= nRows );

  using matX_t = Eigen::Matrix<double, nRows, nCoef>;
  matX_t X;

  using vecY_t = Eigen::Matrix<double, nRows, 1>;
  vecY_t Y;

  using vecCoef_t = Eigen::Matrix<double, nCoef, 1>;
  vecCoef_t coef;

  for ( size_t ix_row = 0; ix_row < nRows; ix_row++ ) {

    X( ix_row, 0 ) = 1;

    const double x = m_X[ ix_row ];
    X( ix_row, 1 ) = x;

    const double xx = x * x;
    X( ix_row, 2 ) = xx;

    const double xxx = xx * x;
    X( ix_row, 3 ) = xxx;

    Y( ix_row ) = m_Y[ ix_row ];
  }

  coef = ( X.transpose() * X ).inverse() * X.transpose() * Y;

  for ( size_t ix = 0; ix < nCoef; ix++ ) {
    m_coef[ ix ] = coef[ ix ];
  }

}

} // namespace Regression
} // namespace tf
} // namespace ou
