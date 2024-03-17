/************************************************************************
 * Copyright(c) 2024, One Unified. All rights reserved.                 *
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
 * File:    Strategy.hpp
 * Author:  raymond@burkholder.net
 * Project: CurrencyTrader
 * Created: March 14, 2024 21:32:07
 */

#pragma once

#include <TFTrading/Position.h>

class Strategy {
public:

  using pPosition_t = ou::tf::Position::pPosition_t;

  Strategy( pPosition_t );
  ~Strategy();

protected:
private:
  pPosition_t m_pPosition;
};