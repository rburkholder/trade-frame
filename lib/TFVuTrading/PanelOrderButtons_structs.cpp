/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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
 * File:    PanelOrderButtons_structs.cpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFVuTrading
 * Created: March 9, 2022 16:29
 */

#include <boost/lexical_cast.hpp>

#include "PanelOrderButtons_structs.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

double PanelOrderButtons_Order::PositionEntryValue() {
  return boost::lexical_cast<double>( m_sPositionEntryValue );
}

double PanelOrderButtons_Order::PositionExitProfitValue() {
  return boost::lexical_cast<double>( m_sPositionExitProfitValue );
}

double PanelOrderButtons_Order::PositionExitStopValue() {
  return boost::lexical_cast<double>( m_sPositionExitStopValue );
}

} // namespace tf
} // namespace ou
