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
 * Project: SP500
 * Created: March 30, 2025 17:59:23
 */

#pragma once

#include <string>
#include <vector>

#include "HyperParameters.hpp"

namespace config {

struct Choices {

  enum class EMode { view_training, view_validate, train_and_validate, unknown } eMode;

  using vFileTraining_t = std::vector<std::string>;
  vFileTraining_t m_vFileTraining; // multiple files for training
  std::string m_sFileValidate; // single file for validation

  HyperParameters m_hp;

};

bool Load( const std::string& sFileName, Choices& );

} // namespace config
