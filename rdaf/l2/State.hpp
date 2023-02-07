/************************************************************************
 * Copyright(c) 2023, One Unified. All rights reserved.                 *
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
 * File:    State.hpp
 * Author:  raymond@burkholder.net
 * Project: rdaf/l2
 * Created: February 6, 2023 18:34:54
 */

#pragma once

#include <map>
#include <array>
#include <vector>
#include <cassert>

class State {
public:

 State(
    const double bid, const double ask
  , const double ma0, const double ma1, const double ma2, const double ma3
  , const double slope0_, const double slope1_, const double slope2_, const double slope3_
  );

  bool EnterLong() const;
  bool EnterShort() const;

  double Stop() const { return m_stop; }

  bool operator==( const State& rhs );

protected:
private:

  enum class EValue { bid, ask, ma0, ma1, ma2, ma3 };

  using vRelative_t = std::vector<EValue>;
  using mapRelative_t = std::map<double,vRelative_t>;

  using rEValue_t = std::array<EValue,6>;
  rEValue_t m_rEValue;

  double m_slope0 {};
  double m_slope1 {};
  double m_slope2 {};
  double m_slope3 {};

  double m_stop {};

  void Insert( mapRelative_t& map, EValue e, const double value );

};

