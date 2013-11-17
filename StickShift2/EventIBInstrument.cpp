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

#include "StdAfx.h"

#include "EventIBInstrument.h"

wxDEFINE_EVENT(EVENT_IB_INSTRUMENT, EventIBInstrument);

EventIBInstrument::EventIBInstrument( wxEventType eventType, int winid, pInstrument_t pInstrument )
  : wxEvent( winid, eventType ), m_pInstrument( pInstrument )
{
}

EventIBInstrument::~EventIBInstrument(void) {
}
