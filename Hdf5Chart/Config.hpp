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
 * File:    Config.hpp
 * Author:  raymond@burkholder.net
 * Project: Hdf5Chart
 * Created: April 4, 2024 19:46
 */

#pragma once

#include <string>

namespace config {

struct Choices {

  std::string m_sHdf5File; // optional

};

bool Load( const std::string& sFileName, Choices& );

} // namespace config
