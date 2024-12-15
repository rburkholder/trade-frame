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
 * File:    LuaControl.cpp
 * Author:  raymond@burkholder.net
 * Project: MarketTrader
 * Created: 2024/12/15 11:58:12
 */

#include "LuaControl.hpp"

namespace lua {

Control::Control() {

}

Control::~Control() {}

void Control::AddPath( const std::string& sPath ) {
  m_fn.AddWatch( sPath, []( ou::FileNotify::EType, const std::string& sFileName ){} );
}

void Control::DelPath( const std::string& sPath ) {
  m_fn.DelWatch( sPath );
}

} // namespace lua