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
 * File:    CrossingEvent.hpp
 * Author:  raymond@burkholder.net
 * Project: SP500
 * Created: November 11, 2025 12:48:33
 */

#pragma once

#include <functional>

namespace ou {
namespace tf {

class CrossingEvent {
public:

  enum Direction { dn_up, up_dn };
  enum State { below, touched, above };
  using fTrigger_t = std::function<void()>;

  CrossingEvent( double level, Direction, fTrigger_t&& );

  void Test( double value );
  double Limit() const { return limit; }

private:

  const double level;
  const Direction direction;
  const fTrigger_t fTrigger;

  State state;
  double limit;

};

} // namespace tf
} // namespace ou