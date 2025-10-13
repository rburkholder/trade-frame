/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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
 * File:    Features.hpp
 * Author:  raymond@burkholder.net
 * Project: SP500
 * Created: July 4, 2025 11:51:31
 */

#pragma once

#include <boost/date_time/posix_time/ptime.hpp>

struct Features_raw {

  boost::posix_time::ptime dt;

  double dblPrice;

  double dblSDDirection;

  double dblTickJ;
  double dblTickL;

  double dblEma013;
  double dblEma029;

  double dblReturnsMean;
  double dblReturnsSlope;

  Features_raw() // maybe not required
  : dblPrice {}
  , dblSDDirection {}
  , dblTickJ {}
  , dblTickL {}
  , dblEma013 {}
  , dblEma029 {}
  , dblReturnsMean {}
  , dblReturnsSlope {}
  {}

};

struct Real {
  double dbl;
  float flt;
  Real( double val ): dbl( val ), flt( val ) {}
  Real( float val ): dbl( val ), flt( val ) {}
  Real() {}
  double operator=( double val ) {
    dbl = val;
    flt = val; // conversion
    return val;
  }
  float operator=( float val ) {
    flt = val;
    dbl = val; // conversion
    return val;
  }

};

struct Features_scaled {

  size_t distance; // how far into future

  Real predicted;

  Real price;

  Real SDDirection;

  Real ema013;
  Real ema029;

  Real tickJ;
  Real tickL;

  Real returns_mean;
  Real returns_slope;

  void Zero() {
    predicted = 0.0;
    price = 0.0;
    SDDirection = 0.0;
    ema013 = 0.0;
    ema029 = 0.0;
    tickJ = 0.0;
    tickL = 0.0;
    returns_mean = 0.0;
    returns_slope = 0.0;
  }

};