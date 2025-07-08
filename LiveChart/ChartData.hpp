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

  typedef ou::tf::ProviderInterfaceBase::pProvider_t pProvider_t;

  ChartData( pProvider_t, const std::string& );
  ~ChartData();

  ou::tf::Watch* GetWatch() { return m_pWatch; };

protected:
private:

  ou::tf::Watch* m_pWatch;

};

