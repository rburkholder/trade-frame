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

  double dblTickJ;
  double dblTickL;

  double dblAdv;
  double dblDec;
  double dblAdvDecRatio;

  double dblEma013;
  double dblEma029;
  double dblEma050;
  double dblEma200;

  //double m_dblTrin;

  Features_raw() // maybe not required
  : dblPrice {}
  , dblTickJ {}
  , dblTickL {}
  , dblAdv {}
  , dblDec {}
  , dblAdvDecRatio {}
  , dblEma013 {}
  , dblEma029 {}
  , dblEma050 {}
  , dblEma200 {}
  //, m_dblTrin {}
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

  double range; // used to reverse the scale (slope)
  double min;   // used to reverse the scale (offset)

  Real price;
  Real predicted;

  size_t distance; // how far into future

  Real ema013;
  Real ema029;
  Real ema050;
  Real ema200;

  Real tickJ;
  Real tickL;
  Real AdvDec;

  void Zero() {
    price = 0.0;
    predicted = 0.0;
    ema013 = 0.0;
    ema029 = 0.0;
    ema050 = 0.0;
    ema200 = 0.0;
    tickJ = 0.0;
    tickL = 0.0;
    AdvDec = 0.0;
  }

};