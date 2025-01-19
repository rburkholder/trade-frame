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
 * File:    CollectGreeks.hpp
 * Author:  raymond@burkholder.net
 * Project: Collector
 * Created: January 18, 2025 22:04/38
 */

#pragma once

#include <TFOptions/Option.h>

#include "FillWrite.hpp"

namespace ou {
namespace tf {
  class HDF5Attributes;
} // namespace tf
} // namespace ou

namespace collect {

class Greeks {
public:

  using pOption_t = ou::tf::option::Option::pOption_t;

  Greeks( const std::string& sPathPrefix, pOption_t );
  ~Greeks();

  void Write(); // incremental write

protected:
private:

  pOption_t m_pOption;

  using fwGreeks_t = ou::tf::FillWrite<ou::tf::Greeks>;
  std::unique_ptr<fwGreeks_t> m_pfwGreeks;

  void HandleWatchGreeks( const ou::tf::Greek& );
  void SetAttributes( ou::tf::HDF5Attributes& );
};

} // namespace collect
