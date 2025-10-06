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
 * File:    CubicRegression.hpp
 * Author:  raymond@burkholder.net
 * Project: CurrencyTrader
 * Created: November 15, 2024 22:58:52
 */

#pragma once

#include <array>

#include <boost/date_time/posix_time/ptime.hpp>

#include <OUCharting/ChartEntryIndicator.h>

namespace ou {
namespace tf {
namespace Regression {

class Cubic {
public:

  Cubic();

  void Append( boost::posix_time::ptime dt, double y );
  bool Full() const { return m_cnt >= nRows; }
  void CalcCoef(); // perform automatically with each Append?

  double Terpolate( double x ) const; // interpolate/extrapolate y

  ou::ChartEntryIndicator& ChartEntry() { return m_ceCurrent; };

  void EmitCubicCoef();

protected:
private:

  std::size_t m_cnt;

  struct Point {

    boost::posix_time::ptime ptime;
    double x;
    double y;

    Point(): x{}, y{} {}
    Point( boost::posix_time::ptime ptime_, double x_, double y_ )
    : ptime( ptime_ ), x( x_), y( y_ ) {}
    Point( Point&& p ): ptime( p.ptime), x( p.x ), y( p.y ) {}
    Point( const Point& p ): ptime( p.ptime), x( p.x ), y( p.y ) {}
    const Point& operator=( const Point& p ){ ptime = p.ptime; x = p.x; y = p.y; return *this; }
  };

  double m_x_offset;

  static const std::size_t nRows = 10;
  using rInput_t = std::array<Point,nRows>;
  rInput_t m_P;

  static const std::size_t nCoef =  4;
  using rOutput_t = std::array<double,nCoef>;
  rOutput_t m_coef;

  ou::ChartEntryIndicator m_ceCurrent;
};

} // namespace Regression
} // namespace tf
} // namespace ou
