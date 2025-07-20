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
 * File:    OptionManager.hpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: July 19, 2025 11:40:02
 */

#pragma once

#include <TFOptions/Option.h>
#include <TFOptions/Engine.h>
#include <TFOptions/NoRiskInterestRateSeries.h>

#include <TFIQFeed/Provider.h>

class OptionManager {
public:

  using pIQFeed_t = ou::tf::iqfeed::Provider::pProvider_t;

  OptionManager( pIQFeed_t );
  ~OptionManager();

  void SaveSeries( const std::string& sFile, const std::string& sPath );

protected:
private:

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pOption_t = ou::tf::option::Option::pOption_t;

  pIQFeed_t m_pIQFeed;

  ou::tf::FedRateFromIQFeed m_fedrate;
  std::unique_ptr<ou::tf::option::Engine> m_pOptionEngine;

};