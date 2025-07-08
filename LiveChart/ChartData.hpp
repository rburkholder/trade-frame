/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

#pragma once

// Started 2013/09/26

#include <OUCharting/ChartDVBasics.h>

#include <TFTrading/Watch.h>
#include <TFTrading/ProviderManager.h>

class ChartData: public ou::ChartDVBasics {
public:

  using pProvider_t = ou::tf::ProviderInterfaceBase::pProvider_t;

  ChartData( pProvider_t, const std::string& );
  ~ChartData();

  using pWatch_t = ou::tf::Watch::pWatch_t;
  void SaveSeries( const std::string& sPrefix );

protected:
private:

  pWatch_t m_pWatch;

};

