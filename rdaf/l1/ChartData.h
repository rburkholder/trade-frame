/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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
 * File:    ChartData.h
 * Author:  raymond@burkholder.net
 * Project: rdaf/l1
 * Created: February 6, 2022 16:15
 * retrofit back to LiveChart
 */

#pragma once

#include <OUCharting/ChartDVBasics.h>

#include <TFTrading/Watch.h>
#include <TFTrading/ProviderManager.h>

class ChartData: public ou::ChartDVBasics {
public:

  using pProvider_t = ou::tf::ProviderInterfaceBase::pProvider_t;
  using pWatch_t    = ou::tf::Watch::pWatch_t;

  ChartData( pProvider_t, const std::string& sIQFeedSymbol );
  virtual ~ChartData();

  pWatch_t GetWatch( void ) { return m_pWatch; };

protected:
private:

  pWatch_t m_pWatch;

};

