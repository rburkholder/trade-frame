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
// Started 2013/11/16

#pragma once

#include <TFTrading/Instrument.h>

class EventIBInstrument: public wxEvent {
public:

  typedef ou::tf::Instrument::pInstrument_t pInstrument_t;

  EventIBInstrument( wxEventType eventType, int winid, pInstrument_t pInstrument );
  ~EventIBInstrument(void);

  pInstrument_t& GetInstrument( void ) { return m_pInstrument; };

  virtual EventIBInstrument* Clone( void ) const { return new EventIBInstrument( *this ); }

protected:
private:
  pInstrument_t m_pInstrument;
};

wxDECLARE_EVENT(EVENT_IB_INSTRUMENT, EventIBInstrument);

