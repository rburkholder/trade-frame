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
 * File:    LuaControl.hpp
 * Author:  raymond@burkholder.net
 * Project: MarketTrader
 * Created: 2024/12/15 11:58:12
 */

#pragma once

#include <string>

#include "FileNotify.hpp"

namespace lua {

class Control {
public:

  Control();
  ~Control();

  void AddPath( const std::string& sPath );
  void DelPath( const std::string& sPath );

protected:
private:
  ou::FileNotify m_fn;
};

} // namespace lua