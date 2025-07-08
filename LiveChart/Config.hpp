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
 * File:    Config.hpp
 * Author:  raymond@burkholder.net
 * Project: LiveChart
 * Created: July 8, 2025 10:07:57
 */

#pragma once

#include <string>
#include <vector>

namespace config {

struct Choices {

  using vSymbol_t = std::vector<std::string>;
  vSymbol_t vSymbol;

};

bool Load( const std::string& sFileName, Choices& );

} // namespace config
