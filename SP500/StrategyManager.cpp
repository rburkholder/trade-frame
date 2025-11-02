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
 * File:    StrategyManager.cpp
 * Author:  raymond@burkholder.net
 * Project: SP500
 * Created: June 28, 2025 17:38:07
 */

#include "StrategyManager.hpp"
#include "StrategyManager_impl.hpp"

StrategyManager::StrategyManager(
  const config::Choices& choices
, fQueueTask_t&& fQueueTask
, fSetChartDataView_t&& fSetChartDataView
, fSetTimeSeriesModel_t&& fSetTimeSeriesModel
, fDone_t&& fDone
) {
  m_pStrategyManager_impl = std::make_unique<StrategyManager_impl>(
    choices
  , std::move( fQueueTask )
  , std::move( fSetChartDataView )
  , std::move( fSetTimeSeriesModel )
  , std::move( fDone )
  );
}

StrategyManager::~StrategyManager() {
  m_pStrategyManager_impl.reset();
}
