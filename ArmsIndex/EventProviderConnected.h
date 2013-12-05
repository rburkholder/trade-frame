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
// Started 2013/12/05

#pragma once

#include <TFTrading/ProviderInterface.h>

class EventProviderConnected: public wxEvent {
public:

  typedef ou::tf::ProviderInterfaceBase::pProvider_t pProvider_t;

  EventProviderConnected( wxEventType eventType, int winid, pProvider_t pProvider );
  ~EventProviderConnected(void);

  pProvider_t& GetProvider( void ) { return m_pProvider; };

  virtual EventProviderConnected* Clone( void ) const { return new EventProviderConnected( *this ); }

protected:
private:
  pProvider_t m_pProvider;
};

wxDECLARE_EVENT(EVENT_PROVIDER_CONNECTED, EventProviderConnected);