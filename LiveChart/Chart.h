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

#include <TFTrading/ProviderManager.h>
#include <TFTrading/Watch.h>

class ChartTest: public ou::ChartDVBasics {
public:

  typedef ou::tf::ProviderInterfaceBase::pProvider_t pProvider_t;

  ChartTest( pProvider_t );
  ~ChartTest(void);

  ou::tf::Watch* GetWatch( void ) { return m_pWatch; };

protected:
private:

  ou::tf::Watch* m_pWatch;

};

