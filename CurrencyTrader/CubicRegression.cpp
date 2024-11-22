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

#include <boost/log/trivial.hpp>

#include <eigen3/Eigen/Eigen>

#include <OUCharting/ChartEntryBase.cpp>

#include "CubicRegression.hpp"

// example:
// https://www.omnicalculator.com/statistics/cubic-regression

namespace ou {
namespace tf {
namespace Regression {

Cubic::Cubic()
: m_cnt {}, m_x_offset {}
{
  assert( 4 <= nRows );
  assert( 4 == nCoef );
  m_coef[ 0 ] = m_coef[ 1 ] = m_coef[ 2 ] = m_coef[ 3 ] = 0.0;
}

void Cubic::Append( boost::posix_time::ptime dt, double y ) {

  rInput_t::iterator p1( m_P.begin() );
  p1++;
  std::move( p1, m_P.end(), m_P.begin() );

  *m_P.rbegin() = Point( dt, ou::ChartEntryTime::Convert( dt ), y );

  m_cnt++;

}

double Cubic::Terpolate( double x_ ) const {
  const double x( x_ - m_x_offset );
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

  m_x_offset = m_P[ 0 ].x;

  for ( size_t ix_row = 0; ix_row < nRows; ix_row++ ) {

    X( ix_row, 0 ) = 1;

    const double x = m_P[ ix_row ].x - m_x_offset;;
    X( ix_row, 1 ) = x;

    const double xx = x * x;
    X( ix_row, 2 ) = xx;

    const double xxx = xx * x;
    X( ix_row, 3 ) = xxx;

    Y( ix_row ) = m_P[ ix_row ].y;
  }

  coef = ( X.transpose() * X ).inverse() * X.transpose() * Y;

  for ( size_t ix = 0; ix < nCoef; ix++ ) {
    m_coef[ ix ] = coef[ ix ];
  }

  m_ceCurrent.Clear();
  for ( const auto& point: m_P ) {
    m_ceCurrent.Append( point.ptime, Terpolate( point.x ) );
  }

}

void Cubic::EmitCubicCoef() {
  BOOST_LOG_TRIVIAL(info)
    << "  " << m_cnt << ',' << m_coef[ 0 ] << ',' << m_coef[ 1 ] << ',' << m_coef[ 2 ] << ',' << m_coef[ 3 ]
    ;
}

} // namespace Regression
} // namespace tf
} // namespace ou
