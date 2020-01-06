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

#include <OUCharting/ChartDVBasics.h>

#include <TFTrading/Watch.h>
#include <TFInteractiveBrokers/IBTWS.h>

class Strategy:
  public ou::ChartDVBasics
{
public:

  using pIB = ou::tf::IBTWS::pProvider_t;
  using pWatch_t = ou::tf::Watch::pWatch_t;

  Strategy( pWatch_t );
  virtual ~Strategy();

protected:
private:
  pWatch_t m_pWatch;

  void HandleQuote( const ou::tf::Quote& );
  void HandleTrade( const ou::tf::Trade& );
};