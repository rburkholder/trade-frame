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
 * File:    Collect.hpp
 * Author:  raymond@burkholder.net
 * Project: Collector
 * Created: January 18, 2025 22:04/38
 */

// base class

#pragma once

#include <TFTrading/Watch.h>

namespace ou {
namespace tf {
  class HDF5Attributes;
} // namespace tf
} // namespace ou

namespace collect {

class Base {
public:
  virtual void Write() = 0; // incremental write
protected:
  using pWatch_t = ou::tf::Watch::pWatch_t;
  void SetAttributes( ou::tf::HDF5Attributes&, pWatch_t );
private:
};

} // namespace collect
