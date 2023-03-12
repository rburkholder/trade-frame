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
 * File:    StrategyEquityOption.hpp
 * Author:  raymond@burkholder.net
 * Project: rdaf/l2
 * Created: March 11, 2034  10:58:03
 */

#pragma once

#include <OUCharting/ChartEntryVolume.h>
#include <OUCharting/ChartEntryIndicator.h>

#include "StrategyBase.hpp"

namespace Strategy {

class EquityOption: public Base {
public:

  using pPosition_t = ou::tf::Position::pPosition_t;

  EquityOption( const ou::tf::config::symbol_t& );
  virtual ~EquityOption();

  virtual void SetPosition( pPosition_t );
  virtual void SaveWatch( const std::string& sPrefix );

protected:
private:

  using pWatch_t = ou::tf::Watch::pWatch_t;

  ou::ChartEntryIndicator m_ceQuoteAsk;
  ou::ChartEntryIndicator m_ceQuoteBid;

  ou::ChartEntryIndicator m_ceTrade;
  ou::ChartEntryVolume m_ceVolume;


  void HandleQuote( const ou::tf::Quote& );
  void HandleTrade( const ou::tf::Trade& );
};

} // namespace Strategy

