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
 * File:    Strategy.hpp
 * Author:  raymond@burkholder.net
 * Project: rdaf/l2
 * Created: March 11, 2034  10:58:03
 */

#pragma once

#include <OUCharting/ChartDataView.h>

#include "ConfigParser.hpp"

namespace Strategy {

class Base {
public:

  Base( const ou::tf::config::symbol_t& );
  virtual ~Base();

  const ou::tf::config::symbol_t& Choices() const { return m_config; }
  ou::tf::config::symbol_t::EFeed Feed() const { return m_config.eFeed; }

  ou::ChartDataView& GetChartDataView() { return m_cdv; }

protected:

  const ou::tf::config::symbol_t& m_config;

  ou::ChartDataView m_cdv;

private:
};

} // namespace Strategy
