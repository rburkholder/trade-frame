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
 * File:    StrategyManager.hpp
 * Author:  raymond@burkholder.net
 * Project: SP500
 * Created: June 28, 2025 17:38:07
 */

#pragma once

#include <memory>

#include <OUCharting/ChartDataView.h>

// need to keep Torch stuff separate from wxWidgets stuff.  Includes negatively interact with each other.

namespace config {
  class Choices;
} // namespace config

class StrategyManager_impl;

class StrategyManager {
public:

  using fTask_t = std::function<void()>;
  using fQueueTask_t = std::function<void( fTask_t&& )>;

  StrategyManager( const config::Choices&, ou::ChartDataView&, fQueueTask_t&& );
  ~StrategyManager();

protected:
private:

  using pStrategyManager_impl_t = std::unique_ptr<StrategyManager_impl>;
  pStrategyManager_impl_t m_pStrategyManager_impl;
};
