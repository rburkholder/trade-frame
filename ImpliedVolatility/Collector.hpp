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
 * File:    Collector.hpp
 * Author:  raymond@burkholder.net
 * Project: ImpliedVolatility
 * Created: January 31, 2023  20:37:13
 */

#include <TFTrading/Instrument.h>
#include <TFTrading/Watch.h>

// watch instrument & start chain watch

class Collector {
public:

  using pWatch_t = ou::tf::Watch::pWatch_t;

  Collector( pWatch_t );
  ~Collector();

protected:
private:

  pWatch_t m_pWatch;
};