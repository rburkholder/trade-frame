/************************************************************************
 * Copyright(c) 2020, One Unified. All rights reserved.                 *
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
 * File:    Strategy.h
 * Author:  raymond@burkholder.net
 * Project: ESBracketOrder
 * Created: January 6, 2020, 11:41 AM
 */

#pragma once

#include "TFTimeSeries/DatedDatum.h"
#include <OUCharting/ChartDVBasics.h>

#include <TFTrading/Watch.h>
#include <TFInteractiveBrokers/IBTWS.h>

#include <TFTrading/Order.h>
#include <TFTrading/Position.h>
#include <TFTrading/Portfolio.h>

class Strategy:
  public ou::ChartDVBasics
{
public:

  using pWatch_t = ou::tf::Watch::pWatch_t;

  Strategy( pWatch_t );
  virtual ~Strategy();

  void HandleButtonUpdate();
  void HandleButtonSend( ou::tf::OrderSide::enumOrderSide );
  //void HandleButtonSend();
  void HandleButtonCancel();

protected:
private:

  using pIB_t = ou::tf::IBTWS::pProvider_t;
  using pOrder_t = ou::tf::Order::pOrder_t;
  using pPosition_t = ou::tf::Position::pPosition_t;
  using pPortfolio_t = ou::tf::Portfolio::pPortfolio_t;

  pIB_t m_pIB;
  pWatch_t m_pWatch;

  ou::tf::Trade m_tradeLast;

  long m_idOrderNext;

  pOrder_t m_pOrderEntry;
  pOrder_t m_pOrderProfit;
  pOrder_t m_pOrderStop;

  void HandleQuote( const ou::tf::Quote& );
  void HandleTrade( const ou::tf::Trade& );
};