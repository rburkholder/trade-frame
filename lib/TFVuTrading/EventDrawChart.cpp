/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

#include "stdafx.h"

#include "EventDrawChart.h"

wxDEFINE_EVENT(EVENT_DRAW_CHART, ou::tf::EventDrawChart);

namespace ou { // One Unified
namespace tf { // TradeFrame

EventDrawChart::EventDrawChart( wxEventType eventType, int winid, pwxBitmap_t pBitmap )
  : wxEvent( winid, eventType ), m_pBitmap( pBitmap )
{
}

EventDrawChart::~EventDrawChart(void) {
}

} // namespace tf
} // namespace ou
