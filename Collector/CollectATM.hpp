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
 * File:    CollectATM.hpp
 * Author:  raymond@burkholder.net
 * Project: Collector
 * Created: January 19, 2025 14:06:45
 */

#pragma once

//#include <TFOptions/Option.h>

#include "Collect.hpp"
#include "FillWrite.hpp"

namespace collect {

class ATM: public Base {
public:

  //using pOption_t = ou::tf::option::Option::pOption_t;
  using pWatch_t = ou::tf::Watch::pWatch_t;

  ATM( const std::string& sPathPrefix, pWatch_t );  // underlying
  ~ATM();

  void Write() override; // incremental write

protected:
private:

  pWatch_t m_pUnderlying;

  using fwATM_t = ou::tf::FillWrite<ou::tf::Greeks>;
  std::unique_ptr<fwATM_t> m_pfwATM;

  void HandleWatchGreeksPut( const ou::tf::Greek& );
  void HandleWatchGreeksCall( const ou::tf::Greek& );
};

} // namespace collect
